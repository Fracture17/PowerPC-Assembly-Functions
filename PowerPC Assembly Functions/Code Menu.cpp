#include "stdafx.h"
#include "Code Menu.h"
#include "DrawDI.h"
#include "IASA Overlay.h"
#include "Control Codes.h"

int MenuID = 0;

int DI_DRAW_INDEX = -1;
int DEBUG_MODE_INDEX = -1;
int DISPLAY_HITBOXES_INDEX = -1;
int DISPLAY_COLLISION_INDEX = -1;
int DISPLAY_LEDGEGRAB_INDEX = -1;
int CHARACTER_SELECT_P1_INDEX = -1;
int CHARACTER_SELECT_P2_INDEX = -1;
int CHARACTER_SELECT_P3_INDEX = -1;
int CHARACTER_SELECT_P4_INDEX = -1;
int INFINITE_SHIELDS_P1_INDEX = -1;
int INFINITE_SHIELDS_P2_INDEX = -1;
int INFINITE_SHIELDS_P3_INDEX = -1;
int INFINITE_SHIELDS_P4_INDEX = -1;
int CAMERA_LOCK_INDEX = -1;
int INFINITE_FRIENDLIES_INDEX = -1;

void CodeMenu()
{
	//player pages
	vector<Line*> P1Lines;
	P1Lines.push_back(new Toggle("Infinite Shield", false, INFINITE_SHIELDS_P1_INDEX));
	P1Lines.push_back(new Selection("P1 Character Select", CHARACTER_LIST, CHARACTER_ID_LIST, 0, CHARACTER_SELECT_P1_INDEX));
	Page P1("Player 1 Codes", P1Lines);

	vector<Line*> P2Lines;
	P2Lines.push_back(new Toggle("Infinite Shield", false, INFINITE_SHIELDS_P2_INDEX));
	P2Lines.push_back(new Selection("P2 Character Select", CHARACTER_LIST, CHARACTER_ID_LIST, 0, CHARACTER_SELECT_P2_INDEX));
	Page P2("Player 2 Codes", P2Lines);

	vector<Line*> P3Lines;
	P3Lines.push_back(new Toggle("Infinite Shield", false, INFINITE_SHIELDS_P3_INDEX));
	P3Lines.push_back(new Selection("P3 Character Select", CHARACTER_LIST, CHARACTER_ID_LIST, 0, CHARACTER_SELECT_P3_INDEX));
	Page P3("Player 3 Codes", P3Lines);

	vector<Line*> P4Lines;
	P4Lines.push_back(new Toggle("Infinite Shield", false, INFINITE_SHIELDS_P4_INDEX));
	P4Lines.push_back(new Selection("P4 Character Select", CHARACTER_LIST, CHARACTER_ID_LIST, 0, CHARACTER_SELECT_P4_INDEX));
	Page P4("Player 4 Codes", P4Lines);
	
	//debug mode
	vector<Line*> DebugLines;
	DebugLines.push_back(new Comment("Turning on debug mode debug pauses the game when start is pressed"));
	DebugLines.push_back(new Toggle("Toggle Debug mode", false, DEBUG_MODE_INDEX));
	DebugLines.push_back(new Selection("Toggle Hitbox display", { "OFF", "ON", "Interpolate" }, 0, DISPLAY_HITBOXES_INDEX));
	DebugLines.push_back(new Toggle("Toggle Collision display", false, DISPLAY_COLLISION_INDEX));
	DebugLines.push_back(new Toggle("Toggle Ledgegrab display", false, DISPLAY_LEDGEGRAB_INDEX));
	DebugLines.push_back(new Toggle("Toggle Camera Lock", false, CAMERA_LOCK_INDEX));
	Page DebugMode("Debug mode settings", DebugLines);

	//main page
	vector<Line*> MainLines;
	MainLines.push_back(new Comment("Legacy TE Code Menu V1.5"));
	MainLines.push_back(new Comment(""));
	MainLines.push_back(new Comment("Use a gamecube controller. Using a wiimote will not work and can cause undefined behavior"));
	MainLines.push_back(new Comment("Green lines are comments"));
	MainLines.push_back(new Comment("Press A on lines with > at the end to enter that subpage"));
	MainLines.push_back(new Comment("Press B to go back one subpage, or exit if you are on the main screen"));
	MainLines.push_back(new Comment("Up and down on the control stick move the line you are highlighting"));
	MainLines.push_back(new Comment("Left and right increment or decrement values"));
	MainLines.push_back(new Comment("Every value has a default, maximum, minimum, and rate of change"));
	MainLines.push_back(&P1.CalledFromLine);
	MainLines.push_back(&P2.CalledFromLine);
	MainLines.push_back(&P3.CalledFromLine);
	MainLines.push_back(&P4.CalledFromLine);
	MainLines.push_back(new Toggle("Draw DI", false, DI_DRAW_INDEX));
	MainLines.push_back(&DebugMode.CalledFromLine);
	MainLines.push_back(new Selection("Infinite Friendlies", { "OFF", "Same Stage", "Random Stage" }, 0, INFINITE_FRIENDLIES_INDEX));
	//Lines.push_back(new Toggle("IASA Overlay", false));
	Page Main("Main", MainLines);

	CreateMenu(Main);

	PrintCodeMenu();

	PrimeCodeMenu();

	ControlCodeMenu();

	ActualCodes();

	MenuFile.close();
}

void ActualCodes()
{
	if (DI_DRAW_INDEX != -1) {
		DrawDI();
	}

	ControlCodes();
}

