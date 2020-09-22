#include "stdafx.h"
#include "C++Injection.h"

const int CODE_ADDRESS = 0x8057a800;
const int DATA_ADDRESS = 0x8057d000;
const int INITIALIZER_ADDRESS = 0x80d00000;
//const int DATA_ADDRESS = 0x804E2200;

void loadCppCodes() {
	ASMStart(0x8002d4f8);
	SaveRegisters();

	LoadFile("codes.bin", CODE_ADDRESS, 31, 30);
	LoadFile("data.bin", DATA_ADDRESS, 31, 30);
	LoadFile("initializers.bin", INITIALIZER_ADDRESS, 31, 30);

	writeInjections(INITIALIZER_ADDRESS, true);
	writeInjections(CODE_ADDRESS, true);

	RestoreRegisters();
	ASMEnd(0x7c0802a6); //mflr r0
}

void writeInjectionsRepeat() {
	ASMStart(0x80023d54);
	//ASMStart(0x80023d5c);
	SaveRegisters();

	writeInjections(CODE_ADDRESS);

	RestoreRegisters();
	ASMEnd(0x80630010); //lwz r3, 0x10(r3)
	//ASMEnd(0x881f0008); //lbz r0, 8(r31)
}

void writeInjections(int address, bool clearCache) {
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;

	int counterReg = 16;
	int lengthReg = 15;
	int dataPtrReg = 14;

	LoadWordToReg(lengthReg, dataPtrReg, address);
	SetRegister(counterReg, 0);
	While(counterReg, LESS, lengthReg); {
		LWZU(reg1, dataPtrReg, 4); //function branch

		XORIS(reg1, reg1, 0xFC00); //remove opcode
		ADD(reg1, reg1, dataPtrReg); //add offset to current address
		//reg1 equals function start address

		LWZU(reg2, dataPtrReg, 4); //injection address

		SUBF(reg3, reg1, reg2); //from injection to function
		SetRegister(reg4, 0xFC000000);
		ANDC(reg3, reg3, reg4);
		ORIS(reg3, reg3, 0x4800); //branch opcode
		if (clearCache) {
			modifyInstruction(reg3, reg2); //write branch
		}
		else {
			STW(reg3, reg2, 0);
		}
		

		/*SUBF(reg3, reg2, reg1);
		//RLWINM(reg3, reg3, 30, 2, 31); //>> 2, / 4
		//SRAWI(reg3, reg3, 2);
		SetRegister(reg4, 0xFC000000);
		ANDC(reg3, reg3, reg4);
		ORIS(reg3, reg3, 0x4800); //branch opcode
		STW(reg3, reg1, 0); //write branch

		LWZU(reg2, dataPtrReg, 4); //function end address
		SUBF(reg3, reg1, reg2);
		ADDI(reg3, reg3, 4); //branch to next instruction
		ANDC(reg3, reg3, reg4);
		ORIS(reg3, reg3, 0x4800); //branch opcode
		STW(reg3, reg2, 0); //write branch*/

		Increment(counterReg);
	} EndWhile();
}