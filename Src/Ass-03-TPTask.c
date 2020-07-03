/*
 *        $Id: Ass-03-TPTask.c 2168 2020-06-14 21:52:43Z Peter $
 *  $Revision: 2168 $
 *      $Date: 2020-06-15 07:52:43 +1000 (Mon, 15 Jun 2020) $
 *    $Author: Peter $
 */

#include "Ass-03.h"

char TheTouchString[MESS_STR_LEN];

struct strButton buttonUp = {
  5, 5, 80, 20, 0, "UP"
},
buttonDown = {
  5, 30, 80, 20, 0, "DOWN"
},
buttonView = {
  5, 55, 80, 20, 0, "VIEW" 
},
buttonDelete = {
  5, 80, 80, 20, 0, "DELETE" 
},
buttonLive = {
  5, 105, 80, 20, 0, "LIVE" 
},
buttonReset = {
  5, 130, 80, 20, 0, "RESET" 
};


void StartTPTask(void const * argument)
{
  printf("INFO: Hello from %s!\n", pcTaskGetName(osThreadGetId()));
  fnvdBulkHandler();
  while (1){
    if(inPhotoTaken > 0){
      osDelay(10);
      continue;
    }
    if (BSP_TP_GetDisplayPoint (&display) == 0){
    }
    if(state)
      strcpy(buttonLive.name, "LIVE");
    else
      strcpy(buttonLive.name, " SD ");
	  osDelay(25);
    fnvdBulkHandler();
    positionPoint();
    fnvdResetTouch();
    osDelay(25);
  }
}

void fnvdButtonHandler(struct strButton *input){
  if((display.x > input->x && display.x < (input->x + input->width)) && (display.y > input->y && display.y < (input->y + input->height))){
    TextBoxSend(input->x, input->y, input->width, input->height, "      ");
    input->pressed = 1;
  }else{
    TextBoxSend(input->x, input->y, input->width, input->height, input->name);
    input->pressed = 0;
  }
}

void fnvdBulkHandler(void){
  fnvdButtonHandler(&buttonUp);
  fnvdButtonHandler(&buttonDown);
  fnvdButtonHandler(&buttonView);
  fnvdButtonHandler(&buttonDelete);
  fnvdButtonHandler(&buttonLive);
  fnvdButtonHandler(&buttonLive);
  // fnvdButtonHandler(&buttonReset);
}

void fnvdResetTouch(void){
  display.x = -1;
  display.y = -1;
}


void positionPoint(void){
  static volatile uint16_t xprev = 0;
  static volatile uint16_t yprev = 0;
  if(display.x != xprev || display.y != display.y){
    TextBoxSend(xprev, yprev, 1, 1, " ");
    TextBoxSend(display.x, display.y, 1, 1, ".");
    xprev = display.x;
    yprev = display.y;
  }
}

int fninCheckPressed(tenButtons choice){
  switch (choice){
  case up:
    return buttonUp.pressed;
  case down:
    return buttonDown.pressed;
  case view:
    return buttonView.pressed;
  case delete:
    return buttonDelete.pressed;
  case live:
    return buttonLive.pressed;
  default:
    return 0;
  }
}
