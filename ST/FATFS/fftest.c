/* Includes ------------------------------------------------------------------*/
#include <stm32h7xx_hal.h>
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "fftest.h"
#include "menu.h"
#include "bsp_spi.h"

/* Private typedef -----------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL TestFile;   /* File object */
char SDPath[4]; /* SD card logical drive path */
uint8_t workBuffer[_MAX_SS];

/**
  * @brief  FS_FileOperations
  * @param  thread not used
  * @retval None
  */
static int FS_FileOperations(void *pio)
{
  uint32_t byteswritten, bytesread;                          /* File write/read counts */
  uint8_t wtext[] = "This is STM32 working with FatFs + TF"; /* File write buffer */
  uint8_t rtext[100];                                        /* File read buffer */
  
  char *filename = NULL;

  GEN_IO io = (GEN_IO)pio;
  if (io != NULL)
    filename = nextarg(io->inbuf);

  if (*filename == 0)
  {
    ns_printf(pio, "usage: write string \n");
    return -1;
  }

  //------------------------ WRITE FILE --------------------------------------------------
  /* Create and Open a new text file object with write access */
  if (f_open(&TestFile, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    TRACE_INFO("open fs error\r\n");
    return -1;
  }

  /* Write data to the text file */
  if (f_write(&TestFile, wtext, sizeof(wtext), (void *)&byteswritten) != FR_OK)
  {
    TRACE_INFO("write fs error\r\n");
    return -1;
  }

  /* Close the open text file */
  if (f_close(&TestFile) != FR_OK)
  {
    TRACE_INFO("close fs error\r\n");
    return -1;
  }

  //------------------------ READ FILE --------------------------------------------------
  /* Open the text file object with read access */
  if (f_open(&TestFile, filename, FA_READ) != FR_OK)
  {
    TRACE_INFO("open fs error\r\n");
    return -1;
  }

  /* Read data from the text file */
  if (f_read(&TestFile, rtext, sizeof(rtext), (void *)&bytesread) != FR_OK)
  {
    TRACE_INFO("read fs error\r\n");
    return -1;
  }

  /* Close the open text file */
  if (f_close(&TestFile) != FR_OK)
  {
    TRACE_INFO("close fs error\r\n");
    return -1;
  }

  //------------------------ COMPARE FILE --------------------------------------------------
  /* strings is equal? */
  if (strncmp((char *)rtext, (char *)wtext, strlen((char *)wtext)) != 0)
  {
    TRACE_INFO("close fs error\r\n");
    return -1;
  }

  TRACE_INFO("write:%s,bytes:%d\r\n", wtext, byteswritten);
  TRACE_INFO("read :%s,bytes:%d\r\n", wtext, bytesread);

  /* Success of the demo: no error occurrence */
  TRACE_INFO("Success of the demo: no error occurrence\r\n");
  return 0;
}

static struct menu_op fsmenu[] =
{
  "fatfs" ,  stooges          ,  "lite fatfs cmds" ,
  "fstest",  FS_FileOperations,  "start test fatfs",
  NULL    ,                  0,                NULL,
};

/**
 * @brief 
 * 
 */
void FS_Init(void)
{
  FRESULT res; /* FatFs function common result code */

  if (FATFS_LinkDriver(&SD_Driver, SDPath) != FR_OK)
  {
    TRACE_INFO("make fs error\r\n");
    return;
  }

  res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));
  if (res != FR_OK)
  {
    TRACE_INFO("make fs error\r\n");
    return;
  }

  res = f_mount(&SDFatFs, (TCHAR const *)SDPath, 0);
  /* Register the file system object to the FatFs module */
  if (res != FR_OK)
  {
    TRACE_INFO("mount fs error\r\n");
    return;
  }

  /* install menu */
  install_menu(&fsmenu[0]);
}

/**
 * @brief 
 * 
 */
void FS_Deinit(void)
{
  /* Unlink SD diskio driver */
  FATFS_UnLinkDriver(SDPath);
}
