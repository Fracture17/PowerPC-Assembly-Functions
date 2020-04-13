#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Tag Based Costumes.h"
#include "Code Menu.h"

void tagBasedCostumes() {
	if(TAG_COSTUME_TOGGLE_INDEX == -1) {
		cout << "BAD!!!!\nHave to build code menu too!!!" << endl;		
	}

	GeckoStringWrite("0123456789ABCDEF", HEX_TO_ASCII_TABLE);

	recordActiveTags();

	setTagCostume();

	reloadCostumeAfterTagSelect();

	teamBattleTagReload();

	updateTagsWhenOptionChanged();
}

void recordActiveTags() {
	//r3 - 0x34 is position in list
	//r3 + 0x70 + 2 * position = tag index
	//[r3 + 0x57] & 7 == port num

	//r31 is menuPtr thing
	//ASMStart(0x806a0714);
	ASMStart(0x806a0718);
	SaveRegisters();

	int menuPtrReg = 3;
	int reg1 = 30;
	int reg2 = 29;
	int reg3 = 28;

	//record tag
	LWZ(reg1, menuPtrReg, -0x34); //tag index

	LBZ(reg2, menuPtrReg, 0x57);
	SetRegister(reg3, ACTIVE_TAG_ID_BY_PORT - 1);
	ANDI(reg2, reg2, 0x7); // port num

	STBX(reg1, reg3, reg2);

	//Force load costume
	SetRegister(reg1, TAG_LOAD_FLAGS_LOC - 1);
	SetRegister(reg3, 1);
	STBX(reg3, reg1, reg2);

	RestoreRegisters();
	//ASMEnd(0x9421fff0); //stwu sp, -0x10 (sp)
	ASMEnd(0x7c0802a6); //mflr r0
}

void teamBattleTagReload() {
	ASMStart(0x8068a4a0);
	SaveRegisters();

	int teamBattleSettingsReg = 31;
	int playerInitDataReg = 30;
	int flagReg = 29;
	int reg1 = 28;
	int reg2 = 27;

	/*SetRegister(playerInitDataReg, TAG_LOAD_FLAGS_LOC);
	SetRegister(flagReg, 1);

	for (int i = 0; i < 4; i++) {
		STB(flagReg, playerInitDataReg, i);
	}*/

	SetRegister(reg1, TEAM_SETTINGS_LOC);
	LWZ(reg2, teamBattleSettingsReg, 0x5C8);
	STW(reg2, reg1, 0);

	RestoreRegisters();
	ASMEnd(0x7fe3fb78); //mr r3, r31
}

