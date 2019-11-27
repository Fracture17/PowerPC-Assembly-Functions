#include "stdafx.h"
#include "FPS Display.h"

void FPSDisplay() {
	vector<unsigned int> x = { 3254926684, 3, 2288895028, 946012161, 2557330484, 2283733000, 1610612736, 0, 3254926716, 6, 2288895029, 946012161, 2557330485, 738394172, 1098907672, 2288895028, 2959983670, 945815552, 2557330484, 2557330485, 2147549204, 0 };

	for(auto a: x) {
		WriteIntToFile(a);
	}



	ASMStart(0x8001792c);
	SaveRegisters(15);

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;
	int reg8 = 24;
	int fontHolderReg = 23;
	int fontHolderReg2 = 22;

	LoadWordToReg(reg1, 0x80497db8);
	If(reg1, NOT_EQUAL_I, 0); {

		vector<int> DSB(0x200 / 4, 0);
		DSB[0x4 / 4] = 0xFFFFFFFF;
		DSB[0x38 / 4] = 1;
		DSB[0x3C / 4] = 1;
		DSB[0x4C / 4] = 0x01000000;
		DSB[0x50 / 4] = 0x3D800000;
		DSB[0x58 / 4] = 0x3f793cde;
		DSB[(0x100 + 0x4C) / 4] = DRAW_SETTINGS_BUFFER_LOC + 0x100;
		DSB[0x1D0 / 4] = DRAW_SETTINGS_BUFFER_LOC + 0x100;

		SetRegister(reg1, DRAW_SETTINGS_BUFFER_LOC);
		WriteVectorToMem(DSB, reg1);

		SetRegister(reg1, 0);
		LWZ(fontHolderReg, 13, -0x42AC); //save font ptr if exist
		LWZ(fontHolderReg2, 13, -0x42A8); //save other font ptr if exist
		STW(reg1, 13, -0x42AC); //clear font ptr
		STW(reg1, 13, -0x42A8); //clear other font ptr

		SetRegister(reg1, STRING_BUFFER + 0x80);
		WriteStringToMem("FPS: %2d", reg1);
		LoadHalfToReg(reg2, 0x805a0856);
		SprintF(reg1, { reg2 });
		SetRegister(reg1, STRING_BUFFER - 1);
		MR(reg6, 3);
		SetRegister(reg3, DRAW_SETTINGS_BUFFER_LOC);

		SetupPrintText(reg3);

		SetRegs(3, { (int)0x805B71F0, 0 });
		CallBrawlFunc(0x801f51dc);


		SetRegister(reg4, 0x805b6df8);
		LFS(1, reg4, 0); //scale factor

		SetFloatingRegister(2, reg4, 0.33);
		FMULS(2, 2, 1);
		STFS(2, reg3, 0x50); //size

							 //set initial text pos
		SetRegister(reg4, GetHexFromFloat(-165));
		SetRegister(reg5, GetHexFromFloat(-115));
		//SetRegister(reg4, GetHexFromFloat(-192));
		//SetRegister(reg5, GetHexFromFloat(-128));
		SetTextPos(reg4, reg5, reg3);

		LoadHalfToReg(reg4, 0x805a0856);
		If(reg2, LESS_I, 60);
		{
			SetRegister(reg2, RED);
		} Else();
		{
			SetRegister(reg2, BLUE);
		} EndIf();
		SetTextColor(reg2, reg3);

		STW(fontHolderReg, 13, -0x42AC);
		STW(fontHolderReg2, 13, -0x42A8);
		PrintString(reg1, reg6, reg3);
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x387e0118); //addi r3, r30, 280
}