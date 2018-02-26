#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

#define CODE_MENU_CLOSED 0
#define CODE_MENU_OPEN 2

#define COMMENT_LINE 0
#define TOGGLE_LINE 1
#define SUB_MENU_LINE 2

#define INITIAL_XPOS -400
#define INITIAL_YPOS -0x100

static int MenuID = 0;

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

	Page *SubMenuPtr;
	int SubMenuOffset;
	int PageOffset;
	int ID_;
	int type = -1;
	int Color = WHITE;
	int DownOffset = -1;
	int UpOffset = -1;
	string Text;
	int Size;
	int Padding;
	//offsets
	static const int SIZE = 0;
	static const int TYPE = 4;
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
	Toggle(string Text)
	: Line(Text + ":")
	{
		type = TOGGLE_LINE;
		ID_ = MenuID++;
		Size += 4 * NUM_WORD_ELEMS;
		Padding = (4 - Size % 4) % 4;
		Size += Padding;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		WriteIntToFile(ID_);
		WriteIntToFile(Color);
		WriteIntToFile(DownOffset);
		WriteIntToFile(UpOffset);
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	static const int NUM_WORD_ELEMS = 6;
	static const int ID = 8;
	static const int COLOR = ID + 4;
	static const int DOWN = COLOR + 4;
	static const int UP = DOWN + 4;
	static const int TEXT = UP + 4;
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
		WriteIntToFile(DownOffset);
		WriteIntToFile(UpOffset);
		WriteMenuTextToFile(Text);
		WritePadding();
	}

	static const int NUM_WORD_ELEMS = 6;
	static const int COLOR = 8;
	static const int SUB_MENU = COLOR + 4;
	static const int DOWN = SUB_MENU + 4;
	static const int UP = DOWN + 4;
	static const int TEXT = UP + 4;
};

class Page
{
public:
	Page(string Name, vector<Line*> Lines, int ValueXPos) {
		CalledFromLine = SubMenu(Name, this);
		this->Lines = Lines;
		this->Lines[0]->Color = YELLOW;
		this->ValueXPos = ValueXPos;
		int CurrentPageOffset = Size;
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
		SelectableLines.insert(SelectableLines.begin(), SelectableLines.back());
		SelectableLines.push_back(SelectableLines[1]);

		for (int i = 1; i < SelectableLines.size() - 1; i++) {
			Lines[SelectableLines[i]]->UpOffset = Lines[SelectableLines[i - 1]]->PageOffset;
			Lines[SelectableLines[i]]->DownOffset = Lines[SelectableLines[i + 1]]->PageOffset;
		}
		CurrentLineOffset = Lines[0]->PageOffset;
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
void PrintPage(int PageReg, int SettingPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6);
void PrintToggleLine(int LinePtrReg, int ValueXPosReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void PrintSubMenuLine(int LinePtrReg, int SettingsPtrReg, int TempReg1, int TempReg2);
void SetTextColor(int ColorReg, int SettingsPtrReg);
void CodeMenu();
void PrintCodeMenu();
void TriggerCodeMenu();
void CreateMenu(Page MainPage);