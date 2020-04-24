#include "stdafx.h"
#include "Controls.h"
#include <cassert>

void MenuControlCodes()
{
	string ButtonList = { "L|||||||R|||||||Z|||||||D UP||||D SIDE||D DOWN||A|||||||B|||||||CSTICK||Y|||||||X|||||||TAP|||||" };
	GeckoStringWrite(ButtonList.c_str(), MENU_BUTTON_STRING_LOC);
	assert(ButtonList.size() == MENU_CONTROL_STRING_LOC - MENU_BUTTON_STRING_LOC);
	
	string ConfigList = { "ATTACK||SPECIAL|JUMP||||SHIELD||GRAB||||U TAUNT|S TAUNT|D TAUNT|NONE||||SMASH|||OFF|||||ON||||||" };
	GeckoStringWrite(ConfigList.c_str(), MENU_CONTROL_STRING_LOC);
	assert(ConfigList.size() == TAG_IN_USE_LOC - MENU_CONTROL_STRING_LOC);

	string ExtraList = { "TAUNT|||CHARGE||TILT||||" };
	GeckoStringWrite(ExtraList.c_str(), CSTICK_TAUNT_SPECIAL_WORDS);
	assert(ExtraList.size() == 8 * 3);

	SetMenuOpen();

	SetIfMenuOpen();

	ControlMenuState();

	WriteMenu();

	SetMenuData();

	ReplaceNameFunctions();

#if BUILD_TYPE != PROJECT_PLUS
	PortRumbleColorChange();
#endif

	TogglePortRumble();

	clearMenuState();

	preventMenuCloseRaceCondition();
}

//Prevents munu from closing unintentionally by setting buttons to 0 if the menu is open
void preventMenuCloseRaceCondition() {
	//r3 = menu ptr
	//r4 + 0xC = buttons
	ASMStart(0x8069fe88);
	SaveRegisters();

	int reg1 = 31;
	int menuReg = 3;
	int buttonReg = 4;

	LBZ(reg1, menuReg, MENU_STATE_INFO_OFFSET);
	If(reg1, GREATER_OR_EQUAL_I, 2); { //in button or control menu
		SetRegister(reg1, 0);
		STW(reg1, buttonReg, 0xC);
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x9421ffc0); //stwu sp, -0x40(sp)
}

//contains the functions to replace names
void ReplaceNameFunctions()
{
	OpenNameEntry();

	NameIsFound();

	ReplaceName();

	//StopNameListReordering();

	ReopenNameList();

	SkipSizeCheck();

	GreyOutUsedNames();

	StopSelectionOfUsedNames();

	ResetTagUsedList();
}

void PortRumbleColorChange()
{
	//r3 = name list object
	//set r17 to 0xFF if rumble is on
	//can use r17-r31
	ASMStart(0x8069f83c);

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;

	LBZ(reg1, 3, 0x60);
	If(reg1, LESS_OR_EQUAL_I, 1); {
		LBZ(reg1, 3, 0x57);
		ADDI(reg1, reg1, -0x31); //get 0 based port num
		SetRegister(reg2, 0x9017be60);
		LBZX(reg1, reg2, reg1);
		If(reg1, EQUAL_I, 1); {
			SetRegister(17, 0xFF);
		}EndIf();
	}EndIf();

	ASMEnd(0x80030000); //lwz r0, r3, 0


	//make "player #" change color if highlighted
	//r3 = name list object
	//can use r4, r31
	ASMStart(0x806a04f0);

	reg1 = 31;
	reg2 = 4;
	
	LWZ(reg1, 3, 0x44);
	If(reg1, EQUAL_I, 0); {
		LBZ(reg1, 3, 0x60);
		If(reg1, LESS_OR_EQUAL_I, 1); {
			LBZ(reg1, 3, 0x57);
			ADDI(reg1, reg1, -0x31); //get 0 based port num
			SetRegister(reg2, 0x9017be60);
			LBZX(reg1, reg2, reg1);
			If(reg1, EQUAL_I, 1); {
				SetRegister(17, 0xFF);
			}EndIf();
		}EndIf();
	}EndIf();

	ASMEnd(0x3fe0806a); //lis r31, 0x806A
}

void TogglePortRumble()
{
	//r6 is inputs
	ASMStart(0x8069fecc);

	LBZ(4, 3, 0x60);
	If(4, LESS_OR_EQUAL_I, 1); {
		ANDI(14, 6, BUTTON_X);
		If(14, EQUAL_I, BUTTON_X); {
			LWZ(14, 3, 0x44);
			If(14, EQUAL_I, 0); {
				LBZ(14, 3, 0x57);
				ADDI(14, 14, -0x31); //get 0 based port num
				SetRegister(4, 0x9017be60);
				LBZUX(14, 4, 14);
				XORI(14, 14, 1);
				STB(14, 4, 0);
			}EndIf();
		}EndIf();
	}EndIf();

	ASMEnd(0x540405ef);
}

