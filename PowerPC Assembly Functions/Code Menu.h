#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

//active codes
extern int MENU_TITLE_CHECK_LOCATION;
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
extern int AUTO_SAVE_REPLAY_INDEX;
extern int SAVE_STATES_INDEX;
extern int SAVE_REPLAY_ANYWHERE_INDEX;
extern int AUTO_SKIP_TO_CSS_INDEX;
extern int CODE_MENU_ACTIVATION_SETTING_INDEX;
extern int PERCENT_SELECT_VALUE_P1_INDEX;
extern int PERCENT_SELECT_ACTIVATOR_P1_INDEX;
extern int PERCENT_SELECT_VALUE_P2_INDEX;
extern int PERCENT_SELECT_ACTIVATOR_P2_INDEX;
extern int PERCENT_SELECT_VALUE_P3_INDEX;
extern int PERCENT_SELECT_ACTIVATOR_P3_INDEX;
extern int PERCENT_SELECT_VALUE_P4_INDEX;
extern int PERCENT_SELECT_ACTIVATOR_P4_INDEX;
extern int DISABLE_DPAD_P1_INDEX;
extern int DISABLE_DPAD_P2_INDEX;
extern int DISABLE_DPAD_P3_INDEX;
extern int DISABLE_DPAD_P4_INDEX;
extern int FPS_DISPLAY_INDEX;
extern int DBZ_MODE_INDEX;
extern int DBZ_MODE_MAX_SPEED_X_INDEX;
extern int DBZ_MODE_ACCEL_X_INDEX;
extern int DBZ_MODE_MAX_SPEED_Y_INDEX;
extern int DBZ_MODE_ACCEL_Y_INDEX;
extern int ALT_STAGE_BEHAVIOR_INDEX;
extern int P1_TAG_STRING_INDEX;
extern int P2_TAG_STRING_INDEX;
extern int P3_TAG_STRING_INDEX;
extern int P4_TAG_STRING_INDEX;
extern int TAG_COSTUME_TOGGLE_INDEX;
extern int CROWD_CHEER_TOGGLE_INDEX;

struct ConstantPair {
	int address;
	int* index;

	ConstantPair(int address, int &index) : address(address), index(&index) {}
};

//constant overrides
extern vector<ConstantPair> constantOverrides;
extern int SDI_DISTANCE_INDEX;
extern int HITSTUN_MULTIPLIER_INDEX;
extern int HITLAG_MAXIMUM_INDEX;
extern int HITLAG_MULTIPLIER_INDEX;
extern int ELECTRIC_HITLAG_MULTIPLIER_INDEX;
extern int ASDI_DISTANCE_INDEX;
extern int WALLJUMP_HORIZONTAL_MULTIPLIER_INDEX;
extern int MINIMUM_SHIELD_SIZE_SCALING_INDEX;
extern int SHIELD_DAMAGE_MULTIPLIER_INDEX;
extern int SHIELD_BASE_DAMAGE_INDEX;
extern int SHIELD_SIZE_MULTIPLIER_INDEX;
extern int SHIELD_TILT_MULTIPLIER_INDEX;
extern int KNOCKBACK_DECAY_MULTIPLIER_INDEX;
extern int WALL_BOUNCE_KNOCKBACK_MULTIPLIER_INDEX;
extern int STALING_TOGGLE_INDEX;

extern vector<int> Defaults;

#define MAX_SUBPAGE_DEPTH 20

//dpad same, + is start, 2 is A, 1 is B, A is Y, B is X, - is Z
const vector<u8> CODE_MENU_WIIMOTE_CONVERSION_TABLE = { 2, 3, 1, 0, 12, 31, 31, 31, 8, 9, 10, 11, 4, 31, 31, 31 };
//dpad same, + and - are start, A and B are A and B, C is Y, Z is start, 1 is X, 2 is Z
//dpad same, + is start, A and B are A and B, C is Y, Z is Z, - is X
const vector<u8> CODE_MENU_WIICHUCK_CONVERSION_TABLE = { 0, 1, 2, 3, 12, 31, 31, 31, 31, 31, 9, 8, 10, 4, 11, 31 };
const vector<u8> CODE_MENU_CLASSIC_CONVERSION_TABLE = { 3, 0, 4, 10, 8, 11, 9, 4, 31, 5, 12, 31, 12, 6, 2, 1 }; //1 to 1

