#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Miscellaneous Code.h"

void fixStanimaTextBug() {
	ASMStart(0x806a8278);

	LBZ(4, 3, 0x66E);
	If(4, GREATER_I, 10); {
		SetRegister(4, 0);
		STB(4, 3, 0x66E);
	} EndIf();

	ASMEnd();
}

void retainTechKB() {
	//[sp + 0x10] = pervious X KB
	//store at r31 + 8
	//use r29+
	ASMStart(0x8086d1c8);

	LWZ(30, 1, 0x10);
	STW(30, 31, 0x8);

	ASMEnd(0xd0010028);
}

void preventRollSlideOff() {
	//if in tech roll, set r3 to 0, else 1
	//r30 is module ptr
	//use r3, r30, r31
	//action = [[[r30 + 0xD8] + 0x70] + 0x34]
	ASMStart(0x8089dbd0);

	LWZ(3, 30, 0xD8);
	LWZ(3, 3, 0x70); //status module
	LWZ(3, 3, 0x34); //action

	If(3, EQUAL_I, 0x61); //tech roll
	{
		SetRegister(3, 0);
	} Else(); 
	{
		SetRegister(3, 1);
	} EndIf();

	ASMEnd();
}

void slipperyTechs() {
	retainTechKB();
	preventRollSlideOff();
}

void DBZModeTest() {
	//use r28 and up
	//r26 = kinetic module ptr
	//r27 = module ptr
	ASMStart(0x807c1a20);
	SaveRegisters(3);

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int controllerModuleReg = 27;
	int kineticModuleReg = 26;

	LWZ(controllerModuleReg, controllerModuleReg, 0xD8);
	LWZ(3, controllerModuleReg, 0x64); //work module
	SetRegister(4, 0x22000002);
	CallBrawlFunc(0x807acd44); //offFlag

	LWZ(controllerModuleReg, controllerModuleReg, 0x5C); //controller module



	LFS(1, controllerModuleReg, 0x38); //x control stick
	LWZ(reg1, kineticModuleReg, 0x64);
	LFS(2, reg1, 8); //x air speed
	FADDS(1, 1, 2);
	constrainFloat(1, 2, reg2, -2.0, 2.0);
	STFS(1, reg1, 8);



	LFS(1, controllerModuleReg, 0x3C); //y control stick
	LWZ(reg1, kineticModuleReg, 0x58);
	LFS(2, reg1, 0xC); //fallspeed
	FADDS(1, 1, 2);
	constrainFloat(1, 2, reg2, -2.0, 2.0);
	STFS(1, reg1, 0xC);


	

	RestoreRegisters();
	ASMEnd(0x39610030); //addi r11, sp, 48
}

void stickInputValueVariables() {
	ASMStart(0x809131a0);
	SaveRegisters();

	int reg = 31;
	int counterReg = 30;
	int variablePtrReg = 29;
	int inputPtrReg = 28;

	SetRegister(variablePtrReg, BASIC_VARIABLE_START_ADDRESS);
	SetRegister(inputPtrReg, PLAY_INPUT_LOC_START);
	CounterLoop(counterReg, 0, 4, 1);
	{
		LBA(reg, inputPtrReg, 2); //x val
		STW(reg, variablePtrReg, 0x2c8); //LA-Basic[178]
		LBA(reg, inputPtrReg, 3); //x val
		STW(reg, variablePtrReg, 0x2cc); //LA-Basic[179]

		ADDI(variablePtrReg, variablePtrReg, BASIC_VARIABLE_BLOCK_SIZE);
		ADDI(inputPtrReg, inputPtrReg, 4);
	} CounterLoopEnd();

	RestoreRegisters();
	ASMEnd(0x38840001); //addi r4, r4, 1
}

void CStickTiltFix() {
	stickInputValueVariables();
}

void musicPercentCode() {
	//r3 = music option object
	//r3 + 0x670 = stage id
	//r3 + 0x698 = song id
	//music object = [0x805a01d8]
	ASMStart(0x8117e698);
	SaveRegisters({1});

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int buttonsReg = 28;

	SetRegister(buttonsReg, 0);
	SetRegister(reg3, PLAY_BUTTON_LOC_START);
	CounterLoop(reg1, 0, 0x40 * 4, 0x40);
	{
		LWZX(reg2, reg3, reg1);
		OR(buttonsReg, buttonsReg, reg2);
	} CounterLoopEnd();

	ANDI(reg1, buttonsReg, BUTTON_L);
	If(reg1, NOT_EQUAL_I, 0);
	{
		SetRegister(6, 0);
		SetFloatingRegister(1, reg3, 100);
	} EndIf();

	ANDI(reg1, buttonsReg, BUTTON_R);
	If(reg1, NOT_EQUAL_I, 0);
	{
		SetRegister(6, 100);
		SetFloatingRegister(1, reg3, 0);
	} EndIf();


	ANDI(reg2, buttonsReg, BUTTON_L | BUTTON_R);
	If(reg2, NOT_EQUAL_I, 0);
	{
		MR(reg3, 3);

		LHZ(reg2, 3, 0x42); //index
		LWZ(4, 3, 0x670); //stage id
		RLWINM(reg2, reg2, 2, 0, 29); //offset
		ADD(3, 3, reg2);
		LWZ(5, 3, 0x698); //song id
		LoadWordToReg(3, 0x805a01d8); //music object
		
		CallBrawlFunc(0x80078cd8); //setRate

		MR(3, reg3);
		LHZ(4, reg3, 0x678); //anim index
		ADDI(4, 4, 92);
		SetRegister(5, 9);
		CallBrawlFunc(0x806a52d4); //setFrameAnim
	} EndIf();


	RestoreRegisters();
	ASMEnd(0x9421ff90); //stwu sp, -0x70 (sp)
}