//resets the tag in use flags when the CSS is loaded
void ResetTagUsedList()
{
	//set r4 = 42 when done

	ASMStart(0x806828e0);

	int Reg1 = 3;
	int Reg2 = 4;


	SetRegister(Reg1, TAG_IN_USE_LOC);
	SetRegister(Reg2, 0xFFFFFFFF);
	STW(Reg2, Reg1, 0);
	SetRegister(4, 42); //reset r4


#if BUILD_TYPE == PMEX
	ASMEnd(0x38600854); //li r3, 2132
#else
	ASMEnd(0x38600654); //li r3, 1620
#endif
}

//stops names that are being edited from being selected
void StopSelectionOfUsedNames()
{
	//r4 == target
	//r26 == menu ptr
	//set r25 = 1 if shouldn't select, else do nothing
	//set r27 = 3 if shouldn't select, else do nothing

	ASMStart(0x806a004c);

	int Reg1 = 3;
	int Reg2 = 5;
	int Reg3 = 6;
	int Reg4 = 7;
	int Reg5 = 8;


	SetRegister(Reg1, TAG_IN_USE_LOC);
	FindInArray(4, Reg1, 4, 1, Reg2, Reg3);
	If(Reg2, GREATER_OR_EQUAL_I, 0); //found
	SetRegister(25, 1);
	SetRegister(27, 3);
	EndIf(); //found

	ASMEnd(0x3c60805a); //lis r3, 0x805A
}

//grays out names that are being edited
//stops names from being grayed if editing
void GreyOutUsedNames()
{
	//r4 == target value
	//r0 == 1 if name has been selected, else 0
	//r24 == menu ptr
	//set r5 to 1 if should be greyed, else 0

	ASMStart(0x8069f9e4);

	int Reg1 = 20;
	int Reg2 = 19;
	int Reg3 = 18;

	SetRegister(5, 0);
	LBZ(Reg1, 24, MENU_STATE_INFO_OFFSET);
	If(Reg1, EQUAL_I, 1); //in name list

	SetRegister(Reg1, TAG_IN_USE_LOC);
	FindInArray(4, Reg1, 4, 1, Reg2, Reg3);
	If(Reg2, GREATER_OR_EQUAL_I, 0); //name is used
	SetRegister(5, 1); //grey out name
	EndIf(); //name is used

	OR(5, 5, 0); //should grey if either 5 or 0 == 1

	EndIf(); //in name list


	ASMEnd(0x2c050000); //cmpwi r5, 0
}

//skips the name list size check when replacing a name
void SkipSizeCheck()
{
	//r29 + 0x1FC == menu ptr
	//r3 has size of name list

	ASMStart(0x8069b840);

	int Reg1 = 19;


	LBZ(Reg1, 29, MENU_STATE_INFO_OFFSET + 0x1FC);
	If(Reg1, EQUAL_I, 0); //is replacing
	CMPI(3, 1000, 0); //should always be smaller
	Else(); //is replacing
	CMPI(3, 120, 0); //normal comparison
	EndIf(); //is replacing

	ASMEnd();
}

//closes the name list and opens the name entry menu
void OpenNameEntry()
{
	//r23 + 0x1FC == menu ptr

	ASMStart(0x8068a0f4);

	int Reg1 = 28;
	//only use if flag is set
	int Reg2 = 3;


	LBZ(Reg1, 23, MENU_STATE_INFO_OFFSET + 0x1FC);
	If(Reg1, EQUAL_I, 0); //flag is set
		//call close name list
		MFCTR(Reg1); //save ctr
		SetRegister(Reg2, 0x806a0714);
		MTCTR(Reg2);
		ADDI(Reg2, 23, 0x1FC); //set r3 to menu ptr
		BCTRL();
		MTCTR(Reg1);

		SetRegister(Reg2, 2);
	EndIf(); //flag is set

	ASMEnd(0x2c030001); //cmpwi r3, 1
}

//clears the flag used to reopen the name list if name already exists
void NameIsFound()
{
	//r29 + 0x1FC == menu ptr

	ASMStart(0x8069b868);

	int Reg1 = 19;


	SetRegister(Reg1, 1);
	STB(Reg1, 29, MENU_STATE_INFO_OFFSET + 0x1FC);

	ASMEnd(0x7c731b78); //mr r19, r3
}