#ifdef RIDLEY
const vector<string> CHARACTER_LIST = { "Bowser", "Captain Falcon", "Charizard", "Dedede", "Diddy Kong", "Donkey Kong", "Falco", "Fox", "Ganondorf", "Giga Bowser", "Ice Climbers", "Ike", "Ivysaur", "Jigglypuff", "Kirby", "Knuckles", "Link", "Lucario", "Lucas", "Luigi", "Mario", "Marth", "Meta Knight", "Mewtwo", "Mr. Game and Watch", "Ness", "Olimar", "Peach", "Pikachu", "Pit", "R.O.B.", "Ridley", "Roy", "Samus", "Sheik", "Snake", "Sonic", "Sopo", "Squirtle", "Toon Link", "Wario", "Warioman", "Wolf", "Yoshi", "Zelda", "Zero Suit Samus" };
const vector<u16> CHARACTER_ID_LIST = { 12, 10, 30, 35, 28, 1, 21, 7, 22, 44, 16, 37, 34, 39, 6, 53, 2, 36, 27, 9, 0, 19, 24, 51, 20, 11, 26, 13, 8, 25, 38, 56, 50, 3, 15, 42, 43, 17, 32, 40, 23, 45, 41, 5, 14, 4 };
#elif BUILD_TYPE == PROJECT_PLUS
const vector<string> CHARACTER_LIST = { "Bowser", "Captain Falcon", "Charizard", "Dedede", "Diddy Kong", "Donkey Kong", "Falco", "Fox", "Ganondorf", "Giga Bowser", "Ice Climbers", "Ike", "Ivysaur", "Jigglypuff", "Kirby", "Knuckles", "Link", "Lucario", "Lucas", "Luigi", "Mario", "Marth", "Meta Knight", "Mewtwo", "Mr. Game and Watch", "Ness", "Olimar", "Peach", "Pikachu", "Pit", "R.O.B.", "Roy", "Samus", "Sheik", "Snake", "Sonic", "Sopo", "Squirtle", "Toon Link", "Wario", "Warioman", "Wolf", "Yoshi", "Zelda", "Zero Suit Samus" };
const vector<u16> CHARACTER_ID_LIST = { 12, 10, 30, 35, 28, 1, 21, 7, 22, 44, 16, 37, 34, 39, 6, 53, 2, 36, 27, 9, 0, 19, 24, 51, 20, 11, 26, 13, 8, 25, 38, 50, 3, 15, 42, 43, 17, 32, 40, 23, 45, 41, 5, 14, 4 };
#else
const vector<string> CHARACTER_LIST = { "Bowser", "Captain Falcon", "Charizard", "Dedede", "Diddy Kong", "Donkey Kong", "Falco", "Fox", "Ganondorf", "Giga Bowser", "Ice Climbers", "Ike", "Ivysaur", "Jigglypuff", "Kirby", "Link", "Lucario", "Lucas", "Luigi", "Mario", "Marth", "Meta Knight", "Mewtwo", "Mr. Game and Watch", "Ness", "Olimar", "Peach", "Pikachu", "Pit", "R.O.B.", "Roy", "Samus", "Sheik", "Snake", "Sonic", "Sopo", "Squirtle", "Toon Link", "Wario", "Warioman", "Wolf", "Yoshi", "Zelda", "Zero Suit Samus" };
const vector<u16> CHARACTER_ID_LIST = { 12, 10, 30, 35, 28, 1, 21, 7, 22, 44, 16, 37, 34, 39, 6, 2, 36, 27, 9, 0, 19, 24, 51, 20, 11, 26, 13, 8, 25, 38, 50, 3, 15, 42, 43, 17, 32, 40, 23, 45, 41, 5, 14, 4 };
#endif



