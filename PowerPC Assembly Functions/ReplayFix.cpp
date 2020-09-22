#include "stdafx.h"
#include "ReplayFix.h"

//contains all functions used to fix replays
void ReplayFix()
{
	RecordInput();
	PlayInput();
	MakeBrawlSaveWholeReplay();
	AlternateStageFix();
	MemCardCheckSkip();
	FastForward();
	FixDebugPause();
	StopBrawlReplay(); //not sure if needed (haven't tested)
	StopFalseCorruption();
	ClearASLData();
	FixSinglePlayer();
}

void FixSinglePlayer() {
	//r4 has flag
	//can use r30
	ASMStart(0x8009c984);

	SetRegister(30, IS_VERSUS_LOC);
	STW(4, 30, 0);

	ASMEnd(0x7c7e1b78); //mr r30, r3
}

//stops the ASL value from classic or versus from interfering with taining mode
void ClearASLData()
{
	ASMStart(0x806f14c8);
	SaveRegisters();

	int Reg1 = 31;
	int Reg2 = 30;

	SetRegister(Reg1, REPLAY_ALT_STAGE_STORAGE_LOC);
	SetRegister(Reg2, 0);
	STH(Reg2, Reg1, 0);

	RestoreRegisters();
	ASMEnd(0x7c7f1b78); //mr r31, r3
}

/*//stops the ASL value from classic or versus from interfering with taining mode
void ClearASLData()
{
	//can use r31, r30
	ASMStart(0x8068290c);
	int Reg1 = 31;
	int Reg2 = 30;

	SetRegister(Reg1, REPLAY_ALT_STAGE_STORAGE_LOC);
	SetRegister(Reg2, 0);
	STH(Reg2, Reg1, 0);

	ASMEnd(0x83e1000c); //lwz r31, sp, 0xC
}*/

//cleans up some settings that might have been changed during playback
void EndReplay()
{
	ASMStart(0x8004acc8);
	//don't change r0
	int Reg1 = 3;
	int Reg2 = 31;

	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, 0);

	SetRegister(Reg1, IS_DEBUG_PAUSED);
	SetRegister(Reg2, 0);
	STW(Reg2, Reg1, 0);

	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, 0);
	WriteIntToFile(0x7c030378); //mr r3, r0
	ASMEnd();
}

void FixAutoLCancel()
{
	//save the current auto L-Cancel setting and use the recorded one
	ASMStart(0x80874598);
	int Reg1 = 4;

	WriteIntToFile(0xec1e002a); //fadds f0, f30, f0
	ASMEnd();

	//restore the current auto L-Cancel setting
	ASMStart(0x808745a0);
	Reg1 = 3;

	WriteIntToFile(0x3c6080ae); //lis r3, 0x80AE
	ASMEnd();
}

//the memory saving techniques can be so good that Brawl thinks the replay is too small, and is corrupted
//this prevents that from happening
void StopFalseCorruption()
{
	ASMStart(0x8004c11c);

	SetRegister(5, 0); //5 is minimum size

	ASMEnd();
}