void cstickTiltTest() {
	ASMStart(0x80764dd0);

	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;
	int reg8 = 24;
	int reg9 = 23;
	int reg10 = 22;

	SetRegister(reg1, 0x805bc05c);

	CounterLoop(reg2, 0, 4, 1);
	{
		MULLI(reg3, reg2, 4);
		ADD(reg3, reg3, reg1);

		LHZ(reg5, reg3, 0xC);
		ANDI(reg4, reg5, 0x120);
		If(reg4, EQUAL_I, 0x120);
		{
			XORI(reg5, reg5, 0x120);
			STH(reg5, reg3, 0xC);
			LBA(reg5, reg3, 0xE);
			LBA(reg6, reg3, 0xF);
			ABS(reg7, reg5, reg4);
			ABS(reg8, reg6, reg4);
			If(reg7, GREATER, reg8);
			{
				If(reg5, LESS_I, 0);
				{
					SetRegister(reg9, 0xC0);
					SetRegister(reg10, 0);
				} Else();
				{
					SetRegister(reg9, 0x40);
					SetRegister(reg10, 0);
				} EndIf();
			} Else();
			{
				If(reg6, LESS_I, 0);
				{
					SetRegister(reg9, 0);
					SetRegister(reg10, 0xC0);
				} Else();
				{
					SetRegister(reg9, 0);
					SetRegister(reg10, 0x40);
				} EndIf();
			} EndIf();
			STB(reg9, reg3, 0xE);
			STB(reg10, reg3, 0xF);
		} EndIf();
	} CounterLoopEnd();
	

	RestoreRegisters();
	ASMEnd(0x9421ffb0);
}

void DoubleFighterTest() {
	ASMStart(0x8081dfb0);
	//use r19 up
	//r27 has ptr
	LWZ(19, 27, 0x30);
	If(19, EQUAL_I, 0x22); //is Ike
	{
		STW(19, 27, 0x38);
		SetRegister(19, 0x02FF00FF);
		STW(19, 27, 8);
		SetRegister(19, 0x01CC0200);
		STW(19, 27, 0xC);
	}EndIf();

	ASMEnd(0x90030008); //stw r0, 0x8(r3)
}

/*
void UCF() {
	//r31 is module ptr
	ASMStart(0x8083a31c);
	SaveRegisters();
	int reg1 = 3;
	int reg2 = 4;
	int reg3 = 5;
	int reg4 = 6;
	int reg5 = 7;
	int reg6 = 8;

	//get and increment roll threshold timer
	LWZ(reg1, 28, 0x68);
	LHZ(reg3, reg1, 0x28);
	Increment(reg3);

	//check if stickX is enough to trigger roll input countdown
	LFS(0, reg1, 0x38); //X
	LFS(2, reg1, 0x48); //X, before match begins
	FADD(0, 0, 2); //only one of these will be non zero at once, so this is an option select
	FABS(0, 0);
	SetFloatingRegister(2, reg2, 0.275);
	FCMPU(0, 2, 0);
	BC(2, BRANCH_IF_TRUE, GT);
	SetRegister(reg3, 0);
	//end branch

	STH(reg3, reg1, 0x28); //update timer

	//change thresholds if timer > 4 and stickX held at rim
	SetRegister(reg5, 0);
	If(reg3, GREATER_I, 4);
	{
		SetFloatingRegister(2, reg2, 0.6);
		LFS(0, reg1, 0x38); //X
		FABS(0, 0);
		FCMPU(0, 2, 0);
		BC(2, BRANCH_IF_TRUE, LT);
		SetRegister(reg5, 1);
		//end branch
	} EndIf();

	//don't change if not shielding
	LWZ(reg4, 28, 0x7C);
	LWZ(reg2, reg4, 0x34);
	If(reg2, NOT_EQUAL_I, 0x1A);
	{
		If(reg2, NOT_EQUAL_I, 0x1B);
		{
			SetRegister(reg5, 0);
		}EndIf();
	}EndIf();

	//if all tests pass, change thresholds
	If(reg5, EQUAL_I, 1);
	{
		SetRegister(reg2, GetHexFromFloat(-.8));
		SetRegister(reg3, 0x80B882EC);
		STW(reg2, reg3, 0);
		SetRegister(reg2, GetHexFromFloat(-.675));
		SetRegister(reg3, 0x80B88388);
		STW(reg2, reg3, 0);
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x7f43d378); //mr r3, r26

	ASMStart(0x8083a324);
	SaveRegisters();

	SetRegister(reg1, 0x80B882EC);
	SetRegister(reg2, GetHexFromFloat(-.75));
	STW(reg2, reg1, 0);

	SetRegister(reg1, GetHexFromFloat(-0.71));
	SetRegister(reg2, 0x80B88388);
	STW(reg1, reg2, 0);

	RestoreRegisters();
	ASMEnd(0x3c6080b8); //lis r3, 0x80B8
}*/

