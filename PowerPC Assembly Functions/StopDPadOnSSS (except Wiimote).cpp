#include "stdafx.h"
#include "StopDPadOnSSS (except Wiimote).h"

void StopDPadOnSSS()
{
	CheckIfOnSSS();

	StopDPad();

	FixDPadASL();
}

void CheckIfOnSSS()
{
	ASMStart(0x806b30fc);

	int Reg1 = 31;
	int Reg2 = 30;


	SetRegister(Reg1, 1);
	SetRegister(Reg2, IN_STAGE_SELECT_MENU_FLAG);
	STB(Reg1, Reg2, 0);

	SetRegister(31, 0x806C0000); //reset r31
	WriteIntToFile(0x3bff9018); //subi r31, r31, 28648
	ASMEnd(0x7c7e1b78); //mr r30, r3
}

void StopDPad()
{
	ASMStart(0x80029ed8);
	WriteIntToFile(0xcb1f0080); //lfd f24, r31, 0x80

	//SaveRegs({ 31, 30, 29, 28 });

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 27;
	int Reg6 = 26;

	SetRegister(Reg2, IN_STAGE_SELECT_MENU_FLAG);
	LBZ(Reg1, Reg2, 0);
	If(Reg1, EQUAL_I, 1); //in SSS

	SetRegister(Reg1, 0);
	STB(Reg1, Reg2, 0);

	SetRegister(Reg6, 0);
	SetRegister(Reg1, PLAY_BUTTON_LOC_START - 0x40 + 2);
	SetRegister(Reg2, 0);
	While(Reg2, LESS_I, 4); //clear DPad for GCC

	LHZU(Reg3, Reg1, 0x40);
	ANDI(Reg4, Reg3, 0xF);
	OR(Reg6, Reg6, Reg4);
	ANDI(Reg3, Reg3, 0xFFF0);
	STH(Reg3, Reg1, 0);

	ADDI(Reg2, Reg2, 1);
	EndWhile(); //clear DPad for GCC

	SetRegister(Reg5, WII_BASED_CONTROLLER_START + WII_BASED_CONTROLLER_TYPE_OFFSET - WII_BASED_CONTROLLER_PORT_OFFSET);
	While(Reg2, LESS_I, 8); //clear Wiimote-based controllers

	LHZU(Reg3, Reg1, 0x40);
	LBZU(Reg4, Reg5, WII_BASED_CONTROLLER_PORT_OFFSET);
	If(Reg4, NOT_EQUAL_I, 0); //not base wiimote

	ANDI(Reg4, Reg3, 0xF);
	OR(Reg6, Reg6, Reg4);
	ANDI(Reg3, Reg3, 0xFFF0);
	STH(Reg3, Reg1, 0);

	EndIf(); //not base wiimote

	ADDI(Reg2, Reg2, 1);
	EndWhile(); //clear Wiimote-based controllers

	SetRegister(Reg1, DISABLE_DPAD_ASL_STORAGE);
	STB(Reg6, Reg1, 0);

	EndIf(); //in SSS

	//RestoreRegs();

	//ASMEnd(0x83e1003c);
	ASMEnd();
}

void FixDPadASL()
{
	ASMStart(0x8094a168);
	
	int Reg1 = 4; //can use without saving
	int Reg2 = 31;
	int Reg3 = 30;
	SaveRegisters();

	LoadByteToReg(Reg1, DISABLE_DPAD_ASL_STORAGE);
	SetRegister(Reg2, ALT_STAGE_VAL_LOC);
	LHZ(Reg3, Reg2, 0);
	OR(Reg1, Reg1, Reg3);
	STH(Reg1, Reg2, 0);

	RestoreRegisters();
	ASMEnd(0x809d0038); //lwz r4, r29, 0x38
}