void setTagCostume() {
	//r8 is port num
	//r4 is path str
	ASMStart(0x8084d0d4);
	SaveRegisters();

	int endStrReg = 20;
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;

	//if in SSE, don' run
	//if tag toggle is off, don't run
	//if toggle is on, don't run if teams, and not S4 teams
	LoadWordToReg(reg1, TAG_COSTUME_TOGGLE_INDEX + 8);
	If(reg1, NOT_EQUAL_I, 2); { //not off
		If(reg1, EQUAL_I, 0); { //on, no teams
			LoadByteToReg(reg2, TEAM_SETTINGS_LOC); //= 1 if team
			LoadByteToReg(reg3, TEAM_SETTINGS_LOC + 3); //= 1 if S4 teams
			If(reg2, EQUAL_I, 1); { //in teams
				If(reg3, EQUAL_I, 1); { //S4 teams
					SetRegister(reg1, 1); //should change costume
				} EndIf();
			} Else(); {
				SetRegister(reg1, 1);
			} EndIf();
		} EndIf();
		IfInSSE(reg2, reg3); {
			SetRegister(reg1, 0);
		} EndIf();

		If(reg1, EQUAL_I, 1); { //logic says should run
			SetRegister(reg1, COSTUME_PATH_ADDRESS_RESULT);
			STW(4, reg1, 0);

			SetRegister(reg3, ACTIVE_TAG_ID_BY_PORT);
			LBZX(reg3, reg3, 8); //tag index
			If(reg3, LESS_I, 120); //within max tag size (not uninitialized or 0xFF)
			{
				MR(reg2, 4);

				string header = "A:/" + MAIN_FOLDER + "/pf";
				SetRegister(reg1, STRING_BUFFER);
				WriteStringToMem(header, reg1);

				SetRegister(3, STRING_BUFFER + header.size());
				MR(4, reg2);
				CallBrawlFunc(STRCPY);
				ADDI(endStrReg, 7, -7);

				MULLI(reg1, reg3, TAG_LIST_SIZE);
				SetRegister(reg2, TAG_LIST_START_LOC);
				LHZUX(reg1, reg2, reg1); //reg1 = first character, reg2 = tag address

				SetRegister(reg5, '$');
				STBU(reg5, endStrReg, 1);

				SetRegister(reg5, HEX_TO_ASCII_TABLE);
				While(reg1, NOT_EQUAL_I, 0);
				{
					CounterLoop(reg3, 20, 33, 4);
					{
						RLWNM(reg4, reg1, reg3, 28, 31); //isolate first hex

						LBZX(reg4, reg5, reg4); //get conversion

						STBU(reg4, endStrReg, 1);
					} CounterLoopEnd();
					LHZU(reg1, reg2, 2);
				} EndWhile();


				SetRegister(reg1, '.pac');
				STW(reg1, endStrReg, 1);
				SetRegister(reg1, 0);
				STW(reg1, endStrReg, 5);

				//check if file exists
				SetRegister(4, STRING_BUFFER + 0xB0);
				SetRegister(reg3, 0x72000000);
				SetRegister(3, STRING_BUFFER);
				STW(reg3, 4, 0);
				CallBrawlFunc(FA_FOPEN);
				If(3, NOT_EQUAL_I, 0);
				{
					CallBrawlFunc(FA_FCLOSE);
					SetRegister(reg1, COSTUME_PATH_ADDRESS_RESULT);
					SetRegister(reg2, STRING_BUFFER + header.size());
					STW(reg2, reg1, 0);
				} EndIf();
			} EndIf();

			RestoreRegisters();
			LoadWordToReg(4, COSTUME_PATH_ADDRESS_RESULT);

		} Else(); {
			RestoreRegisters();
		} EndIf();
	} Else(); {
		RestoreRegisters();
	} EndIf();

	ASMEnd(0x392000ff); //li r9, 255
}

void reloadCostumeAfterTagSelect() {
	//r31 + 8 = flag location
	//if flag == 1, set r3 to 0x7F, else lbz r3, 0x5B(r30)
	ASMStart(0x8094641c);

	LBZ(3, 31, 2); //port
	SetRegister(4, TAG_LOAD_FLAGS_LOC);
	LBZX(6, 3, 4); //flag

	If(6, EQUAL_I, 1); {
		SetRegister(6, 0);
		STBX(6, 4, 3); //clear flag
		LoadWordToReg(6, IS_IN_GAME_FLAG);
		If(6, NOT_EQUAL_I, 1); {
			SetRegister(3, 0x7F);
		} Else(); {
			LBZ(3, 30, 0x5B);
		} EndIf();
	} Else(); {
		LBZ(3, 30, 0x5B);
	} EndIf();
	
	ASMEnd();
}

void updateTagsWhenOptionChanged() {
	ASMStart(0x8001735c);
	SaveRegisters();

	int playerInitDataReg = 30;
	int flagReg = 29;
	int reg1 = 28;
	int reg2 = 27;

	LoadWordToReg(reg1, PREV_TAG_COSTUMES_SETTING_LOC);
	LoadWordToReg(reg2, TAG_COSTUME_TOGGLE_INDEX + 8);
	If(reg1, NOT_EQUAL, reg2); { //setting has been changed
		SetRegister(playerInitDataReg, TAG_LOAD_FLAGS_LOC);
		SetRegister(flagReg, 1);

		for (int i = 0; i < 4; i++) {
			STB(flagReg, playerInitDataReg, i);
		}
	} EndIf();

	SetRegister(reg1, PREV_TAG_COSTUMES_SETTING_LOC);
	LoadWordToReg(reg2, TAG_COSTUME_TOGGLE_INDEX + 8);
	STW(reg2, reg1, 0);

	RestoreRegisters();
	ASMEnd(0x2c180001); //cmpwi r24, 1
}