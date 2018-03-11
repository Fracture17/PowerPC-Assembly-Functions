#include "stdafx.h"
#include "Code Menu.h"
#include "DrawDI.h"
#include "IASA Overlay.h"
#include "Control Codes.h"

void CodeMenu()
{
	//player pages
	Page P1("Player 1 Codes", { new Toggle("Infinite Shield", false) }, 0);
	Page P2("Player 2 Codes", { new Toggle("Infinite Shield", false) }, 0);
	Page P3("Player 3 Codes", { new Toggle("Infinite Shield", false) }, 0);
	Page P4("Player 4 Codes", { new Toggle("Infinite Shield", false) }, 0);

	//settings page
	vector<Line*> Lines;
	//colors
	Lines.push_back(new Comment("Normal line"));
	Lines.push_back(new Number("Red", 0, 255, 255));
	Lines.push_back(new Number("Green", 0, 255, 255));
	Lines.push_back(new Number("Blue", 0, 255, 255));
	Lines.push_back(new Number("Alpha", 0, 255, 255));
	Lines.push_back(new Comment(""));

	Lines.push_back(new Comment("Comment line"));
	Lines.push_back(new Number("Red", 0, 255, 0x33));
	Lines.push_back(new Number("Green", 0, 255, 0xCC));
	Lines.push_back(new Number("Blue", 0, 255, 0x33));
	Lines.push_back(new Number("Alpha", 0, 255, 255));
	Lines.push_back(new Comment(""));

	Lines.push_back(new Comment("Highlighted line"));
	Lines.push_back(new Number("Red", 0, 255, 255));
	Lines.push_back(new Number("Green", 0, 255, 255));
	Lines.push_back(new Number("Blue", 0, 255, 0));
	Lines.push_back(new Number("Alpha", 0, 255, 255));
	
	Page Colors("Menu color settings", Lines, 0);

	//navigation
	Lines.clear();
	Lines.push_back(new Comment("Change the speed of navigation"));
	Lines.push_back(new Comment(""));
	Lines.push_back(new Number("Frames to wait after first move", 0, 255, 20));
	Lines.push_back(new Number("Frames to wait while scrolling", 0, 255, 5));
	Lines.push_back(new Comment(""));
	Lines.push_back(new Number("Frames to wait after first value change", 0, 255, 20));
	Lines.push_back(new Number("Frames to wait while scrolling values", 0, 255, 5));
	Lines.push_back(new Comment(""));
	Lines.push_back(new Number("Vertical threshold (out of 127)", 10, 120, 65));
	Lines.push_back(new Number("Horizontal threshold (out of 127)", 10, 120, 65));

	Page Navigation("Change the way your inputs affect the menu", Lines, 0);

	//text
	Lines.clear();
	Lines.push_back(new Comment("Change the size and positioning of text"));
	Lines.push_back(new Comment(""));
	Lines.push_back(new Floating("Text scaling", 0.00390625, 1, 0.0625, 0.00390625));
	Lines.push_back(new Floating("Horizontal text start pos", -0x500, 0x500, -470, 5));
	Lines.push_back(new Floating("Vertical text start pos", -0x480, 0x480, -350, 5));
	Lines.push_back(new Floating("Start of changeable values", -0x480, 0x480, 200, 5));
	Lines.push_back(new Floating("Vertical size of each line", 0, 0x100, 0x20, 2));

	Page Text("Change the way the menu is drawn", Lines, 0);


	//main page
	Lines.clear();
	Lines.push_back(new Comment("Legacy TE Code Menu V0"));
	Lines.push_back(new Comment(""));
	Lines.push_back(new Comment("Use a gamecube controller. Using a wiimote will not work and can cause undefined behavior"));
	Lines.push_back(new Comment("Green lines are comments"));
	Lines.push_back(new Comment("Press A on lines with > at the end to enter that submenu"));
	Lines.push_back(new Comment("Press B to go back one submenu, or exit if you are on the main screen"));
	Lines.push_back(new Comment("Up and down on the control stick move the line you are highlighting"));
	Lines.push_back(new Comment("Left and right increment or decrement values"));
	Lines.push_back(new Comment("Every value has a default, maximum, minimum, and rate of change"));
	Lines.push_back(new Comment("You can change the settings of this menu by going to the settings submenu"));
	Lines.push_back(new Comment("Infinite shields are in order of active players, so turn on P1 if P4 was the only player"));
	Lines.push_back(&P1.CalledFromLine);
	Lines.push_back(&P2.CalledFromLine);
	Lines.push_back(&P3.CalledFromLine);
	Lines.push_back(&P4.CalledFromLine);
	Lines.push_back(new Toggle("Draw DI", false));
	Lines.push_back(&Colors.CalledFromLine);
	Lines.push_back(&Navigation.CalledFromLine);
	Lines.push_back(&Text.CalledFromLine);
	Lines.push_back(new Toggle("IASA Overlay", false));
	Page Main("Main", Lines, 0);

	CreateMenu(Main);

	PrintCodeMenu();

	TriggerCodeMenu();

	ControlCodeMenu();

	ActualCodes();
}