//makes debug pause during match or replay not cause desyncs
void FixDebugPause()
{
	//fix for match
	ASMStart(0x8004b9e0);

	LoadByteToReg(30, IS_DEBUG_PAUSED + 3);
	If(30, EQUAL_I, 1); //is debug paused

	SetRegister(0, 0);

	Else(); //is debug paused

	LWZ(0, 3, 0);

	EndIf(); //is debug paused

	ASMEnd();


	//fix for replay
	ASMStart(0x8004bcb8);

	int Reg1 = 24;
	int Reg2 = 31;
	int Reg3 = 30;

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;

	//get union of all players' buttons
	SetRegister(Reg1, PLAY_BUTTON_LOC_START);
	LWZ(Reg2, Reg1, 0); //p1
	LWZ(Reg3, Reg1, 0x40); //p2
	OR(Reg2, Reg2, Reg3);
	LWZ(Reg3, Reg1, 0x80); //p3
	OR(Reg2, Reg2, Reg3);
	LWZ(Reg3, Reg1, 0xC0); //p4
	OR(Reg2, Reg2, Reg3);

	ANDI(Reg1, Reg2, 0x200);
	If(Reg1, EQUAL_I, 0x200); //b is pressed

	LoadHalfToReg(Reg1, B_HELD_FRAME_COUNTER_LOC); //get num frames held
	ANDI(Reg2, Reg1, 0x100);
	If(Reg2, NOT_EQUAL_I, 0x100); //first frame b pressed

								  //save current debug state
	LoadHalfToReg(Reg2, IS_DEBUG_PAUSED + 2);
	SetRegister(Reg3, B_HELD_FRAME_COUNTER_LOC + 2);
	STH(Reg2, Reg3, 0);

	//reset frame advance timer and mark that b was pressed
	SetRegister(Reg1, 0x100);

	EndIf(); //first frame b pressed

	ANDI(Reg1, Reg1, 0x103); //modulus 4, while saving b pressed bit
	If(Reg1, EQUAL_I, 0x100); //should advance

							  //trigger frame advance
	SetRegister(Reg2, IS_DEBUG_PAUSED);
	SetRegister(Reg3, 0x100);
	STH(Reg3, Reg2, 2);

	EndIf(); //should advance

	ADDI(Reg1, Reg1, 1); //increment counter

	Else(); //b is pressed

	LoadByteToReg(Reg1, B_HELD_FRAME_COUNTER_LOC);
	If(Reg1, EQUAL_I, 1); //b just released

	SetRegister(Reg3, B_HELD_FRAME_COUNTER_LOC);
	LHZ(Reg1, Reg3, 2);
	SetRegister(Reg2, IS_DEBUG_PAUSED);
	STH(Reg1, Reg2, 2);

	EndIf(); //b just released

	SetRegister(Reg1, 0); //reset

	EndIf(); //b is pressed

			 //update counter
	SetRegister(Reg2, B_HELD_FRAME_COUNTER_LOC);
	STH(Reg1, Reg2, 0);

	LoadByteToReg(24, IS_DEBUG_PAUSED + 3);
	If(24, EQUAL_I, 1); //is debug paused

	SetRegister(0, 0);

	Else(); //is debug paused

	LWZ(0, 3, 0);

	EndIf(); //is debug paused


	offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, offset); offset += 4;
	LWZ(Reg3, Reg1, offset); offset += 4;

	ASMEnd();
}

void FastForward()
{
	ASMStart(0x80017388);

	int Reg1 = 3;
	int Reg2 = 31;
	int Reg3 = 30;

	If(Reg1, EQUAL_I, 1); //is allowed to speed up
	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	XORI(Reg1, Reg1, 1); //will be 0 if in replay, else will be nonzero and will function properly
	If(Reg1, EQUAL_I, 0); //is replay

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;


	//get union of all inputs
	SetRegister(Reg1, PLAY_BUTTON_LOC_START);
	LWZ(Reg2, Reg1, 0); //p1 input
	LWZ(Reg3, Reg1, 0x40); //p2 input
	OR(Reg2, Reg2, Reg3);
	LWZ(Reg3, Reg1, 0x80); //p3 input
	OR(Reg2, Reg2, Reg3);
	LWZ(Reg3, Reg1, 0xC0); //p4 input
	OR(Reg2, Reg2, Reg3);

	RLWINM(Reg2, Reg2, 0, 23, 23); //check if anyone pressed a
	If(Reg2, NOT_EQUAL_I, 0);//a pressed

	SetRegister(Reg3, 2);

	EndIf();//a pressed

	SetRegister(Reg1, 1); //r3 == 1

	If(19, LESS, Reg3);//should speed up

	SetRegister(Reg1, 0); //r3 == 0

	EndIf();//should speed up

	offset = 0;
	SetRegister(Reg3, REGISTER_BUFFER);
	STW(Reg2, Reg3, offset); offset += 4;
	STW(Reg3, Reg3, offset); offset += 4;
	//Reg1/r3 is left as it is

	EndIf();//is replay

	EndIf(); //is allowed to speed up

	WriteIntToFile(0x2c030000); //cmpwi r3, 0

	ASMEnd();
}

//stops Brawl from checking how much room is left on the memory card, and gives a fake value
//skips a check when first loading PM, one every time the stage select screen is acessed,
//3 every time a replay is saved, and 2 every time the replay screen is loaded
//each check saves several seconds
//I don't know if this function is called in other places, nor the full implications
//of skipping it.  I haven't run into any problems though and it saves a ton of time
void MemCardCheckSkip()
{
	//skips the function
	ASMStart(0x803e16c8);

	SetRegister(3, 0);

	WriteIntToFile(0x4e800020); //blr

	ASMEnd();

	//writes in a fake value for how much memory remains
	ASMStart(0x8001efdc);

	SetRegister(27, 1337);

	ASMEnd();
}

