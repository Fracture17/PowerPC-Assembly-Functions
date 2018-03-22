#include "stdafx.h"
#include "Control Codes.h"

void ControlCodes()
{
	StartMatch();

	EndMatch();

	Draw();

	LoadCodeMenu();

	AddNewCharacterBuffer();
}

void LoadCodeMenu()
{
	ASMStart(0x8002d4f4);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;

	SetRegister(reg1, STRING_BUFFER);

	SetRegister(reg2, STRING_BUFFER + 0x18);
	STW(reg2, reg1, 0); //file path ptr

	SetRegister(reg2, 0);
	STW(reg2, reg1, 4);
	STW(reg2, reg1, 8);
	STW(reg2, reg1, 0x10);

	SetRegister(reg2, START_OF_CODE_MENU);
	STW(reg2, reg1, 0xC); //storage loc

	SetRegister(reg2, -1);
	STW(reg2, reg1, 0x14);

	ADDI(reg2, reg1, 0x18);
	WriteStringToMem("legacyte/cm.bin\0"s, reg2);

	MR(3, reg1);
	CallBrawlFunc(0x8001cbf4); //readSDFile

	RestoreRegisters();
	ASMEnd(0x9421ffe0); //stwu sp, sp, -0x20
}

void StartMatch()
{
	ASMStart(0x806cf15c);
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

	//allocate memory
#ifdef IASA_OVERLAY
	SetupIASABuffers();
#endif

	if (DI_DRAW_INDEX != -1) {
		SetupCharacterBuffer();
	}

	if (INFINITE_FRIENDLIES_INDEX != -1) {
		InfiniteFriendlies(reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
	}

	//set in game flag
	SetRegister(reg1, 1);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	RestoreRegisters();
	ASMEnd(0x3fa08070); //lis r29, 0x8070
}

void EndMatch()
{
	ASMStart(0x806d4850);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int MainBufferReg = 15;
	int CharacterBufferReg = 14;

	//clear in game flag
	SetRegister(reg1, 0);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	//free memory
#ifdef IASA_OVERLAY
	FreeIASABuffers();
#endif

	//reload stage
	if (INFINITE_FRIENDLIES_INDEX != -1) {
		LoadWordToReg(reg1, INFINITE_FRIENDLIES_INDEX);
		If(reg1, GREATER_I, 0); {
			If(reg1, GREATER_I, 1); {
				SetRegister(reg1, 1);
				SetRegister(reg2, INFINITE_FRIENDLIES_FLAG_LOC);
				STW(reg1, reg2, 0);
			}EndIf();
			LoadWordToReg(reg1, 0x805b4fd8 + 0xd4);
			LWZ(reg1, reg1, 0x10);
			SetRegister(reg2, 2);
			STW(reg2, reg1, 8);
		}EndIf();
	}

	LoadWordToReg(MainBufferReg, MAIN_BUFFER_PTR);
	LWZU(CharacterBufferReg, MainBufferReg, 4);
	While(CharacterBufferReg, NOT_EQUAL_I, 0); {
		if (DI_DRAW_INDEX != -1) {
			LWZ(reg1, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
			FreeDIBuffer(reg1, reg2);
		}

		FreeMem(CharacterBufferReg);
		LWZU(CharacterBufferReg, MainBufferReg, 8);
	}EndWhile();
	LoadWordToReg(MainBufferReg, MAIN_BUFFER_PTR);
	FreeMem(MainBufferReg);

	RestoreRegisters();
	ASMEnd(0x7c7f1b78); //mr r31, r3
}

void Draw()
{
	ASMStart(0x8000e588);
	vector<int> FPRegs(21);
	iota(FPRegs.begin(), FPRegs.end(), 0);
	SaveRegisters(FPRegs);

	//draw di
	if (DI_DRAW_INDEX != -1) {
		//DrawDI();
		DrawTrajectories();
	}

	//IASA overlay
#ifdef IASA_OVERLAY
	EndOverlay();
#endif

	RestoreRegisters();
	ASMEnd(0x38210030); //addi sp, sp, 48
}

void AddNewCharacterBuffer()
{
	//[r3 + 0x60] = module ptr
	//f31

	int BaseModuleTableReg = 31;
	int reg1 = 30;
	int reg2 = 29;
	int reg3 = 28;
	int reg4 = 27;
	int reg5 = 26;
	int ModulePtrReg = 25;
	int HeadOfFighterReg = 24;
	int CharacterBufferReg = 23;

	ASMStart(0x80835f20);

	LWZ(BaseModuleTableReg, 3, 0x60); //overwritten op
	LoadWordToReg(28, IS_IN_GAME_FLAG);
	If(28, EQUAL_I, 1); {
		//is in game
		SaveRegisters(14);

		MR(HeadOfFighterReg, 3);
		LWZ(ModulePtrReg, BaseModuleTableReg, 0xD8);

		SetRegister(reg1, 0);
		FindEndOfCharacterBuffers(reg1, reg2);
		STW(BaseModuleTableReg, reg2, 0);
		STW(reg1, reg2, 8); //clear next slot
		STW(reg1, reg2, 0xC); //clear next slot

		SetRegister(reg1, CHR_BUFFER_SIZE + 0x20);
		Allocate(reg1);
		STW(3, reg2, 4);

		MR(CharacterBufferReg, 3);

		MR(3, HeadOfFighterReg);
		CallBrawlFunc(0x8083ae38); //getInput
		STW(3, CharacterBufferReg, CHR_BUFFER_FIGHTER_INPUT_PTR_OFFSET);
		SetRegister(3, 0x80629a00);
		CallBrawlFunc(0x80815ad0); //get player number
		STW(3, CharacterBufferReg, CHR_BUFFER_PORT_OFFSET);

		MULLI(reg2, 3, 0x5C);
		SetRegister(reg1, CHARACTER_INFO_START_ADDRESS);
		ADD(reg1, reg1, reg2);
		STW(reg1, CharacterBufferReg, CHR_BUFFER_INFO_PTR_OFFSET);

		if (CHARACTER_SELECT_P1_INDEX != -1) {
			LBZ(reg3, reg1, 0); //get char ID
			GetArrayValueFromIndex(CHARACTER_SWITCHER_ARRAY_LOC, 3, 0, 3, reg2); {
				STW(reg3, reg2, 4);
			}EndIf(); EndIf();
		}

		MR(3, BaseModuleTableReg);
		SetRegs(4, { 3023, 0 }); //gravity
		CallBrawlFunc(0x80796c6c); //get constant float core
		FNEG(1, 1);
		STFS(1, CharacterBufferReg, CHR_BUFFER_GRAVITY_OFFSET);

		MR(3, BaseModuleTableReg);
		SetRegs(4, { 3024, 0 }); //mfs
		CallBrawlFunc(0x80796c6c); //get constant float core
		FNEG(1, 1);
		STFS(1, CharacterBufferReg, CHR_BUFFER_MFS_OFFSET);

		LWZ(reg2, ModulePtrReg, 0x64);
		LWZ(reg2, reg2, 0x20);
		LWZ(reg2, reg2, 0xC);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_VARIABLES_ADDRESS_OFFSET); //variables start address
		LWZU(reg3, reg2, 0xE0);
		STW(reg3, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_OFFSET); //frames
		STW(reg2, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET); //ptr

		LWZ(reg2, ModulePtrReg, 0xC);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_POS_PTR_OFFSET);

		LWZ(reg3, ModulePtrReg, 0x7C);
		LWZ(reg2, reg3, 0x7C);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_KB_VECTOR_PTR_OFFSET);

		LWZ(reg2, ModulePtrReg, 0x5C);
		LWZ(reg2, reg2, 0x14C);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_IP_SWITCH_PTR_OFFSET);

		//create buffers
		if (DI_DRAW_INDEX != -1) {
			CreateDIBuffer(CharacterBufferReg, reg2, reg3, reg4);
		}

		RestoreRegisters();
	}EndIf();
	ASMEnd();
}

