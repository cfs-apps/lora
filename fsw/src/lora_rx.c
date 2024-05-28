/*
** Copyright 2022 bitValence, Inc.
** All Rights Reserved.
**
** This program is free software; you can modify and/or redistribute it
** under the terms of the GNU Affero General Public License
** as published by the Free Software Foundation; version 3 with
** attribution addendums as found in the LICENSE.txt
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** Purpose:
**   Manage Lora Receive demo
**
** Notes:
**   None
**
*/

/*
** Includes
*/

#include "lora_rx.h"


/***********************/
/** Macro Definitions **/
/***********************/


/**********************/
/** Type Definitions **/
/**********************/


/********************************** **/
/** Local File Function Prototypes **/
/************************************/


/*****************/
/** Global Data **/
/*****************/

static LORA_RX_Class_t *LoraRx;


/******************************************************************************
** Function: LORA_RX_Constructor
**
** Initialize the LoRa Transmit object
**
** Notes:
**   1. This must be called prior to any other member functions.
**
*/
void LORA_RX_Constructor(LORA_RX_Class_t *LoraRxPtr)
{

   LoraRx = LoraRxPtr;
   
   memset(LoraRx, 0, sizeof(LORA_RX_Class_t));
   
} /* End LORA_RX_Constructor() */


/******************************************************************************
** Function: LORA_RX_ChildTask
**
** Notes:
**   1. Returning false causes the child task to terminate.
**   2. Information events are sent because this is instructional code and the
**      events provide feedback. The events are filtered so they won't flood
**      the ground. A reset app command resets the event filter.  
*/
bool LORA_RX_ChildTask(CHILDMGR_Class_t *ChildMgr)
{

   // TODO: Add child task logic
   while (true)
   {  
      LoraRx->PktCnt++;
      CFE_EVS_SendEvent (LORA_RX_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION,
                         "Rx child task execution. Msgcnt = %d", LoraRx->PktCnt);
      OS_TaskDelay(2000);
   }

   return true;
   
} /* End LORA_RX_ChildTask() */


/******************************************************************************
** Function: LORA_RX_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
*/
void LORA_RX_ResetStatus(void)
{

   LoraRx->PktCnt    = 0;
   LoraRx->PktErrCnt = 0;

} /* End LORA_RX_ResetStatus() */