//saves what alternate stage was used during a match, and makes them load properly during replay
void AlternateStageFix()
{
	//can use registers 3, 5, 6, 7, 8
	ASMStart(0x8094a16c);

	//get correct alt stage
	SetRegister(7, 0x800B9EA2); //alt stage helper ASL loc
	LWZ(5, 7, -2);
	If(5, LESS_I, 0); //alt stage helper not used
	SetRegister(7, ALT_STAGE_VAL_LOC);
	LHZ(5, 7, 0); //equals 0 if in a replay
	EndIf(); //alt stage helper not used
	LoadHalfToReg(3, REPLAY_ALT_STAGE_STORAGE_LOC); //equals 0 if in a match
	OR(3, 3, 5);

	//store alt stage value
	STH(3, 7, 0); //set current match ASL
	SetRegister(7, CURRENT_ALT_STAGE_INFO_LOC);
	STW(3, 7, 0); //store value so record function can save it if in match

	WriteIntToFile(0x38610008); //addi r3, sp, 8
	ASMEnd();

	/*
	ASMStart(0x80569168);

	//get correct alt stage
	LoadHalfToReg(7, REPLAY_ALT_STAGE_STORAGE_LOC); //equals 0 if in a match
	LoadHalfToReg(3, ALT_STAGE_VAL_LOC); //equals 0 if in a replay
	OR(3, 3, 7);

	//store alt stage value
	SetRegister(7, CURRENT_ALT_STAGE_INFO_LOC);
	STW(3, 7, 0); //store value so record function can save it if in match

	LHZ(7, 11, 0); //reset r7

	WriteIntToFile(0x7ce71839); //and. r7, r7, r3
	ASMEnd();
	*/
}

//stops recording when memory runs out to prevent crashes
void StopRecording()
{
	//changes the end of the replay buffer so it won't overwrite required memory
	ASMStart(0x8004ba84);

	//reset should stop recording flag
	ADDI(0, 0, 0); //set r0 to 0
	SetRegister(4, SHOULD_STOP_RECORDING);
	STW(0, 4, 0);

	//set new end location
	SetRegister(4, END_OF_REPLAY_BUFFER);
	ADD(0, 0, 4);

	ASMEnd();

	//stops Brawl from overwriting certain values so unfinished replays can be saved (by skipping operations)
	//also tells recording function to stop recording
	ASMStart(0x8004ba9c);

	ADDI(0, 0, 1); //set r0 to 1
	SetRegister(4, SHOULD_STOP_RECORDING);
	STW(0, 4, 0);

	ASMEnd();
}

//prevents Brawl from running its replay function
void StopBrawlReplay()
{
	ASMStart(0x8004ab7c);

	NOP();

	ASMEnd();
}

//changes the offset
void SetupPlayback()
{
	ASMStart(0x8004be50);

	LoadWordToReg(3, IS_VERSUS_LOC);
	LoadWordToReg(8, FRAME_COUNTER_LOC);
	OR(3, 3, 8);
	If(3, NOT_EQUAL_I, 0);
	{
		LBZ(3, 6, 1); //get size of offset
		If(3, NOT_EQUAL_I, 0);
		{
			If(3, LESS_I, 0x80);
			{
				MR(4, 3);
			} EndIf();
		} EndIf();
	}EndIf();

	ASMEnd();
}

//sets r20 to the start of a sync data segment and resets ptrs
void AlignSyncDuringPlayback()
{
	ASMStart(0x8004bd6c);

	//reset input buffer offset
	SetRegister(20, FRAME_INPUT_BUFFER_OFFSET);
	ADDI(0, 0, 0);
	STW(0, 20, 0);

	//reset cstick asdi counter
	SetRegister(20, CSTICK_ASDI_COUNT_LOC);
	STW(0, 20, 0);

	//set input ptr
	SetRegister(20, INPUT_BUFFER_PTR_LOC);
	LWZ(0, 20, 4); //get current frame's ptr
	STW(0, 20, 0); //store ptr
	ADDI(0, 30, 2); //get next frame's ptr
	STW(0, 20, 4); //change offset to 4

				   //set cstick asdi ptr
	LWZ(0, 20, 0);
	SetRegister(20, CSTICK_ASDI_PTR_LOC);
	STW(0, 20, 0);

	//adjust for sync
	LBZ(20, 30, 1); //get size of input segment
	ADD(20, 20, 30); //add beginning ptr to size
	RLWINM(0, 31, 2, 1, 0); // << 2
	SUBF(20, 20, 0); //get beginning of sync data

	ASMEnd();
}

//changes the destination of a memory move during recording to a location that won't be overwritten
void SaveSyncData()
{
	ASMStart(0x8004bb18);

	LoadWordToReg(3, CURRENT_FRAME_HEADER_BYTE_LOC);
	SetRegister(5, 0x06);
	STB(5, 3, 1);
	SetRegister(5, 0x06000000);
	STW(5, 3, 2);


	RLWINM(5, 0, 2, 22, 29);
	SetRegister(3, SYNC_DATA_LENGTH);
	STW(5, 3, 0);
	SetRegister(3, SYNC_DATA_BUFFER);
	SUBF(27, 27, 5);

	ASMEnd();
}

