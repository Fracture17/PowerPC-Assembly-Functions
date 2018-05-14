#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Miscellaneous Code.h"

void LXPGreenOverlayFix() {
	ASMStart(0x806d4858);

	int reg1 = 3;
	int reg2 = 4;
	int x = 0;

	SetRegister(reg1, 0x90180FC0 + 2);
	SetRegister(reg2, 0);
	for (int i = 0; i < 4; i++) {
		STH(reg2, reg1, x); x += 0x5C;
	}

	ASMEnd(0x7fe3fb78); //mr r3, r31
}

void FixStickyRAlts() {
	//can use r0, r3, r4, r5
	ASMStart(0x806cbfa8);

	int reg1 = 3;
	int reg2 = 4;
	int reg3 = 5;

	SetRegister(reg2, 0x43AD8 - 0x5C);
	LoadWordToReg(reg1, 0x805A00E0);
	LWZ(reg1, reg1, 0x10);
	ADD(reg1, reg1, reg2);
	SetRegister(reg2, 0);
	CounterLoop(reg3, 0, 4, 1); {
		STWU(reg2, reg1, 0x5C);
	}CounterLoopEnd();

	ASMEnd(0x38000005); //li r0, 5
}

void CStickSlowFix()
{
	ASMStart(0x80048b70);

	LoadWordToReg(6, 0x80B84EB0);
	If(6, NOT_EQUAL_I, 0); {
		SetRegister(7, 0);
		LBZ(0, 6, 0x48);
		If(0, NOT_EQUAL_I, 0); {
			LWZ(0, 6, 0x40);
			If(0, NOT_EQUAL_I, 0); {
				SetRegister(7, 1);
			}EndIf();
		}EndIf();

		LoadByteToReg(6, IS_DEBUG_PAUSED + 3);
		If(6, NOT_EQUAL_I, 0); {
			SetRegister(7, 1);
		}EndIf();

		SUBF(3, 3, 7);
	}EndIf();

	ASMEnd(0x5460063e);
}

void FixTr4shTeamToggle()
{
	//logic
	//use r3, r12, r4
	//r31 has menu ptr?
	ASMStart(0x8068a494);
	//sets S4 toggle
	//LoadWordToReg(3, 0x805A00E0);
	//LWZ(3, 3, 0x10);
	LBZ(4, 31, 0x5CB); //S4 toggle
	LBZ(12, 31, 0x5C8); //team flag
	/*If(12, EQUAL_I, 0); {
		SetRegister(4, 0);
	}EndIf();*/
	XOR(4, 4, 12);
	STB(4, 31, 0x5CB);

	ASMEnd(0x7fe3fb78); //mr r3, r31


	//save flag
	//use r4, r5, r0
	//r3 has menu ptr
	ASMStart(0x806846e0);

	LBZ(4, 3, 0x5CB);
	LoadWordToReg(5, 0x805A00E0);
	LWZ(5, 5, 0x10);
	STB(4, 5, 0x30); //S4 toggle

	ASMEnd(0x9421ff10); //stwu sp, sp, -0xF0


	//clear flag
	//r3 has menu ptr
	//use r4, r5
	ASMStart(0x80684480);

	LoadWordToReg(4, 0x805A00E0);
	LWZ(4, 4, 0x10);
	LBZ(4, 4, 0x30); //get flag
	STB(4, 3, 0x5CB); //set flag

	ASMEnd(0x9421ffd0); //stwu sp, sp, -0x30



	//fix toggle
	//use r4, r3, r5
	ASMStart(0x80686c04);

	LoadWordToReg(3, 0x805A00E0);
	LWZ(3, 3, 0x10);
	LBZ(4, 3, 0x30); //S4 toggle
	LBZ(5, 3, 0x33); //team flag

	SetRegister(3, 0x9018a0e4); //pyotr state mem
	STB(4, 3, 1);
	XOR(4, 4, 5);
	STB(4, 3, 0);

	ASMEnd(0x889d05c8); //lbz r4, r29, 0x5C8
}

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