static const int START_OF_CODE_MENU_HEADER = 0x804E0000;
static const int CURRENT_PAGE_PTR_LOC = START_OF_CODE_MENU_HEADER; //4
static const int MAIN_PAGE_PTR_LOC = CURRENT_PAGE_PTR_LOC + 4; //4
static const int SALTY_RUNBACK_BUTTON_COMBO_LOC = MAIN_PAGE_PTR_LOC + 4; //4
static const int SKIP_RESULTS_BUTTON_COMBO_LOC = SALTY_RUNBACK_BUTTON_COMBO_LOC + 4; //4
//colors
static const int COLOR_ARRAY_START = SKIP_RESULTS_BUTTON_COMBO_LOC + 4; //4 * num colors
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
static const int AUTO_SAVE_REPLAY_FLAG_LOC = INFINITE_FRIENDLIES_FLAG_LOC + 4; //4
static const int ON_GROUP_RECORDS_FLAG_LOC = AUTO_SAVE_REPLAY_FLAG_LOC + 4; //4

static const int CODE_MENU_BUTTON_MASK_LOC = ON_GROUP_RECORDS_FLAG_LOC + 4; //4
static const int BUTTON_ACTIVATOR_MASK_LOC = CODE_MENU_BUTTON_MASK_LOC + 4; //4
static const int MAIN_BUTTON_MASK_LOC = BUTTON_ACTIVATOR_MASK_LOC + 4; //4 * 8

static const int OLD_DEBUG_STATE_LOC = MAIN_BUTTON_MASK_LOC + 4 * 8; //4
static const int OLD_CAMERA_LOCK_STATE_LOC = OLD_DEBUG_STATE_LOC + 4; //4

static const int OLD_CAMERA_POS_LOC = OLD_CAMERA_LOCK_STATE_LOC + 4; //4

static const int SAVE_STATE_BUFFER_PTR_LOC = OLD_CAMERA_POS_LOC + 4; //4
static const int SAVE_STATE_ARTICLE_LIST_PTR_LOC = SAVE_STATE_BUFFER_PTR_LOC + 4; //4
static const int SAVE_STATE_ARTICLE_ID_LIST_PTR_LOC = SAVE_STATE_ARTICLE_LIST_PTR_LOC + 4; //4
static const int SAVE_STATE_ARTICLE_SAVED_RESOURCE_LIST_PTR_LOC = SAVE_STATE_ARTICLE_ID_LIST_PTR_LOC + 4; //4
static const int SAVE_STATE_LOCATIONS_TO_UPDATE_PTR_LOC = SAVE_STATE_ARTICLE_SAVED_RESOURCE_LIST_PTR_LOC + 4; //4
static const int SAVE_STATE_LOCATIONS_TO_CLEAR_PTR_LOC = SAVE_STATE_LOCATIONS_TO_UPDATE_PTR_LOC + 4; //4
static const int SAVE_STATE_SAVED_ARTICLE_LIST_PTR_LOC = SAVE_STATE_LOCATIONS_TO_CLEAR_PTR_LOC + 4; //4

static const int RESET_LINES_STACK_LOC = SAVE_STATE_SAVED_ARTICLE_LIST_PTR_LOC + 4; // 4 * MAX_SUBPAGE_DEPTH + 8

static const int CHARACTER_SWITCHER_ARRAY_LOC = RESET_LINES_STACK_LOC + 4 * MAX_SUBPAGE_DEPTH + 8; //0x10
static const int INIFINITE_SHIELDS_ARRAY_LOC = CHARACTER_SWITCHER_ARRAY_LOC + 0x10; //0x10
static const int PERCENT_SELCTION_VALUE_ARRAY_LOC = INIFINITE_SHIELDS_ARRAY_LOC + 0x10; //0x10
static const int PERCENT_SELCTION_ACTIVATOR_ARRAY_LOC = PERCENT_SELCTION_VALUE_ARRAY_LOC + 0x10; //0x10
static const int DISABLE_DPAD_ACTIVATOR_ARRAY_LOC = PERCENT_SELCTION_ACTIVATOR_ARRAY_LOC + 0x10; //0x10