void ConvertButtons()
{
	GeckoStringWrite("\x2\x3\x1\x0\xC\x1F\x1F\x1F\x8\x9\x6\x5\x4\x1F\x1F\x1F", WIIMOTE_CONVERSION_TABLE);
	GeckoStringWrite("\x0\x1\x2\x3\xC\x1F\x1F\x1F\xA\x5\x9\x8\x4\x6\xB\x1F", WIICHUCK_CONVERSION_TABLE);
	GeckoStringWrite("\x3\x0\x4\xA\x8\xB\x9\x4\x1F\x5\xC\x1F\x1F\x6\x2\x1", CLASSIC_CONVERSION_TABLE);

	ConvertedWiimoteASL();

	ASMStart(0x80029ed4);
	//don't use r31
	int Reg1 = 23;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 27;
	int Reg6 = 26;
	int Reg7 = 25;
	int Reg8 = 24;

	SetRegister(Reg1, PLAY_BUTTON_LOC_START - 0x40 + 2);
	SetRegister(Reg2, GCC_BUTTON_STORAGE_LOC - 2);
	SetRegister(Reg3, 0);
	While(Reg3, LESS_I, 4); //copy GCC buttons

	LHZU(Reg4, Reg1, 0x40);
	STHU(Reg4, Reg2, 2);
	
	ADDI(Reg3, Reg3, 1);
	EndWhile(); //copy GCC buttons

	//reg2 == WIIMOTE button loc (-2)
	SetRegister(Reg1, WII_BASED_CONTROLLER_START - WII_BASED_CONTROLLER_PORT_OFFSET);
	SetRegister(Reg3, 1);
	SetRegister(Reg4, 0);
	While(Reg4, LESS_I, 4); //Wii conversion loop

	//reg8 has converted buttons, reg4 is loop counter, reg3 is 1, reg1 is wiimote location, reg2 is storage loc
	//reg5 has wiimote buttons
	SetRegister(Reg8, 0);
	LHZU(Reg5, Reg1, WII_BASED_CONTROLLER_PORT_OFFSET); //get buttons
	SetRegister(Reg6, WIIMOTE_CONVERSION_TABLE);
	LBZ(Reg7, Reg1, WII_BASED_CONTROLLER_TYPE_OFFSET); //get type
	If(Reg7, EQUAL_I, 2); //is classic
	LHZ(Reg5, Reg1, WII_BASED_CONTROLLER_TYPE_OFFSET + 2); //buttons are in different place
	EndIf(); //is classic
	MULLI(Reg7, Reg7, 16);
	ADD(Reg6, Reg6, Reg7); //get proper conversion table

	While(Reg5, GREATER_I, 0); //convert buttons

	ANDI(Reg7, Reg5, 1);
	If(Reg7, NOT_EQUAL_I, 0); //button pressed

	LBZ(Reg7, Reg6, 0);
	RLWNM(Reg7, Reg3, Reg7, 15, 31);
	OR(Reg8, Reg8, Reg7);

	EndIf(); //button pressed

	ADDI(Reg6, Reg6, 1);
	RLWINM(Reg5, Reg5, 31, 15, 31);
	EndWhile(); //convert buttons

	STHU(Reg8, Reg2, 2);

	ADDI(Reg4, Reg4, 1);
	EndWhile(); //Wii conversion loop

	ASMEnd(0x396a0080); //addi r11, r10, 128
}

void ConvertedWiimoteASL()
{
	ASMStart(0x800b9e9c);

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;

	SaveRegisters();

	SetRegister(Reg1, GCC_BUTTON_STORAGE_LOC - 2);
	SetRegister(Reg2, 0);
	SetRegister(Reg3, 0);
	SetRegister(Reg4, 0);
	While(Reg2, LESS_I, 8); //union loop

	LHZU(Reg3, Reg1, 2);
	OR(Reg4, Reg4, Reg3);

	ADDI(Reg2, Reg2, 1);
	EndWhile(); //union loop

	SetRegister(Reg1, ALT_STAGE_VAL_LOC);
	STH(Reg4, Reg1, 0);

	RestoreRegisters();
	ASMEnd(0x909e0014); //stw r4, r30, 0x14
}