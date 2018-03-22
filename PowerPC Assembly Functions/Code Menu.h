#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

//active codes
extern int DI_DRAW_INDEX;
extern int DEBUG_MODE_INDEX;
extern int DISPLAY_HITBOXES_INDEX;
extern int DISPLAY_COLLISION_INDEX;
extern int DISPLAY_LEDGEGRAB_INDEX;
extern int CHARACTER_SELECT_P1_INDEX;
extern int CHARACTER_SELECT_P2_INDEX;
extern int CHARACTER_SELECT_P3_INDEX;
extern int CHARACTER_SELECT_P4_INDEX;
extern int COSTUME_SELECT_P1_INDEX;
extern int COSTUME_SELECT_P2_INDEX;
extern int COSTUME_SELECT_P3_INDEX;
extern int COSTUME_SELECT_P4_INDEX;
extern int INFINITE_SHIELDS_P1_INDEX;
extern int INFINITE_SHIELDS_P2_INDEX;
extern int INFINITE_SHIELDS_P3_INDEX;
extern int INFINITE_SHIELDS_P4_INDEX;
extern int CAMERA_LOCK_INDEX;
extern int INFINITE_FRIENDLIES_INDEX;
extern vector<int> Defaults;

#define MAX_SUBPAGE_DEPTH 20

const vector<string> CHARACTER_LIST = { "Mario", "Donkey Kong", "Link", "Samus", "Zero Suit Samus", "Yoshi", "Kirby", "Fox", "Pikachu", "Luigi", "Captain Falcon",
										"Ness", "Bowser", "Peach", "Zelda", "Shiek", "Ice Climbers", "Popo", "Marth", "Mr. Game and Watch", "Falco", "Ganondorf",
										"Wario", "Metaknight", "Pit", "Olimar", "Lucas", "Diddy Kong", "Charizard", "SquirlTail", "Ivysaur", "DeDeDe", "Lucario",
										"Ike", "R.O.B.", "Jigglypuff", "Toon Link", "Wolf", "Roy", "Mewtwo" };
const vector<u16> CHARACTER_ID_LIST = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 30, 32, 34, 35, 36,
										37, 38, 39, 40, 41, 50, 51 };

static const int START_OF_CODE_MENU = 0x804E0000;
static const int CURRENT_PAGE_PTR_LOC = START_OF_CODE_MENU; //4
//colors
static const int COLOR_ARRAY_START = CURRENT_PAGE_PTR_LOC + 4; //4 * num colors
static const u8 NORMAL_LINE_COLOR_OFFSET = 0;
static const u8 HIGHLIGHTED_LINE_COLOR_OFFSET = NORMAL_LINE_COLOR_OFFSET + 4;
static const u8 CHANGED_LINE_COLOR_OFFSET = HIGHLIGHTED_LINE_COLOR_OFFSET + 4;
static const u8 CHANGED_AND_HIGHLIGHTED_LINE_COLOR_OFFSET = CHANGED_LINE_COLOR_OFFSET + 4;
static const u8 COMMENT_LINE_COLOR_OFFSET = CHANGED_AND_HIGHLIGHTED_LINE_COLOR_OFFSET + 4;

static const int MOVE_FRAME_TIMER_LOC = COLOR_ARRAY_START + 0x14; //4
static const int INCREMENT_FRAME_TIMER_LOC = MOVE_FRAME_TIMER_LOC + 4; //4
static const int FRAME_ADVANCE_FRAME_TIMER = INCREMENT_FRAME_TIMER_LOC + 4; //4

static const int PREV_CODE_MENU_CONTROL_FLAG = FRAME_ADVANCE_FRAME_TIMER + 4; //4
static const int CODE_MENU_CONTROL_FLAG = PREV_CODE_MENU_CONTROL_FLAG + 4; //4
static const int INFINITE_FRIENDLIES_FLAG_LOC = CODE_MENU_CONTROL_FLAG + 4; //4

