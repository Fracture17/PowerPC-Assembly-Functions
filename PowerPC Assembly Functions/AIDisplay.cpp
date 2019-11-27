#include "stdafx.h"
#include "AIDisplay.h"

void makeBuffer() {
	int fontHolderReg = 23;
	int fontHolderReg2 = 22;
	int reg1 = 31;

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
}

void AIDisplay() {
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
	int AIModuleReg = 21;
	int reg9 = 20;
	int counterReg = 19;

	IfInVersus(reg1); {
		makeBuffer();

		LoadWordToReg(reg1, 0x80497db8);
		If(reg1, NOT_EQUAL_I, 0); {
			SetRegister(3, 0x80629a00);
			SetRegister(4, 0);
			CallBrawlFunc(0x80815bf8); //getEntryIdFromIndex
			If(3, GREATER_OR_EQUAL_I, 0); {
				SetRegister(counterReg, 0);

				int NanaLabel = GetNextLabel();
				Label(NanaLabel);

				MR(4, 3);
				SetRegister(3, 0x80629a00);
				If(counterReg, EQUAL_I, 0); {
					CallBrawlFunc(0x80815a38); //getINput[ftManager]
				} Else(); {
					CallBrawlFunc(0x80815a64); //getSubINput[ftManager]
				} EndIf();
				MR(AIModuleReg, 3);
				
				SetRegister(reg1, 0x81200000);
				If(AIModuleReg, GREATER_OR_EQUAL_L, reg1); {
					LWZ(reg9, AIModuleReg, 0x44);
					for (int i = 0; i < 24;) {
						string format = "";
						for (auto freg : { 1, 2, 3, 4, 5, 6 }) {
							format += "var";
							format += to_string(i);
							if (to_string(i).size() == 1) {
								format += ": % 9.2F ";
							}
							else {
								format += ": % 8.2F ";
							}

							LFS(freg, reg9, i * 4);
							i++;
						}
						cout << format << endl;
						SetRegister(reg1, STRING_BUFFER + 0xB0);
						WriteStringToMem(format, reg1);
						SetRegister(reg8, STRING_BUFFER);
						SprintF(reg1, {}, { 1, 2, 3, 4, 5, 6 }, reg8);
						MR(reg8, 3);

						SetRegister(reg3, DRAW_SETTINGS_BUFFER_LOC);
						SetupPrintText(reg3);

						SetRegs(3, { (int)0x805B71F0, 0 });
						CallBrawlFunc(0x801f51dc);

						SetRegister(reg4, 0x805b6df8);
						LFS(1, reg4, 0); //scale factor
						SetFloatingRegister(2, reg4, 0.15);
						FMULS(2, 2, 1);
						STFS(2, reg3, 0x50); //size
						//set initial text pos
						SetRegister(reg4, GetHexFromFloat(-400));
						If(counterReg, EQUAL_I, 0); {
							SetRegister(reg5, GetHexFromFloat(-210 + 25 * (i / 6 - 1)));
						} Else(); {
							SetRegister(reg5, GetHexFromFloat(-210 + 25 * 5 + 25 * (i / 6 - 1)));
						} EndIf();
						
						SetTextPos(reg4, reg5, reg3);

						SetRegister(reg2, BLUE);
						SetTextColor(reg2, reg3);

						SetRegister(reg1, STRING_BUFFER - 1);

						STW(fontHolderReg, 13, -0x42AC);
						STW(fontHolderReg2, 13, -0x42A8);
						PrintString(reg1, reg8, reg3);
					}

					SetRegister(reg1, STRING_BUFFER + 0xB0);
					WriteStringToMem("md value: %2d   script: %4X", reg1);

					LWZ(reg2, AIModuleReg, 0x48); //md
					LWZ(reg9, AIModuleReg, 0x44);
					LHZ(reg3, reg9, 0x78); //script

					SetRegister(reg8, STRING_BUFFER);
					SprintF(reg1, { reg2, reg3 }, reg8);
					MR(reg8, 3);

					SetRegister(reg3, DRAW_SETTINGS_BUFFER_LOC);
					SetupPrintText(reg3);

					SetRegs(3, { (int)0x805B71F0, 0 });
					CallBrawlFunc(0x801f51dc);

					SetRegister(reg4, 0x805b6df8);
					LFS(1, reg4, 0); //scale factor
					SetFloatingRegister(2, reg4, 0.15);
					FMULS(2, 2, 1);
					STFS(2, reg3, 0x50); //size
					//set initial text pos
					SetRegister(reg4, GetHexFromFloat(-400));
					If(counterReg, EQUAL_I, 0); {
						SetRegister(reg5, GetHexFromFloat(-210 + 25 * 4));
					} Else(); {
						SetRegister(reg5, GetHexFromFloat(-210 + 25 * 4 + 25 * 5));
					} EndIf();
					SetTextPos(reg4, reg5, reg3);

					SetRegister(reg2, BLUE);
					SetTextColor(reg2, reg3);

					SetRegister(reg1, STRING_BUFFER - 1);

					STW(fontHolderReg, 13, -0x42AC);
					STW(fontHolderReg2, 13, -0x42A8);
					PrintString(reg1, reg8, reg3);
				} EndIf();

				If(counterReg, EQUAL_I, 0); {
					SetRegister(counterReg, 1);
					JumpToLabel(NanaLabel);
				} EndIf();
			} EndIf();
		} EndIf();
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x387e0118); //addi r3, r30, 280
}