void CreateMenu(Page MainPage)
{
	//setup header
	vector<u8> Header;
	AddValueToByteArray(START_OF_CODE_MENU_SETTINGS + MenuID, Header); //current page ptr
	//line colors
	AddValueToByteArray(WHITE, Header); //normal line color
	AddValueToByteArray(YELLOW, Header); //highlighted line color
	AddValueToByteArray(BLUE, Header); //changed line color
	AddValueToByteArray(YELLOW, Header); //changed and highlighted line color
	AddValueToByteArray(GREEN, Header); //comment line color
	//frame timers
	AddValueToByteArray(0, Header); //move frame timer
	AddValueToByteArray(0, Header); //value frame timer
	AddValueToByteArray(0, Header); //frame advance frame timer
	//control flags
	AddValueToByteArray(CODE_MENU_CLOSED, Header); //prev flag
	AddValueToByteArray(CODE_MENU_CLOSED, Header); //current flag
	AddValueToByteArray(0, Header); //infinite friendlies flag
	//button mask
	AddValueToByteArray(0, Header); //code menu mask
	AddValueToByteArray(0, Header); //button activator mask
	AddValueToByteArray(0, Header); //main mask
	AddValueToByteArray(0, Header); //main mask
	//old debug state
	AddValueToByteArray(0, Header); //old debug state
	AddValueToByteArray(0, Header); //camera lock state
	//old camera pos
	AddValueToByteArray(0, Header); //old camera pos
	//reset line stack
	AddValueToByteArray(RESET_LINES_STACK_LOC, Header); //reset line stack
	for (int i = 0; i < MAX_SUBPAGE_DEPTH + 1; i++) { AddValueToByteArray(0, Header); }
	//address arrays
	//character switcher
	AddValueToByteArray(CHARACTER_SELECT_P1_INDEX, Header); //P1
	AddValueToByteArray(CHARACTER_SELECT_P2_INDEX, Header); //P2
	AddValueToByteArray(CHARACTER_SELECT_P3_INDEX, Header); //P3
	AddValueToByteArray(CHARACTER_SELECT_P4_INDEX, Header); //P4
	//infinite shields
	AddValueToByteArray(INFINITE_SHIELDS_P1_INDEX, Header); //P1
	AddValueToByteArray(INFINITE_SHIELDS_P2_INDEX, Header); //P2
	AddValueToByteArray(INFINITE_SHIELDS_P3_INDEX, Header); //P3
	AddValueToByteArray(INFINITE_SHIELDS_P4_INDEX, Header); //P4

	//draw settings buffer
	vector<u32> DSB(0x200 / 4, 0);
	DSB[0x4 / 4] = 0xFFFFFFFF;
	DSB[0x38 / 4] = 1;
	DSB[0x3C / 4] = 1;
	DSB[0x4C / 4] = 0x01000000;
	DSB[0x50 / 4] = 0x3D800000;
	DSB[0x58 / 4] = 0x3f793cde;
	DSB[(0x100 + 0x4C) / 4] = DRAW_SETTINGS_BUFFER_LOC + 0x100;
	DSB[0x1D0 / 4] = DRAW_SETTINGS_BUFFER_LOC + 0x100;
	for (auto x : DSB) {
		AddValueToByteArray(x, Header);
	}

	if (START_OF_CODE_MENU_SETTINGS - START_OF_CODE_MENU != Header.size()) {
		cout << "Messed up header\n";
		exit(-1);
	}

	//defaults
	for (int x : Defaults) {
		AddValueToByteArray(x, Header);
	}

	copy(Header.begin(), Header.end(), ostreambuf_iterator<char>(MenuFile));

	//make pages
	vector<Page*> Pages(1, &MainPage);
	vector<int> CalledPageOffsets(1, 0);
	int EndOffset = MainPage.Size;
	for (int i = 0; i < Pages.size(); i++) {
		for (Line* &x : Pages[i]->Lines) {
			if (x->type == SUB_MENU_LINE) {
				x->SubMenuOffset = EndOffset - CalledPageOffsets[i];
				Pages.push_back(x->SubMenuPtr);
				CalledPageOffsets.push_back(EndOffset);
				EndOffset += x->SubMenuPtr->Size;
			}
		}
	}

	for (auto x : Pages) {
		x->WritePage();
	}
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
	int CharacterBufferReg = 21;
	int MainBufferReg = 20;
	int OpenFlagReg = 19;
	int ControlStickXReg = 18;
	int ControlStickYReg = 17;
	int CounterReg = 16;
	int ButtonReg = 15;
	int ActionReg = 14;

	LoadWordToReg(OpenFlagReg, Reg4, CODE_MENU_CONTROL_FLAG);
	LoadWordToReg(Reg3, Reg5, IS_DEBUG_PAUSED);

	SetRegister(Reg1, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
	SetRegister(Reg7, 0x804DE4B0 - 8);
	SetRegister(ButtonReg, 0);
	SetRegister(ControlStickXReg, 0);
	SetRegister(ControlStickYReg, 0);
	CounterLoop(CounterReg, 0, 4, 1); {
		LHZU(Reg8, Reg7, 8);
		LWZU(Reg2, Reg1, BUTTON_PORT_OFFSET);
		ANDI(Reg8, Reg8, 0x1000);
		OR(ButtonReg, ButtonReg, Reg2);
		OR(ButtonReg, ButtonReg, Reg8);

		ANDI(Reg8, Reg2, BUTTON_L | BUTTON_R | BUTTON_DPAD);
		If(Reg8, GREATER_I, BUTTON_L | BUTTON_R); {
			//L + R + Dpad is pressed
			If(OpenFlagReg, NOT_EQUAL_I, CODE_MENU_OPEN); {
				SetRegister(OpenFlagReg, CODE_MENU_TRIGGERED);
			}EndIf();
		}EndIf();
		
		LBA(Reg2, Reg1, GCC_CONTROL_STICK_X_START - PLAY_BUTTON_LOC_START);
		ADD(ControlStickXReg, ControlStickXReg, Reg2);

		LBA(Reg2, Reg1, GCC_CONTROL_STICK_Y_START - PLAY_BUTTON_LOC_START);
		ADD(ControlStickYReg, ControlStickYReg, Reg2);
	}CounterLoopEnd();
	
	If(OpenFlagReg, EQUAL_I, CODE_MENU_PRIMED); {
		//check for A press
		SetRegister(OpenFlagReg, CODE_MENU_CLOSED);

		ANDI(Reg8, ButtonReg, BUTTON_A | BUTTON_START);
		If(Reg8, NOT_EQUAL_I, 0); {
			//A or start is pressed
			SetRegister(OpenFlagReg, CODE_MENU_TRIGGERED); //set open
		}EndIf();
	}EndIf();

	If(OpenFlagReg, EQUAL_I, CODE_MENU_TRIGGERED); {
		SetRegister(Reg2, OLD_DEBUG_STATE_LOC);
		SetRegister(OpenFlagReg, CODE_MENU_OPEN); //set open
		STW(Reg3, Reg2, 0); //store debug state

		//save and set camera pos
		SetRegister(Reg3, OLD_CAMERA_POS_LOC);
		LoadWordToReg(Reg1, 0x805b6df8);
		STW(Reg1, Reg3, 0);

		SetRegister(Reg2, CODE_MENU_BUTTON_MASK_LOC);
		STW(ButtonReg, Reg2, 0);
	}EndIf();

	STW(OpenFlagReg, Reg4, 0); //set flag

	If(OpenFlagReg, EQUAL_I, CODE_MENU_OPEN); {
		//prevent buttons from affecting the game
		SetRegister(Reg2, 0xFFFFFFFF);
		SetRegister(Reg3, MAIN_BUTTON_MASK_LOC - 4);
		CounterLoop(CounterReg, 0, 2, 1); {
			STWU(Reg2, Reg3, 4);
		}CounterLoopEnd();

		SetRegister(Reg1, 1);
		STW(Reg1, Reg5, 0); //set debug paused

		//get only pressed button
		LoadWordToReg(Reg2, Reg3, CODE_MENU_BUTTON_MASK_LOC);
		STW(ButtonReg, Reg3, 0); //update mask
		ANDI(Reg2, Reg2, ~BUTTON_Z); //let Z be held
		ANDC(ButtonReg, ButtonReg, Reg2);

		GetActionFromInputs(ButtonReg, ControlStickXReg, ControlStickYReg, ActionReg);

		ExecuteAction(ActionReg);

	}EndIf(); //run logic
	
	SetRegister(Reg1, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
	SetRegister(Reg2, MAIN_BUTTON_MASK_LOC - 2);
	CounterLoop(CounterReg, 0, 4, 1); {
		LHZU(Reg3, Reg2, 2); //mask
		LWZU(Reg4, Reg1, BUTTON_PORT_OFFSET); //buttons

		AND(Reg5, Reg4, Reg3);
		STH(Reg5, Reg2, 0); //update mask
		ANDC(Reg4, Reg4, Reg3); //apply mask
		STW(Reg4, Reg1, 0); //store new buttons
	}CounterLoopEnd();

	LoadWordToReg(Reg1, Reg3, 0x80584000);
	ANDI(Reg6, ButtonReg, BUTTON_Z);
	LoadWordToReg(Reg4, Reg5, FRAME_ADVANCE_FRAME_TIMER);
	SetRegister(Reg2, ~(0x10000000 | 0x00100000)); //stop all debug commands except pause and frame advance
	If(Reg6, NOT_EQUAL_I, 0); {
		Decrement(Reg4);
		If(Reg4, LESS_OR_EQUAL_I, 0); {
			If(Reg4, LESS_I, 0); {
				SetRegister(Reg4, FIRST_FRAME_ADVANCE_NUM_WAIT_FRAMES - FRAME_ADVANCE_NUM_WAIT_FRAMES);
			}EndIf();
			ANDIS(Reg1, Reg1, ~0x0010); //allow frame advance
			ADDI(Reg4, Reg4, FRAME_ADVANCE_NUM_WAIT_FRAMES);
		}EndIf();
	}Else(); {
		SetRegister(Reg4, 0);
	}EndIf();
	STW(Reg4, Reg5, 0);

	If(OpenFlagReg, EQUAL_I, CODE_MENU_OPEN); {
		SetRegister(Reg2, -1); //stop all debug commands
	}EndIf();
	OR(Reg1, Reg1, Reg2);
	STW(Reg1, Reg3, 0);
	//stop Z and start if in menu

	ApplyMenuSetting(DEBUG_MODE_INDEX, 0x80583FFC + 3, Reg1, Reg2, 1);

	ApplyMenuSetting(DISPLAY_HITBOXES_INDEX, 0x80583FFC + 1, Reg1, Reg2, 1);

	ApplyMenuSetting(DISPLAY_LEDGEGRAB_INDEX, 0x80583FF8 + 1, Reg1, Reg2, 1);

	ApplyMenuSetting(DISPLAY_COLLISION_INDEX, 0x80583FF4 + 3, Reg1, Reg2, 1);

	ApplyMenuSetting(CAMERA_LOCK_INDEX, 0x80583FF8 + 3, Reg1, Reg2, 1);

	//port based codes
	LoadWordToReg(Reg7, IS_IN_GAME_FLAG);
	If(OpenFlagReg, NOT_EQUAL_I, CODE_MENU_OPEN); {
		If(Reg7, EQUAL_I, 1); {
			//in match
			LoadWordToReg(MainBufferReg, MAIN_BUFFER_PTR);
			LWZU(CharacterBufferReg, MainBufferReg, 4);
			While(CharacterBufferReg, NOT_EQUAL_I, 0); {
				LWZ(Reg8, CharacterBufferReg, CHR_BUFFER_PORT_OFFSET);

				if (CHARACTER_SELECT_P1_INDEX != -1) {
					GetArrayValueFromIndex(CHARACTER_SWITCHER_ARRAY_LOC, Reg8, 0, 3); {
						LWZ(Reg2, CharacterBufferReg, CHR_BUFFER_INFO_PTR_OFFSET);
						LWZ(3, 3, 4); //get setting
						LBZ(4, Reg2, 0); //get current ID
						If(4, NOT_EQUAL, 3); {
							SetRegister(Reg1, 0);
							STB(3, Reg2, 0);
							STB(Reg1, Reg2, 5); //force costume to 0
						}EndIf();
					}EndIf(); EndIf();
				}

				if (INFINITE_SHIELDS_P1_INDEX != -1) {
					GetArrayValueFromIndex(INIFINITE_SHIELDS_ARRAY_LOC, Reg8, 0, 3); {
						LWZ(3, 3, 0);
						If(3, EQUAL_I, 1); {
							LWZ(Reg1, CharacterBufferReg, CHR_BUFFER_VARIABLES_ADDRESS_OFFSET);
							SetRegister(Reg2, 0x42700000);
							STW(Reg2, Reg1, 0x19C);
						}EndIf();
					}EndIf(); EndIf();
				}

				LWZU(CharacterBufferReg, MainBufferReg, 8);
			}EndWhile();
		}EndIf();
	}EndIf();

	RestoreRegisters();
	ASMEnd(0x4e800020); //blr
}

void ApplyMenuSetting(int Index, int Destination, int reg1, int reg2, int size)
{
	if (Index != -1) {
		LoadWordToReg(reg1, Index);
		SetRegister(reg2, Destination);
		if (size == 4) { STW(reg1, reg2, 0); }
		if (size == 2) { STH(reg1, reg2, 0); }
		if (size == 1) { STB(reg1, reg2, 0); }
	}
}

void ExecuteAction(int ActionReg)
{
	int PageReg = 4;
	int LineReg = 5;
	int TempReg1 = 6;
	int TypeReg = 7;
	int TempReg2 = 8;
	int TempReg3 = 9;
	int TempReg4 = 10;
	int TempReg5 = 11;
	int TempReg6 = 12;

	int move = GetNextLabel();

	LoadWordToReg(PageReg, CURRENT_PAGE_PTR_LOC);
	LWZ(LineReg, PageReg, Page::CURRENT_LINE_OFFSET);
	ADD(LineReg, LineReg, PageReg);
	LBZ(TypeReg, LineReg, Line::TYPE);

	//move
	If(ActionReg, EQUAL_I, MOVE_UP); {
		//move up
		LHZ(TempReg1, LineReg, Line::UP);

		JumpToLabel(move);
	}EndIf();
	If(ActionReg, EQUAL_I, MOVE_DOWN); {
		//move down
		LHZ(TempReg1, LineReg, Line::DOWN);

		Label(move);
		Move(LineReg, PageReg, TempReg1, TempReg2, TempReg3);
	}EndIf();


	//change value
	If(ActionReg, EQUAL_I, INCREMENT); //increment
	IncreaseValue(LineReg, PageReg, TypeReg, TempReg1, TempReg2, TempReg3, TempReg4, TempReg5);
	EndIf(); //increment
	
	If(ActionReg, EQUAL_I, DECREMENT); //decrement
	DecreaseValue(LineReg, PageReg, TypeReg, TempReg1, TempReg2, TempReg3, TempReg4, TempReg5);
	EndIf(); //decrement


	//reset to defaults
	SetRegister(TempReg1, RESET_LINES_STACK_LOC);
	If(ActionReg, EQUAL_I, RESET_LINE); {
		ResetLine(LineReg, PageReg, TempReg1, TypeReg, TempReg2, TempReg3, TempReg4);
	}EndIf();

	If(ActionReg, EQUAL_I, RESET_PAGE); {
		PushOnStack(PageReg, TempReg1, TempReg2);
	}EndIf();

	//reset page if applicable
	ResetPage(TempReg1, TempReg2, TempReg3, TempReg4, TempReg5, TempReg6, 3);

	//navigate menu
	If(ActionReg, EQUAL_I, ENTER_SUB_MENU); {
		EnterMenu(LineReg, PageReg, TypeReg, TempReg1, TempReg2);
	}EndIf();

	If(ActionReg, EQUAL_I, LEAVE_SUB_MENU); {
		LeaveMenu(PageReg, TempReg1, TempReg2, TempReg3, TempReg4, TempReg5, TempReg6);
	}EndIf();

	If(ActionReg, EQUAL_I, EXIT_MENU); {
		ExitMenu();
	}EndIf();
}

void ResetLine(int LineReg, int PageReg, int StackReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3)
{
	LBZ(TempReg2, LineReg, Line::COLOR);
	LWZ(TempReg1, PageReg, Page::NUM_CHANGED_LINES);
	RLWINM(TempReg3, TempReg2, 29, 31, 31);
	ANDI(TempReg2, TempReg2, ~0x8);
	SUBF(TempReg1, TempReg1, TempReg3);
	STB(TempReg2, LineReg, Line::COLOR);
	STW(TempReg1, PageReg, Page::NUM_CHANGED_LINES);

	If(TypeReg, LESS_OR_EQUAL_I, HAS_VALUE_LIMIT); {
		SetRegister(TempReg3, START_OF_CODE_MENU_SETTINGS);
		LWZ(TempReg1, LineReg, Line::DEFAULT);
		LHZ(TempReg2, LineReg, Line::INDEX);
		STWX(TempReg1, TempReg3, TempReg2);
	}Else(); If(TypeReg, EQUAL_I, SUB_MENU_LINE); {
		LHZ(TempReg1, LineReg, Line::SUB_MENU);
		ADD(TempReg1, TempReg1, PageReg);
		PushOnStack(TempReg1, StackReg, TempReg2);
	}EndIf(); EndIf();
}

void ResetPage(int StackReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5, int TempReg6)
{
	IterateStack(TempReg1, StackReg, TempReg2); {
		SetRegister(TempReg6, 1);
		ADDI(TempReg2, TempReg1, Page::FIRST_LINE_OFFSET); //first line
		While(TempReg6, NOT_EQUAL_I, 0); {
			LBZ(TempReg3, TempReg2, Line::TYPE);
			ResetLine(TempReg2, TempReg1, StackReg, TempReg3, TempReg4, TempReg5, TempReg6);

			LHZ(TempReg6, TempReg2, Line::SIZE);
			ADD(TempReg2, TempReg2, TempReg6); //next line
		}EndWhile();
	}IterateStackEnd();
}

void ExitMenu()
{
	LoadWordToReg(3, 4, PREV_CODE_MENU_CONTROL_FLAG);
	STW(3, 4, CODE_MENU_CONTROL_FLAG - PREV_CODE_MENU_CONTROL_FLAG);
	LoadWordToReg(3, OLD_DEBUG_STATE_LOC);
	SetRegister(4, IS_DEBUG_PAUSED);
	STW(3, 4, 0);
}

void EnterMenu(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2)
{
	If(TypeReg, EQUAL_I, SUB_MENU_LINE); {
		LHZ(TempReg1, LineReg, SubMenu::SUB_MENU);
		ADD(TempReg2, PageReg, TempReg1);
		NEG(TempReg1, TempReg1);
		STW(TempReg1, TempReg2, Page::PREV_PAGE);
		SetRegister(TempReg1, CURRENT_PAGE_PTR_LOC);
		STW(TempReg2, TempReg1, 0);
	}EndIf();
}

void LeaveMenu(int PageReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5, int TempReg6)
{
	LWZ(TempReg1, PageReg, Page::PREV_PAGE);
	If(TempReg1, EQUAL_I, 0); {
		LoadWordToReg(TempReg1, TempReg2, PREV_CODE_MENU_CONTROL_FLAG);
		STW(TempReg1, TempReg2, CODE_MENU_CONTROL_FLAG - PREV_CODE_MENU_CONTROL_FLAG);
		LoadWordToReg(TempReg1, OLD_DEBUG_STATE_LOC);
		SetRegister(TempReg2, IS_DEBUG_PAUSED);
		STW(TempReg1, TempReg2, 0);
	}Else(); {
		ADD(TempReg2, PageReg, TempReg1);
		SetRegister(TempReg1, CURRENT_PAGE_PTR_LOC);
		STW(TempReg2, TempReg1, 0);

		//reg2 == super page
		LWZ(TempReg1, TempReg2, Page::CURRENT_LINE_OFFSET);
		ADD(TempReg1, TempReg1, TempReg2); //super page current line
		LBZ(TempReg3, TempReg1, Line::COLOR); //color of super line
		RLWINM(TempReg4, TempReg3, 29, 31, 31);
		LWZ(TempReg5, TempReg2, Page::NUM_CHANGED_LINES); //super page num changed lines
		ANDI(TempReg3, TempReg3, ~0x8);
		SUBF(TempReg5, TempReg5, TempReg4);
		LWZ(TempReg4, PageReg, Page::NUM_CHANGED_LINES);
		SetRegister(TempReg6, 0);
		If(TempReg4, NOT_EQUAL_I, 0); {
			SetRegister(TempReg6, 1);
		}EndIf();
		ADD(TempReg5, TempReg5, TempReg6);
		RLWINM(TempReg6, TempReg6, 3, 0, 31); //<<3
		STW(TempReg5, TempReg2, Page::NUM_CHANGED_LINES);
		OR(TempReg3, TempReg3, TempReg6);
		STB(TempReg3, TempReg1, Line::COLOR);
	}EndIf();
}

void DecreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5)
{
	LHZ(TempReg1, LineReg, Line::INDEX);
	SetRegister(TempReg2, START_OF_CODE_MENU_SETTINGS);

	If(TypeReg, LESS_OR_EQUAL_I, HAS_VALUE_LIMIT); {
		//has a value to change
		If(TypeReg, EQUAL_I, SELECTION_LINE); {
			//selection
			LWZUX(TempReg1, TempReg2, TempReg1); //get setting
			Decrement(TempReg1);

			If(TempReg1, LESS_I, 0); {
				LWZ(TempReg1, LineReg, Line::MAX);
			}EndIf();
			STW(TempReg1, TempReg2, 0);

			RLWINM(TempReg3, TempReg1, 2, 0, 31); //<< 2
			ADDI(TempReg1, LineReg, Selection::SELECTION_LINE_OFFSETS_START + 2);
			LHZX(TempReg1, TempReg1, TempReg3); //get mapped value
			STW(TempReg1, TempReg2, 4);
		}Else(); If(TypeReg, EQUAL_I, INTEGER_LINE); {
			//integer
			LWZUX(TempReg1, TempReg2, TempReg1); //get setting
			LWZ(TempReg3, LineReg, Integer::SPEED);
			SUBF(TempReg1, TempReg1, TempReg3);

			LWZ(TempReg3, LineReg, Integer::MIN);
			If(TempReg1, LESS, TempReg3); {
				STW(TempReg3, TempReg2, 0);
			}Else(); {
				STW(TempReg1, TempReg2, 0);
			}EndIf();
		}Else(); {
			//floating
			LFSUX(2, TempReg2, TempReg1); //get setting
			LFS(1, LineReg, Floating::SPEED);
			FSUB(1, 2, 1);

			LFS(2, LineReg, Floating::MIN);
			FCMPU(1, 2, 0);
			BC(3, BRANCH_IF_FALSE, LT);
			STFS(2, TempReg2, 0);
			B(2);
			STFS(1, TempReg2, 0);
		}EndIf(); EndIf(); //done

		//set changed flag
		LWZ(TempReg1, TempReg2, 0);
		LBZ(TempReg2, LineReg, Line::COLOR);
		LWZ(TempReg3, LineReg, Line::DEFAULT);
		LWZ(TempReg5, PageReg, Page::NUM_CHANGED_LINES);
		RLWINM(TempReg4, TempReg2, 29, 31, 31);
		ANDI(TempReg2, TempReg2, ~0x8);
		If(TempReg1, NOT_EQUAL, TempReg3); {
			//not default
			Increment(TempReg5);
			ORI(TempReg2, TempReg2, 0x8);
		}EndIf();
		SUBF(TempReg5, TempReg5, TempReg4);
		STB(TempReg2, LineReg, Line::COLOR);
		STW(TempReg5, PageReg, Page::NUM_CHANGED_LINES);
	}EndIf();
}

void IncreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5)
{
	LHZ(TempReg1, LineReg, Line::INDEX);
	SetRegister(TempReg2, START_OF_CODE_MENU_SETTINGS);

	If(TypeReg, LESS_OR_EQUAL_I, HAS_VALUE_LIMIT); {
		//has a value to change
		If(TypeReg, EQUAL_I, SELECTION_LINE); {
			//selection
			LWZUX(TempReg1, TempReg2, TempReg1); //get setting
			Increment(TempReg1);

			LWZ(TempReg3, LineReg, Line::MAX);
			If(TempReg1, GREATER, TempReg3); {
				SetRegister(TempReg1, 0);
			}EndIf();
			STW(TempReg1, TempReg2, 0);

			RLWINM(TempReg3, TempReg1, 2, 0, 31); //<< 2
			ADDI(TempReg1, LineReg, Selection::SELECTION_LINE_OFFSETS_START + 2);
			LHZX(TempReg1, TempReg1, TempReg3); //get mapped value
			STW(TempReg1, TempReg2, 4);
		}Else(); If(TypeReg, EQUAL_I, INTEGER_LINE); {
			//integer
			LWZUX(TempReg1, TempReg2, TempReg1); //get setting
			LWZ(TempReg3, LineReg, Integer::SPEED);
			ADD(TempReg1, TempReg1, TempReg3);

			LWZ(TempReg3, LineReg, Integer::MAX);
			If(TempReg1, GREATER, TempReg3); {
				STW(TempReg3, TempReg2, 0);
			}Else(); {
				STW(TempReg1, TempReg2, 0);
			}EndIf();
		}Else(); {
			//floating
			LFSUX(2, TempReg2, TempReg1); //get setting
			LFS(1, LineReg, Floating::SPEED);
			FADD(1, 1, 2);

			LFS(2, LineReg, Floating::MAX);
			FCMPU(1, 2, 0);
			BC(3, BRANCH_IF_FALSE, GT);
			STFS(2, TempReg2, 0);
			B(2);
			STFS(1, TempReg2, 0);
		}EndIf(); EndIf(); //done

		//set changed flag
		LWZ(TempReg1, TempReg2, 0);
		LBZ(TempReg2, LineReg, Line::COLOR);
		LWZ(TempReg3, LineReg, Line::DEFAULT);
		LWZ(TempReg5, PageReg, Page::NUM_CHANGED_LINES);
		RLWINM(TempReg4, TempReg2, 29, 31, 31);
		ANDI(TempReg2, TempReg2, ~0x8);
		If(TempReg1, NOT_EQUAL, TempReg3); {
			//not default
			Increment(TempReg5);
			ORI(TempReg2, TempReg2, 0x8);
		}EndIf();
		SUBF(TempReg5, TempReg5, TempReg4);
		STB(TempReg2, LineReg, Line::COLOR);
		STW(TempReg5, PageReg, Page::NUM_CHANGED_LINES);
	}EndIf();
}

