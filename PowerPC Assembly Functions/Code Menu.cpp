#include "stdafx.h"
#include "Code Menu.h"

void CreateMenu(Page MainPage)
{
	SetGeckoPointerAddress(CODE_MENU_CREATED);
	WriteIntToFile(0x32000000);
	WriteIntToFile(1); //if not created
	WriteIntToFile(0x4E000018);
	WriteIntToFile(0); //get address
	WriteIntToFile(0x4C000000);
	WriteIntToFile(CODE_MENU_PAGE_ADDRESS ^ 0x80000000); //store address
	WriteIntToFile(0xE0000000);
	WriteIntToFile(0x80008000); //full termiator

	vector<Page*> Pages(1, &MainPage);
	vector<int> CalledPageOffsets(1, 0);
	int EndOffset = MainPage.Size;
	for (int i = 0; i < Pages.size(); i++) {
		for (Line* &x : Pages[i]->Lines) {
			if (x->type == SUB_MENU_LINE) {
				x->SubMenuOffset = EndOffset - CalledPageOffsets[i];
				Pages.push_back(x->SubMenuPtr);
				CalledPageOffsets.push_back(EndOffset);
				EndOffset += x->Size;
			}
		}
	}

	int size = 0;
	for (auto x : Pages) {
		size += x->Size;
	}

	if (size % 16 == 8)
	{
		size += 8;
	}

	WriteIntToFile(0x68200000 + size / 8);
	WriteIntToFile(0); //skip

	for (auto x : Pages) {
		x->WritePage();
	}

	
	/*WriteIntToFile(0xE0000000);
	WriteIntToFile(0x80008000); //full termiator
	WriteIntToFile(0xE0000000);
	WriteIntToFile(0x80008000); //full termiator*/
}

//needs to be fixed, discards buffers
void CreateDrawSettingsBuffer(int TempReg)
{
	SetRegister(TempReg, DRAW_SETTINGS_PTR_LOC);

	SetRegs(3, { 1, 0x2A, 0x2B });
	CallBrawlFunc(0x800b8930); //create
	STW(3, TempReg, 0);

	SetRegs(4, { 0x200, 1 });
	CallBrawlFunc(0x800b8b08); //allocMsgBuffer

	LWZ(3, TempReg, 0);
	SetRegister(4, 0x928929e0);
	CallBrawlFunc(0x800b8c7c); //setMsgData

	LWZ(3, TempReg, 0);
	LWZ(3, 3, 8);
	STW(3, TempReg, 0);
	SetRegister(4, 0x01000000);
	STW(4, 3, 0x4C);
	SetRegister(4, GetHexFromFloat(0.0625));
	STW(4, 3, 0x50);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x68);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x24);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x28);
	SetRegister(4, GetHexFromFloat(0.973585));
	STW(4, 3, 0x58);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x5C);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x2C);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x30);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x34);
	SetRegister(4, 1);
	STW(4, 3, 0x38);
	SetRegister(4, 1);
	STW(4, 3, 0x3C);
	SetRegister(4, RED);
	STW(4, 3, 8);
	SetRegister(4, RED);
	STW(4, 3, 0xC);
	SetRegister(4, RED);
	STW(4, 3, 0x10);
	SetRegister(4, RED);
	STW(4, 3, 0x14);
	SetRegister(4, 0x80497d7c);
	STW(4, 3, 0x48);
	SetRegister(4, 0);
	STW(4, 3, 0x40);
}

void CodeMenu()
{
	Page P1("Player 1 Codes", { new Toggle("Infinite Shield") }, 0);
	Page P2("Player 2 Codes", { new Toggle("Infinite Shield") }, 0);
	Page P3("Player 3 Codes", { new Toggle("Infinite Shield") }, 0);
	Page P4("Player 4 Codes", { new Toggle("Infinite Shield") }, 0);

	//main page
	vector<Line*> Lines;
	Lines.push_back(&P1.CalledFromLine);
	Lines.push_back(&P2.CalledFromLine);
	Lines.push_back(&P3.CalledFromLine);
	Lines.push_back(&P4.CalledFromLine);
	Lines.push_back(new Toggle("Draw DI"));
	Page Main("Main", Lines, 0);

	CreateMenu(Main);

	PrintCodeMenu();

	TriggerCodeMenu();
}