//if the replace flag is set, overwrites the selected tag with the new one
//sets the flag to reopen name list
void ReplaceName()
{
	//set r19 to 0 based tag index
	//r29 + 0x1FC == menu ptr

	ASMStart(0x8069b87c);

	int Reg1 = 3;
	int Reg2 = 4;
	int Reg3 = 5;
	int Reg4 = 6;


	LBZ(Reg1, 29, MENU_STATE_INFO_OFFSET + 0x1FC); //get state
	If(Reg1, EQUAL_I, 0); //should replace name
		//get tag index
		LBZ(19, 29, MENU_POS_OFFSET + 3 + 0x1FC);
		MULLI(19, 19, 2);
		ADDI(19, 19, MENU_INDEX_OFFSET - 2 + 0x1FC);
		LHZX(19, 29, 19); //r19 has correct value
		
		//compensate for L customs
		LoadWordToReg(Reg3, 0x80002800); //get if is triggered
		If(Reg3, EQUAL_I, 1); //need to compensate
		LoadWordToReg(Reg3, 0x805a00E0);
		LWZ(Reg3, Reg3, 0x1C);
		STB(19, Reg3, 0x28);
		EndIf(); //need to compensate

		MULLI(Reg1, 19, TAG_LIST_SIZE);
		SetRegister(Reg2, TAG_LIST_START_LOC + TAG_TIME_OFFSET);
		ADD(Reg2, Reg2, Reg1);
		SetRegister(Reg1, REPLACE_NAME_TIME_ADDRESS);
		STW(Reg2, Reg1, 0); //store address of name
		LWZ(Reg2, Reg2, 0);

		//store creation time
		SetRegister(Reg1, REPLACE_NAME_OLD_TIME_LOC);
		STW(Reg2, Reg1, 0);

		//set replace time flag
		SetRegister(Reg2, 1);
		STB(Reg2, 29, MENU_STATE_INFO_OFFSET + 9 + 0x1FC);

		//correct name list cursor index
		//may not be needed
		//ADDI(Reg1, 19, 1);
		//STW(Reg1, 29, MENU_POS_OFFSET + 0x1FC);

	EndIf(); //should replace name

	SetRegister(Reg1, 0);
	STB(Reg1, 29, MENU_STATE_INFO_OFFSET + 0x1FC); //set flag to reopen menu

	ASMEnd(0x7e83a378); //mr r3, r20
}

//stops the call to qsort that messes up the order of the names
void StopNameListReordering()
{
	ASMStart(0x8069f378);
	ASMEnd();
}

//opens the name list after entry list closes if flag is set
void ReopenNameList()
{
	OpenNameList();

	FixCursorSetting();
}

//opens the name list
void OpenNameList()
{
	//r30 == port num
	//r29 + 0x1FC == menu ptr
	//r3 should equal menu ptr
	//r4 should equal 0
	//r5 should equal position in name list

	ASMStart(0x8069b9f8);

	int Reg1 = 3;
	//use after call to close
	int Reg2 = 11;


	//close name entry
	SetRegister(Reg1, 0x80674b64); //address of close name entry
	MTCTR(Reg1);
	ADDI(Reg1, 29, 880);
	BCTRL();

	//reset "tag in use" flag
	SetRegister(Reg1, TAG_IN_USE_LOC);
	SetRegister(Reg2, 0xFF);
	STBX(Reg2, Reg1, 30);

	LBZ(Reg1, 29, MENU_STATE_INFO_OFFSET + 0x1FC);
	If(Reg1, EQUAL_I, 0); //should re-open
		//restore old creation time
		//add conditional check
		LoadWordToReg(Reg1, REPLACE_NAME_OLD_TIME_LOC);
		LoadWordToReg(Reg2, REPLACE_NAME_TIME_ADDRESS);
		STW(Reg1, Reg2, 0);

		SetRegister(Reg1, 1);
		STB(Reg1, 29, MENU_STATE_INFO_OFFSET + 8 + 0x1FC);
		SetRegister(Reg1, 0x8069f240); //address of open name list
		MTCTR(Reg1);
		ADDI(3, 29, 0x1FC);
		SetRegister(4, 0);
		LWZ(5, 3, MENU_POS_OFFSET);
		LWZ(Reg2, 3, MENU_SIZE_OFFSET);
		If(5, LESS_I, 0); //at top of list
			LBZ(5, 3, MENU_SIZE_OFFSET + 3);
			//ADDI(5, 5, 1);
		Else(); If(5, GREATER, Reg2);//at top of list
			LBZ(5, 3, MENU_SIZE_OFFSET + 3);
			//ADDI(5, 5, 1);
		Else(); //at top of list
			MULLI(5, 5, 2);
			ADDI(5, 5, MENU_INDEX_OFFSET - 2);
			LHZX(5, 3, 5);
			//ADDI(5, 5, 1); //get tag index
		EndIf(); EndIf(); //at top of list
		//ADDI(5, 5, -1);
		BCTRL();
	EndIf(); //should re-open

	ASMEnd();
}

//makes the name list cursor work
void FixCursorSetting()
{
	//r24 + 0x1FC == menu ptr
	//use r5

	ASMStart(0x8068a278);

	LBZ(5, 24, MENU_STATE_INFO_OFFSET + 8 + 0x1FC);
	If(5, EQUAL_I, 1); //should change settings
		//r4 already equals 0
		STB(4, 24, MENU_STATE_INFO_OFFSET + 8 + 0x1FC);
		SetRegister(4, 7);
		LBZ(5, 24, MENU_PORT_NUM_OFFSET + 0x1FC);
		ADDI(5, 5, -0x31);
	Else(); //should change settings
		SetRegister(5, 0);
	EndIf(); //should change settings

	ASMEnd();
}