static const int CODE_MENU_BUTTON_MASK_LOC = INFINITE_FRIENDLIES_FLAG_LOC + 4; //4
static const int BUTTON_ACTIVATOR_MASK_LOC = CODE_MENU_BUTTON_MASK_LOC + 4; //4
static const int MAIN_BUTTON_MASK_LOC = BUTTON_ACTIVATOR_MASK_LOC + 4; //8

static const int OLD_DEBUG_STATE_LOC = MAIN_BUTTON_MASK_LOC + 8; //4
static const int OLD_CAMERA_LOCK_STATE_LOC = OLD_DEBUG_STATE_LOC + 4; //4

static const int OLD_CAMERA_POS_LOC = OLD_CAMERA_LOCK_STATE_LOC + 4; //4

static const int RESET_LINES_STACK_LOC = OLD_CAMERA_POS_LOC + 4; // 4 * MAX_SUBPAGE_DEPTH + 8

static const int CHARACTER_SWITCHER_ARRAY_LOC = RESET_LINES_STACK_LOC + 4 * MAX_SUBPAGE_DEPTH + 8; //0x10
static const int INIFINITE_SHIELDS_ARRAY_LOC = CHARACTER_SWITCHER_ARRAY_LOC + 0x10; //0x10

static const int DRAW_SETTINGS_BUFFER_LOC = INIFINITE_SHIELDS_ARRAY_LOC + 0x10; //0x200

static const int START_OF_CODE_MENU_SETTINGS = DRAW_SETTINGS_BUFFER_LOC + 0x200;

extern int MenuID;