void PrintCodeMenu()
{
	ASMStart(0x800b0888);
	vector<int> FPRegs(14);
	iota(FPRegs.begin(), FPRegs.end(), 0);
	SaveRegisters(FPRegs);

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 27;
	int Reg6 = 26;
	int Reg7 = 25;
	int Reg8 = 24;

	LoadWordToReg(Reg1, CODE_MENU_CONTROL_FLAG);
	If(Reg1, EQUAL_I, CODE_MENU_OPEN); //menu is open

	LoadWordToReg(Reg1, CODE_MENU_PAGE_ADDRESS);
	LoadWordToReg(Reg2, DRAW_SETTINGS_PTR_LOC);
	PrintPage(Reg1, Reg2, Reg3, Reg4, Reg5, Reg6, Reg7, Reg8);

	Else(); //menu is open

	CallBrawlFunc(0x801ace24); //DrawXlu

	EndIf(); //menu is open

	RestoreRegisters();
	ASMEnd();
}

void TriggerCodeMenu()
{
	//r4 has location
	ASMStart(0x811997c8);
	SaveRegisters();

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 27;
	int Reg6 = 26;
	int Reg7 = 25;
	int Reg8 = 24;

	LoadWordToReg(Reg2, Reg1, CODE_MENU_CREATED);
	If(4, EQUAL_I, 9); //on rotation
	
	If(Reg2, NOT_EQUAL_I, 1); //first time

	SetRegister(Reg2, 1);
	STW(Reg2, Reg1, 0); //clear first time flag

	SetRegister(Reg4, CODE_MENU_SETTINGS - 4);
	SetRegister(Reg3, 0);
	SetRegister(Reg2, CODE_MENU_END);
	While(Reg4, LESS_L, Reg2); //clear settings

	STWU(Reg3, Reg4, 4);

	EndWhile(); //clear settings

	SetRegister(Reg2, "\3Off"s);
	SetRegister(Reg3, OFF_TEXT_LOC);
	STW(Reg2, Reg3, 0);
	SetRegister(Reg2, "\2On\0"s);
	SetRegister(Reg3, ON_TEXT_LOC);
	STW(Reg2, Reg3, 0);

	CreateDrawSettingsBuffer(Reg3);

	EndIf(); //first time

	//set open flag last to prevent thread errors
	SetRegister(Reg2, CODE_MENU_OPEN);
	SetRegister(Reg1, CODE_MENU_CONTROL_FLAG);
	STW(Reg2, Reg1, 0);

	Else();  //on rotation

	//set menu closed
	SetRegister(Reg1, CODE_MENU_CONTROL_FLAG);
	SetRegister(Reg2, CODE_MENU_CLOSED);
	STW(Reg2, Reg1, 0);

	EndIf(); //on rotation

	RestoreRegisters();
	ASMEnd(0x9421ffe0); //stwu sp, sp, -0x20
}

void SetTextColor(int ColorReg, int SettingsPtrReg)
{
	STW(ColorReg, SettingsPtrReg, 0x8);
	STW(ColorReg, SettingsPtrReg, 0xC);
	STW(ColorReg, SettingsPtrReg, 0x10);
	STW(ColorReg, SettingsPtrReg, 0x14);
}

void SetTextXPos(int XPosReg, int SettingPtrReg)
{
	STW(XPosReg, SettingPtrReg, DRAW_BUFFER_XPOS_OFFSET);
}

void SetTextYPos(int YPosReg, int SettingPtrReg)
{
	STW(YPosReg, SettingPtrReg, DRAW_BUFFER_YPOS_OFFSET);
}

void SetTextPos(int XPosReg, int YPosReg, int SettingPtrReg)
{
	SetTextXPos(XPosReg, SettingPtrReg);
	SetTextYPos(YPosReg, SettingPtrReg);
}