//sets the state of the menu to "open", and clears any other state data
void SetMenuOpen()
{
	ASMStart(0x8069f254);

	SetRegister(11, 0x01000000);
	STW(11, 3, MENU_STATE_INFO_OFFSET);
	SetRegister(11, 0xFFFF00F0);
	STW(11, 3, MENU_STATE_INFO_OFFSET + 4);

	ASMEnd(0x39610020); //addi r11, r1, 32
}

//stores the ptr to the menu the port is attatched to
//if not 0, the menu is open
void SetIfMenuOpen()
{
	ASMStart(0x8069feac);

	int Reg1 = 26;
	int Reg2 = 28;

	LWZ(Reg1, 3, 0);
	If(Reg1, EQUAL_I, 0); //is in name menu

	LBZ(Reg1, 3, MENU_PORT_NUM_OFFSET);
	SetRegister(Reg2, MENU_BLOCK_PTRS);

	If(Reg1, EQUAL_I, '1'); //port 1
	STW(3, Reg2, 0);
	EndIf(); //port 1

	If(Reg1, EQUAL_I, '2'); //port 2
	STW(3, Reg2, 4);
	EndIf(); //port 2

	If(Reg1, EQUAL_I, '3'); //port 3
	STW(3, Reg2, 8);
	EndIf(); //port 3

	If(Reg1, EQUAL_I, '4'); //port 4
	STW(3, Reg2, 0xC);
	EndIf(); //port 4

	EndIf(); //is in name menu

	ASMEnd(0x7c7a1b78); //mr r26, r3
}

void PinTag(int MenuPtrReg, int TempReg1, int TempReg2, int TempReg3)
{
	//get current tag's creation time offset
	LWZ(TempReg1, MenuPtrReg, MENU_POS_OFFSET);
	MULLI(TempReg1, TempReg1, 2);
	ADDI(TempReg1, TempReg1, MENU_INDEX_OFFSET - 2);
	LHZX(TempReg1, MenuPtrReg, TempReg1);
	MULLI(TempReg1, TempReg1, TAG_LIST_SIZE);

	//get current first tag's creation time offset
	LHZ(TempReg2, MenuPtrReg, MENU_INDEX_OFFSET);
	MULLI(TempReg2, TempReg2, TAG_LIST_SIZE);

	SetRegister(TempReg3, TAG_LIST_START_LOC + TAG_TIME_OFFSET);

	LWZX(TempReg2, TempReg3, TempReg2); //get current first time
	ADDI(TempReg2, TempReg2, -1); //get a smaller time
	STWX(TempReg2, TempReg3, TempReg1); //replace current tag's time
}

void clearMenuState() {
	ASMStart(0x806a0714);

	int reg1 = 4;
	int reg2 = 5;
	int reg3 = 6;
	int menuPtrReg = 3;

	//clear menu state
	SetRegister(reg1, 0x01000000);
	STW(reg1, menuPtrReg, MENU_STATE_INFO_OFFSET);
	//STW(reg1, menuPtrReg, MENU_STATE_INFO_OFFSET + 4);
	//STW(reg1, menuPtrReg, MENU_STATE_INFO_OFFSET + 8);

	//clear tag in use
	LBZ(reg1, menuPtrReg, MENU_PORT_NUM_OFFSET);
	SetRegister(reg2, TAG_IN_USE_LOC - 1);
	ANDI(reg1, reg1, 0xF);
	SetRegister(reg3, 0xFF);
	STBX(reg3, reg2, reg1);

	ASMEnd(0x9421fff0); //stwu sp, -0x10 (sp)
}