//stores the head byte location, sets up the input ptr, and resets sync data
void SetupRecord()
{
	//need r30
	ASMStart(0x8004bb04);

	int Reg1 = 5;
	int Reg2 = 31;
	int Reg3 = 29;

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;


	//reset input offset
	SetRegister(Reg1, FRAME_INPUT_BUFFER_OFFSET);
	SetRegister(Reg2, 0);
	STW(Reg2, Reg1, 0);

	//reset sync data
	SetRegister(Reg1, SYNC_DATA_LENGTH);
	STW(Reg2, Reg1, 0);

	//store head byte loc
	LWZ(Reg1, 30, 0x28); //get location
	SetRegister(Reg2, CURRENT_FRAME_HEADER_BYTE_LOC);
	STW(Reg1, Reg2, 0);

	//set input ptr
	SetRegister(Reg2, INPUT_BUFFER_PTR_LOC);
	ADDI(Reg1, Reg1, 2);
	STW(Reg1, Reg2, 0);


	offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, offset); offset += 4;
	LWZ(Reg3, Reg1, offset); offset += 4;

	WriteIntToFile(0x881e007a); //lbz r0, r30, 0x7A
	ASMEnd();
}

//records input during a match
void RecordInput()
{
	SetupRecord();
	SaveSyncData();
	StopRecording();

	//need r28, r27
	ASMStart(0x80764f18);

	int Reg1 = 12;
	int Reg2 = 31;
	int Reg3 = 30;
	int Reg4 = 29;
	int Reg5 = 26;
	int Reg6 = 25;
	int Reg7 = 24;
	int FPReg1 = 0;
	int FPReg2 = 1;

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;
	STW(Reg4, Reg1, offset); offset += 4;
	STW(Reg5, Reg1, offset); offset += 4;
	STW(Reg6, Reg1, offset); offset += 4;
	STW(Reg7, Reg1, offset); offset += 4;
	STFD(FPReg1, Reg1, offset); offset += 8;
	STFD(FPReg2, Reg1, offset); offset += 8;


	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	If(Reg1, EQUAL_I, 2); //in match

						  //save alt stage info
	LoadWordToReg(Reg1, CURRENT_ALT_STAGE_INFO_LOC);
	SetRegister(Reg2, REPLAY_ALT_STAGE_STORAGE_LOC);
	STH(Reg1, Reg2, 0);

	LoadWordToReg(Reg1, SHOULD_STOP_RECORDING);
	If(Reg1, EQUAL_I, 0); //still has memory

	SetRegister(Reg1, CURRENT_FRAME_INPUT_BUFFER);
	LoadWordToReg(Reg2, FRAME_INPUT_BUFFER_OFFSET);
	ADD(Reg1, Reg1, Reg2);
	offset = 0;

	//main input
	RLWINM(Reg2, 28, 0, 24, 24);
	If(Reg2, EQUAL_I, 0); //is not secondary climber

	LBZ(Reg2, 28, 0xB); //get port num
	If(Reg2, NOT_EQUAL_I, 0xFF); //is not CPU

	SetRegister(Reg3, PLAY_INPUT_LOC_START);
	RLWINM(Reg2, Reg2, 2, 1, 0); //<< 2
	ADD(Reg2, Reg2, Reg3); //get input loc
	LWZ(Reg3, Reg2, 0);
	STW(Reg3, Reg1, offset); offset += 4;

	EndIf(); //is not CPU

	EndIf(); //is not secondary climber

			 //independent button press
	LWZ(Reg2, 27, 0x78);
	STH(Reg2, Reg1, offset); offset += 2;

	//get base for light press and C-Stick
	LWZ(Reg2, 27, 0x2C);
	LWZ(Reg2, Reg2, 0x70);
	LWZ(Reg2, Reg2, 0x20);
	LWZ(Reg2, Reg2, 0x14);

	//c-stick
	SetFloatingRegister(FPReg1, Reg3, Reg4, MAX_CSTICK_VALUE);
	SetRegister(Reg5, SET_FLOAT_REG_TEMP_MEM);

	//x pos
	LFS(FPReg2, Reg2, 0x88);
	FMUL(FPReg2, FPReg2, FPReg1);
	FCTIW(FPReg2, FPReg2);
	STFD(FPReg2, Reg5, 0);
	LBZ(Reg3, Reg5, 7);
	STB(Reg3, Reg1, offset); offset += 1;

	//y pos
	LFS(FPReg2, Reg2, 0x8C);
	FMUL(FPReg2, FPReg2, FPReg1);
	FCTIW(FPReg2, FPReg2);
	STFD(FPReg2, Reg5, 0);
	LBZ(Reg3, Reg5, 7);
	STB(Reg3, Reg1, offset); offset += 1;

	//light press
	LWZ(Reg3, Reg2, -0x5C); //light press
	STB(Reg3, Reg1, offset); offset += 1;


	//if a byte of input this frame is identical to the last frame, don't save it
	LoadWordToReg(Reg3, FRAME_INPUT_BUFFER_OFFSET);
	SetRegister(Reg1, CURRENT_FRAME_INPUT_BUFFER);
	SetRegister(Reg2, LAST_FRAME_INPUT_BUFFER);
	ADD(Reg1, Reg1, Reg3);
	ADD(Reg2, Reg2, Reg3);
	LoadWordToReg(Reg7, INPUT_BUFFER_PTR_LOC);
	LoadWordToReg(Reg4, FRAME_COUNTER_LOC);
	If(Reg4, LESS_I, 0x70); //might still have new characters

	SetRegister(Reg6, 0xFF); //all 1's
	LWZ(Reg4, Reg1, 0);
	STW(Reg4, Reg7, 1); //save room for "new" byte
	STW(Reg4, Reg2, 0);
	LWZ(Reg4, Reg1, 4);
	STW(Reg4, Reg7, 5);
	STW(Reg4, Reg2, 4);
	ADDI(Reg7, Reg7, 8); //update replay ptr

	Else();//might still have new characters

	SetRegister(Reg3, 0); //index
	SetRegister(Reg6, 0); //stores whether the input is different

	While(Reg3, LESS_I, 8); //store new input loop

	RLWINM(Reg6, Reg6, 1, 1, 0); //<< 1
	LBZX(Reg4, Reg1, Reg3); //new input
	LBZX(Reg5, Reg2, Reg3); //old input

	If(Reg4, NOT_EQUAL, Reg5); //is different input

	STBU(Reg4, Reg7, 1); //store new input in replay
	STBX(Reg4, Reg2, Reg3); //update last frame buffer
	ORI(Reg6, Reg6, 1); //bit is set to 1

	EndIf(); //is different input

	ADDI(Reg3, Reg3, 1);

	EndWhile(); //store new input loop

	EndIf();//might still have new characters

			//store light press input
	LBZ(Reg3, Reg1, 8);
	STBU(Reg3, Reg7, 1);

	//store "new" bit
	LoadWordToReg(Reg1, INPUT_BUFFER_PTR_LOC);
	STB(Reg6, Reg1, 0);

	ADDI(Reg7, Reg7, 1);

	//update offset
	SetRegister(Reg3, FRAME_INPUT_BUFFER_OFFSET);
	LWZ(Reg4, Reg3, 0);
	ADDI(Reg4, Reg4, 8);
	STW(Reg4, Reg3, 0);

#if IS_DEBUGGING
	LWZ(Reg3, 27, 0x2C);
	LWZ(Reg3, Reg3, 0xD8);
	LWZ(Reg3, Reg3, 0xC);

	LWZ(Reg4, Reg3, 0x8C); //x pos
	STW(Reg4, Reg7, 0);
	LWZ(Reg4, Reg3, 0x90); //y pos
	STW(Reg4, Reg7, 4);

	ADDI(Reg7, Reg7, 8);
#endif

	//copy sync data if it exists
	LoadWordToReg(Reg3, SYNC_DATA_LENGTH);
	SetRegister(Reg4, SYNC_DATA_BUFFER);
	ADDI(Reg6, Reg7, 0); //copy replay ptr
	While(Reg3, GREATER_I, 0); //copy sync data loop

	LWZ(Reg5, Reg4, 0);
	STW(Reg5, Reg6, 0);

	ADDI(Reg4, Reg4, 4);
	ADDI(Reg6, Reg6, 4);
	ADDI(Reg3, Reg3, -4);

	EndWhile(); //copy sync data loop

				//update brawl replay ptr
	SetRegister(Reg3, BRAWL_REPLAY_PTR_LOC);
	STW(Reg6, Reg3, 0);

	//update size
	LoadWordToReg(Reg3, CURRENT_FRAME_HEADER_BYTE_LOC);
	SUBF(Reg6, Reg6, Reg3); //get size
	STB(Reg6, Reg3, 1);

	//update ptr
	SetRegister(Reg2, INPUT_BUFFER_PTR_LOC);
	STW(Reg7, Reg2, 0);

	EndIf(); //still has memory

	EndIf(); //in match


	offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, offset); offset += 4;
	LWZ(Reg3, Reg1, offset); offset += 4;
	LWZ(Reg4, Reg1, offset); offset += 4;
	LWZ(Reg5, Reg1, offset); offset += 4;
	LWZ(Reg6, Reg1, offset); offset += 4;
	LWZ(Reg7, Reg1, offset); offset += 4;
	LFD(FPReg1, Reg1, offset); offset += 8;
	LFD(FPReg2, Reg1, offset); offset += 8;

	WriteIntToFile(0x819c0004); //lwz r12, r28, 0x4

	ASMEnd();
}

