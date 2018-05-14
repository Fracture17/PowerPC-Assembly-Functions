#include "stdafx.h"
#include "IASA Overlay.h"

#define OVERLAY_LENGTH 12
#define IASA_OVERLAY_COLOR 0x9a9af9ff

//color blend module + 0x64 for trigger (byte, make second byte 0 for safety)
//color blend module + 0xB8 for color

void EndOverlay()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;

	
	LoadWordToReg(reg1, IS_IN_GAME_FLAG);
	If(reg1, EQUAL_I, 1); {
		LoadWordToReg(reg1, FRAME_COUNTER_LOC);
		LoadWordToReg(reg2, IASA_OVERLAY_MEM_PTR_LOC);
		ADDI(reg2, reg2, -4);
		CounterLoop(reg3, 0, IASA_BUFFER_LENGTH, 1); {
			LWZU(reg4, reg2, 8);
			If(reg4, EQUAL, reg1); {
				LWZ(reg4, reg2, -4);
				LWZ(reg5, reg4, 0xC);
				LoadWordToReg(reg6, IASA_TERMINATE_OVERLAY_COMMAND_PTR_LOC);
				STW(reg6, reg4, 0xC);
				MR(3, reg4);
				SetRegister(4, 0); //safety
				CallBrawlFunc(0x8077afdc); //interpretNotSystemCmd
				STW(reg5, reg4, 0xC);

				SetRegister(reg5, 0);
				STW(reg5, reg2, 0);
			}EndIf();
		}CounterLoopEnd();
	}EndIf();
}

void TriggerIASAOverlay()
{
	//use r25-r31
	//r4 is ptr to anim cmd
	//r27 is module ptr
	ASMStart(0x8077aff0);

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int TriggerOverlay = GetNextLabel();
	
	/*LoadWordToReg(reg4, reg5, IASA_STATE);
	LWZ(reg3, 4, 4);
	LWZ(reg3, reg3, 4);

	LWZ(reg2, 4, 0);

	SetRegister(reg1, 0x020B0100);
	If(reg2, EQUAL, reg1); {
		SetRegister(reg1, 1);
		RLWNM(reg3, reg1, reg3, 0, 31);
		ANDC(reg4, reg4, reg3); //disallow specific interupt
	}EndIf();

	SetRegister(reg1, 0x64010000);
	If(reg2, EQUAL, reg1); {
		SetRegister(reg4, 0); //disallow all interupts
	}EndIf();

	SetRegister(reg1, 0x020A0100);
	If(reg2, EQUAL, reg1); {
		SetRegister(reg1, 1);
		RLWNM(reg3, reg1, reg3, 0, 31);
		AND(reg1, reg4, reg3);
		If(reg1, EQUAL_I, 0); {
			OR(reg4, reg4, reg3); //allow specific interupt
			JumpToLabel(TriggerOverlay);
		}EndIf();
	}EndIf();

	SetRegister(reg1, 0x64000000); //allow interupt
	If(reg2, EQUAL, reg1); {
		SetRegister(reg4, -1);
		Label(TriggerOverlay);
		LoadWordToReg(reg1, IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC);
		STW(reg1, 3, 0xC);
		LoadWordToReg(reg1, IASA_OVERLAY_MEM_PTR_LOC);
		FindInTerminatedArray(3, reg1, 0, 8, reg2, reg3, 4);
		STW(3, reg1, 0);
		LoadWordToReg(reg2, FRAME_COUNTER_LOC);
		ADDI(reg2, reg2, OVERLAY_LENGTH);
		STW(reg2, reg1, 4);
		
		ADDI(reg1, 3, 0);
		ADDI(reg2, 4, 0);
		SetRegister(4, 0);
		CallBrawlFunc(0x8077afdc); //interpretNotSystemCmd
		ADDI(3, reg1, 0);
		STW(reg2, 3, 0xC);
	}EndIf();

	STW(reg4, reg5, 0);*/

	LoadWordToReg(reg1, IS_IN_GAME_FLAG);
	If(reg1, EQUAL_I, 1); {
		LoadWordToReg(reg1, START_OF_CODE_MENU);
		If(reg1, EQUAL_I, 1); {
			LWZ(reg2, 4, 0);

			SetRegister(reg1, 0x64000000); //allow interupt
			If(reg2, EQUAL, reg1); {
				LoadWordToReg(reg1, IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC);
				STW(reg1, 3, 0xC);
				LoadWordToReg(reg1, IASA_OVERLAY_MEM_PTR_LOC);
				FindInTerminatedArray(3, reg1, 0, 8, reg2, reg3, 4);
				STW(3, reg1, 0);
				LoadWordToReg(reg2, FRAME_COUNTER_LOC);
				ADDI(reg2, reg2, OVERLAY_LENGTH);
				STW(reg2, reg1, 4);

				ADDI(reg1, 3, 0);
				ADDI(reg2, 4, 0);
				SetRegister(4, 0);
				CallBrawlFunc(0x8077afdc); //interpretNotSystemCmd
				ADDI(3, reg1, 0);
				STW(reg2, 3, 0xC);
			}EndIf();
		}EndIf();
	}EndIf();

	ASMEnd(0x7c7e1b78); //mr r30, r3
}

void SetupIASABuffers()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;

	//allocate and initialize buffers
	//control buffer
	SetRegister(reg1, IASA_BUFFER_SIZE);
	Allocate(reg1);
	SetRegister(reg1, IASA_OVERLAY_MEM_PTR_LOC);
	STW(3, reg1, 0);
	//initialize
	SetRegister(reg1, 0);
	STW(reg1, 3, 0);
	CounterLoop(reg2, 0, IASA_BUFFER_LENGTH - 1, 1); {
		STW(reg1, 3, 4);
		STWU(reg1, 3, 8);
	}CounterLoopEnd();
	
	//trigger buffer
	SetRegister(reg1, IASA_TRIGGER_BUFFER_SIZE);
	Allocate(reg1);
	SetRegister(reg1, IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC);
	STW(3, reg1, 0);
	//initialize
	SetRegister(reg1, 0x21010400);
	ADDI(reg3, 3, 8);
	STW(reg1, 3, 0);
	STWU(reg3, 3, 4);
	SetRegister(reg3, 0);
	for (auto x : { (IASA_OVERLAY_COLOR >> 24) & 0xFF, (IASA_OVERLAY_COLOR >> 16) & 0xFF, (IASA_OVERLAY_COLOR >> 8) & 0xFF, IASA_OVERLAY_COLOR & 0xFF }) {
		SetRegister(reg2, x);
		STW(reg3, 3, 4);
		STWU(reg2, 3, 8);
	}

	//terminate buffer
	SetRegister(reg1, IASA_TERMINATE_BUFFER_SIZE);
	Allocate(reg1);
	SetRegister(reg1, IASA_TERMINATE_OVERLAY_COMMAND_PTR_LOC);
	STW(3, reg1, 0);
	//initialize
	SetRegister(reg1, 0x21000000);
	SetRegister(reg3, 0);
	STW(reg1, 3, 0);
	STWU(reg3, 3, 4);
}

void FreeIASABuffers()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;

	LoadWordToReg(reg1, IASA_OVERLAY_MEM_PTR_LOC);
	FreeMem(reg1);

	LoadWordToReg(reg1, IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC);
	FreeMem(reg1);

	LoadWordToReg(reg1, IASA_TERMINATE_OVERLAY_COMMAND_PTR_LOC);
	FreeMem(reg1);
}