void ControlMenuState()
{
	//r26 == PLAY_BUTTON_LOC_START - 4

	//ASMStart(0x8002973c);
	ASMStart(0x80029738);
	
	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 5;
	int Reg4 = 4;
	int Reg5 = 3;
	int Reg6 = 6;
	int Reg7 = 0;

	int ButtonReg = 26;

	SetRegister(Reg1, 0); //port counter
	ADDI(ButtonReg, ButtonReg, 6); //port input ptr
	While(Reg1, LESS_I, 16); { //loop through ports

		SetRegister(Reg2, MENU_BLOCK_PTRS);
		LWZX(Reg3, Reg2, Reg1);
		If(Reg3, NOT_EQUAL_I, 0); { //is open for port

		//stop buttons that shouldn't be pressed
			LHZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
			LHZ(Reg2, ButtonReg, 0);
			AND(Reg4, Reg4, Reg2);
			STH(Reg4, ButtonReg, 0);
			EQV(Reg4, Reg4, Reg2);
			STH(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);

			LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //get menu state
			If(Reg4, EQUAL_I, 1); { //determine state
			//in name selection //////////////////////////////////////////////////////////////////////////////////////

			//check if in proper bounds
				LBZ(Reg6, Reg3, MENU_POS_OFFSET + 3);
				ADDI(Reg2, Reg6, -1);
				LBZ(Reg4, Reg3, MENU_SIZE_OFFSET + 3);
				If(Reg2, LESS_L, Reg4); { //is in bounds

				//check if tag is in use
				//set Reg4 to 0 if true
				/*LBZ(Reg2, Reg3, MENU_POS_OFFSET + 3);
				ADDI(Reg2, Reg2, -1); //target value*/
					LBZ(Reg2, Reg3, MENU_POS_OFFSET + 3);
					ADD(Reg2, Reg2, Reg2); //Reg2 *= 2
					ADDI(Reg2, Reg2, MENU_INDEX_OFFSET - 2);
					LHZX(Reg2, Reg3, Reg2); //target index
					SetRegister(Reg6, TAG_IN_USE_LOC); //search location
					FindInArray(Reg2, Reg6, 4, 1, Reg4, Reg7);

					ADDI(Reg6, Reg3, MENU_SELECTED_TAG_OFFSET + 3); //search location
					FindInArray(Reg2, Reg6, 4, 4, Reg5, Reg7);

					AND(Reg4, Reg4, Reg5); //if both == -1, result is negative

					If(Reg4, GREATER_OR_EQUAL_I, 0); { //found match
						SetRegister(Reg4, 0); //stop potential Z or Y press
					} Else(); { //found match
						LHZ(Reg4, ButtonReg, 0); //get buttons pressed
					} EndIf(); //found match

					//pin tag if start pressed
					ANDI(Reg6, Reg4, 0x1000);
					If(Reg6, NOT_EQUAL_I, 0); { //start is pressed
						PinTag(Reg3, Reg6, Reg5, Reg4);
						//stop new start input
						LHZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
						ANDI(Reg4, Reg4, ~0x1000);
						STH(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
						SetRegister(Reg4, 0); //clear Reg4
					} EndIf(); //start is pressed

					//set "tag in use" flag if Z or Y pressed
					ANDI(Reg6, Reg4, 0x810);
					If(Reg6, NOT_EQUAL_I, 0); { //Z or Y pressed
						SetRegister(Reg6, TAG_IN_USE_LOC);
						RLWINM(Reg5, Reg1, 30, 0, 31); // Reg1 >> 2
						STBX(Reg2, Reg6, Reg5);
					} EndIf(); //Z or Y pressed

					ANDI(Reg2, Reg4, 0x10); //get if Z is pressed
					If(Reg2, NOT_EQUAL_I, 0); { //Z is pressed
						SetRegister(Reg4, 0);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //change state
						//Reg4 won't trigger Y is pressed
					} EndIf(); //Z is pressed

					ANDI(Reg4, Reg4, 0x800); //get if Y is pressed
					If(Reg4, NOT_EQUAL_I, 0); { //Y is pressed
					//record state info
						SetRegister(Reg4, 2);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //menu state
						LBZ(Reg4, Reg3, MENU_POS_OFFSET + 3);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 1); //name list position
						LBZ(Reg4, Reg3, MENU_SIZE_OFFSET + 3);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 2); //name list size

						//set new info
						SetRegister(Reg4, 0);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 7); //menu pos
						SetRegister(Reg4, BUTTON_MENU_SIZE);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 6); //menu size

					} EndIf(); //Y is pressed

				} EndIf(); //is in bounds

			} Else(); { //determine state
			//make sure cursor is in bounds
				LBZ(Reg2, Reg3, MENU_POS_OFFSET + 3);

				If(Reg2, EQUAL_I, 0xFF); { //too high
					SetRegister(Reg2, 0);
					STB(Reg2, Reg3, MENU_STATE_INFO_OFFSET + 7);
					SetRegister(Reg4, 4);
				} EndIf(); //too high

				LBZ(Reg6, Reg3, MENU_SIZE_OFFSET + 3);
				If(Reg2, GREATER, Reg6); {//too low
					ADDI(Reg2, Reg6, 0);
					STB(Reg2, Reg3, MENU_STATE_INFO_OFFSET + 7);
					SetRegister(Reg4, 4);
				} EndIf(); //too low

				If(Reg2, EQUAL, Reg6); { //shouldn't go down

					LBZ(Reg6, ButtonReg, 0x2B); //Y control stick
					EXTSB(Reg6, Reg6);
					If(Reg6, LESS_OR_EQUAL_I, -0x3D); { //is down input
						SetRegister(Reg6, 0);
						STB(Reg6, ButtonReg, 0x2B);
					} EndIf(); //is down input

					LBZ(Reg6, ButtonReg, 1); //DPad
					ANDI(Reg5, Reg6, BUTTON_DD);
					If(Reg5, NOT_EQUAL_I, 0); { //is down input
						ANDI(Reg6, Reg6, ~BUTTON_DD);
						STB(Reg6, ButtonReg, 1);
					} EndIf(); //is down input

				} EndIf(); //shouldn't go down

				If(Reg2, EQUAL_I, 0); { //shouldn't go up

					LBZ(Reg6, ButtonReg, 0x2B); //Y control stick
					EXTSB(Reg6, Reg6);
					If(Reg6, GREATER_OR_EQUAL_I, 0x3D); { //is up input
						SetRegister(Reg6, 0);
						STB(Reg6, ButtonReg, 0x2B);
					} EndIf(); //is up input

					LBZ(Reg6, ButtonReg, 1); //DPad
					ANDI(Reg5, Reg6, BUTTON_DU);
					If(Reg5, NOT_EQUAL_I, 0); { //is up input
						ANDI(Reg6, Reg6, ~BUTTON_DU);
						STB(Reg6, ButtonReg, 1);
					} EndIf(); //is up input

				} EndIf(); //shouldn't go up

				If(Reg4, EQUAL_I, 2); { //determine state
				//button menu ////////////////////////////////////////////////////////////////////////////////////////

					LHZ(Reg4, ButtonReg, 0); //get buttons pressed
					ANDI(Reg2, Reg4, 0x200); //get if B is pressed
					If(Reg2, NOT_EQUAL_I, 0); { //B is pressed
					//clear "tag in use" flag
						SetRegister(Reg4, 0xFF);
						SetRegister(Reg5, TAG_IN_USE_LOC);
						RLWINM(Reg6, Reg1, 30, 0, 31); // Reg1 >> 2
						STBX(Reg4, Reg5, Reg6);

						//record state info
						SetRegister(Reg4, 1);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //menu state

						//load old info
						LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 1);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 7); //name list position
						LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 2);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 6); //name list size

						//set buttons to stop B input
						LHZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
						ANDI(Reg4, Reg4, ~0x0200);
						STH(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);

						//erase current B input
						LHZ(Reg4, ButtonReg, 0);
						ANDI(Reg4, Reg4, ~0x0200);
						STH(Reg4, ButtonReg, 0);

					} Else(); { //B is pressed
						ANDI(Reg2, Reg4, 0x100); //get if A is pressed
						If(Reg2, NOT_EQUAL_I, 0); //A is pressed
						//record state info
						SetRegister(Reg4, 3);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //menu state
						LBZ(Reg4, Reg3, MENU_POS_OFFSET + 3);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 3); //button list position

						//set position
						//maybe set position to current setting in future
						//right now, just set to pos 0
						SetRegister(Reg2, 0);
						//STB(Reg2, Reg3, MENU_POS_OFFSET + 3);
						STB(Reg2, Reg3, MENU_STATE_INFO_OFFSET + 7);

						//set size
						SetRegister(Reg2, CONTROL_MENU_BASE_SIZE);
						LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 3); //get current pos
						If(Reg4, EQUAL_I, 8); { //is c-stick
							ADDI(Reg2, Reg2, 1);
						} EndIf(); //is c-stick

						If(Reg4, EQUAL_I, 11); { //is tap
							SetRegister(Reg2, 1);
						} EndIf(); //is tap

						//STB(Reg2, Reg3, MENU_SIZE_OFFSET + 3); //set size
						STB(Reg2, Reg3, MENU_STATE_INFO_OFFSET + 6); //set size

						//set buttons to stop A input
						LHZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
						ANDI(Reg4, Reg4, ~0x0100);
						STH(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);

						//erase current A input
						LHZ(Reg4, ButtonReg, 0);
						ANDI(Reg4, Reg4, ~0x0100);
						STH(Reg4, ButtonReg, 0);

					} EndIf(); EndIf(); //buttons pressed

				} Else(); If(Reg4, EQUAL_I, 3); { //determine state
				//control menu //////////////////////////////////////////////////////////////////////////////////////

					LHZ(Reg4, ButtonReg, 0); //get buttons pressed
					ANDI(Reg4, Reg4, 0x300); //get if B or A is pressed
					If(Reg4, NOT_EQUAL_I, 0); { //B or A is pressed

						If(Reg4, EQUAL_I, 0x100); { //A is pressed

						//change button setting
						//get address of button
							LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 1); //tag cursor index
							MULLI(Reg4, Reg4, 2);
							ADDI(Reg4, Reg4, MENU_INDEX_OFFSET - 2);
							LHZX(Reg4, Reg3, Reg4); //get tag index
							MULLI(Reg4, Reg4, TAG_LIST_SIZE);
							ADDI(Reg4, Reg4, GCC_SETTINGS_TAG_OFFSET);
							LBZ(Reg2, Reg3, MENU_STATE_INFO_OFFSET + 3); //button index
							ADD(Reg2, Reg2, Reg4);

							//get new setting
							LBZ(Reg6, Reg3, MENU_POS_OFFSET + 3); //control index
							If(Reg6, GREATER_OR_EQUAL_I, 5); { //is dpad or smash
								ADDI(Reg6, Reg6, 5);
							} EndIf(); //is dpad or smash

#if SPECIAL_CSTICK_OPTIONS
							/*LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 3); //button
							If(Reg4, EQUAL_I, 8); //setting c stick
							{
								If(Reg6, GREATER_OR_EQUAL_I, 12);
								{
									ADDI(Reg6, Reg6, 1);
								} EndIf();
							}EndIf();*/
#endif
							If(Reg6, EQUAL_I, 14); { //is smash
								SetRegister(Reg6, 5);
							} EndIf(); //is smash

							If(Reg6, EQUAL_I, 13); { //is NONE
								SetRegister(Reg6, 0xE);
							} EndIf(); // is NONE

							LBZ(Reg4, Reg3, MENU_SIZE_OFFSET + 3);
							If(Reg4, EQUAL_I, 1); { //is tap config
								RLWINM(Reg6, Reg6, 7, 24, 24); // Reg6 << 7
							} EndIf(); //is tap config

							SetRegister(Reg4, TAG_LIST_START_LOC);
							STBX(Reg6, Reg4, Reg2);

						} EndIf(); //A is pressed

						//record state info
						SetRegister(Reg4, 2);
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET); //menu state

						//load old position
						LBZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 3);
						//STB(Reg4, Reg3, MENU_POS_OFFSET + 3); //button list position
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 7); //button list position

						//load old size
						SetRegister(Reg4, BUTTON_MENU_SIZE);
						//STB(Reg4, Reg3, MENU_SIZE_OFFSET + 3); //menu size
						STB(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 6); //button list size

						//set buttons to stop A or B input
						LHZ(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);
						ANDI(Reg4, Reg4, ~0x0300);
						STH(Reg4, Reg3, MENU_STATE_INFO_OFFSET + 4);

						//erase current B or A input
						LHZ(Reg4, ButtonReg, 0);
						ANDI(Reg4, Reg4, ~0x0300);
						STH(Reg4, ButtonReg, 0);

					} EndIf(); //B or A is pressed

				} EndIf(); EndIf(); EndIf(); //determine state /////////////////////////////////////////////////////////////
			}
		} EndIf(); //is open for port

		//add stuff to registers or whatever
		ADDI(Reg1, Reg1, 4); //port counter
		ADDI(ButtonReg, ButtonReg, 0x40); //port input ptr

	} EndWhile(); //loop through ports

	//clear menu open data
	SetRegister(Reg2, MENU_BLOCK_PTRS);
	SetRegister(Reg3, 0);
	STW(Reg3, Reg2, 0);
	STW(Reg3, Reg2, 4);
	STW(Reg3, Reg2, 8);
	STW(Reg3, Reg2, 0xC);

	SetRegister(ButtonReg, PLAY_BUTTON_LOC_START - 4); //reset r26

	//ASMEnd(0x83e1003c); //lwz r31, r1, 0x3C
	ASMEnd(0x80010044); //lwz r0, sp, 0x44
}