void getStickMagnitide(int stickBaseReg, int resultFReg = 2) {
	int stickXReg = 0;
	int stickYReg = 1;

	SetFloatingRegister(resultFReg, 3, 0.0125);

	LFS(stickXReg, stickBaseReg, 0x38); //X
	FABS(stickXReg, stickXReg);
	FADD(stickXReg, stickXReg, resultFReg);
	FMUL(stickXReg, stickXReg, stickXReg);

	LFS(stickYReg, stickBaseReg, 0x3C); //Y
	FABS(stickYReg, stickYReg);
	FADD(stickYReg, stickYReg, resultFReg);
	FMUL(stickYReg, stickYReg, stickYReg);

	FADD(resultFReg, stickXReg, stickYReg);

	FRSQRTE(resultFReg, resultFReg);
	SetFloatingRegister(stickXReg, 3, 1);
	FDIV(resultFReg, stickXReg, resultFReg);

	//FSQRT(resultFReg, resultFReg);
}

void UCF() {
	//r31 is module ptr
	ASMStart(0x8083a31c);
	SaveRegisters(4);
	int reg1 = 3;
	int reg2 = 4;
	int reg3 = 5;
	int reg4 = 6;
	int reg5 = 7;
	int reg6 = 8;

	//get and increment roll threshold timer
	LWZ(reg1, 28, 0x68);
	LHZ(reg3, reg1, 0x28);
	Increment(reg3);

	//check if stickX is enough to trigger roll input countdown
	LFS(0, reg1, 0x38); //X
	LFS(2, reg1, 0x48); //X, before match begins
	FADD(0, 0, 2); //only one of these will be non zero at once, so this is an option select
	FABS(0, 0);
	SetFloatingRegister(2, reg2, 0.275);
	FCMPU(0, 2, 0);
	BC(2, BRANCH_IF_TRUE, GT);
	SetRegister(reg3, 0);
	//end branch

	STH(reg3, reg1, 0x28); //update timer

	//change thresholds if timer > 4 and stickX held at rim
	SetRegister(reg5, 0);
	If(reg3, GREATER_I, 4);
	{
		getStickMagnitide(reg1, 2);
		SetFloatingRegister(3, 31, 1.001);

		FCMPU(2, 3, 0);
		BC(2, BRANCH_IF_TRUE, LT);
		SetRegister(reg5, 1);
		//end branch
	} EndIf();

	//don't change if not shielding
	LWZ(reg4, 28, 0x7C);
	LWZ(reg2, reg4, 0x34);
	If(reg2, NOT_EQUAL_I, 0x1A);
	{
		If(reg2, NOT_EQUAL_I, 0x1B);
		{
			SetRegister(reg5, 0);
		}EndIf();
	}EndIf();

	//if all tests pass, change thresholds
	If(reg5, EQUAL_I, 1);
	{
		SetRegister(reg2, GetHexFromFloat(-.8));
		SetRegister(reg3, 0x80B882EC);
		STW(reg2, reg3, 0);
		SetRegister(reg2, GetHexFromFloat(-.675));
		SetRegister(reg3, 0x80B88388);
		STW(reg2, reg3, 0);
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x7f43d378); //mr r3, r26

	ASMStart(0x8083a324);
	SaveRegisters();

	SetRegister(reg1, 0x80B882EC);
	SetRegister(reg2, GetHexFromFloat(-.75));
	STW(reg2, reg1, 0);

	SetRegister(reg1, GetHexFromFloat(-0.71));
	SetRegister(reg2, 0x80B88388);
	STW(reg1, reg2, 0);

	RestoreRegisters();
	ASMEnd(0x3c6080b8); //lis r3, 0x80B8
}

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