void ActualCodes()
{
	int SettingsAddress = CODE_MENU_SETTINGS;

	SettingsAddress += 4; //di draw

	#ifdef IASA_OVERLAY
	TriggerIASAOverlay(); //iasa overlay
	#endif
	SettingsAddress += 4; //IASA

	GeckoIf(SettingsAddress, EQUAL, 1); {
		Gecko32BitWrite(0x901ae19c, 0x42700000); //P1 infinite shield
	}GeckoEndIf();
	SettingsAddress += 4;

	GeckoIf(SettingsAddress, EQUAL, 1); {
		Gecko32BitWrite(0x901ae19c + 0x870, 0x42700000); //P2 infinite shield
	}GeckoEndIf();
	SettingsAddress += 4;

	GeckoIf(SettingsAddress, EQUAL, 1); {
		Gecko32BitWrite(0x901ae19c + 0x870 * 2, 0x42700000); //P3 infinite shield
	}GeckoEndIf();
	SettingsAddress += 4;

	GeckoIf(SettingsAddress, EQUAL, 1); {
		Gecko32BitWrite(0x901ae19c + 0x870 * 3, 0x42700000); //P4 infinite shield
	}GeckoEndIf();
	SettingsAddress += 4;


	ControlCodes();
}

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
			if (x->type == TOGGLE_LINE || x->type == NUMBER_LINE || x->type == FLOATING_LINE) {
				x->Index = MenuID++ * 4;
				Defaults.push_back(x->GetDefault());
			}
			if (x->type == SUB_MENU_LINE) {
				x->SubMenuOffset = EndOffset - CalledPageOffsets[i];
				Pages.push_back(x->SubMenuPtr);
				CalledPageOffsets.push_back(EndOffset);
				EndOffset += x->SubMenuPtr->Size;
			}
		}
	}

	int size = 0;
	bool paddingFlag = false;
	for (auto x : Pages) {
		size += x->Size;
	}

	if (size % 8 == 4)
	{
		size += 4;
		paddingFlag = true;
	}

	WriteIntToFile(0x66200000 + size / 8);
	WriteIntToFile(0); //skip

	for (auto x : Pages) {
		x->WritePage();
	}

	if (paddingFlag) {
		WriteIntToFile(0); //padding
	}
	
	/*WriteIntToFile(0xE0000000);
	WriteIntToFile(0x80008000); //full termiator
	WriteIntToFile(0xE0000000);
	WriteIntToFile(0x80008000); //full termiator*/
}

void CreateDrawSettingsBuffer(int TempReg)
{
	SetRegister(TempReg, DRAW_DELETE_BLOCK_PTR_LOC);

	SetRegs(3, { 1, 0x2A, 0x2B });
	CallBrawlFunc(0x800b8930); //create
	STW(3, TempReg, 0);

	SetRegs(4, { 0x200, 1 });
	CallBrawlFunc(0x800b8b08); //allocMsgBuffer

	LWZ(3, TempReg, 0);
	SetRegister(4, 0x928929e0);
	CallBrawlFunc(0x800b8c7c); //setMsgData

	LWZ(4, TempReg, 0);
	LWZ(3, 4, 8);
	STW(4, 3, 0x1D0);
	SetRegister(5, 0);
	STW(4, 4, 0x4C);
	STW(5, 4, 0x50);
	STW(3, TempReg, -4);
	SetRegister(4, 0x01000000);
	STW(4, 3, 0x4C);
	SetRegister(4, GetHexFromFloat(0.0625));
	STW(4, 3, 0x50);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x68);
	STW(4, 3, 0x24);
	STW(4, 3, 0x28);
	SetRegister(4, GetHexFromFloat(0.973585));
	STW(4, 3, 0x58);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x5C);
	STW(4, 3, 0x2C);
	STW(4, 3, 0x30);
	STW(4, 3, 0x34);
	SetRegister(4, 1);
	STW(4, 3, 0x38);
	SetRegister(4, 1);
	STW(4, 3, 0x3C);
	SetRegister(4, RED);
	STW(4, 3, 8);
	STW(4, 3, 0xC);
	STW(4, 3, 0x10);
	STW(4, 3, 0x14);
	SetRegister(4, 0x80497d7c);
	STW(4, 3, 0x48);
	SetRegister(4, 0);
	STW(4, 3, 0x40);
}

