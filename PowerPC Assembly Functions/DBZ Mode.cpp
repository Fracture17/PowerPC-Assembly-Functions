#include "stdafx.h"
#include "DBZ Mode.h"

void DBZMode() {
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

	LoadWordToReg(reg1, DBZ_MODE_INDEX + Line::VALUE);
	If(reg1, EQUAL_I, 1); {
		LWZ(controllerModuleReg, controllerModuleReg, 0xD8);
		LWZ(3, controllerModuleReg, 0x64); //work module
		SetRegister(4, 0x22000002);
		CallBrawlFunc(0x807acd44); //offFlag

		LWZ(controllerModuleReg, controllerModuleReg, 0x5C); //controller module

		LFS(1, controllerModuleReg, 0x38); //x control stick
		LWZ(reg1, kineticModuleReg, 0x64);
		LFS(2, reg1, 8); //x air speed
		SetRegister(reg2, DBZ_MODE_ACCEL_X_INDEX);
		LFS(0, reg2, Line::VALUE);
		FMULS(1, 1, 0);
		FADDS(1, 1, 2);

		SetRegister(reg2, DBZ_MODE_MAX_SPEED_X_INDEX);
		LFS(2, reg2, Line::VALUE);
		FNEG(0, 2);
		constrainFloatDynamic(1, 0, 2);
		STFS(1, reg1, 8);


		LFS(1, controllerModuleReg, 0x3C); //y control stick
		LWZ(reg1, kineticModuleReg, 0x58);
		LFS(2, reg1, 0xC); //fallspeed
		SetRegister(reg2, DBZ_MODE_ACCEL_Y_INDEX);
		LFS(0, reg2, Line::VALUE);
		FMULS(1, 1, 0);
		FADDS(1, 1, 2);

		SetRegister(reg2, DBZ_MODE_MAX_SPEED_Y_INDEX);
		LFS(2, reg2, Line::VALUE);
		FNEG(0, 2);
		constrainFloatDynamic(1, 0, 2);
		STFS(1, reg1, 0xC);
	} EndIf();


	RestoreRegisters();
	ASMEnd(0x39610030); //addi r11, sp, 48

	forceDeathOffTop();
}

void forceDeathOffTop() {
	//set r3 to checkDeadArea if DBZ mode active, checkDead returned -1, not dying, and not respawning
	//r31 is module ptr
	ASMStart(0x8083ade0);

	int EndLabel = GetNextLabel();

	LoadWordToReg(4, DBZ_MODE_INDEX + Line::VALUE);
	If(4, EQUAL_I, 1); { //in dbz mode
		If(3, EQUAL_I, -1); { //checkDead returned -1
			LWZ(3, 31, 0x64 + 0xC); //work module
			SetRegister(4, 0x12000001);
			CallBrawlFunc(0x807accdc); //isFlag/WorkModule
			If(3, EQUAL_I, 0); { //not descending on angel platform
				LWZ(3, 31, 0x14 + 0xC); //situation module
				CallBrawlFunc(0x80740628); //getKind/SituationModule
				If(3, NOT_EQUAL_I, 5); { //not dying
					SetRegister(3, STRING_BUFFER);
					LWZ(4, 31, 0xC + 0xC);
					CallBrawlFunc(0x8072fa9c); //getPos/PostureModule

					SetFloatingRegister(1, 5, 0.0);
					FMR(2, 1);
					CallBrawlFunc(0x8073b8dc); //checkDeadArea
					//r3 is the new result
					JumpToLabel(EndLabel);
				} EndIf();
			} EndIf();
			//if shouldn't trigger death
			SetRegister(3, -1);
		} EndIf();
	} EndIf();

	Label(EndLabel);

	ASMEnd(0x2c03ffff); //cmpwi r3, -1
}