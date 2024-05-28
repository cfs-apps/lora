/*
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Lesser General Public License as
**  published by the Free Software Foundation, either version 3 of the
**  License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this program.  If not, see <https://www.gnu.org/licenses/>.
**
**  Purpose:
**    Implement the Transmit Demo Class methods
**
**  Notes:
**    1. Serves as a bridge between the C++ Radio object and the Lora app.
**       This object is needed because using extern "C" with cfe.h wouldn't
**       compile. If this worked then radio_if and radio could have been
**       one object.
**    2. For each radio command, the Radio object in the SX128X library
**       performs radio level validation checks. The Radio object does not
**       have a cFE interface so these command functions issue events
**       messages.
**    TODO: Determine which command validity checks should be implemented
**    TODO: Detmerine what radio status can be provide in command failure events
**    TODO: Determine how the radio state is maintained and reported in tlm.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "radio.h"
#include "radio_if.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ   (RadioIf->IniTbl)


/**********************/
/** Global File Data **/
/**********************/

static RADIO_IF_Class_t *RadioIf = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/


/******************************************************************************
** Function: RADIO_IF_Constructor
**
** Initialize the Radio Interface object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void RADIO_IF_Constructor(RADIO_IF_Class_t *RadioIfPtr, INITBL_Class_t *IniTbl)
{
   
   RadioIf = RadioIfPtr;
   
   memset(RadioIf, 0, sizeof(RADIO_IF_Class_t));
   
   RadioIf->IniTbl = IniTbl;
   
   RadioIf->RadioConfig.Frequency = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_FREQUENCY);
   
   RadioIf->RadioConfig.LoRa.SpreadingFactor = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_SF);
   RadioIf->RadioConfig.LoRa.Bandwidth       = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_BW);
   RadioIf->RadioConfig.LoRa.CodingRate      = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_CR);
      
   CFE_MSG_Init(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_LORA_RADIO_TLM_TOPICID)), sizeof(LORA_RadioTlm_t));

} /* End RADIO_IF_Constructor() */


/******************************************************************************
** Function: RADIO_IF_ChildTask
**
** Notes:
**   1. Returning false causes the child task to terminate.
**   2. Information events are sent because this is instructional code and the
**      events provide feedback. The events are filtered so they won't flood
**      the ground. A reset app command resets the event filter.  
**
*/
bool RADIO_IF_ChildTask(CHILDMGR_Class_t *ChildMgr)
{
   
   bool RetStatus = true;
 
   CFE_EVS_SendEvent (RADIO_IF_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION, 
                      "RADIO_IF_ChildTask()"); 
   OS_TaskDelay(2000);
       
   return RetStatus;

} /* End RADIO_IF_ChildTask() */


/******************************************************************************
** Function: RADIO_IF_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void RADIO_IF_ResetStatus(void)
{

   return;

} /* End RADIO_IF_ResetStatus() */


/******************************************************************************
** Function: RADIO_IF_SendRadioTlmCmd
**
** Notes:
**   1. See radio_if.h file prologue for data source details.
*/
bool RADIO_IF_SendRadioTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   LORA_RadioTlm_Payload_t *RadioTlmPayload = &RadioIf->RadioTlm.Payload;
   
   strcpy(RadioTlmPayload->SpiDevStr, "TODO");   
   RadioTlmPayload->SpiDevNum      = 0;
   RadioTlmPayload->SpiSpeed       = 0; //TODO: Get SPI setting
   RadioTlmPayload->RadioPinBusy   = 1;
   RadioTlmPayload->RadioPinNrst   = 2;
   RadioTlmPayload->RadioPinNss    = 3;
   RadioTlmPayload->RadioPinDio1   = 4;
   RadioTlmPayload->RadioPinDio2   = 5;
   RadioTlmPayload->RadioPinDio3   = 6;
   RadioTlmPayload->RadioPinTxEn   = 7;
   RadioTlmPayload->RadioPinRxEn   = 8;

   RadioTlmPayload->RadioFrequency      = RadioIf->RadioConfig.Frequency;
   RadioTlmPayload->LoRaSpreadingFactor = RadioIf->RadioConfig.LoRa.SpreadingFactor;
   RadioTlmPayload->LoRaBandwidth       = RadioIf->RadioConfig.LoRa.Bandwidth;
   RadioTlmPayload->LoRaCodingRate      = RadioIf->RadioConfig.LoRa.CodingRate;
   
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader), true);

   CFE_EVS_SendEvent(RADIO_IF_SEND_RADIO_TLM_CMD_EID, CFE_EVS_EventType_INFORMATION,
                     "Sent Radio configuration telemetry message");
   return true;
   
} /* RADIO_IF_SendRadioTlmCmd() */


/******************************************************************************
** Function: RADIO_IF_SetLoRaParamsCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetLoRaParamsCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetLoRaParams_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetLoRaParams_t);
   bool RetStatus = false;

   RadioIf->RadioConfig.LoRa.SpreadingFactor = Cmd->SpreadingFactor;
   RadioIf->RadioConfig.LoRa.Bandwidth       = Cmd->Bandwidth;
   RadioIf->RadioConfig.LoRa.CodingRate      = Cmd->CodingRate;

   RetStatus = RADIO_SetLoraParams(RadioIf->RadioConfig.LoRa.SpreadingFactor,
                                   RadioIf->RadioConfig.LoRa.Bandwidth,
                                   RadioIf->RadioConfig.LoRa.CodingRate);
   if (RetStatus)
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_LORA_PARAMS_CMD_EID, CFE_EVS_EventType_INFORMATION,
                        "Set LoRa paramaters: SF=%d, BW=%d, RC=%d", Cmd->SpreadingFactor,
                        Cmd->Bandwidth, Cmd->CodingRate);
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_LORA_PARAMS_CMD_EID, CFE_EVS_EventType_ERROR,
                        "Set LoRa parameters failed");
   }

   return RetStatus;
   
} /* RADIO_IF_SetLoRaParamsCmd() */


/******************************************************************************
** Function: RADIO_IF_SetRadioFrequencyCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetRadioFrequencyCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetRadioFrequency_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetRadioFrequency_t);
   bool RetStatus = false;

   if (Cmd->Frequency >= 0 && Cmd->Frequency <= 48000)
   {
      RadioIf->RadioConfig.Frequency = Cmd->Frequency;
      
      RetStatus = RADIO_SetRadioFrequency(Cmd->Frequency*1000000UL);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio frequency to %d Mhz", Cmd->Frequency);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio frequency failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio frequency failed, invalid frequency %d.",
                           Cmd->Frequency);
   }

   return RetStatus;
   
} /* RADIO_IF_SetRadioFrequencyCmd() */