void FindCharacterBuffer(int TargetReg, int ResultReg)
{
	LoadWordToReg(ResultReg, MAIN_BUFFER_PTR);
	LWZ(3, ResultReg, 0);
	While(3, NOT_EQUAL, TargetReg); {
		LWZU(3, ResultReg, 8);
	}EndWhile();
	LWZ(ResultReg, ResultReg, 4);
}

void FindEndOfCharacterBuffers(int TargetReg, int ResultReg)
{
	LoadWordToReg(ResultReg, MAIN_BUFFER_PTR);
	LWZ(3, ResultReg, 0);
	While(3, NOT_EQUAL, TargetReg); {
		LWZU(3, ResultReg, 8);
	}EndWhile();
}

void GetCharacterValue(int CharacterBufferReg, vector<int> ValuePath, int ResultReg)
{
	GetValueFromPtrPath(ValuePath, CharacterBufferReg, ResultReg);
}

void InfiniteFriendlies(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9)
{
	LoadWordToReg(reg1, reg2, INFINITE_FRIENDLIES_FLAG_LOC);
	If(reg1, EQUAL_I, 1); {
		SetRegister(reg1, 0);
		STW(reg1, reg2, 0); //clear flag

		LoadWordToReg(reg1, INFINITE_FRIENDLIES_INDEX);
		If(reg1, EQUAL_I, 2); {
			//random stage
			GetLegalStagesArray(reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
			MR(3, reg4);
			CallBrawlFunc(0x8003fc7c); //randi
			//RandomCapped(reg4, reg1);
			LoadWordToReg(reg2, 0x805a00e0);
			LBZX(3, reg3, 3);
			LWZ(reg2, reg2, 8);
			CallBrawlFunc(0x800af614); //exchangeMuStageForScStage
			STH(3, reg2, 0x1A);
		}EndIf();
	}EndIf();
}

//reg4 contains total size
//reg3 contains start address
void GetLegalStagesArray(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9)
{
	SetRegister(reg1, 0x806b9298);
	LoadWordToReg(reg2, 0x805A00E0);
	LWZ(reg2, reg2, 0x24);
	ADDI(reg2, reg2, 2064);
	SetRegister(reg3, STRING_BUFFER);
	SetRegister(reg4, 0);
	LWZ(reg8, reg2, 0x24);
	LWZ(reg9, reg2, 0x20);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
	ADDI(reg1, reg1, 8);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
}

void AddLegalStagesToArray(int StageListReg, int LegalStageHighMaskReg, int LegalStageLowMaskReg, int ArrayReg, int PosReg, int reg2, int reg3, int reg4)
{
	LBZ(4, StageListReg, 4); //num stages
	LWZ(reg2, StageListReg, 0); //list ptr
	SetRegister(3, 1);
	While(4, GREATER_I, 0); {
		Decrement(4);
		LBZX(reg3, reg2, 4);
		If(reg3, GREATER_OR_EQUAL_I, 31); {
			ADDI(reg4, reg3, -23);
			RLWNM(reg4, 3, reg4, 0, 31);
			AND(reg4, reg4, LegalStageHighMaskReg);
		}Else(); {
			RLWNM(reg4, 3, reg3, 0, 31);
			AND(reg4, reg4, LegalStageLowMaskReg);
		}EndIf();
		
		If(reg4, NOT_EQUAL_I, 0); {
			STBX(reg3, ArrayReg, PosReg);
			Increment(PosReg);
		}EndIf();
	}EndWhile();
}