void Move(int LineReg, int PageReg, int NextLineOffset, int TempReg1, int TempReg2) {
	LBZ(TempReg2, LineReg, Line::COLOR);
	XORI(TempReg2, TempReg2, 0x4);
	STB(TempReg2, LineReg, Line::COLOR);

	STW(NextLineOffset, PageReg, Page::CURRENT_LINE_OFFSET);

	ADD(TempReg1, NextLineOffset, PageReg);
	LBZ(TempReg2, TempReg1, Line::COLOR);
	XORI(TempReg2, TempReg2, 0x4);
	STB(TempReg2, TempReg1, Line::COLOR);
}

void GetActionFromInputs(int ButtonReg, int ControlStickXReg, int ControlStickYReg, int ResultReg)
{
	//action checks at the bottom have a higher priority
	SetRegister(ResultReg, NO_ACTION); //default

	//set Z for fast
	ANDI(4, ButtonReg, BUTTON_Z);
	If(4, NOT_EQUAL_I, 0); {
		//set frame timers to 1
		SetRegister(4, MOVE_FRAME_TIMER_LOC);
		SetRegister(5, 1);
		SetRegister(6, INCREMENT_FRAME_TIMER_LOC);
		STW(5, 4, 0);
		STW(5, 6, 0);
	}EndIf();

	//move
	SetControlStickAction(ControlStickYReg, MOVE_FRAME_TIMER_LOC, MOVE_NUM_WAIT_FRAMES, FIRST_MOVE_NUM_WAIT_FRAMES, MOVE_THRESHHOLD, MOVE_UP, MOVE_DOWN, ResultReg);

	//increment
	SetControlStickAction(ControlStickXReg, INCREMENT_FRAME_TIMER_LOC, INCREMENT_NUM_WAIT_FRAMES, FIRST_INCREMENT_NUM_WAIT_FRAMES, INCREMENT_THRESHHOLD, INCREMENT, DECREMENT, ResultReg);

	ANDI(4, ButtonReg, TRIGGER_RESET_LINE_BUTTON);
	If(4, NOT_EQUAL_I, 0); //enter sub menu
	SetRegister(ResultReg, RESET_LINE);
	EndIf(); //enter sub menu

	ANDI(4, ButtonReg, TRIGGER_RESET_PAGE_BUTTON);
	If(4, NOT_EQUAL_I, 0); //leave sub menu
	SetRegister(ResultReg, RESET_PAGE);
	EndIf(); //leave sub menu

	ANDI(4, ButtonReg, TRIGGER_ENTER_SUB_MENU_BUTTON);
	If(4, NOT_EQUAL_I, 0); //enter sub menu
	SetRegister(ResultReg, ENTER_SUB_MENU);
	EndIf(); //enter sub menu

	ANDI(4, ButtonReg, TRIGGER_LEAVE_SUB_MENU_BUTTON);
	If(4, NOT_EQUAL_I, 0); //leave sub menu
	SetRegister(ResultReg, LEAVE_SUB_MENU);
	EndIf(); //leave sub menu

	ANDI(4, ButtonReg, BUTTON_START);
	If(4, NOT_EQUAL_I, 0); //leave menu
	SetRegister(ResultReg, EXIT_MENU);
	EndIf(); //leave menu
}