//needed because the PlayInput function occurs too late for c-stick asdi and this one is too early for everything else
void CStickASDIFix()
{
	//need r21
	ASMStart(0x8091319c);

	int Reg1 = 12;
	int Reg2 = 31;
	int Reg3 = 30;
	int Reg4 = 29;
	int Reg5 = 26;
	int Reg6 = 25;
	int FPReg1 = 0;
	int FPReg2 = 1;

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;
	STW(Reg4, Reg1, offset); offset += 4;
	STW(Reg5, Reg1, offset); offset += 4;
	STW(Reg6, Reg1, offset); offset += 4;
	STFD(FPReg1, Reg1, offset); offset += 8;
	STFD(FPReg2, Reg1, offset); offset += 8;


	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	If(Reg1, EQUAL_I, 1); //is in replay

	LoadWordToReg(Reg1, CSTICK_ASDI_PTR_LOC);

	//get base for light press and C-Stick
	LWZ(Reg2, 21, 0x70);
	LWZ(Reg2, Reg2, 0x20);
	LWZ(Reg2, Reg2, 0x14);

	//c-stick
	//count how many byte to offset
	SetRegister(Reg4, 0);
	SetRegister(Reg5, 1);
	LBZ(Reg3, Reg1, 0);
	While(Reg4, LESS_I, 6); //count offset loop

	ANDI(Reg6, Reg3, 0x80);
	If(Reg6, NOT_EQUAL_I, 0); //new byte

	ADDI(Reg5, Reg5, 1);

	EndIf(); //new byte

	RLWINM(Reg3, Reg3, 1, 1, 0); //<< 1
	ADDI(Reg4, Reg4, 1);

	EndWhile(); //count offset loop

				//get x and y values
				//x value
	ANDI(Reg6, Reg3, 0x80);
	If(Reg6, NOT_EQUAL_I, 0); //new input

	LBZX(Reg4, Reg1, Reg5);
	ADDI(Reg5, Reg5, 1);

	Else(); //new input

	SetRegister(Reg4, CURRENT_FRAME_INPUT_BUFFER);
	LoadWordToReg(Reg6, CSTICK_ASDI_COUNT_LOC);
	RLWINM(Reg6, Reg6, 3, 1, 0); // << 3
	ADD(Reg4, Reg4, Reg6);
	LBZ(Reg4, Reg4, 6);

	EndIf(); //new input

			 //y value
	RLWINM(Reg3, Reg3, 1, 1, 0); // << 1
	ANDI(Reg6, Reg3, 0x80);
	If(Reg6, NOT_EQUAL_I, 0); //new input

	LBZX(Reg3, Reg1, Reg5);
	ADDI(Reg5, Reg5, 1);

	Else(); //new input

	SetRegister(Reg3, CURRENT_FRAME_INPUT_BUFFER);
	LoadWordToReg(Reg6, CSTICK_ASDI_COUNT_LOC);
	RLWINM(Reg6, Reg6, 3, 1, 0); // << 3
	ADD(Reg3, Reg3, Reg6);
	LBZ(Reg3, Reg3, 7);

	EndIf(); //new input

			 //update ptr
	ADD(Reg1, Reg1, Reg5);
	ADDI(Reg1, Reg1, 1);
#if IS_DEBUGGING
	ADDI(Reg1, Reg1, 8);
#endif
	SetRegister(Reg6, CSTICK_ASDI_PTR_LOC);
	STW(Reg1, Reg6, 0);

	//update run counter
	SetRegister(Reg6, CSTICK_ASDI_COUNT_LOC);
	LWZ(Reg1, Reg6, 0);
	ADDI(Reg1, Reg1, 1);
	STW(Reg1, Reg6, 0);

	SetFloatingRegister(FPReg1, Reg1, Reg6, CSTICK_COEFFICIENT);
	SetRegister(Reg6, SET_FLOAT_REG_TEMP_MEM);

	//x pos
	If(Reg4, EQUAL_I, 0x55); //max positive value

	SetRegister(Reg4, 0x3f800000);
	STW(Reg4, Reg2, 0x88);

	Else(); //max positive value
	If(Reg4, EQUAL_I, 0xAB); //max negative value

	SetRegister(Reg4, 0xBf800000);
	STW(Reg4, Reg2, 0x88);

	Else(); //max negative value

	EXTSB(Reg4, Reg4);
	ConvertIntToFloat(Reg4, Reg1, Reg5);
	STW(Reg5, Reg6, 0);
	LFS(FPReg2, Reg6, 0);
	FMUL(FPReg2, FPReg2, FPReg1);
	STFS(FPReg2, Reg2, 0x88);

	EndIf(); //max negative value
	EndIf(); //max positive value

			 //y pos
	If(Reg3, EQUAL_I, 0x55); //max positive value

	SetRegister(Reg3, 0x3f800000);
	STW(Reg3, Reg2, 0x8C);

	Else(); //max positive value
	If(Reg3, EQUAL_I, 0xAB); //max negative value

	SetRegister(Reg3, 0xBf800000);
	STW(Reg3, Reg2, 0x8C);

	Else(); //max negative value

	EXTSB(Reg3, Reg3);
	ConvertIntToFloat(Reg3, Reg4, Reg5);
	STW(Reg5, Reg6, 0);
	LFS(FPReg2, Reg6, 0);
	FMUL(FPReg2, FPReg2, FPReg1);
	STFS(FPReg2, Reg2, 0x8C);

	EndIf(); //max negative value
	EndIf(); //max positive value

	EndIf(); //is in replay


	offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, offset); offset += 4;
	LWZ(Reg3, Reg1, offset); offset += 4;
	LWZ(Reg4, Reg1, offset); offset += 4;
	LWZ(Reg5, Reg1, offset); offset += 4;
	LWZ(Reg6, Reg1, offset); offset += 4;
	LFD(FPReg1, Reg1, offset); offset += 8;
	LFD(FPReg2, Reg1, offset); offset += 8;

	WriteIntToFile(0x81940000); //lwz r12, r20, 0
	ASMEnd();
}

