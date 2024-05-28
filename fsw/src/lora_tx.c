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
**   Manage Lora Transmit demo
**
** Notes:
**   None
**
*/

/*
** Includes
*/

#include "lora_tx.h"


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

static LORA_TX_Class_t *LoraTx;


/******************************************************************************
** Function: LORA_TX_Constructor
**
** Initialize the LoRa Transmit object
**
** Notes:
**   1. This must be called prior to any other member functions.
**
*/
void LORA_TX_Constructor(LORA_TX_Class_t *LoraTxPtr)
{

   LoraTx = LoraTxPtr;
   
   memset(LoraTx, 0, sizeof(LORA_TX_Class_t));
   
} /* End LORA_TX_Constructor() */


/******************************************************************************
** Function: LORA_TX_ChildTask
**
** Notes:
**   1. Returning false causes the child task to terminate.
**   2. Information events are sent because this is instructional code and the
**      events provide feedback. The events are filtered so they won't flood
**      the ground. A reset app command resets the event filter.  
*/
bool LORA_TX_ChildTask(CHILDMGR_Class_t *ChildMgr)
{

   // TODO: Add child task logic
   while (true)
   {  
      LoraTx->PktCnt++;
      CFE_EVS_SendEvent (LORA_TX_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION,
                         "Tx child task execution. Msgcnt = %d", LoraTx->PktCnt);
      OS_TaskDelay(2000);
   }

   return true;
   
} /* End LORA_TX_ChildTask() */


/******************************************************************************
** Function: LORA_TX_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
*/
void LORA_TX_ResetStatus(void)
{

   LoraTx->PktCnt    = 0;
   LoraTx->PktErrCnt = 0;

} /* End LORA_TX_ResetStatus() */


/******************************************************************************
** Function: LORA_TX_StartDemoCmd
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
*/
bool LORA_TX_StartDemoCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr)
{

   LoraTx->DemoActive = true;
   CFE_EVS_SendEvent (LORA_TX_START_DEMO_EID, CFE_EVS_EventType_INFORMATION,
                      "LoRa Tx demo started");
   return true;
   
} /* LORA_TX_StartDemoCmd() */


/******************************************************************************
** Function: LORA_TX_StopDemoCmd
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
*/
bool LORA_TX_StopDemoCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   LoraTx->DemoActive = false;
   CFE_EVS_SendEvent (LORA_TX_STOP_DEMO_EID, CFE_EVS_EventType_INFORMATION,
                      "LoRa Tx demo stopped");
   return true;

} /* LORA_TX_StopDemoCmd() */


