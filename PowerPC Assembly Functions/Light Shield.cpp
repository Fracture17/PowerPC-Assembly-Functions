#include "stdafx.h"
#include "Light Shield.h"

//uses volitile regs, returns value in f1
void getAnalogShieldValue(int modulePtrReg) {
	/*LWZ(3, modulePtrReg, 0xD8);
	LWZ(3, 3, 0x64); //work module
	SetRegister(4, 0x1100004e); //LA-Float[78]
	CallBrawlFunc(GET_FLOAT_WORK_MODULE);*/

	MR(8, modulePtrReg);

	LWZ(3, 8, 0xD8);
	LWZ(3, 3, 0x64); //work module
	SetRegister(4, 0x11000024);
	CallBrawlFunc(GET_FLOAT_WORK_MODULE);
	FMR(0, 1);

	LWZ(3, 8, 0xD8);
	LWZ(3, 3, 0x64); //work module
	SetRegister(4, 0x11000025);
	CallBrawlFunc(GET_FLOAT_WORK_MODULE);

	//compare f0 to f1, if f0 is greater, move to f1
	FCMPU(0, 1, 0);
	BC(2, BRANCH_IF_TRUE, LT);
	FMR(1, 0);

	//make sure value is greater than 0.3
	SetFloatingRegister(0, 3, 0.31);
	FCMPU(0, 1, 0);
	BC(2, BRANCH_IF_TRUE, LT);
	FMR(1, 0);

	//SetFloatingRegister(1, 3, 1);
}

void lightShield() {
	shieldScale();
	shieldDegeneration();
	shieldDamage();
	shieldStun();
	shieldDefenderPushback();
}

void shieldScale() {
	const float SCALE = .5;
	//f1 is default scale multiplier
	//f1 needs to equal new multiplier

	//newScale = 1 + (1 - (A - .3) / .7) * SCALE

	int modulePtrReg = 30;

	ASMStart(0x80874ca0);
	SaveRegisters(2);

	getAnalogShieldValue(modulePtrReg);

	SetFloatingRegister(0, 3, 0.3);
	FSUBS(1, 1, 0);

	SetFloatingRegister(0, 3, .7);
	FDIVS(1, 1, 0);

	SetFloatingRegister(0, 3, 1);
	FSUBS(1, 0, 1);

	SetFloatingRegister(0, 3, SCALE);
	FMULS(1, 1, 0);

	SetFloatingRegister(0, 3, 1);
	FADDS(1, 1, 0);

	FMULS(31, 31, 1);

	RestoreRegisters();
	ASMEnd();
}

void shieldDegeneration() {
	//f31 needs to equal degeneration

	int modulePtrReg = 31;

	ASMStart(0x807ad4dc);
	SaveRegisters(2);

	getAnalogShieldValue(modulePtrReg);

	SetFloatingRegister(0, 3, 0.3);
	FSUBS(31, 1, 0);

	SetFloatingRegister(0, 3, .7);
	FDIVS(31, 31, 0);

	SetFloatingRegister(0, 3, .266);
	FMULS(31, 31, 0);

	SetFloatingRegister(0, 3, .014);
	FADDS(31, 31, 0);
	
	RestoreRegisters();
	ASMEnd(0x819c0000); //lwz r12, 0, r28
}

void shieldDamage() {
	//need to add analog effect to f1

	int modulePtrReg = 3;

	ASMStart(0x80840540);
	SaveRegisters({0,2});

	FMR(2, 1);
	getAnalogShieldValue(modulePtrReg);

	SetFloatingRegister(0, 3, 0.3);
	FSUBS(1, 1, 0);

	SetFloatingRegister(0, 3, .7);
	FDIVS(1, 1, 0);

	SetFloatingRegister(0, 3, 1);
	FSUBS(1, 0, 1);

	SetFloatingRegister(0, 3, .2);
	FMULS(0, 1, 0);

	FADDS(1, 0, 2);

	RestoreRegisters();

	ASMEnd(0xec000072); //fmuls f0, f0, f1
}