//plays back input during replay
void PlayInput()
{
	AlignSyncDuringPlayback();
	SetupPlayback();
	CStickASDIFix();
	EndReplay();

	//need r28, r27
	ASMStart(0x80764e8c);

	int Reg1 = 3;
	int Reg2 = 31;
	int Reg3 = 30;
	int Reg4 = 29;
	int Reg5 = 26;
	int Reg6 = 25;
	int FPReg1 = 0;
	int FPReg2 = 1;

	int offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	STW(Reg2, Reg1, offset); offset += 4;
	STW(Reg3, Reg1, offset); offset += 4;
	STW(Reg4, Reg1, offset); offset += 4;
	STW(Reg5, Reg1, offset); offset += 4;
	STW(Reg6, Reg1, offset); offset += 4;
	STFD(FPReg1, Reg1, offset); offset += 8;
	STFD(FPReg2, Reg1, offset); offset += 8;


	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	If(Reg1, EQUAL_I, 1); //is in replay

	//setup input buffer
	LoadWordToReg(Reg4, FRAME_INPUT_BUFFER_OFFSET);
	LoadWordToReg(Reg1, INPUT_BUFFER_PTR_LOC);
	SetRegister(Reg2, CURRENT_FRAME_INPUT_BUFFER);
	ADD(Reg2, Reg2, Reg4);
	LBZ(Reg3, Reg1, 0); //get "new" byte
	SetRegister(Reg4, 0);

	While(Reg4, LESS_I, 8); //setup inputs loop

	ANDI(Reg5, Reg3, 0x80);
	If(Reg5, NOT_EQUAL_I, 0); //new input

	LBZU(Reg5, Reg1, 1);
	STBX(Reg5, Reg2, Reg4);

	EndIf(); //new input

	ADDI(Reg4, Reg4, 1);
	RLWINM(Reg3, Reg3, 1, 1, 0); // << 1

	EndWhile(); //setup inputs loop

	//light press
	LBZ(Reg3, Reg1, 1);
	SetRegister(Reg4, LIGHT_PRESS_TEMP_MEM);
	STW(Reg3, Reg4, 0);

	//update replay ptr
	SetRegister(Reg3, INPUT_BUFFER_PTR_LOC);
	ADDI(Reg1, Reg1, 2);
	STW(Reg1, Reg3, 0);

	//update offset
	SetRegister(Reg3, FRAME_INPUT_BUFFER_OFFSET);
	LWZ(Reg4, Reg3, 0);
	ADDI(Reg4, Reg4, 8);
	STW(Reg4, Reg3, 0);

	ADDI(Reg1, Reg2, 0);
	offset = 0;

	//main input
	RLWINM(Reg2, 28, 0, 24, 24);
	If(Reg2, EQUAL_I, 0); //is not secondary climber

	LBZ(Reg2, 28, 0xB); //get port num
	If(Reg2, NOT_EQUAL_I, 0xFF); //is not CPU

	RLWINM(Reg2, Reg2, 0, 29, 31);
	SetRegister(Reg3, PLAY_INPUT_LOC_START);
	RLWINM(Reg2, Reg2, 2, 1, 0); //<< 2
	ADD(Reg2, Reg2, Reg3); //get input loc
	LWZ(Reg3, Reg1, offset); offset += 4;
	STW(Reg3, Reg2, 0);

	EndIf(); //is not CPU

	EndIf(); //is not secondary climber

	//independent button press
	LHZ(Reg2, Reg1, offset); offset += 2;
	STW(Reg2, 27, 0x78);

	//get base for light press and C-Stick
	LWZ(Reg2, 27, 0x2C);
	LWZ(Reg2, Reg2, 0x70);
	LWZ(Reg2, Reg2, 0x20);
	LWZ(Reg2, Reg2, 0x14);

	//c-stick
	SetFloatingRegister(FPReg1, Reg3, Reg4, CSTICK_COEFFICIENT);
	SetRegister(Reg6, SET_FLOAT_REG_TEMP_MEM);

	//x pos
	LBZ(Reg3, Reg1, offset); offset += 1;
	EXTSB(Reg3, Reg3);
	ConvertIntToFloat(Reg3, Reg4, Reg5);
	STW(Reg5, Reg6, 0);
	LFS(FPReg2, Reg6, 0);
	FMUL(FPReg2, FPReg2, FPReg1);
	STFS(FPReg2, Reg2, 0x88);

	//y pos
	LBZ(Reg3, Reg1, offset); offset += 1;
	EXTSB(Reg3, Reg3);
	ConvertIntToFloat(Reg3, Reg4, Reg5);
	STW(Reg5, Reg6, 0);
	LFS(FPReg2, Reg6, 0);
	FMUL(FPReg2, FPReg2, FPReg1);
	STFS(FPReg2, Reg2, 0x8C);

	//light press
	LoadWordToReg(Reg3, LIGHT_PRESS_TEMP_MEM);
	//LBZ(Reg3, Reg1, offset); offset += 1;
	STW(Reg3, Reg2, -0x5C); //light press

#if IS_DEBUGGING
	LoadWordToReg(Reg1, INPUT_BUFFER_PTR_LOC);

	SetRegister(Reg5, END_OF_REPLAY_BUFFER + 0x40);
	STW(27, Reg5, 0);

	LWZ(Reg3, 27, 0x2C);
	LWZ(Reg3, Reg3, 0xD8);
	LWZ(Reg3, Reg3, 0xC);

	//get recorded x pos
	LWZ(Reg2, Reg1, 0);
	//get current x pos
	LWZ(Reg4, Reg3, 0x8C);

	//display xpos information
	STW(Reg2, Reg5, 4);
	STW(Reg4, Reg5, 8);

	If(Reg4, NOT_EQUAL, Reg2); //x positions don't match
							   //set breakpoint here
	NOP();
	NOP();
	NOP();
	NOP();

	EndIf(); //x positions don't match end

			 //get recorded y pos
	LWZ(Reg2, Reg1, 4);
	//get current y pos
	LWZ(Reg4, Reg3, 0x90);

	//display ypos information
	STW(Reg2, Reg5, 0xC);
	STW(Reg4, Reg5, 0x10);

	If(Reg4, NOT_EQUAL, Reg2); //y positions don't match
							   //set breakpoint here
	NOP();
	NOP();
	NOP();
	NOP();

	EndIf(); //y positions don't match end 

	ADDI(Reg1, Reg1, 8);
	SetRegister(Reg2, INPUT_BUFFER_PTR_LOC);
	STW(Reg1, Reg2, 0);
#endif

	EndIf(); //is in replay


	offset = 0;
	SetRegister(Reg1, REGISTER_BUFFER);
	LWZ(Reg2, Reg1, offset); offset += 4;
	LWZ(Reg3, Reg1, offset); offset += 4;
	LWZ(Reg4, Reg1, offset); offset += 4;
	LWZ(Reg5, Reg1, offset); offset += 4;
	LWZ(Reg6, Reg1, offset); offset += 4;
	LFD(FPReg1, Reg1, offset); offset += 8;
	LFD(FPReg2, Reg1, offset); offset += 8;

	WriteIntToFile(0x7f83e378); //mr r3, r28
	ASMEnd();
}

//makes Brawl save the entire replay
//also adds values at the end of the replay to make it properly terminate
//saves current auto L-Cancel setting to byte
void MakeBrawlSaveWholeReplay()
{
	ASMStart(0x80152C64);

	//set size
	LoadWordToReg(5, INPUT_BUFFER_PTR_LOC);
	ADDI(5, 5, 0xA);
	SetRegister(30, REPLAY_BUFFER_BEGIN);
	SUBF(5, 5, 30);

	WriteIntToFile(0x7cbe2b78); //mr r30, r5
	ASMEnd();
}