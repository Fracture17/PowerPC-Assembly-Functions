#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

#define SPECIAL_CSTICK_OPTIONS BUILD_TYPE == PROJECT_PLUS

class Color {
public:
	Color(u8 red, u8 green, u8 blue, u8 alpha = 0) {
		this->red = red;
		this->green = green;
		this->blue = blue;
		this->alpha = alpha;
	}

private:
	u8 red;
	u8 green;
	u8 blue;
	u8 alpha;
};

class ShieldColor {
public:
	ShieldColor(string name, Color);
};

/*#define BUTTON_MENU_SIZE 11
//#define CONTROL_MENU_BASE_SIZE 7
#define CONTROL_MENU_BASE_SIZE 8
#define MENU_WRITE_LOC 0x805b4c00
#define TAG_LIST_START_LOC 0x90172e20
#define TAG_LIST_SIZE 0x124
#define GCC_SETTINGS_TAG_OFFSET 0x14
#define TAG_TIME_OFFSET 0x10
#define MENU_FIRST_CREATION_TIME_LOC 0x90172e30

const int MENU_BLOCK_PTRS = 0x935ce300;
const int MENU_BUTTON_STRING_LOC = MENU_BLOCK_PTRS + 4 * 4;
const int MENU_CONTROL_STRING_LOC = MENU_BUTTON_STRING_LOC + 8 * 12;
const int TAG_IN_USE_LOC = MENU_CONTROL_STRING_LOC + 8 * 12;
const int REPLACE_NAME_OLD_TIME_LOC = TAG_IN_USE_LOC + 4;
const int REPLACE_NAME_TIME_ADDRESS = REPLACE_NAME_OLD_TIME_LOC + 4;
//state=0, pos=1, size=2, buttonPos=3, buttonCancel=4-5, newSize=6, newPos=7, reopenFlag=8, replaceTimeFlag=9, openFlag=10
const int MENU_STATE_INFO_OFFSET = 0x60;
const int MENU_SIZE_OFFSET = 0x6C;
const int MENU_INDEX_OFFSET = 0x70;
const int MENU_POS_OFFSET = 0x44;
//const int MENU_PORT_OFFSET = 0x47F80;
//const int MENU_PORT1_LOC = 0x81578BDC;
const int MENU_PORT_NUM_OFFSET = 0x57;
const int MENU_SELECTED_TAG_OFFSET = 0x164;*/

void MenuControlCodes();
void SetMenuOpen();
void SetIfMenuOpen();
void ControlMenuState();
void WriteMenu();
void SetMenuData();
void clearMenuState();
void ReplaceNameFunctions();
void PortRumbleColorChange();
void TogglePortRumble();
void OpenNameEntry();
void NameIsFound();
void ReplaceName();
void StopNameListReordering();
void ReopenNameList();
void OpenNameList();
void FixCursorSetting();
void SkipSizeCheck();
void GreyOutUsedNames();
void StopSelectionOfUsedNames();
void ResetTagUsedList();
void PinTag(int MenuPtrReg, int TempReg1, int TempReg2, int TempReg3);
void preventMenuCloseRaceCondition();