void SetControlStickAction(int StickValReg, int TimerLoc, int NumWaitFrames, int FirstTimeNumWaitFrames, int Threshhold, int PositiveAction, int NegativeAction, int ResultReg)
{
	int FrameTimerReg = 6;
	LoadWordToReg(FrameTimerReg, 7, TimerLoc);
	ABS(8, StickValReg, 3);
	Decrement(FrameTimerReg);
	If(8, GREATER_OR_EQUAL_I, Threshhold); {
		If(FrameTimerReg, LESS_OR_EQUAL_I, 0); {
			If(FrameTimerReg, LESS_I, 0); {
				//first move
				SetRegister(FrameTimerReg, FirstTimeNumWaitFrames - NumWaitFrames);
			}EndIf();
			ADDI(FrameTimerReg, FrameTimerReg, NumWaitFrames + 1);
			SetRegister(ResultReg, NegativeAction);
			If(StickValReg, GREATER_OR_EQUAL_I, Threshhold); {
				SetRegister(ResultReg, PositiveAction);
			}EndIf();
		}EndIf();
	}Else(); {
		SetRegister(FrameTimerReg, 0);
	}EndIf();
	STW(FrameTimerReg, 7, 0);
}

void PrintCodeMenu()
{
	ASMStart(0x80017928);
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
	LoadWordToReg(Reg2, Reg4, OLD_CAMERA_POS_LOC);
	OR(Reg3, Reg1, Reg2);
	If(Reg3, GREATER_OR_EQUAL_I, CODE_MENU_OPEN); {
		//code menu is open, or old camera pos is not 0
		SetRegister(3, 0x805b6d20);
		WriteVectorToMem(DEFAULT_CAMERA_MATRIX, 3);
		DrawBlackBackground();
	}EndIf();

	If(Reg1, EQUAL_I, CODE_MENU_OPEN); {
		SetRegister(Reg2, 0x805b6df8);
		SetRegister(Reg1, 0x3eb2b8c2);
		STW(Reg1, Reg2, 0);

		LoadWordToReg(Reg1, CURRENT_PAGE_PTR_LOC);
		SetRegister(Reg2, DRAW_SETTINGS_BUFFER_LOC);
		PrintPage(Reg1, Reg2, Reg3, Reg4, Reg5, Reg6, Reg7, Reg8);
	}Else(); {
		If(Reg2, NOT_EQUAL_I, 0); {
			//need to reset camera and wait a frame
			SetRegister(Reg3, 0x805b6df8);
			SetRegister(Reg1, 0);
			STW(Reg2, Reg3, 0); //reset camera
			STW(Reg1, Reg4, 0); //clear saved camera
		}Else(); {
			CallBrawlFunc(0x8002e844); //render
		}EndIf();
	}EndIf();

	RestoreRegisters();
	ASMEnd();
}

