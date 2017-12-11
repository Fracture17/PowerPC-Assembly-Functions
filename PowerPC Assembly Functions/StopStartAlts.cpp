#include "stdafx.h"
#include "StopStartAlts.h"

void StopStartAltFunctions()
{
	CheckIfSelectingStage();

	NegateRButton();

	//ChangeRToStart();
}

void CheckIfSelectingStage()
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

//needs CheckIfSelectingStage
void NegateRButton()
{
	ASMStart(0x8002973c);

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 3;
	int Reg4 = 4;
	int Reg5 = 5;

	//check if in stage selection menu
	LoadByteToReg(Reg1, IN_STAGE_SELECT_MENU_FLAG);
	If(Reg1, EQUAL_I, 1); //is in menu
	SetRegister(Reg1, PLAY_BUTTON_LOC_START - 0x40 + 2);
	SetRegister(Reg2, 0);
	SetRegister(Reg3, 0);
	While(Reg3, LESS_I, 4); //loop through players

	LHZ(Reg4, Reg1, 0x40);
	ANDI(Reg5, Reg4, ~0x20);
	If(Reg4, NOT_EQUAL, Reg5); //R is pressed
	SetRegister(Reg2, 0x20);
	EndIf();

	STHU(Reg5, Reg1, 0x40); //negate possible R input
	ADDI(Reg3, Reg3, 1);

	EndWhile(); //loop through players

	SetRegister(Reg3, ALT_STAGE_VAL_LOC);
	LHZ(Reg4, Reg3, 0);
	OR(Reg2, Reg2, Reg4);
	STH(Reg2, Reg3, 0);

	SetRegister(Reg1, IN_STAGE_SELECT_MENU_FLAG);
	SetRegister(Reg2, 0);
	STB(Reg2, Reg1, 0); //clear flag

	EndIf(); //is in menu

	ASMEnd(0x83e1003c); //lwz r31, r1, 0x3C
}

void ChangeRToStart()
{
	//eliminate start input from alt stage value
	ASMStart(0x8094a168);

	int Reg1 = 3;
	int Reg2 = 4;
	int Reg3 = 5;
	int Reg4 = 6;
	int Reg5 = 7;
	int Reg6 = 8;


	SetRegister(Reg2, ALT_STAGE_VAL_LOC);
	LHZ(Reg1, Reg2, 0);
	ANDI(Reg3, Reg1, 0x1040); //check for start + L
	If(Reg3, EQUAL_I, 0x1040); //both start and L are pressed
	ANDI(Reg1, Reg1, ~0x40); //negate L
	Else(); //both start and L are pressed
	ANDI(Reg1, Reg1, ~0x1000); //negate possible start input
	EndIf(); //both start and L are pressed
	STH(Reg1, Reg2, 0);

	/*
	SetRegister(Reg2, ALT_STAGE_VAL_LOC);
	LHZ(Reg1, Reg2, 0);
	ANDI(Reg1, Reg1, ~0x1000);
	ANDI(Reg3, Reg1, 0x20);
	If(Reg3, NOT_EQUAL_I, 0); //R is pressed
	XORI(Reg1, Reg1, 0x1020); //make start input and negate R input
	EndIf(); //R is pressed
	STH(Reg1, Reg2, 0);
	*/
	

	ASMEnd(0x809d0038); //lwz r4, r29, 0x38
}