void WriteMenu()
{
	ASMStart(0x8069f684);
	//r30 is write location
	//r5 is position in menu
	//r6 is menu ptr

	int Reg1 = 3;
	int Reg2 = 4;
	//only use these if menu is in state 2 or 3
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 0;
	int WriteLocReg = 30;

	//check if in transition (only important for builds without chase's code)
	LHZ(Reg1, 6, MENU_STATE_INFO_OFFSET + 6);
	If(Reg1, EQUAL_I, 0x00F0); { //not in transition

	//check if menu is in state 2 or 3
		LBZ(Reg1, 6, MENU_STATE_INFO_OFFSET);
		If(Reg1, GREATER_OR_EQUAL_I, 2); { //should edit
			MR(Reg4, 5); //store for later

			ADDI(Reg3, 5, 0);

			If(Reg1, EQUAL_I, 2); { //in button menu

			//use string offset to write button name
				MULLI(Reg1, 5, 8);
				SetRegister(Reg2, MENU_BUTTON_STRING_LOC);
				ADD(Reg1, Reg1, Reg2);
				StrCpy(WriteLocReg, Reg1, Reg2);

				//write ": "
				SetRegister(Reg1, ": ");
				STW(Reg1, WriteLocReg, 0);
				ADDI(WriteLocReg, WriteLocReg, 2);

				//use string offset to get current button setting, and adjust it for string offset
				//get button location
				LBZ(Reg1, 6, MENU_STATE_INFO_OFFSET + 1); //get selected tag
				MULLI(Reg1, Reg1, 2);
				ADDI(Reg1, Reg1, MENU_INDEX_OFFSET - 2);
				LHZX(Reg1, 6, Reg1); //get selected tag index
				MULLI(Reg1, Reg1, TAG_LIST_SIZE);
				SetRegister(Reg2, TAG_LIST_START_LOC);
				ADD(Reg1, Reg1, Reg2);
				ADDI(Reg1, Reg1, GCC_SETTINGS_TAG_OFFSET);
				LBZX(Reg2, Reg1, 5);

				//adjust value for string offset
				If(Reg2, EQUAL_I, 5); { //is Smash

					SetRegister(Reg2, 9);

				} EndIf(); //is Smash

				If(5, EQUAL_I, 11); { //is tap
				//shift tap jump setting to make it 1 or 0
					RLWINM(Reg2, Reg2, 25, 31, 31); // Reg2 >> 7
					//add setting to base on/off string + 5 to compensate
					ADDI(Reg2, Reg2, 15);
				} EndIf(); //is tap

				If(Reg2, EQUAL_I, 0xE); { //is none
					SetRegister(Reg2, 8);
				} Else(); If(Reg2, GREATER_OR_EQUAL_I, 0xA); { //is DPAD
					ADDI(Reg2, Reg2, -5);
				} EndIf(); EndIf(); //is DPAD or none

				ADDI(Reg3, Reg2, 0);

			} EndIf(); //in button menu

			//fix tap offset
			LBZ(Reg1, 6, MENU_SIZE_OFFSET + 3);
			If(Reg1, EQUAL_I, 1); { //in tap config
				ADDI(Reg3, Reg3, 10);
			} EndIf(); //in tap config


			
			SetRegister(Reg1, MENU_CONTROL_STRING_LOC);
#if SPECIAL_CSTICK_OPTIONS
			LBZ(Reg2, 6, MENU_STATE_INFO_OFFSET);
			If(Reg2, EQUAL_I, 2); //in button menu
			{
				If(Reg4, EQUAL_I, 8); //is c stick
				{
					If(Reg3, EQUAL_I, 5); //utaunt
					{
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 0);
					} Else(); If(Reg3, EQUAL_I, 6); //staunt
					{
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 1);
					} Else(); If(Reg3, EQUAL_I, 7); {
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 2);
					} EndIf(); EndIf(); EndIf();
				} EndIf();
			} EndIf();

			If(Reg2, EQUAL_I, 3); //in config menu
			{
				//LBZ(Reg2, 6, MENU_SIZE_OFFSET + 3);
				//If(Reg2, EQUAL_I, 9); //in c stick settings
				LBZ(Reg4, 6, MENU_STATE_INFO_OFFSET + 3);
				If(Reg4, EQUAL_I, 8); //in c stick
				{
					If(Reg3, EQUAL_I, 5); //utaunt
					{
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 0);
					} Else(); If(5, EQUAL_I, 6); //staunt
					{
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 1);
					} Else(); If(Reg3, EQUAL_I, 7); //d taunt
					{
						SetRegister(Reg1, CSTICK_TAUNT_SPECIAL_WORDS);
						SetRegister(Reg3, 2);
					} EndIf(); EndIf(); EndIf();
				} EndIf();
			} EndIf();