void ControlCodeMenu()
{
	ASMStart(0x80029574);
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
	int ControlStickXReg = 18;
	int ControlStickYReg = 17;
	int CounterReg = 16;
	int ButtonReg = 15;
	int ActionReg = 14;

	//get buttons and control stick
	SetRegister(Reg1, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
	SetRegister(ButtonReg, 0);
	SetRegister(ControlStickXReg, 0);
	SetRegister(ControlStickYReg, 0);
	CounterLoop(CounterReg, 0, 4, 1); {
		LWZU(Reg2, Reg1, BUTTON_PORT_OFFSET);
		OR(ButtonReg, ButtonReg, Reg2);

		LBZ(Reg2, Reg1, GCC_CONTROL_STICK_X_START - PLAY_BUTTON_LOC_START);
		EXTSB(Reg2, Reg2);
		ADD(ControlStickXReg, ControlStickXReg, Reg2);

		LBZ(Reg2, Reg1, GCC_CONTROL_STICK_Y_START - PLAY_BUTTON_LOC_START);
		EXTSB(Reg2, Reg2);
		ADD(ControlStickYReg, ControlStickYReg, Reg2);
	}CounterLoopEnd();

	LoadWordToReg(Reg1, Reg4, CODE_MENU_CONTROL_FLAG);
	If(Reg1, EQUAL_I, CODE_MENU_PRIMED); //check for A press

	SetRegister(Reg1, CODE_MENU_CLOSED);

	ANDI(Reg3, ButtonReg, BUTTON_A);
	If(Reg3, NOT_EQUAL_I, 0); //A is pressed

	SetRegister(Reg1, CODE_MENU_OPEN); //set open

	SetRegister(Reg2, CODE_MENU_BUTTON_MASK_LOC);
	STW(ButtonReg, Reg2, 0);

	EndIf(); //A is pressed

	STW(Reg1, Reg4, 0); //set flag

	EndIf(); //check for A press

	If(Reg1, EQUAL_I, CODE_MENU_OPEN); //run logic

	//prevent buttons from affecting the menu
	SetRegister(Reg2, 0xFFFFFFFF);
	SetRegister(Reg3, MAIN_BUTTON_MASK_LOC - 4);
	CounterLoop(CounterReg, 0, 2, 1); {
		STWU(Reg2, Reg3, 4);
	}CounterLoopEnd();

	//get only pressed button
	LoadWordToReg(Reg2, Reg3, CODE_MENU_BUTTON_MASK_LOC);
	STW(ButtonReg, Reg3, 0); //update mask
	ANDC(ButtonReg, ButtonReg, Reg2);

	GetActionFromInputs(ButtonReg, ControlStickXReg, ControlStickYReg);
	//r3 has action

	ExecuteAction();

	EndIf(); //run logic

	//negate buttons and stick
	LoadWordToReg(Reg1, CODE_MENU_CONTROL_FLAG);
	If(Reg1, EQUAL_I, CODE_MENU_OPEN); {
		SetRegister(Reg6, 0);
	}Else(); {
		SetRegister(Reg6, 0xFFFF);
	}EndIf();
	
	SetRegister(Reg1, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
	SetRegister(Reg2, MAIN_BUTTON_MASK_LOC - 2);
	CounterLoop(CounterReg, 0, 4, 1); {
		LHZU(Reg3, Reg2, 2); //mask
		LWZU(Reg4, Reg1, BUTTON_PORT_OFFSET); //buttons

		AND(Reg5, Reg4, Reg3);
		STH(Reg5, Reg2, 0); //update mask
		ANDC(Reg4, Reg4, Reg3); //apply mask
		STW(Reg4, Reg1, 0); //store new buttons

		LHZ(Reg3, Reg1, GCC_CONTROL_STICK_X_START - PLAY_BUTTON_LOC_START); //stick
		AND(Reg3, Reg3, Reg6); //set to 0 or remain unchanged
		STH(Reg3, Reg1, GCC_CONTROL_STICK_X_START - PLAY_BUTTON_LOC_START); //store new stick
	}CounterLoopEnd();

	RestoreRegisters();
	ASMEnd(0x4e800020); //blr
}

//r3 has action
void ExecuteAction()
{
	int ActionReg = 3;
	int PageReg = 4;
	int LineReg = 5;
	int TempReg = 6;
	int TypeReg = 7;
	int TempReg2 = 8;
	int TempReg3 = 9;

	LoadWordToReg(PageReg, CODE_MENU_PAGE_ADDRESS);
	LWZ(LineReg, PageReg, Page::CURRENT_LINE);
	ADD(LineReg, LineReg, PageReg);
	LWZ(TypeReg, LineReg, Line::TYPE);

	//move
	If(ActionReg, EQUAL_I, MOVE_UP); //move up
	MoveUp(LineReg, PageReg, TypeReg, TempReg, TempReg2);
	EndIf(); //execute move up

	If(ActionReg, EQUAL_I, MOVE_DOWN); //move down
	MoveDown(LineReg, PageReg, TypeReg, TempReg, TempReg2);
	EndIf(); //move


	//change value
	If(ActionReg, EQUAL_I, INCREMENT); //increment
	IncreaseValue(LineReg, TypeReg, TempReg, TempReg2, TempReg3);
	EndIf(); //increment
	
	If(ActionReg, EQUAL_I, DECREMENT); //decrement
	DecreaseValue(LineReg, TypeReg, TempReg, TempReg2, TempReg3);
	EndIf(); //decrement


	//navigate menu
	If(ActionReg, EQUAL_I, ENTER_SUB_MENU); {
		EnterMenu(LineReg, PageReg, TypeReg, TempReg, TempReg2);
	}EndIf();

	If(ActionReg, EQUAL_I, LEAVE_SUB_MENU); {
		LeaveMenu(LineReg, PageReg, TempReg, TempReg2);
	}EndIf();
}

void EnterMenu(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2)
{
	If(TypeReg, EQUAL_I, SUB_MENU_LINE); {
		LWZ(TempReg1, LineReg, SubMenu::SUB_MENU);
		ADD(TempReg2, PageReg, TempReg1);
		NEG(TempReg1, TempReg1);
		STW(TempReg1, TempReg2, Page::PREV_PAGE);
		SetRegister(TempReg1, CODE_MENU_PAGE_ADDRESS);
		STW(TempReg2, TempReg1, 0);
	}EndIf();
}

void LeaveMenu(int LineReg, int PageReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, PageReg, Page::PREV_PAGE);
	If(TempReg1, EQUAL_I, 0); {
		SetRegister(TempReg1, CODE_MENU_PRIMED);
		SetRegister(TempReg2, CODE_MENU_CONTROL_FLAG);
		STW(TempReg1, TempReg2, 0);
	}Else(); {
		ADD(TempReg1, PageReg, TempReg1);
		SetRegister(TempReg2, CODE_MENU_PAGE_ADDRESS);
		STW(TempReg1, TempReg2, 0);
	}EndIf();
}

void DecreaseValue(int LineReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3)
{
	If(TypeReg, EQUAL_I, TOGGLE_LINE); //toggle

	LWZ(TempReg1, LineReg, Toggle::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZUX(TempReg1, TempReg2, TempReg1); //get setting
	XORI(TempReg1, TempReg1, 1); //toggle
	STW(TempReg1, TempReg2, 0);

	Else(); If(TypeReg, EQUAL_I, NUMBER_LINE); //number

	LWZ(TempReg1, LineReg, Toggle::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZUX(TempReg1, TempReg2, TempReg1); //get setting
	LWZ(TempReg3, LineReg, Number::SPEED);
	SUBF(TempReg1, TempReg1, TempReg3);
	
	LWZ(TempReg3, LineReg, Number::MIN);
	If(TempReg1, LESS, TempReg3); {
		STW(TempReg3, TempReg2, 0);
	}Else(); {
		STW(TempReg1, TempReg2, 0);
	}EndIf();

	Else(); If(TypeReg, EQUAL_I, FLOATING_LINE); //number

	LWZ(TempReg1, LineReg, Floating::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LFSUX(2, TempReg2, TempReg1); //get setting
	LFS(1, LineReg, Floating::SPEED);
	FSUB(1, 2, 1);
	
	LFS(2, LineReg, Floating::MIN);
	FCMPU(1, 2, 0);
	BC(3, BRANCH_IF_FALSE, LT);
	STFS(2, TempReg2, 0);
	B(2);
	STFS(1, TempReg2, 0);

	EndIf(); EndIf(); EndIf(); //done
}

void IncreaseValue(int LineReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3)
{
	If(TypeReg, EQUAL_I, TOGGLE_LINE); //toggle

	LWZ(TempReg1, LineReg, Toggle::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZUX(TempReg1, TempReg2, TempReg1); //get setting
	XORI(TempReg1, TempReg1, 1); //toggle
	STW(TempReg1, TempReg2, 0);

	Else(); If(TypeReg, EQUAL_I, NUMBER_LINE); //number

	LWZ(TempReg1, LineReg, Number::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZUX(TempReg1, TempReg2, TempReg1); //get setting
	LWZ(TempReg3, LineReg, Number::SPEED);
	ADD(TempReg1, TempReg1, TempReg3);
	
	LWZ(TempReg3, LineReg, Number::MAX);
	If(TempReg1, GREATER, TempReg3); {
		STW(TempReg3, TempReg2, 0);
	}Else(); {
		STW(TempReg1, TempReg2, 0);
	}EndIf();

	Else(); If(TypeReg, EQUAL_I, FLOATING_LINE); //number

	LWZ(TempReg1, LineReg, Floating::INDEX);
	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LFSUX(2, TempReg2, TempReg1); //get setting
	LFS(1, LineReg, Floating::SPEED);
	FADD(1, 1, 2);
	
	LFS(2, LineReg, Floating::MAX);
	FCMPU(1, 2, 0);
	BC(3, BRANCH_IF_FALSE, GT);
	STFS(2, TempReg2, 0);
	B(2);
	STFS(1, TempReg2, 0);

	EndIf(); EndIf(); EndIf(); //done
}

void MoveUp(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2) {
	If(TypeReg, EQUAL_I, TOGGLE_LINE); //toggle

	LWZ(TempReg1, LineReg, Toggle::UP);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Toggle::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Toggle::COLOR);

	Else(); If(TypeReg, EQUAL_I, SUB_MENU_LINE); //sub menu

	LWZ(TempReg1, LineReg, SubMenu::UP);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, SubMenu::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, SubMenu::COLOR);

	Else(); If(TypeReg, EQUAL_I, NUMBER_LINE); //number

	LWZ(TempReg1, LineReg, Number::UP);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Number::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Number::COLOR);

	Else(); If(TypeReg, EQUAL_I, FLOATING_LINE); //number

	LWZ(TempReg1, LineReg, Floating::UP);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Floating::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Floating::COLOR);

	EndIf(); EndIf(); EndIf(); EndIf(); //done
}

void MoveDown(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2) {
	If(TypeReg, EQUAL_I, TOGGLE_LINE); //toggle

	LWZ(TempReg1, LineReg, Toggle::DOWN);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Toggle::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Toggle::COLOR);

	Else(); If(TypeReg, EQUAL_I, SUB_MENU_LINE); //sub menu

	LWZ(TempReg1, LineReg, SubMenu::DOWN);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, SubMenu::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, SubMenu::COLOR);

	Else(); If(TypeReg, EQUAL_I, NUMBER_LINE); //number

	LWZ(TempReg1, LineReg, Number::DOWN);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Number::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Number::COLOR);

	Else(); If(TypeReg, EQUAL_I, FLOATING_LINE); //number

	LWZ(TempReg1, LineReg, Floating::DOWN);
	STW(TempReg1, PageReg, Page::CURRENT_LINE);

	ADD(TempReg1, TempReg1, PageReg);
	SetRegister(TempReg2, 0);
	STW(TempReg2, LineReg, Floating::COLOR);
	SetRegister(TempReg2, 1);
	STW(TempReg2, TempReg1, Floating::COLOR);

	EndIf(); EndIf(); EndIf(); EndIf(); //done	
}