void PrimeCodeMenu()
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

	LWZ(Reg3, 3, 0x40);
	If(Reg3, EQUAL_I, 8); {
		//on tourney
		LoadWordToReg(Reg1, Reg2, CODE_MENU_CONTROL_FLAG);
		If(Reg1, NOT_EQUAL_I, CODE_MENU_OPEN); {
			SetRegister(Reg2, CODE_MENU_CONTROL_FLAG);
			SetRegister(Reg1, CODE_MENU_PRIMED);
			STW(Reg1, Reg2, 0);
		}EndIf();
	}EndIf();

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
	SetRegister(3, GetHexFromFloat(INITIAL_XPOS));
	LFS(0, SettingsPtrReg, DRAW_BUFFER_YPOS_OFFSET);
	SetFloatingRegister(1, 4, LINE_HEIGHT);
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

void SetupPrintText(int SettingPtrReg)
{
	ADDI(3, SettingPtrReg, 0);
	SetRegs(4, { 0, 9 });
	CallBrawlFunc(0x8006a964); //SetDefaultEnv

	CallBrawlFunc(0x80019FA4); //(getManager[cameraManager])

	CallBrawlFunc(0x80018DE4); //(setGX[camera])

	SetRegister(3, DRAW_SETTINGS_BUFFER_LOC + 0x48);
	SetRegister(4, 0x80497e44);
	STW(4, 3, 0); //set font
}