static const int ENDLESS_ROTATION_QUEUE_LOC = DISABLE_DPAD_ACTIVATOR_ARRAY_LOC + 0x10; //8
static const int ENDLESS_ROTATION_PLACEMENT_LIST_LOC = ENDLESS_ROTATION_QUEUE_LOC + 8; //4 * 4
static const int ENDLESS_ROTATION_COMP_FUNC_LOC = ENDLESS_ROTATION_PLACEMENT_LIST_LOC + 4 * 4; //4 * 4

static const int REPLAY_NTE_DATA_BUFFER_LOC = ENDLESS_ROTATION_COMP_FUNC_LOC + 4 * 4; //0x14
static const int REPLAY_CREATE_SECTION_BUFFER_LOC = REPLAY_NTE_DATA_BUFFER_LOC + 0x14; //8
static const int REPLAY_CRYPTO_BUFFER_LOC = REPLAY_CREATE_SECTION_BUFFER_LOC + 8; //0x30

static const int CODE_MENU_WIIMOTE_CONVERSION_TABLE_LOC = REPLAY_CRYPTO_BUFFER_LOC + 0x30; //0x10 * 3

static const int P1_TAG_HEX_LOC = CODE_MENU_WIIMOTE_CONVERSION_TABLE_LOC + 0x10 * 3; //0x18
static const int P2_TAG_HEX_LOC = P1_TAG_HEX_LOC + 0x18; //0x18
static const int P3_TAG_HEX_LOC = P2_TAG_HEX_LOC + 0x18; //0x18
static const int P4_TAG_HEX_LOC = P3_TAG_HEX_LOC + 0x18; //0x18

static const int P1_STOP_LOAD_FLAG_PTR_LOC = P4_TAG_HEX_LOC + 0x18; //4
static const int P2_STOP_LOAD_FLAG_PTR_LOC = P1_STOP_LOAD_FLAG_PTR_LOC + 4; //4
static const int P3_STOP_LOAD_FLAG_PTR_LOC = P2_STOP_LOAD_FLAG_PTR_LOC + 4; //4
static const int P4_STOP_LOAD_FLAG_PTR_LOC = P3_STOP_LOAD_FLAG_PTR_LOC + 4; //4

static const int RANDOM_ALTS_RNG = P4_STOP_LOAD_FLAG_PTR_LOC + 4; //4
static const int RANDOM_ALTS_MATCH_START_FLAG = RANDOM_ALTS_RNG + 4; //4

static const int TEAM_SETTINGS_LOC = RANDOM_ALTS_MATCH_START_FLAG + 4; //4
static const int TAG_LOAD_FLAGS_LOC = TEAM_SETTINGS_LOC + 4; //4

static const int PREV_TAG_COSTUMES_SETTING_LOC = TAG_LOAD_FLAGS_LOC + 4; //4

static const int DOLPHIN_MOUNT_VF_LOC = PREV_TAG_COSTUMES_SETTING_LOC + 4; //4

static const int DRAW_SETTINGS_BUFFER_LOC = DOLPHIN_MOUNT_VF_LOC + 4; //0x200

static const int START_OF_CODE_MENU = DRAW_SETTINGS_BUFFER_LOC + 0x200;

static int CurrentOffset = START_OF_CODE_MENU;