#define CODE_MENU_GECKO_IF(MenuIndex) if(MenuIndex != -1) {\
									GeckoIf(MenuIndex, EQUAL, 1); {
#define CODE_MENU_GECKO_ENDIF }GeckoEndIf(); }

//code menu flags
#define CODE_MENU_CLOSED 0
#define CODE_MENU_PRIMED 1
#define CODE_MENU_TRIGGERED 2
#define CODE_MENU_OPEN 3

//line types
#define SELECTION_LINE 0
#define INTEGER_LINE 1
#define FLOATING_LINE 2
#define HAS_VALUE_LIMIT 2
#define SUB_MENU_LINE 3
#define COMMENT_LINE 4

//default code menu settings
#define INITIAL_XPOS -200
#define INITIAL_YPOS -150
#define LINE_HEIGHT 18
#define MOVE_THRESHHOLD 65
#define INCREMENT_THRESHHOLD 50
#define MOVE_NUM_WAIT_FRAMES 5
#define FIRST_MOVE_NUM_WAIT_FRAMES 15
#define INCREMENT_NUM_WAIT_FRAMES 5
#define FIRST_INCREMENT_NUM_WAIT_FRAMES 15
#define FRAME_ADVANCE_NUM_WAIT_FRAMES 4
#define FIRST_FRAME_ADVANCE_NUM_WAIT_FRAMES 12
#define TRIGGER_ENTER_SUB_MENU_BUTTON BUTTON_A
#define TRIGGER_LEAVE_SUB_MENU_BUTTON BUTTON_B
#define TRIGGER_RESET_LINE_BUTTON BUTTON_X
#define TRIGGER_RESET_PAGE_BUTTON BUTTON_Y

//action types
#define NO_ACTION 0
#define MOVE_UP 1
#define MOVE_DOWN 2
#define ENTER_SUB_MENU 3
#define LEAVE_SUB_MENU 4
#define INCREMENT 5
#define DECREMENT 6
#define EXIT_MENU 7
#define RESET_LINE 8
#define RESET_PAGE 9

#define FRAMES_UNTIL_SLOW_MOTION 12
#define FRAMES_WAITED_DURING_SLOW_MOTION 3

static vector<int> Defaults;
static fstream MenuFile("C:\\Program Files (x86)\\WinImage\\cm.bin", fstream::out | fstream::binary);

class Page;

class Line
{
public:
	Line() {}

	Line(string Text, u16 TextOffset, u8 type, u8 flags, u8 ColorOffset, u16 Index) {
		this->Text = Text + "\0"s;
		this->type = type;
		this->Flags = flags;
		this->Color = ColorOffset;
		this->TextOffset = TextOffset;
		this->Index = Index;
		Size = Text.size() + TextOffset + 1;

		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	virtual void WriteLineData()
	{
		WriteLineData({});
	}

	void WriteLineData(vector<u8> SelectionOffsets)
	{
		vector<u8> output;
		AddValueToByteArray(Size, output);
		AddValueToByteArray(type, output);
		AddValueToByteArray(Flags, output);
		AddValueToByteArray(Color, output);
		AddValueToByteArray(TextOffset, output);
		AddValueToByteArray(Index, output);
		if (type != COMMENT_LINE) {
			AddValueToByteArray(UpOffset, output);
			AddValueToByteArray(DownOffset, output);
			if (type == SUB_MENU_LINE) {
				AddValueToByteArray(SubMenuOffset, output);
			}
			else {
				AddValueToByteArray(Default, output);
				AddValueToByteArray(Max, output);
				if (type == INTEGER_LINE || type == FLOATING_LINE) {
					AddValueToByteArray(Min, output);
					AddValueToByteArray(Speed, output);
				}
			}
		}
		copy(output.begin(), output.end(), ostreambuf_iterator<char>(MenuFile));
		copy(SelectionOffsets.begin(), SelectionOffsets.end(), ostreambuf_iterator<char>(MenuFile));
		MenuFile << Text;
		WritePadding();
	}

	void WritePadding() {
		for (int i = 0; i < Padding; i++) {
			MenuFile << '\0';
		}
	}

	u16 Index = 0;
	u32 Default;
	u32 Max;
	u32 Min;
	u32 Speed;
	Page *SubMenuPtr;
	u16 SubMenuOffset;
	int PageOffset;
	u8 type;
	u8 Color;
	u8 Flags = 0;
	u8 TextOffset;
	u16 DownOffset;
	u16 UpOffset;
	string Text;
	u16 Size;
	int Padding;
	//offsets
	static const int SIZE = 0; //2
	static const int TYPE = SIZE + 2; //1
	static const int FLAGS = TYPE + 1; //1
	static const int COLOR = FLAGS + 1; //1
	static const int TEXT_OFFSET = COLOR + 1; //1
	static const int INDEX = TEXT_OFFSET + 1; //2
	static const int COMMENT_LINE_TEXT_START = INDEX + 2;
	static const int UP = INDEX + 2; //2
	static const int DOWN = UP + 2; //2
	static const int SUB_MENU = DOWN + 2; //2
	static const int SUB_MENU_LINE_TEXT_START = SUB_MENU + 2;
	static const int DEFAULT = DOWN + 2; //4
	static const int MAX = DEFAULT + 4; //4
	static const int SELECTION_LINE_OFFSETS_START = MAX + 4;
	static const int MIN = MAX + 4; //4
	static const int SPEED = MIN + 4; //4
	static const int NUMBER_LINE_TEXT_START = SPEED + 4;
};

class Comment : public Line
{
public:
	Comment(string Text)
	: Line(Text, COMMENT_LINE_TEXT_START, COMMENT_LINE, 0, COMMENT_LINE_COLOR_OFFSET, 0) {}

	void WriteLineData()
	{
		Line::WriteLineData();
	}
};

class Selection : public Line
{
public:
	Selection(string Text, vector<string> Options, vector<u16> Values, int Default, int &Index)
	: Line(CreateSelectionString(Text + ":  %s", Options), SELECTION_LINE_OFFSETS_START + Options.size() * 4, SELECTION_LINE, 0, NORMAL_LINE_COLOR_OFFSET, MenuID)
	{
		if (Options.size() != Values.size()) {
			cout << "Mismatched values" << endl;
			exit(-1);
		}
		u16 offset = Text.size() + 5 + 1 + SELECTION_LINE_OFFSETS_START + Options.size() * 4;
		for (int i = 0; i < Options.size(); i++) {
			AddValueToByteArray(offset, OptionOffsets);
			AddValueToByteArray(Values[i], OptionOffsets);
			offset += Options[i].size() + 1;
		}
		Index = MenuID + START_OF_CODE_MENU_SETTINGS;
		MenuID += 8;
		this->Default = Default;
		Defaults.push_back(Default);
		Defaults.push_back(Values[Default]);
		this->Max = Options.size() - 1;
	}

	Selection(string Text, vector<string> Options, int Default, int &Index)
		: Selection(Text, Options, CreateVector(Options), Default, Index) {}

	Selection(string Text, vector<string> Options, vector<u16> Values, string Default, int &Index)
		: Selection(Text, Options, Values, distance(Options.begin(), find(Options.begin(), Options.end(), Default)), Index) {}

	Selection(string Text, vector<string> Options, string Default, int &Index)
		: Selection(Text, Options, distance(Options.begin(), find(Options.begin(), Options.end(), Default)), Index) {}

	string CreateSelectionString(string Text, vector<string> Options)
	{
		for (string x : Options) {
			Text += "\0"s + x;
		}
		return Text;
	}

	vector<u16> CreateVector(vector<string> x) 
	{
		vector<u16> Values;
		for (u16 i = 0; i < x.size(); i++) {
			Values.push_back(i);
		}
		return Values;
	}

	void WriteLineData()
	{
		Line::WriteLineData(OptionOffsets);
	}

	vector<u8> OptionOffsets;
};

class Toggle : public Selection
{
public:
	Toggle(string Text, bool Default, int &Index)
		: Selection(Text,  { "OFF", "ON" }, Default, Index) {}
};

class SubMenu : public Line
{
public:
	SubMenu() {}

	SubMenu(string Text, Page* SubMenuPtr)
	: Line(Text + " >", SUB_MENU_LINE_TEXT_START, SUB_MENU_LINE, 0, NORMAL_LINE_COLOR_OFFSET, 0)
	{
		this->SubMenuPtr = SubMenuPtr;
	}
};

class Integer : public Line
{
public:
	Integer(string Text, int Min, int Max, int Default, int Speed, int &Index)
	: Line(Text + ":  %d", NUMBER_LINE_TEXT_START, INTEGER_LINE, 0, NORMAL_LINE_COLOR_OFFSET, MenuID)
	{
		Index = MenuID + START_OF_CODE_MENU_SETTINGS;
		MenuID += 4;
		this->Min = Min;
		this->Max = Max;
		this->Default = Default;
		Defaults.push_back(Default);
		this->Speed = Speed;
	}
};

class Floating : public Line
{
public:
	Floating(string Text, float Min, float Max, float Default, float Speed, int &Index)
	: Line(Text + ":  %f", NUMBER_LINE_TEXT_START, FLOATING_LINE, 0, NORMAL_LINE_COLOR_OFFSET, MenuID)
	{
		Index = MenuID + START_OF_CODE_MENU_SETTINGS;
		MenuID += 4;
		this->Min = GetHexFromFloat(Min);
		this->Max = GetHexFromFloat(Max);
		this->Default = GetHexFromFloat(Default);
		Defaults.push_back(GetHexFromFloat(Default));
		this->Speed = GetHexFromFloat(Speed);
	}
};

class Page
{
public:
	Page(string Name, vector<Line*> Lines) {
		CalledFromLine = SubMenu(Name, this);
		this->Lines = Lines;
		Size = NUM_WORD_ELEMS * 4;
		for (auto x : Lines) {
			x->PageOffset = Size;
			Size += x->Size;
		}
		Lines.back()->Size = 0;
		ConnectSelectableLines();
	}
	
	void WritePage()
	{
		vector<u8> output;
		AddValueToByteArray(CurrentLineOffset, output);
		AddValueToByteArray(PrevPageOffset, output);
		AddValueToByteArray(NumChangedLines, output);
		copy(output.begin(), output.end(), ostreambuf_iterator<char>(MenuFile));
		for (auto x : Lines) {
			x->WriteLineData();
		}
	}

	void ConnectSelectableLines()
	{
		vector<int> SelectableLines;
		GetSelectableLines(SelectableLines);
		if (SelectableLines.size() > 0) {
			SelectableLines.insert(SelectableLines.begin(), SelectableLines.back());
			SelectableLines.push_back(SelectableLines[1]);

			for (int i = 1; i < SelectableLines.size() - 1; i++) {
				Lines[SelectableLines[i]]->UpOffset = Lines[SelectableLines[i - 1]]->PageOffset;
				Lines[SelectableLines[i]]->DownOffset = Lines[SelectableLines[i + 1]]->PageOffset;
			}
		
			CurrentLineOffset = Lines[SelectableLines[1]]->PageOffset;
			Lines[SelectableLines[1]]->Color = HIGHLIGHTED_LINE_COLOR_OFFSET;
		}
		else {
			CurrentLineOffset = NUM_WORD_ELEMS * 4;
		}
	}

	void GetSelectableLines(vector<int> &SelectableLines)
	{
		for (int i = 0; i < Lines.size(); i++) {
			if (Lines[i]->type != COMMENT_LINE) {
				SelectableLines.push_back(i);
			}
		}
	}

	u32 CurrentLineOffset;
	u32 Size;
	u32 PrevPageOffset = 0;
	u32 NumChangedLines = 0;
	vector<Line*> Lines;
	SubMenu CalledFromLine;
	static const int NUM_WORD_ELEMS = 3;
	static const int CURRENT_LINE_OFFSET = 0;
	static const int PREV_PAGE = CURRENT_LINE_OFFSET + 4;
	static const int NUM_CHANGED_LINES = PREV_PAGE + 4;
	static const int FIRST_LINE_OFFSET = NUM_WORD_ELEMS * 4;
};

void PrintChar(int SettingsPtrReg, int CharReg);
void PrintString(int StringPtrReg, int NumCharsReg, int SettingsPtrReg);
void DrawBlackBackground();
void PrintPage(int PageReg, int SettingPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6);
void PrintCodeMenuLine(int LinePtrReg, int SettingsPtrReg, int ColorArrayPtrReg, int CodeMenuSettingsPtrReg, int TempReg1, int TempReg2);
void SetTextColor(int ColorReg, int SettingsPtrReg);
void SetTextSize(int FPSizeReg, int SettingsPtrReg);
void CodeMenu();
void ActualCodes();
void ControlCodeMenu();
void PrintCodeMenu();
void PrimeCodeMenu();
void CreateMenu(Page MainPage);
void ExecuteAction(int ActionReg);
void ResetLine(int LineReg, int PageReg, int StackReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3);
void ResetPage(int StackReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5, int TempReg6);
void ExitMenu();
void EnterMenu(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2);
void LeaveMenu(int PageReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5, int TempReg6);
void DecreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5);
void IncreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5);
void Move(int LineReg, int PageReg, int NextLineOffset, int TempReg1, int TempReg2);
void GetActionFromInputs(int ButtonReg, int ControlStickXReg, int ControlStickYReg, int ResultReg);
void SetControlStickAction(int StickValReg, int TimerLoc, int NumWaitFrames, int FirstTimeNumWaitFrames, int Threshhold, int PositiveAction, int NegativeAction, int ResultReg);
void ApplyMenuSetting(int Index, int Destination, int reg1, int reg2, int size = 4);
void GetArrayValueFromIndex(int ArrayLoc, int IndexReg, int min, int max, int ResultReg = 3);