void DrawBlackBackground()
{
	CallBrawlFunc(0x80019FA4); //(getManager[cameraManager])

	CallBrawlFunc(0x80018DE4); //(setGX[camera])

	CallBrawlFunc(0x8001A5C0); //gfDrawSetVtxColorPrimEnvrment

	vector<float> positions;
	float size = 0x1000;
	positions.push_back(-size);
	positions.push_back(size);
	positions.push_back(size);
	positions.push_back(size);
	positions.push_back(size);
	positions.push_back(-size);
	positions.push_back(-size);
	positions.push_back(-size);
	DrawPrimitive(PRIMITIVE_QUAD, positions, BLACK, 1);
}

void PrintPage(int PageReg, int SettingsPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6)
{
	SetupPrintText(SettingsPtrReg);

	SetRegister(Reg6, START_OF_CODE_MENU_SETTINGS);
	SetRegister(Reg5, COLOR_ARRAY_START);

	//set initial text pos
	SetRegister(Reg1, GetHexFromFloat(INITIAL_XPOS));
	SetRegister(Reg2, GetHexFromFloat(INITIAL_YPOS));
	SetTextPos(Reg1, Reg2, SettingsPtrReg);

	ADDI(Reg2, PageReg, Page::FIRST_LINE_OFFSET); //first line
	SetRegister(3, 1);
	While(3, NOT_EQUAL_I, 0); {
		PrintCodeMenuLine(Reg2, SettingsPtrReg, Reg5, Reg6, Reg3, Reg4);

		LHZ(3, Reg2, Line::SIZE);
		ADD(Reg2, Reg2, 3); //next line
	}EndWhile();
}