//returns r3
void GetActionFromInputs(int ButtonReg, int ControlStickXReg, int ControlStickYReg)
{
	//set frame timers
	LoadWordToReg(3, CODE_MENU_SETTINGS + 4 * 18);
	SetRegister(4, FIRST_MOVE_NUM_WAIT_FRAMES);
	STW(3, 4, 0);
	LoadWordToReg(3, CODE_MENU_SETTINGS + 4 * 19);
	SetRegister(4, MOVE_NUM_WAIT_FRAMES);
	STW(3, 4, 0);
	LoadWordToReg(3, CODE_MENU_SETTINGS + 4 * 20);
	SetRegister(4, FIRST_INCREMENT_NUM_WAIT_FRAMES);
	STW(3, 4, 0);
	LoadWordToReg(3, CODE_MENU_SETTINGS + 4 * 21);
	SetRegister(4, INCREMENT_NUM_WAIT_FRAMES);
	STW(3, 4, 0);

	//action checks at the bottom have a higher priority
	int ActionReg = 3;
	SetRegister(ActionReg, NO_ACTION); //default

	int FrameTimerReg = 6;
	LoadWordToReg(FrameTimerReg, 7, MOVE_FRAME_TIMER_LOC);
	ADDI(FrameTimerReg, FrameTimerReg, -1);
	LoadWordToReg(8, CODE_MENU_SETTINGS + 4 * 22); //move threshold
	If(ControlStickYReg, GREATER_OR_EQUAL, 8); //move up
	ControlStickFrameTimerLogic(FrameTimerReg, 7, FIRST_MOVE_NUM_WAIT_FRAMES, MOVE_NUM_WAIT_FRAMES, MOVE_UP);
	EndIf(); //move up

	NEG(8, 8);
	If(ControlStickYReg, LESS_OR_EQUAL, 8); //move down
	ControlStickFrameTimerLogic(FrameTimerReg, 7, FIRST_MOVE_NUM_WAIT_FRAMES, MOVE_NUM_WAIT_FRAMES, MOVE_DOWN);
	EndIf(); //move down

	NEG(8, 8);
	If(ControlStickYReg, LESS_I, 0); {
		NEG(ControlStickYReg, ControlStickYReg);
	}EndIf();
	If(ControlStickYReg, LESS, 8); {
		SetRegister(FrameTimerReg, -1);
		STW(FrameTimerReg, 7, 0);
	}EndIf();


	LoadWordToReg(FrameTimerReg, 7, INCREMENT_FRAME_TIMER_LOC);
	ADDI(FrameTimerReg, FrameTimerReg, -1);
	LoadWordToReg(8, CODE_MENU_SETTINGS + 4 * 23); //increment threshold
	If(ControlStickXReg, GREATER_OR_EQUAL, 8); //increment
	ControlStickFrameTimerLogic(FrameTimerReg, 7, FIRST_INCREMENT_NUM_WAIT_FRAMES, INCREMENT_NUM_WAIT_FRAMES, INCREMENT);
	EndIf(); //increment

	NEG(8, 8);
	If(ControlStickXReg, LESS_OR_EQUAL_I, -INCREMENT_THRESHHOLD); //decrement
	ControlStickFrameTimerLogic(FrameTimerReg, 7, FIRST_INCREMENT_NUM_WAIT_FRAMES, INCREMENT_NUM_WAIT_FRAMES, DECREMENT);
	EndIf(); //decrement

	NEG(8, 8);
	If(ControlStickXReg, LESS_I, 0); {
		NEG(ControlStickXReg, ControlStickXReg);
	}EndIf();
	If(ControlStickXReg, LESS, 8); {
		SetRegister(FrameTimerReg, -1);
		STW(FrameTimerReg, 7, 0);
	}EndIf();


	ANDI(4, ButtonReg, TRIGGER_ENTER_SUB_MENU_BUTTON);
	If(4, NOT_EQUAL_I, 0); //enter sub menu
	SetRegister(ActionReg, ENTER_SUB_MENU);
	EndIf(); //enter sub menu


	ANDI(4, ButtonReg, TRIGGER_LEAVE_SUB_MENU_BUTTON);
	If(4, NOT_EQUAL_I, 0); //leave sub menu
	SetRegister(ActionReg, LEAVE_SUB_MENU);
	EndIf(); //leave sub menu
}

