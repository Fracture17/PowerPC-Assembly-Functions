#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Miscellaneous Code.h"

void StopPokemonTrainerSwitch()
{
	ASMStart(0x80a8c390);

	If(4, EQUAL_I, 0x123); //down B switch
	SetRegister(4, 0x120);
	EndIf(); //down B switch

	ASMEnd(0x9421ff90); //stwu sp, sp, -0x70
}

void SetKappaItemFlag()
{
	//can use r5, r6, r7, r8
	ASMStart(0x8094a170);

	SetRegister(6, KAPPA_ITEM_FLAG);
	LoadHalfToReg(5, ALT_STAGE_VAL_LOC);
	If(5, EQUAL_I, 0xD00); //X + Y + A

	SetRegister(5, 1);

	Else(); //X + Y + A

	SetRegister(5, 0);

	EndIf(); //X + Y + A

	STW(5, 6, 0);

	ASMEnd(0x80bf0088); //lwz r5, r31, 0x88
}

void SetTeamAttackTraining()
{
	//r29 + 8 + 2 has team attack byte
	//can use r3, r5
	ASMStart(0x806f1984);

	LBZ(3, 29, 10);
	ORI(3, 3, 1); //turn on team attack
	STB(3, 29, 10);

	//WTF Brawl, why do you overwrite this value 2 instructions later?
	//ASMEnd(0x881d0008); //lbz r0, r29, 8
	ASMEnd();
}

void ItemSpawnControl()
{
	SetKappaItemFlag();

	//r3 + 0x64 is item spawn frame
	ASMStart(0x80951f3c);

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	//SaveRegs({ Reg1, Reg2, Reg3 });

	/*LoadHalfToReg(Reg1, PLAY_BUTTON_LOC_START + 2);
	ANDI(Reg1, Reg1, 0xF); //get DPad
	SetRegister(Reg2, 0x42700000); //60.0
	If(Reg1, EQUAL_I, 1); //DLeft
	SetRegister(Reg2, 0x41f00000); //30.0
	EndIf(); //DLeft
	If(Reg1, GREATER_I, 1); //DRight
	SetRegister(Reg2, 0x41700000); //15.0
	EndIf(); //DRight
	If(Reg1, GREATER_I, 2); //DDown
	SetRegister(Reg2, 0x41200000); //10.0
	EndIf(); //DDown
	If(Reg1, GREATER_I, 4); //DUp
	SetRegister(Reg2, 0x40a00000); //5.0
	EndIf(); //DUp*/

	LoadWordToReg(Reg1, KAPPA_ITEM_FLAG);
	If(Reg1, EQUAL_I, 1); //is kappa
	SetRegister(Reg1, 0x42700000); //60.0
	STW(Reg1, 3, 0x64);
	EndIf(); //is kappa

	//RestoreRegs();
	ASMEnd(0x3fe080ae); //lis r31, 0x80AE
}