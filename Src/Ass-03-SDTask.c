/*
 *        $Id: Ass-03-SDTask.c 2171 2020-06-14 22:25:52Z Peter $
 *  $Revision: 2171 $
 *      $Date: 2020-06-15 08:25:52 +1000 (Mon, 15 Jun 2020) $
 *    $Author: Peter $
 */

#include "Ass-03.h"

uint16_t inPhotoNumber;
uint8_t inPhotoTaken = 0;



#define BUFF_SIZE 128
void ExitError();
void StartSDTask (void const * argument)
{
  uint8_t inPhotoSet = 0;
  const TCHAR *pCtrlFileName = ".ctrl.txt";
  //const TCHAR *fn = "FIRST.TXT";

  uint16_t inNumPhotoTemp;
  FIL fd;
  FRESULT res;
  UINT bytesread;
  //DIR dir;
  uint8_t textread[BUFF_SIZE];
  //FILINFO fno;

  printf ("INFO: Hello from %s!\n", pcTaskGetName (osThreadGetId ()));
  osDelay(10000);
  while (1)
  {
    if(inPhotoTaken == 1){
      osDelay(20);
      continue;
    }
    if(state && inPhotoTaken != 2){
      sprintf(textread, "Number of photos: %d", inPhotoNumber);
      TextBoxSend(5, 185, 310, 20, textread);
    }
    osMutexWait (SDIODCMIMutexHandle, osWaitForever);
    Swap_Camera_to_SDCard ();
    if ((res = f_mount (&SDFatFS, (TCHAR const *) SDPath, 0)) != FR_OK)
    {

      printf ("ERROR: Could not mount file system '%s' (%d).\n", SDPath, res);
      ExitError();
    }else{
      /* SD CODE GOES HERE */
      if(f_open(&fd, pCtrlFileName, FA_OPEN_EXISTING) != FR_OK){
          f_open(&fd, pCtrlFileName, FA_CREATE_ALWAYS);
          fprintf(&fd, "0");
      }else{
        f_gets(&fd, BUFF_SIZE, textread);
        sscanf(textread, "%d", &inNumPhotoTemp);
        if(inPhotoSet){
          inPhotoNumber = inNumPhotoTemp;
          inPhotoSet = 1;
        }else{
          if(inNumPhotoTemp < inPhotoNumber){
            fprintf(&fd, "%d\r", inPhotoNumber);
          }
        }
      }
      f_close(&fd);
      if(inPhotoTaken == 2){
        sprintf(textread, "%d.ppm", inPhotoNumber); // Only stores the photo in "grey scale", but it's a start.
        f_open(&fd, textread, FA_CREATE_ALWAYS);
        f_printf(&fd, "P5\nPHOTO_X_SIZE PHOTO_Y_SIZE\n");
        f_close(&fd);
        f_open(&fd, textread, FA_OPEN_APPEND);
        f_write(&fd, archPhotoBuffer, PHOTO_X_SIZE * PHOTO_Y_SIZE, bytesread);
        f_close(&fd);
        inPhotoTaken = 0;
        inPhotoNumber++;
        HAL_GPIO_WritePin(GPIOD, LD4_GPIO_Port, SET); // Clear when read sd function is impletemented
        BSP_LCD_Clear(0xFFFFFFFF);
      }
      /* SD CODE GOES HERE */
      if ((res = f_mount (0, "", 0)) != FR_OK)
      {
        printf ("ERROR: Unmounting file system\n");
        ExitError();
      }
    }

    Swap_SDCard_to_Camera ();
    osMutexRelease (SDIODCMIMutexHandle);
    osDelay(10);
  }
}


// Stop if an error occurs
//
void ExitError()
{

  // Give back control to DCMI interface and release MUTEX
  Swap_SDCard_to_Camera ();
  osMutexRelease (SDIODCMIMutexHandle);

  // Wait forever
  while (1)
    osDelay (1000);

}