void ControlStickFrameTimerLogic(int FrameReg, int FrameLocReg, int FirstMoveFramesLoc, int MoveFramesLoc, int action)
{
	int ActionReg = 3;
	If(FrameReg, EQUAL_I, -2); //new move

	LoadWordToReg(FrameReg, FirstMoveFramesLoc);
	SetRegister(ActionReg, action);

	Else(); If(FrameReg, EQUAL_I, -1); //normal move

	LoadWordToReg(FrameReg, MoveFramesLoc);
	SetRegister(ActionReg, action);

	EndIf(); EndIf(); //move
	STW(FrameReg, FrameLocReg, 0); //update frame
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

	//set color
	SetRegister(Reg1, CODE_MENU_SETTINGS + 4 * 6 - 4);
	SetRegister(Reg2, CODE_MENU_NORMAL_LINE_COLOR_LOC - 1);
	CounterLoop(Reg3, 0, 12, 1); {
		LWZU(Reg4, Reg1, 4); //get color
		STBU(Reg4, Reg2, 1); //store color
	}CounterLoopEnd();

	CreateDrawSettingsBuffer(Reg3);

	LoadWordToReg(Reg1, CODE_MENU_PAGE_ADDRESS);
	LoadWordToReg(Reg2, DRAW_SETTINGS_PTR_LOC);
	PrintPage(Reg1, Reg2, Reg3, Reg4, Reg5, Reg6, Reg7, Reg8);

	/*LoadWordToReg(Reg6, DRAW_SETTINGS_PTR_LOC);
	SetRegister(Reg4, STRING_BUFFER);
	SetRegister(Reg5, 0x2020);
	SetRegister(Reg1, GCC_CONTROL_STICK_X_START - BUTTON_PORT_OFFSET);
	CounterLoop(Reg3, 0, 4, 1); {
		LBZU(Reg2, Reg1, BUTTON_PORT_OFFSET); //xpos
		EXTSB(Reg2, Reg2);
		SetRegister(Reg2, SPRINTF_INT_TEXT);
		SprintF(Reg2, {});
		ADDI(Reg2, 3, 2);
		STWX(Reg5, Reg4, 3);
		SetRegister(Reg1, STRING_BUFFER - 1);
		PrintString(Reg1, Reg2, Reg6);

		LBZ(Reg2, Reg1, 1); //ypos
		EXTSB(Reg2, Reg2);
		SetRegister(Reg2, SPRINTF_INT_TEXT);
		SprintF(Reg2, {});
		ADDI(Reg2, 3, 2);
		STWX(Reg5, Reg4, 3);
		SetRegister(Reg1, STRING_BUFFER - 1);
		PrintString(Reg1, Reg2, Reg6);
	}CounterLoopEnd();*/
	
	LoadWordToReg(3, DRAW_DELETE_BLOCK_PTR_LOC);
	SetRegister(4, 0x81);
	STB(4, 3, 0);
	SetRegister(4, 1);
	CallBrawlFunc(0x800b8a0c); //delete msgBuffer

	Else(); //menu is open

	CallBrawlFunc(0x801ace24); //DrawXlu

	EndIf(); //menu is open

	RestoreRegisters();
	ASMEnd();
}