#endif
			//use offset to write setting name
			MULLI(Reg3, Reg3, 8);
			ADD(Reg1, Reg1, Reg3);
			StrCpy(WriteLocReg, Reg1, Reg2);

			//end function early
			WriteIntToFile(0x7fe3fb78); //mr r3, r31
			WriteIntToFile(0x83e1007c); //lwz r31, sp, 0x7C
			WriteIntToFile(0x83c10078); //lwz r30, sp, 0x78
			WriteIntToFile(0x83a10074); //lwz r29, sp, 0x74
			WriteIntToFile(0x83810070); //lwz r28, sp, 0x70
			WriteIntToFile(0x80010084); //lwz r0, sp, 0x84
			WriteIntToFile(0x7c0803a6); //mtlr r0
			WriteIntToFile(0x38210080); //addi sp, sp, 128
			WriteIntToFile(0x4e800020); //blr

		} EndIf(); //should edit

	} EndIf(); //not in transition

	ASMEnd(0x2c050000); //cmpwi r5, 0
}

void SetMenuData()
{
	ASMStart(0x806a0154);

	int Reg1 = 4;
	int Reg2 = 3;

	LBZ(Reg1, 26, MENU_STATE_INFO_OFFSET + 6);
	If(Reg1, NOT_EQUAL_I, 0); //should set new data

	STW(Reg1, 26, MENU_SIZE_OFFSET); //set new size

	EndIf(); //should set new data

	LBZ(Reg1, 26, MENU_STATE_INFO_OFFSET + 7);
	If(Reg1, NOT_EQUAL_I, 0xF0); //should set new position

	STW(Reg1, 26, MENU_POS_OFFSET); //set new pos

	EndIf(); //should set new position

	SetRegister(Reg1, 0x00F0);
	STH(Reg1, 26, MENU_STATE_INFO_OFFSET + 6); //clear old data

	ASMEnd(0x809a0044); //lwz r4, r26, 0x44
}