void PrintCodeMenuLine(int LinePtrReg, int SettingsPtrReg, int ColorArrayPtrReg, int CodeMenuSettingsPtrReg, int TempReg1, int TempReg2)
{
	LBZ(TempReg2, LinePtrReg, Line::TYPE);

	LBZ(TempReg1, LinePtrReg, Line::COLOR);
	LWZX(TempReg1, ColorArrayPtrReg, TempReg1);
	SetTextColor(TempReg1, SettingsPtrReg);

	LBZ(4, LinePtrReg, Line::TEXT_OFFSET);
	ADD(4, 4, LinePtrReg);
	LHZ(5, LinePtrReg, Line::INDEX);

	If(TempReg2, EQUAL_I, FLOATING_LINE); {
		LFSX(1, CodeMenuSettingsPtrReg, 5);
		SprintF(4, {}, { 1 });
	}Else(); {
		LWZX(5, CodeMenuSettingsPtrReg, 5); //get setting

		If(TempReg2, EQUAL_I, SELECTION_LINE); {
			ADDI(TempReg1, LinePtrReg, Selection::SELECTION_LINE_OFFSETS_START);
			RLWINM(5, 5, 2, 0, 31); //<< 2
			LHZX(5, TempReg1, 5); //get string offset
			ADD(5, 5, LinePtrReg); //get string offset
		}EndIf();

		SprintF(4, { 5 });
	}EndIf();

	MR(TempReg2, 3); //num chars
	SetRegister(TempReg1, STRING_BUFFER - 1);
	PrintString(TempReg1, TempReg2, SettingsPtrReg);

	NewLine(SettingsPtrReg);
}

//requires 2 endifs
void GetArrayValueFromIndex(int ArrayLoc, int IndexReg, int min, int max, int ResultReg)
{
	If(IndexReg, LESS_OR_EQUAL_I, max); {
		If(IndexReg, GREATER_OR_EQUAL_I, min); {
			RLWINM(4, IndexReg, 2, 0, 31); //<< 2
			SetRegister(ResultReg, ArrayLoc - (min * 4));
			LWZX(ResultReg, ResultReg, 4);
		}
	}
}