void TriggerCodeMenu()
{
	GeckoIf(0x8119969c, EQUAL, 0x9421fff0);
	//r3 + 0x40 has location
	ASMStart(0x8119969c);
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
	LWZ(Reg3, 3, 0x40);
	If(Reg3, EQUAL_I, 8); //on tourney
	
	If(Reg2, NOT_EQUAL_I, 1); //first time

	SetRegister(Reg2, 1);
	STW(Reg2, Reg1, 0); //clear first time flag

	//set up scroll speed
	SetRegister(Reg2, 6);
	SetRegister(Reg3, MOVE_FRAME_TIMER_LOC - 4);
	SetRegister(Reg4, 0);
	While(Reg4, LESS_I, 6);
	{
		STWU(Reg2, Reg3, 4);
		ADDI(Reg4, Reg4, 1);
	}
	EndWhile();

	SetRegister(Reg4, CODE_MENU_SETTINGS - 4);
	/*SetRegister(Reg3, 0);
	SetRegister(Reg2, CODE_MENU_END);
	While(Reg4, LESS_L, Reg2); //clear settings

	STWU(Reg3, Reg4, 4);

	EndWhile(); //clear settings*/

	for (auto x : Defaults) {
		SetRegister(Reg2, x);
		STWU(Reg2, Reg4, 4);
	}

	SetRegister(Reg2, "\3Off"s);
	SetRegister(Reg3, OFF_TEXT_LOC);
	STW(Reg2, Reg3, 0);
	SetRegister(Reg2, "\2On\0"s);
	SetRegister(Reg3, ON_TEXT_LOC);
	STW(Reg2, Reg3, 0);

	SetRegister(Reg2, "%F\0\0"s);
	SetRegister(Reg3, SPRINTF_FLOAT_TEXT);
	STW(Reg2, Reg3, 0);
	SetRegister(Reg2, "%d\0\0"s);
	SetRegister(Reg3, SPRINTF_INT_TEXT);
	STW(Reg2, Reg3, 0);

	EndIf(); //first time

	//set open flag last to prevent thread errors
	LoadWordToReg(Reg1, Reg2, CODE_MENU_CONTROL_FLAG);
	If(Reg1, NOT_EQUAL_I, CODE_MENU_OPEN); {
		SetRegister(Reg1, CODE_MENU_PRIMED);
		STW(Reg1, Reg2, 0);
	}EndIf();

	EndIf(); //on rotation

	RestoreRegisters();
	ASMEnd(0x9421fff0); //stwu sp, sp, -0x10
	GeckoEndIf();
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

void SetTextSize(int FPSizeReg, int SettingsPtrReg)
{
	STFS(FPSizeReg, SettingsPtrReg, 0x50);
}

void NewLine(int SettingsPtrReg)
{
	//SetRegister(3, GetHexFromFloat(INITIAL_XPOS));
	LoadWordToReg(3, CODE_MENU_SETTINGS + 4 * 25); //xpos
	LFS(0, SettingsPtrReg, DRAW_BUFFER_YPOS_OFFSET);
	//SetFloatingRegister(1, 4, 0x20);
	SetRegister(4, CODE_MENU_SETTINGS + 4 * 28);
	LFS(1, 4, 0);
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

	SetRegister(Reg1, CODE_MENU_SETTINGS + 4 * 24);
	LFS(1, Reg1, 0);
	SetTextSize(1, SettingPtrReg);

	//set initial text pos
	//SetRegister(Reg1, GetHexFromFloat(INITIAL_XPOS));
	//SetRegister(Reg2, GetHexFromFloat(INITIAL_YPOS));
	LoadWordToReg(Reg1, CODE_MENU_SETTINGS + 4 * 25); //xpos
	LoadWordToReg(Reg2, CODE_MENU_SETTINGS + 4 * 26); //ypos
	SetTextPos(Reg1, Reg2, SettingPtrReg);

	/*SetRegister(Reg1, STRING_BUFFER);
	WriteStringToMem("Control Stick Value: XPos: %d YPos: %d", Reg1);

	LoadByteToReg(Reg2, Reg3, GCC_CONTROL_STICK_X_START);
	LBZ(Reg3, Reg3, 1);
	SprintF(Reg1, { Reg2, Reg3 });
	MR(Reg1, 3); //num chars
	SetRegister(Reg2, STRING_BUFFER);
	LoadWordToReg(Reg3, DRAW_SETTINGS_PTR_LOC);
	PrintString(Reg2, Reg1, Reg3);
	NewLine(Reg3);*/

	//LWZ(Reg1, PageReg, Page::XPOS); //get value xpos
	LoadWordToReg(Reg1, CODE_MENU_SETTINGS + 4 * 27);
	ADDI(Reg2, PageReg, Page::FIRST_LINE_OFFSET); //first line
	SetRegister(3, 1);
	While(3, NOT_EQUAL_I, 0); //draw line loop

	LWZ(Reg4, Reg2, Line::TYPE);
	If(Reg4, EQUAL_I, TOGGLE_LINE); //is toggle

	PrintToggleLine(Reg2, Reg1, SettingPtrReg, Reg4, Reg5);

	Else(); If(Reg4, EQUAL_I, SUB_MENU_LINE); //is sub menu

	PrintSubMenuLine(Reg2, SettingPtrReg, Reg4, Reg5);

	Else(); If(Reg4, EQUAL_I, COMMENT_LINE); //is comment

	PrintCommentLine(Reg2, SettingPtrReg, Reg4, Reg5);

	Else(); If(Reg4, EQUAL_I, NUMBER_LINE); //is comment

	PrintNumberLine(Reg2, Reg1, SettingPtrReg, Reg4, Reg5);

	Else(); If(Reg4, EQUAL_I, FLOATING_LINE); //is floating

	PrintFloatingLine(Reg2, Reg1, SettingPtrReg, Reg4, Reg5);

	EndIf(); EndIf(); EndIf(); EndIf(); EndIf(); //done

	LWZ(3, Reg2, Line::SIZE);
	ADD(Reg2, Reg2, 3); //next line
	EndWhile(); //draw line loop
}

void PrintFloatingLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, Floating::COLOR);
	If(TempReg1, EQUAL_I, 1); {
		LoadWordToReg(TempReg1, CODE_MENU_SELECTED_LINE_COLOR_LOC);
	}Else(); {
		LoadWordToReg(TempReg1, CODE_MENU_NORMAL_LINE_COLOR_LOC);
	}EndIf();
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, Floating::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	SetTextXPos(ValueXPosReg, SettingsPtrReg);

	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZ(TempReg1, LinePtrReg, Floating::INDEX);
	LFSX(1, TempReg1, TempReg2); //value
	SetRegister(TempReg2, SPRINTF_FLOAT_TEXT);
	SprintF(TempReg2, {}, {1});

	ADDI(TempReg2, 3, 0);
	SetRegister(TempReg1, STRING_BUFFER - 1);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

void PrintToggleLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, Toggle::COLOR);
	If(TempReg1, EQUAL_I, 1); {
		LoadWordToReg(TempReg1, CODE_MENU_SELECTED_LINE_COLOR_LOC);
	}Else(); {
		LoadWordToReg(TempReg1, CODE_MENU_NORMAL_LINE_COLOR_LOC);
	}EndIf();
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, Toggle::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	SetTextXPos(ValueXPosReg, SettingsPtrReg);

	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZ(TempReg1, LinePtrReg, Toggle::INDEX);
	//RLWINM(TempReg1, TempReg1, 2, 0, 31); //<<2
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

void PrintNumberLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, Number::COLOR);
	If(TempReg1, EQUAL_I, 1); {
		LoadWordToReg(TempReg1, CODE_MENU_SELECTED_LINE_COLOR_LOC);
	}Else(); {
		LoadWordToReg(TempReg1, CODE_MENU_NORMAL_LINE_COLOR_LOC);
	}EndIf();
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, Number::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	SetTextXPos(ValueXPosReg, SettingsPtrReg);

	SetRegister(TempReg2, CODE_MENU_SETTINGS);
	LWZ(TempReg1, LinePtrReg, Number::INDEX);
	LWZX(TempReg1, TempReg1, TempReg2); //value
	SetRegister(TempReg2, SPRINTF_INT_TEXT);
	SprintF(TempReg2, { TempReg1 });

	ADDI(TempReg2, 3, 0);
	SetRegister(TempReg1, STRING_BUFFER - 1);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

void PrintCommentLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LoadWordToReg(TempReg1, CODE_MENU_COMMENT_LINE_COLOR_LOC);
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, Comment::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

void PrintSubMenuLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, LinePtrReg, SubMenu::COLOR);
	If(TempReg1, EQUAL_I, 1); {
		LoadWordToReg(TempReg1, CODE_MENU_SELECTED_LINE_COLOR_LOC);
	}Else(); {
		LoadWordToReg(TempReg1, CODE_MENU_NORMAL_LINE_COLOR_LOC);
	}EndIf();
	SetTextColor(TempReg1, SettingsPtrReg);

	ADDI(TempReg1, LinePtrReg, SubMenu::TEXT);
	LBZ(TempReg2, TempReg1, 0);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}