void shieldStun() {
	//(f31 - 0.1) * 2.85714 = damage

	int modulePtrReg = 31;
	int stunResultReg = 28;

	ASMStart(0x80875400);
	SaveRegisters(3);

	getAnalogShieldValue(modulePtrReg);

	//Analog effect
	SetFloatingRegister(0, 3, 0.3);
	FSUBS(1, 1, 0);

	SetFloatingRegister(0, 3, .7);
	FDIVS(1, 1, 0);

	SetFloatingRegister(0, 3, 1);
	FSUBS(1, 0, 1);

	SetFloatingRegister(0, 3, .65);
	FMULS(1, 1, 0);
	/////////

	//Damage
	SetFloatingRegister(0, 3, .1);
	FSUBS(0, 31, 0);

	SetFloatingRegister(2, 3, 2.85714);
	FMULS(2, 2, 0);
	/////////



	SetFloatingRegister(0, 3, .3);
	FADDS(1, 1, 0);

	FMULS(1, 1, 2); //times damage

	SetFloatingRegister(0, 3, 1.5);
	FMULS(1, 1, 0);

	ConvertFloatingRegToInt(1, stunResultReg);

	ADDI(stunResultReg, stunResultReg, 4);
	SetRegister(3, STRING_BUFFER);
	STW(stunResultReg, 3, 0);

	RestoreRegisters();
	LoadWordToReg(stunResultReg, STRING_BUFFER);
	ASMEnd(0x7f84e378); //mr r4, r28
}

void shieldDefenderPushback() {
	//(f30 * 5 - .1) / .35 is effective damage
	//f1 is direction (1 or -1)
	//f1 needs to equal new shield pushback

	int modulePtrReg = 31;

	ASMStart(0x808755ac);
	SaveRegisters({0, 2, 3});

	FMR(3, 1);

	getAnalogShieldValue(modulePtrReg);

	//analog effect
	SetFloatingRegister(0, 3, 0.3);
	FSUBS(1, 1, 0);

	SetFloatingRegister(0, 3, .7);
	FDIVS(1, 1, 0);

	SetFloatingRegister(0, 3, 1);
	FSUBS(1, 0, 1);

	SetFloatingRegister(0, 3, .195);
	FMULS(1, 1, 0);
	/////////////


	//Effective Damage
	SetFloatingRegister(0, 3, 5);
	FMULS(2, 30, 0);

	SetFloatingRegister(0, 3, .1);
	FSUBS(2, 2, 0);

	SetFloatingRegister(0, 3, .35);
	FDIVS(2, 2, 0);
	//////////////


	SetFloatingRegister(0, 3, .054);
	FADDS(1, 1, 0);

	FMULS(1, 1, 2);

	SetFloatingRegister(0, 3, .24);
	FADDS(1, 1, 0);

	//cap at 2
	SetFloatingRegister(0, 3, 2);
	FCMPU(0, 1, 0);
	BC(2, BRANCH_IF_TRUE, GT);
	FMR(1, 0);

	FMULS(1, 1, 3); //direction

	RestoreRegisters();

	ASMEnd();
	//ASMEnd(0xd001000c); //stfs f0, 0xC (sp)
}

void shieldAttackerPushback() {
	//[r31 + 0x130] = damage
	//f1 needs to equal pushback

	int modulePtrReg = 29;

	ASMStart(0x808401f0);
	SaveRegisters({0});

	getAnalogShieldValue(modulePtrReg);

	//analog effect
	SetFloatingRegister(0, 3, 0.3);
	FSUBS(1, 1, 0);

	SetFloatingRegister(0, 3, .1);
	FMULS(1, 1, 0);
	/////////////

	LFS(0, 31, 0x130); //Damage

	FMULS(1, 1, 0);

	SetFloatingRegister(0, 3, 0.02);
	FADDS(1, 1, 0);

	RestoreRegisters();
	ASMEnd();
}

void setTriggerValue() {
	
}