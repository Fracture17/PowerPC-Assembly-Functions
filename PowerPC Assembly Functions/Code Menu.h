#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

//active codes
#define DI_DRAW
//#define IASA_OVERLAY

#define START_OF_MENU_LOC 0x804E0000

//code menu flags
#define CODE_MENU_CLOSED 0
#define CODE_MENU_PRIMED 1
#define CODE_MENU_OPEN 2

//line types
#define COMMENT_LINE 0
#define TOGGLE_LINE 1
#define SUB_MENU_LINE 2
#define NUMBER_LINE 3
#define FLOATING_LINE 4

//default code menu settings
#define INITIAL_XPOS -400
#define INITIAL_YPOS -0x100
#define MOVE_THRESHHOLD 100
#define INCREMENT_THRESHHOLD 100
#define TRIGGER_ENTER_SUB_MENU_BUTTON BUTTON_A
#define TRIGGER_LEAVE_SUB_MENU_BUTTON BUTTON_B

//action types
#define NO_ACTION 0
#define MOVE_UP 1
#define MOVE_DOWN 2
#define ENTER_SUB_MENU 3
#define LEAVE_SUB_MENU 4
#define INCREMENT 5
#define DECREMENT 6

static vector<int> Defaults;

class Page;

class Line
{
public:
	Line() {}

	Line(string Text) {
		this->Text = Text;
		Size = Text.size() + 1;
	}

	virtual void WriteLineData()
	{
		WriteIntToFile(Size);
		WriteIntToFile(type);
	}

	void WritePadding() {
		string x = "";
		for (int i = 0; i < Padding; i++) {
			x += "\0"s;
		}
		WriteTextToFile(x);
	}

	virtual int GetDefault() {
		return Default;
	}

	int Default;
	Page *SubMenuPtr;
	int SubMenuOffset;
	int PageOffset;
	int Index;
	int type = -1;
	int Color = 0;
	int DownOffset = -1;
	int UpOffset = -1;
	string Text;
	int Size;
	int Padding;
	//offsets
	static const int SIZE = 0;
	static const int TYPE = 4;
	static const int COLOR = 8;
};

class Comment : public Line
{
public:
	Comment(string Text)
	: Line(Text)
	{
		type = COMMENT_LINE;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	static const int NUM_WORD_ELEMS = 2;
	static const int TEXT = 8;
};

class Toggle : public Line
{
public:
	Toggle(string Text, bool Default)
	: Line(Text + ":")
	{
		this->Default = Default;
		type = TOGGLE_LINE;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteIntToFile(Color);
		WriteIntToFile(Index);
		WriteIntToFile(UpOffset);
		WriteIntToFile(DownOffset);
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	int GetDefault() {
		return (int) Default;
	}

	bool Default;
	static const int NUM_WORD_ELEMS = 6;
	static const int COLOR = 8;
	static const int INDEX = COLOR + 4;
	static const int UP = INDEX + 4;
	static const int DOWN = UP + 4;
	static const int TEXT = DOWN + 4;
};

class SubMenu : public Line
{
public:
	SubMenu() {}