void NewLine(int SettingsPtrReg)
{
	SetRegister(3, GetHexFromFloat(INITIAL_XPOS));
	LFS(0, SettingsPtrReg, DRAW_BUFFER_YPOS_OFFSET);
	LFS(1, 2, FS_20_0);
	FADD(0, 0, 1);
	STW(3, SettingsPtrReg, DRAW_BUFFER_XPOS_OFFSET);
	STFS(0, SettingsPtrReg, DRAW_BUFFER_YPOS_OFFSET);
}

//r3 is SettingsPtr, r4 is CharReg
void PrintChar(int SettingsPtrReg, int CharReg)
{
	SetArgumentsFromRegs(3, { SettingsPtrReg, CharReg });
	CallBrawlFunc(0x8006fe50); //printChar
}

//StringPtr should be 1 less than start
//StringPtr gets moved to end of string, numCharsReg gets set to 0
void PrintString(int StringPtrReg, int NumCharsReg, int SettingsPtrReg)
{
	While(NumCharsReg, GREATER_I, 0); //print loop

	LBZU(4, StringPtrReg, 1);
	PrintChar(SettingsPtrReg, 4);

	Decrement(NumCharsReg);
	EndWhile(); //print loop
}

void PrintMenuLine(int LinePtrReg, int ValueStringPtrReg, int ValueXPosReg, int SettingsPtrReg, int ColorReg, int TempReg1, int TempReg2)
{
	SetTextColor(ColorReg, SettingsPtrReg);

	LBZ(TempReg1, TempReg2, 0); //length
	PrintString(TempReg2, TempReg1, SettingsPtrReg);

	SetTextXPos(ValueXPosReg, SettingsPtrReg);

	ADDI(TempReg2, ValueStringPtrReg, 0);
	LBZ(TempReg1, TempReg2, 0); //length
	PrintString(TempReg2, TempReg1, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

void SetupPrintText(int SettingPtrReg)
{
	ADDI(3, SettingPtrReg, 0);
	SetRegs(4, { 0, 9 });
	CallBrawlFunc(0x8006a964); //SetDefaultEnv

	CallBrawlFunc(0x80019FA4); //(getManager[cameraManager])

	CallBrawlFunc(0x80018DE4); //(setGX[camera])
}

void PrintPage(int PageReg, int SettingPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6)
{
	SetupPrintText(SettingPtrReg);

	//set initial text pos
	SetRegister(Reg1, GetHexFromFloat(INITIAL_XPOS));
	SetRegister(Reg2, GetHexFromFloat(INITIAL_YPOS));
	SetTextPos(Reg1, Reg2, SettingPtrReg);

	LWZ(Reg1, PageReg, Page::XPOS); //get value xpos
	ADDI(Reg2, PageReg, Page::FIRST_LINE_OFFSET); //first line
	SetRegister(3, 1);
	While(3, NOT_EQUAL_I, 0); //draw line loop

	LWZ(Reg4, Reg2, Line::TYPE);
	If(Reg4, EQUAL_I, TOGGLE_LINE); //is toggle

	PrintToggleLine(Reg2, Reg1, SettingPtrReg, Reg4, Reg5);

	Else(); If(Reg4, EQUAL_I, SUB_MENU_LINE); //is sub menu

	PrintSubMenuLine(Reg2, SettingPtrReg, Reg4, Reg5);

	EndIf(); EndIf(); //done

	LWZ(3, Reg2, Line::SIZE);
	ADD(Reg2, Reg2, 3); //next line
	EndWhile(); //draw line loop
}

void PrintToggleLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, Toggle::COLOR);
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, Toggle::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	SetTextXPos(ValueXPosReg, SettingsPtrReg);

	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZ(TempReg1, LinePtrReg, Toggle::ID);
	RLWINM(TempReg1, TempReg1, 2, 0, 31); //<<2
	LWZX(TempReg1, TempReg1, TempReg2); //value
	If(TempReg1, EQUAL_I, 0); //false
	SetRegister(TempReg1, OFF_TEXT_LOC);
	Else(); //false
	SetRegister(TempReg1, ON_TEXT_LOC);
	EndIf(); //false
	
	LBZ(TempReg2, TempReg1, 0); //length
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

void PrintCommentLine()
{

}

void PrintSubMenuLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, SubMenu::COLOR);
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, SubMenu::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}