#define CODE_MENU_GECKO_IF(MenuIndex) if(MenuIndex != -1) {\
									GeckoIf(MenuIndex, EQUAL, 1); {
#define CODE_MENU_GECKO_ENDIF }GeckoEndIf(); }

//code menu flags
#define CODE_MENU_CLOSED 0
#define CODE_MENU_PRIMED 1
#define CODE_MENU_TRIGGERED 2
#define CODE_MENU_CLOSING 3
#define CODE_MENU_OPEN 4

//line types
#define SELECTION_LINE 0
#define INTEGER_LINE 1
#define FLOATING_LINE 2
#define HAS_VALUE_LIMIT 2
#define SUB_MENU_LINE 3
#define COMMENT_LINE 4
#define PRINT_LINE 5

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
#define FIRST_FRAME_ADVANCE_NUM_WAIT_FRAMES 16
#define TRIGGER_ENTER_SUB_MENU_BUTTON BUTTON_A
#define TRIGGER_LEAVE_SUB_MENU_BUTTON BUTTON_B
#define TRIGGER_RESET_LINE_BUTTON BUTTON_X
#define TRIGGER_RESET_PAGE_BUTTON BUTTON_Y
#define SCROLL_DISTANCE 15

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
#if BUILD_TYPE == PROJECT_PLUS
static fstream MenuFile("C:\\Users\\johno\\Documents\\Modding\\Dolphin\\SD_Raws\\data.cmnu", fstream::out | fstream::binary);
#else
static fstream MenuFile("C:\\Users\\johno\\Documents\\Modding\\Dolphin\\SD_Raws\\cm.bin", fstream::out | fstream::binary);
#endif

class Page;

class Line
{
public:
	Line() {}

	Line(string Text, u16 TextOffset, u8 type, u8 flags, u8 ColorOffset, int* Index = nullptr) {
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
		if (Size == 0) {
			cout << Text << endl;
		}
		AddValueToByteArray(type, output);
		AddValueToByteArray(Flags, output);
		AddValueToByteArray(Color, output);
		AddValueToByteArray(TextOffset, output);
		AddValueToByteArray(lineNum, output);
		//AddValueToByteArray((u8) 0, output);
		AddValueToByteArray(Value, output);
		if (type == PRINT_LINE) {
			AddValueToByteArray(numArgs, output);
		}
		else if (type != COMMENT_LINE) {
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

	int* Index = nullptr;
	u32 numArgs;
	u32 Value;
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
	u16 TextOffset;
	u16 DownOffset;
	u16 UpOffset;
	string Text;
	u16 Size;
	u8 lineNum;
	int Padding;
	vector<int*> args;
	//offsets
	static const int SIZE = 0; //2
	static const int TYPE = SIZE + 2; //1
	static const int FLAGS = TYPE + 1; //1
	static const int COLOR = FLAGS + 1; //1
	static const int TEXT_OFFSET = COLOR + 1; //2
	static const int LINE_NUM = TEXT_OFFSET + 2; //1
	static const int VALUE = LINE_NUM + 1; //4
	static const int PRINT_LINE_NUM_ARGS = VALUE + 4; //4
	static const int PRINT_LINE_TEXT_START = PRINT_LINE_NUM_ARGS + 4;
	static const int COMMENT_LINE_TEXT_START = VALUE + 4;
	static const int UP = VALUE + 4; //2
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
	Comment(string Text, int* Index = nullptr)
	: Line(Text, COMMENT_LINE_TEXT_START, COMMENT_LINE, 0, COMMENT_LINE_COLOR_OFFSET, Index) {}

	void WriteLineData()
	{
		Line::WriteLineData();
	}
};

class Print : public Line {
public:
	Print(string Text, vector<int*> args = {})
		: Line(Text, PRINT_LINE_TEXT_START, PRINT_LINE, 0, COMMENT_LINE_COLOR_OFFSET) {
		this->args = args;
		this->numArgs = args.size();
		Size += args.size() * 4;
		for (auto x : args) {
			argValues.push_back(*x);
		}
		//cout << Size << endl;
	}

	void WriteLineData()
	{
		Line::WriteLineData();
		for (auto x : argValues) {
			sprintf(OpHexBuffer, "%08X", x);
			cout << Text << ": " << OpHexBuffer << endl;
			x = _byteswap_ulong(x);
			
			MenuFile.write((const char*)& x, 4);
			//sprintf(OpHexBuffer, "%08X", x);
			//cout << Text << ": " << OpHexBuffer << endl;
			//MenuFile << OpHexBuffer;
		}
	}

private:
	vector<int> argValues;
};

class Selection : public Line
{
public:
	Selection(string Text, vector<string> Options, vector<u16> Values, int Default, int &Index)
	: Line(CreateSelectionString(Text + ":  %s", Options), SELECTION_LINE_OFFSETS_START + Options.size() * 4, SELECTION_LINE, 0, NORMAL_LINE_COLOR_OFFSET, &Index)
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
		Value = Default;
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
	: Line(Text + " >", SUB_MENU_LINE_TEXT_START, SUB_MENU_LINE, 0, NORMAL_LINE_COLOR_OFFSET)
	{
		this->SubMenuPtr = SubMenuPtr;
	}
};

class Integer : public Line
{
public:
	Integer(string Text, int Min, int Max, int Default, int Speed, int &Index)
	: Line(Text + ":  %d", NUMBER_LINE_TEXT_START, INTEGER_LINE, 0, NORMAL_LINE_COLOR_OFFSET, &Index)
	{
		this->Min = Min;
		this->Max = Max;
		Value = Default;
		this->Default = Default;
		Defaults.push_back(Default);
		this->Speed = Speed;
	}
};

class Floating : public Line
{
public:
	Floating(string Text, float Min, float Max, float Default, float Speed, int &Index, string format = "%f")
	: Line(Text + ":  " + format, NUMBER_LINE_TEXT_START, FLOATING_LINE, 0, NORMAL_LINE_COLOR_OFFSET, &Index)
	{
		this->Min = GetHexFromFloat(Min);
		this->Max = GetHexFromFloat(Max);
		Value = GetHexFromFloat(Default);
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
		for(int i = 0; i < Lines.size(); i++) {
			Lines[i]->lineNum = i;
		}
		//Lines.back()->Size = 0;
		ConnectSelectableLines();
	}
	
	void WritePage()
	{
		vector<u8> output;
		AddValueToByteArray(CurrentLineOffset, output);
		AddValueToByteArray(PrevPageOffset, output);
		AddValueToByteArray(NumChangedLines, output);
		AddValueToByteArray(PrintLowHold, output);
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
			if (Lines[i]->type != COMMENT_LINE && Lines[i]->type != PRINT_LINE) {
				SelectableLines.push_back(i);
			}
		}
	}

	u32 CurrentLineOffset;
	u32 Size;
	u32 PrevPageOffset = 0;
	u32 NumChangedLines = 0;
	u32 PrintLowHold = 0;
	vector<Line*> Lines;
	SubMenu CalledFromLine;
	static const int NUM_WORD_ELEMS = 4;
	static const int CURRENT_LINE_OFFSET = 0;
	static const int PREV_PAGE = CURRENT_LINE_OFFSET + 4;
	static const int NUM_CHANGED_LINES = PREV_PAGE + 4;
	static const int PRINT_LOW_HOLD = NUM_CHANGED_LINES + 4;
	static const int FIRST_LINE_OFFSET = NUM_WORD_ELEMS * 4;
};

void PrintChar(int SettingsPtrReg, int CharReg);
void PrintString(int StringPtrReg, int NumCharsReg, int SettingsPtrReg);
void DrawBlackBackground();
void PrintPage(int PageReg, int SettingPtrReg, int Reg1, int Reg2, int Reg3, int Reg4, int Reg5, int Reg6);
void PrintCodeMenuLine(int LinePtrReg, int SettingsPtrReg, int ColorArrayPtrReg, int TempReg1, int TempReg2);
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
void LeaveMenu(int PageReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5, int TempReg6, int ActionReg);
void DecreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5);
void IncreaseValue(int LineReg, int PageReg, int TypeReg, int TempReg1, int TempReg2, int TempReg3, int TempReg4, int TempReg5);
void Move(int LineReg, int PageReg, int NextLineOffset, int TempReg1, int TempReg2);
void GetActionFromInputs(int ButtonReg, int ControlStickXReg, int ControlStickYReg, int ResultReg);
void SetControlStickAction(int StickValReg, int TimerLoc, int NumWaitFrames, int FirstTimeNumWaitFrames, int Threshhold, int PositiveAction, int NegativeAction, int ResultReg);
void ApplyMenuSetting(int Index, int Destination, int reg1, int reg2, int size = 4);
void GetArrayValueFromIndex(int ArrayLoc, int IndexReg, int min, int max, int ResultReg = 3);
void RunIfPortToggle(int ARRAY_LOC, int PortReg);
void SaveReplay();
void SetupPrintText(int SettingPtrReg);
void SetTextPos(int XPosReg, int YPosReg, int SettingPtrReg);
void constantOverride();