	SubMenu(string Text, Page* SubMenuPtr)
	: Line(Text + " >")
	{
		type = SUB_MENU_LINE;
		this->SubMenuPtr = SubMenuPtr;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteIntToFile(Color);
		WriteIntToFile(SubMenuOffset);
		WriteIntToFile(UpOffset);
		WriteIntToFile(DownOffset);
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	static const int NUM_WORD_ELEMS = 6;
	static const int COLOR = 8;
	static const int SUB_MENU = COLOR + 4;
	static const int UP = SUB_MENU + 4;
	static const int DOWN = UP + 4;
	static const int TEXT = DOWN + 4;
};

class Number : public Line
{
public:
	Number(string Text, int Min, int Max, int Default, int Speed = 1)
	: Line(Text + ":")
	{
		this->Min = Min;
		this->Max = Max;
		this->Default = Default;
		this->Speed = Speed;
		type = NUMBER_LINE;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteIntToFile(Color);
		WriteIntToFile(Index);
		WriteIntToFile(UpOffset);
		WriteIntToFile(DownOffset);
		WriteIntToFile(Speed);
		WriteIntToFile(Min);
		WriteIntToFile(Max);
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	int GetDefault() {
		return Default;
	}

	int Min;
	int Max;
	int Default;
	int Speed;
	static const int NUM_WORD_ELEMS = 9;
	static const int COLOR = 8;
	static const int INDEX = COLOR + 4;
	static const int UP = INDEX + 4;
	static const int DOWN = UP + 4;
	static const int SPEED = DOWN + 4;
	static const int MIN = SPEED + 4;
	static const int MAX = MIN + 4;
	static const int TEXT = MAX + 4;
};

class Floating : public Line
{
public:
	Floating(string Text, float Min, float Max, float Default, float Speed = 1)
	: Line(Text + ":")
	{
		this->Min = Min;
		this->Max = Max;
		this->Default = Default;
		this->Speed = Speed;
		type = FLOATING_LINE;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteIntToFile(Color);
		WriteIntToFile(Index);
		WriteIntToFile(UpOffset);
		WriteIntToFile(DownOffset);
		WriteIntToFile(GetHexFromFloat(Speed));
		WriteIntToFile(GetHexFromFloat(Min));
		WriteIntToFile(GetHexFromFloat(Max));
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	int GetDefault() {
		return GetHexFromFloat(Default);
	}

	float Min;
	float Max;
	float Default;
	float Speed;
	static const int NUM_WORD_ELEMS = 9;
	static const int COLOR = 8;
	static const int INDEX = COLOR + 4;
	static const int UP = INDEX + 4;
	static const int DOWN = UP + 4;
	static const int SPEED = DOWN + 4;
	static const int MIN = SPEED + 4;
	static const int MAX = MIN + 4;
	static const int TEXT = MAX + 4;
};

class Page
{
public:
	Page(string Name, vector<Line*> Lines, int ValueXPos) {
		CalledFromLine = SubMenu(Name, this);
		this->Lines = Lines;
		this->ValueXPos = ValueXPos;
		for (auto x : Lines) {
			x->PageOffset = Size;
			Size += x->Size;
		}
		Lines.back()->Size = 0;
		ConnectSelectableLines();
	}
	
	void WritePage()
	{
		WriteIntToFile(ValueXPos);
		WriteIntToFile(CurrentLineOffset);
		WriteIntToFile(PrevPageOffset);
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
			Lines[SelectableLines[1]]->Color = 1;
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

	int ValueXPos;
	int CurrentLineOffset;
	int Size = NUM_WORD_ELEMS * 4;
	int PrevPageOffset = 0;
	vector<Line*> Lines;
	SubMenu CalledFromLine;
	static const int NUM_WORD_ELEMS = 3;
	static const int XPOS = 0;
	static const int CURRENT_LINE = XPOS + 4;
	static const int PREV_PAGE = CURRENT_LINE + 4;
	static const int FIRST_LINE_OFFSET = NUM_WORD_ELEMS * 4;
};

void PrintChar(int SettingsPtrReg, int CharReg);
void PrintString(int StringPtrReg, int NumCharsReg, int SettingsPtrReg);
void PrintPage(int PageReg, int SettingPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6);
void PrintFloatingLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void PrintToggleLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void PrintNumberLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void PrintCommentLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void PrintSubMenuLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void SetTextColor(int ColorReg, int SettingsPtrReg);
void SetTextSize(int FPSizeReg, int SettingsPtrReg);
void CodeMenu();
void ActualCodes();
void ControlCodeMenu();
void PrintCodeMenu();
void TriggerCodeMenu();
void CreateMenu(Page MainPage);

void ExecuteAction();

void EnterMenu(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2);

void LeaveMenu(int LineReg, int PageReg, int TempReg1, int TempReg2);

void DecreaseValue(int LineReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3);

void IncreaseValue(int LineReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3);

void MoveUp(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2);

void MoveDown(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2);

void GetActionFromInputs(int ButtonReg, int ControlStickXReg, int ControlStickYReg);

void ControlStickFrameTimerLogic(int FrameReg, int FrameLocReg, int FirstMoveFramesLoc, int MoveFramesLoc, int action);
