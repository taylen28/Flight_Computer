/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   SPI SD card disk I/O driver for FatFS
 ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "main.h"

extern SPI_HandleTypeDef hspi3;

/* SD card type flags */
#define CT_MMC    0x01
#define CT_SD1    0x02
#define CT_SD2    0x04
#define CT_SDC    (CT_SD1|CT_SD2)
#define CT_BLOCK  0x08

static uint8_t CardType;

#define CS_LOW()  HAL_GPIO_WritePin(SDCARD_SPI_CS_GPIO_Port, SDCARD_SPI_CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(SDCARD_SPI_CS_GPIO_Port, SDCARD_SPI_CS_Pin, GPIO_PIN_SET)

static void SPI_SetSlow(void) {
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // ~140kHz for SD init
    HAL_SPI_Init(&hspi3);
}

static void SPI_SetFast(void) {
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 4.5MHz for data
    HAL_SPI_Init(&hspi3);
}

static uint8_t SPI_TxRx(uint8_t data) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi3, &data, &rx, 1, 10);
    return rx;
}

static uint8_t wait_ready(uint32_t ms) {
    uint32_t start = HAL_GetTick();
    uint8_t d;
    do { d = SPI_TxRx(0xFF); } while (d != 0xFF && (HAL_GetTick() - start) < ms);
    return d == 0xFF;
}

static void deselect(void) {
    CS_HIGH();
    SPI_TxRx(0xFF);
}

static uint8_t select_card(void) {
    CS_LOW();
    SPI_TxRx(0xFF);
    if (wait_ready(500)) return 1;
    deselect();
    return 0;
}

static uint8_t send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t n, res;
    if (cmd & 0x80) {        // ACMD: send CMD55 first
        cmd &= 0x7F;
        res = send_cmd(55, 0);
        if (res > 1) return res;
    }
    deselect();
    if (!select_card()) return 0xFF;
    SPI_TxRx(0x40 | cmd);
    SPI_TxRx((uint8_t)(arg >> 24));
    SPI_TxRx((uint8_t)(arg >> 16));
    SPI_TxRx((uint8_t)(arg >> 8));
    SPI_TxRx((uint8_t)arg);
    n = 0x01;
    if (cmd == 0) n = 0x95;  // CMD0 CRC
    if (cmd == 8) n = 0x87;  // CMD8 CRC
    SPI_TxRx(n);
    n = 10;
    do { res = SPI_TxRx(0xFF); } while ((res & 0x80) && --n);
    return res;
}

static uint8_t rcvr_datablock(uint8_t *buff, UINT btr) {
    uint8_t token;
    uint32_t start = HAL_GetTick();
    do { token = SPI_TxRx(0xFF); } while (token == 0xFF && (HAL_GetTick() - start) < 200);
    if (token != 0xFE) return 0;
    for (UINT i = 0; i < btr; i++) buff[i] = SPI_TxRx(0xFF);
    SPI_TxRx(0xFF); SPI_TxRx(0xFF); // discard CRC
    return 1;
}

static uint8_t xmit_datablock(const uint8_t *buff, uint8_t token) {
    if (!wait_ready(500)) return 0;
    SPI_TxRx(token);
    if (token == 0xFD) return 1; // stop token — no data
    for (UINT i = 0; i < 512; i++) SPI_TxRx(buff[i]);
    SPI_TxRx(0xFF); SPI_TxRx(0xFF); // dummy CRC
    uint8_t resp = SPI_TxRx(0xFF);
    return (resp & 0x1F) == 0x05;
}

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat = STA_NOINIT;

/* Private functions ---------------------------------------------------------*/

DSTATUS USER_initialize(BYTE pdrv)
{
  /* USER CODE BEGIN INIT */
    uint8_t n, type, ocr[4];

    SPI_SetSlow();
    CS_HIGH();
    for (n = 10; n; n--) SPI_TxRx(0xFF); // 80 dummy clocks to enter SPI mode

    type = 0;
    if (send_cmd(0, 0) == 1) {             // Reset card (CMD0)
        uint32_t start = HAL_GetTick();
        if (send_cmd(8, 0x1AA) == 1) {     // CMD8: check SDHC support
            for (n = 0; n < 4; n++) ocr[n] = SPI_TxRx(0xFF);
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
                // SDv2: wait for ACMD41 with HCS bit
                while ((HAL_GetTick() - start) < 1000 && send_cmd(0x80|41, 0x40000000));
                if ((HAL_GetTick() - start) < 1000 && send_cmd(58, 0) == 0) {
                    for (n = 0; n < 4; n++) ocr[n] = SPI_TxRx(0xFF);
                    type = (ocr[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2;
                }
            }
        } else {
            // SDv1 or MMC
            uint8_t cmd;
            if (send_cmd(0x80|41, 0) <= 1) { type = CT_SD1; cmd = 0x80|41; }
            else                            { type = CT_MMC; cmd = 1; }
            while ((HAL_GetTick() - start) < 1000 && send_cmd(cmd, 0));
            if ((HAL_GetTick() - start) >= 1000 || send_cmd(16, 512) != 0) type = 0;
        }
    }

    CardType = type;
    deselect();

    if (type) {
        SPI_SetFast();
        Stat &= ~STA_NOINIT;
    }

    return Stat;
  /* USER CODE END INIT */
}

DSTATUS USER_status(BYTE pdrv)
{
  /* USER CODE BEGIN STATUS */
    return Stat;
  /* USER CODE END STATUS */
}

DRESULT USER_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN READ */
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (!(CardType & CT_BLOCK)) sector *= 512; // byte address for SDSC

    if (count == 1) {
        if (send_cmd(17, sector) == 0 && rcvr_datablock(buff, 512)) count = 0;
    } else {
        if (send_cmd(18, sector) == 0) {
            do { if (!rcvr_datablock(buff, 512)) break; buff += 512; } while (--count);
            send_cmd(12, 0); // STOP_TRANSMISSION
        }
    }
    deselect();
    return count ? RES_ERROR : RES_OK;
  /* USER CODE END READ */
}

#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN WRITE */
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (!(CardType & CT_BLOCK)) sector *= 512;

    if (count == 1) {
        if (send_cmd(24, sector) == 0 && xmit_datablock(buff, 0xFE)) count = 0;
    } else {
        if (CardType & CT_SDC) send_cmd(0x80|23, count); // ACMD23 pre-erase
        if (send_cmd(25, sector) == 0) {
            do { if (!xmit_datablock(buff, 0xFC)) break; buff += 512; } while (--count);
            xmit_datablock(0, 0xFD); // stop token
        }
    }
    deselect();
    return count ? RES_ERROR : RES_OK;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
DRESULT USER_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
  /* USER CODE BEGIN IOCTL */
    DRESULT res = RES_ERROR;
    uint8_t n, csd[16];
    DWORD csize;

    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (cmd) {
        case CTRL_SYNC:
            if (select_card()) { deselect(); res = RES_OK; }
            break;
        case GET_SECTOR_COUNT:
            if (send_cmd(9, 0) == 0 && rcvr_datablock(csd, 16)) {
                if ((csd[0] >> 6) == 1) { // SDv2
                    csize = csd[9] + ((DWORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
                    *(DWORD*)buff = csize << 10;
                } else { // SDv1/MMC
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((DWORD)csd[7] << 2) + ((DWORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = csize << (n - 9);
                }
                res = RES_OK;
            }
            break;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
    }
    deselect();
    return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */
