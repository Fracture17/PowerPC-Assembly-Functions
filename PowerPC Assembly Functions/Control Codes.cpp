#include "stdafx.h"
#include "Control Codes.h"
#include "DrawDI.h"
#include "IASA Overlay.h"
#include "Code Menu.h"

void ControlCodes()
{
	StartMatch();

	EndMatch();

	Draw();

	LoadCodeMenu();
}

void LoadCodeMenu()
{
	ASMStart(0x8002d4f4);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;

	SetRegister(reg1, STRING_BUFFER);

	SetRegister(reg2, STRING_BUFFER + 0x18);
	STW(reg2, reg1, 0); //file path ptr

	SetRegister(reg2, 0);
	STW(reg2, reg1, 4);
	STW(reg2, reg1, 8);
	STW(reg2, reg1, 0x10);

	SetRegister(reg2, START_OF_MENU_LOC);
	STW(reg2, reg1, 0xC); //storage loc

	SetRegister(reg2, -1);
	STW(reg2, reg1, 0x14); //storage loc

	ADDI(reg2, reg1, 0x18);
	WriteStringToMem("LegacyTE/rsbe01.txt\0"s, reg2);

	MR(3, reg1);
	CallBrawlFunc(0x8001cbf4); //readSDFile

	RestoreRegisters();
	ASMEnd(0x9421ffe0); //stwu sp, sp, -0x20
}

void StartMatch()
{
	ASMStart(0x806cf15c);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;

	//allocate memory
#ifdef IASA_OVERLAY
	SetupIASABuffers();
#endif

#ifdef DI_DRAW
	SetupDIBuffer();
#endif

	//set in game flag
	SetRegister(reg1, 1);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	RestoreRegisters();
	ASMEnd(0x3fa08070); //lis r29, 0x8070
}

void EndMatch()
{
	ASMStart(0x806d4850);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;

	//clear in game flag
	SetRegister(reg1, 0);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	//free memory
#ifdef IASA_OVERLAY
	FreeIASABuffers();
#endif

#ifdef DI_DRAW
	FreeDIBuffer();
#endif
	

	RestoreRegisters();
	ASMEnd(0x7c7f1b78); //mr r31, r3
}

void Draw()
{
	ASMStart(0x8000e588);
	vector<int> FPRegs(21);
	iota(FPRegs.begin(), FPRegs.end(), 0);
	SaveRegisters(FPRegs);

	//draw di
#ifdef DI_DRAW
	DrawDI();
#endif

	//IASA overlay
#ifdef IASA_OVERLAY
	EndOverlay();
#endif

	LoadWordToReg(31, IS_DEBUG_PAUSED);
	SetRegister(30, LAST_DEBUG_STATE);
	STW(31, 30, 0);

	RestoreRegisters();
	ASMEnd(0x38210030); //addi sp, sp, 48
}