/******* Demo logic
int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage: Lora_tx <freq in MHz> <file to send>\n");
		return 1;
	}

	// Pins based on hardware configuration
	SX128x_Linux Radio("/dev/spidev0.0", 0, {27, 26, 20, 16, -1, -1, 24, 25});

	// Assume we're running on a high-end Raspberry Pi,
	// so we set the SPI clock speed to the maximum value supported by the chip
	Radio.SetSpiSpeed(8000000);


	Radio.Init();
	puts("Init done");
	Radio.SetStandby(SX128x::STDBY_XOSC);
	puts("SetStandby done");
	Radio.SetRegulatorMode(static_cast<SX128x::RadioRegulatorModes_t>(0));
	puts("SetRegulatorMode done");
	Radio.SetLNAGainSetting(SX128x::LNA_HIGH_SENSITIVITY_MODE);
	puts("SetLNAGainSetting done");
	Radio.SetTxParams(0, SX128x::RADIO_RAMP_20_US);
	puts("SetTxParams done");

	Radio.SetBufferBaseAddresses(0x00, 0x00);
	puts("SetBufferBaseAddresses done");


	SX128x::ModulationParams_t ModulationParams;
	SX128x::PacketParams_t PacketParams;

    // Modulation Parameters
    ModulationParams.PacketType = SX128x::PACKET_TYPE_LORA;
    ModulationParams.Params.LoRa.CodingRate = SX128x::LORA_CR_4_8;
    ModulationParams.Params.LoRa.Bandwidth = SX128x::LORA_BW_1600;
    ModulationParams.Params.LoRa.SpreadingFactor = SX128x::LORA_SF7;

    PacketParams.PacketType = SX128x::PACKET_TYPE_LORA;

    // Packet Parameters
    auto &l = PacketParams.Params.LoRa;
    // l.PayloadLength = PACKET_SIZE;
    l.HeaderType = SX128x::LORA_PACKET_VARIABLE_LENGTH;
    l.PreambleLength = 12;
    l.Crc = SX128x::LORA_CRC_ON;
    l.InvertIQ = SX128x::LORA_IQ_NORMAL;

    Radio.SetPacketType(SX128x::PACKET_TYPE_LORA);

	puts("SetPacketType done");
	Radio.SetModulationParams(ModulationParams);
	puts("SetModulationParams done");
	Radio.SetPacketParams(PacketParams);
	puts("SetPacketParams done");

	auto freq = strtol(argv[1], nullptr, 10);
	Radio.SetRfFrequency(freq * 1000000UL);
	puts("SetRfFrequency done");

	std::cout << "Firmware version: " << Radio.GetFirmwareVersion() << "\n";

    // TX done interrupt handler
	Radio.callbacks.txDone = []{
		puts("Done!");
	};

	auto IrqMask = SX128x::IRQ_TX_DONE | SX128x::IRQ_RX_TX_TIMEOUT;
	Radio.SetDioIrqParams(IrqMask, IrqMask, SX128x::IRQ_RADIO_NONE, SX128x::IRQ_RADIO_NONE);
	puts("SetDioIrqParams done");

	Radio.StartIrqHandler();
	puts("StartIrqHandler done");


	auto pkt_ToA = Radio.GetTimeOnAir();

    // Open file
    std::ifstream fileToSend(argv[2], std::ios::binary);

    // Get size and number of packets
    fileToSend.seekg(0, std::ios::end);
    unsigned int fileSize = fileToSend.tellg();
    unsigned int numPackets = (fileSize + PACKET_SIZE - 1) / PACKET_SIZE;

    // Seek back to start
    fileToSend.seekg(0, std::ios::beg);


    printf("Sending %d packets (%d bytes)...\n", numPackets, fileSize);

    // Send packet with file size
    char numPacketsText[10];
    sprintf(numPacketsText, "%d", numPackets);
    unsigned int packetsTextLen = strlen(numPacketsText);

    printf("Sending num bytes...");
    Radio.SendPayload((uint8_t*)numPacketsText, packetsTextLen, {SX128x::RADIO_TICK_SIZE_1000_US, 1000});
    usleep((pkt_ToA + 20) * 1000);

    // Send all packets
    uint8_t buf[PACKET_SIZE];
    unsigned int index = 0;
    unsigned int totalPackets = 0;

    while (fileToSend) {
        char c = fileToSend.get();
        buf[index] = c;
        index++;

        if (index >= PACKET_SIZE) {
            // Reset index and send package 
            index = 0;
            Radio.SendPayload(buf, PACKET_SIZE, {SX128x::RADIO_TICK_SIZE_1000_US, 1000});
            printf("Sening packet %d...", totalPackets);

            printf("\n\n\nPACKET %d:\n", totalPackets);
            for (int i = 0; i < PACKET_SIZE; i++) {
                printf("%c", buf[i]);
            }
            printf("\n"); 

            usleep((pkt_ToA + 20) * 1000);
            totalPackets++;
        }
    }

    // Send partial last packet
    Radio.SendPayload(buf, index, {SX128x::RADIO_TICK_SIZE_1000_US, 1000});
    printf("Sent packet %d (partial last packet)...", totalPackets);
    usleep((pkt_ToA + 20) * 1000);

    printf("\n\n\nPACKET %d:\n", totalPackets);
    for (int i = 0; i < PACKET_SIZE; i++) {
        printf("%c", buf[i]);
    }


    fileToSend.close();

    printf("Exiting...\n");
    Radio.StopIrqHandler();
    return EXIT_SUCCESS;
}
**********/
