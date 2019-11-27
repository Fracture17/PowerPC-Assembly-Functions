#include "stdafx.h"
#include "IkeClimbers.h"

void getInput() {
	ASMStart(0x8083ae38);

	LWZ(4, 3, 0x110); //fighter id
	If(4, EQUAL_I, 0x22); { //Ike
		//LWZ(4, 3, 0x18);
		ANDI(4, 3, 0xFFFF);
		//If(4, NOT_EQUAL_I, 0); { //is owner
		If(4, NOT_EQUAL_I, 0x4a60); {
			SetRegister(4, 0x80815a38); //getInput
		} Else(); {
			SetRegister(4, 0x80815a64); //getSubInput
		} EndIf();

		MTCTR(4);
		MR(4, 3);
		LoadWordToReg(3, 0x80B87C28);
		LWZ(4, 4, 0x10C);
		BCTR();
	} EndIf();

	ASMEnd(0x7c641b78); //mr r4, r3
}

void getOwner() {
	ASMStart(0x8083ae24);

	LWZ(4, 3, 0x110); //fighter id
	If(4, EQUAL_I, 0x22); { //Ike
		//LWZ(4, 3, 0x18);
		ANDI(4, 3, 0xFFFF);
		//If(4, NOT_EQUAL_I, 0); { //is owner
		If(4, NOT_EQUAL_I, 0x4a60); {
			SetRegister(4, 0x808159e4); //getOwner
		} Else(); {
			SetRegister(4, 0x80815a0c); //getSubOwner
		} EndIf();
		MTCTR(4);
		MR(4, 3);
		LoadWordToReg(3, 0x80B87C28);
		LWZ(4, 4, 0x10C);
		BCTR();
	} EndIf();

	ASMEnd(0x7c641b78); //mr r4, r3
}

void stopImageLoadCrash() {
	ASMStart(0x80014af8);

	If(4, EQUAL_I, 0); {
		BLR();
	} EndIf();

	ASMEnd(0x9421ffe0); //stwu sp, -0x20 (sp)
}

void stopParamCrash() {
	ASMStart(0x8084fee0);

	If(4, EQUAL_I, 0xF); {
		SetRegister(4, 0x22);
	} EndIf();

	ASMEnd(0x9421fff0); //stwu sp, -0x10 (sp)
}

void setData() {
	stopImageLoadCrash();
	//stopParamCrash();

	//use r27+
	ASMStart(0x8082e7a0);

	int reg1 = 27;
	int reg2 = 28;

	//Load 2
	SetRegister(reg1, 0x806232f0);
	SetRegister(reg2, 2);

	STB(reg2, reg1, 8);
	STB(reg2, reg1, 0xE);

	SetRegister(reg2, 0x22);
	STW(reg2, reg1, 0x38);

	//sub death isn't true death
	SetRegister(reg1, 0x80AD8B98);
	SetRegister(reg2, 0x1E);
	STB(reg2, reg1, 0);

	//input
	SetRegister(reg1, 0x806232f0);
	SetRegister(reg2, 0x10);
	STW(reg2, reg1, 0x5C);

	ASMEnd(0x88c30000); //lbz r6, 0, (r3)

	Gecko32BitWrite(0x80621f78, 0x22);
	Gecko32BitWrite(0x80903670, 0x4C401382);

	Gecko32BitWrite(0x80918cd4, 0x60000000);
	Gecko32BitWrite(0x80918208, 0x60000000);
}

void IkeClimbers() {
	getInput();

	getOwner();
	
	setData();
}