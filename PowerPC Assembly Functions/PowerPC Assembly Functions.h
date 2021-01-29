#pragma once

#include "stdafx.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

//set to 1 if debugging.  records positions every frame and compares them during replay
#define IS_DEBUGGING 0
#define NORMAL 0
#define PMEX 1
#define PROJECT_PLUS 2
#define BUILD_TYPE PROJECT_PLUS
#define DOLPHIN_BUILD false
#define EON_DEBUG_BUILD false
#define TOURNAMENT_ADDITION_BUILD false

//ROTC floating offsets
#define FS_20_0 -0x7920

//draw buffer offsets
#define DRAW_BUFFER_XPOS_OFFSET 0x2C
#define DRAW_BUFFER_YPOS_OFFSET 0x30

const vector<float> DEFAULT_CAMERA_MATRIX = { 1,0,0,0, 0,1,0,0, 0,0,1,-64 };

#if BUILD_TYPE == PROJECT_PLUS
const string MAIN_FOLDER = "Project+";
#else
const string MAIN_FOLDER = "LegacyTE";
#endif

///addresses start

///Function addresses start
#define GET_MEM_ALLOCATOR 0x80024430
#define ALLOC 0x80204e5c
#define MEMMOVE 0x803f602c
#define STRCPY 0x803fa280
#define FA_FOPEN 0x803ebeb8
#define FA_FCLOSE 0x803ebe8c
#define GF_POOL_FREE 0x8002632c
#define GX_SET_CULL_MODE 0x801f136c
#define GX_CLEAR_VTX_DESC 0x801efb10
#define GX_SET_VTX_DESC 0x801ef280
#define GX_SET_VTX_ATTRIBUTE_FORMAT 0x801efb44
#define GET_CAMERA_MTX 0x801a7dbc
#define GX_SET_CURRENT_MTX 0x801f52e4
#define GX_LOAD_POS_MTX_IMM 0x801f51dc
#define GX_SET_Z_MODE 0x801f4774
#define GX_BEGIN 0x801f1088
#define GX_SET_LINE_WIDTH 0x801f12ac
#define GF_DRAW_SETUP_COORD_2D 0x8001abbc
#define GX_DRAW_SET_VTX_COLOR_PRIM_ENVIROMENT 0x8001a5c0
#define GET_FLOAT_WORK_MODULE 0x807acbb4 //r3 = work module ptr, r4 is variable, returns value in f1
///Function addresses end

///addresses maintained by Brawl start
#define IS_REPLAY_LOC 0x805BBFC0 //equals 1 if in replay, 2 if in match
#define REPLAY_BUFFER_END_ADDRESS 0x9134CA00 //end of replay buffer
#define BRAWL_REPLAY_PTR_LOC 0x805BBFE8 //address of pointer to where to store input data
#define FRAME_COUNTER_LOC 0x901812A4 //gives the frame count of the match, increments through debug pause
#define PLAY_INPUT_LOC_START 0x805BC068 //the location of P1's inputs.  Add 4 for the next player during playback
#define PLAY_BUTTON_LOC_START 0x805BAD04 //the location of P1's buttons.  Add 0x40 for the next player
#define GCC_CONTROL_STICK_X_START 0x805bad30
#define GCC_CONTROL_STICK_Y_START 0x805bad31
#define WII_BASED_CONTROLLER_START 0x804F7880 //P1 wiimote loc, can't affect inputs from here
#define WII_BASED_CONTROLLER_PORT_OFFSET 0x9A0 //offset between wii controller ports
#define WII_BASED_CONTROLLER_TYPE_OFFSET 0x28 //offset from Wii contoller of byte containing type of controller
#if BUILD_TYPE == PMEX
#define ALT_STAGE_VAL_LOC 0x805858ba //half word that defines what alt stage should be loaded
#else
#define ALT_STAGE_VAL_LOC 0x815e8422 //half word that defines what alt stage should be loaded
#endif
#define REPLAY_BUFFER_BEGIN 0x91301c00 //start of the replay buffer
#define IS_DEBUG_PAUSED 0x805B8A08 //word that equals 1 if game is debug paused, 0x100 if frame advancing
#define IS_AUTO_L_CANCEL 0x9017f36f //byte that is 1 if auto L-cancel is on
#define BUTTON_CONFIG_START 0x805b7480 //start of in game custom control map
#define BASIC_VARIABLE_START_ADDRESS 0x901ae000
#define BASIC_VARIABLE_BLOCK_SIZE 0x870
///addresses maintained by Brawl end

///reserved memory for storage start
///in replay
///add memory by subtracting the constant above it by the reserved size
#define REGISTER_BUFFER   (REPLAY_BUFFER_END_ADDRESS - 4 * 10) //place where registers are stored
#define CURRENT_FRAME_HEADER_BYTE_LOC   (REGISTER_BUFFER - 4) //stores a pointer to the current head byte
#define INPUT_BUFFER_PTR_LOC   (CURRENT_FRAME_HEADER_BYTE_LOC - 8) //holds current and next ptr inside input buffer
#define CSTICK_ASDI_PTR_LOC   (INPUT_BUFFER_PTR_LOC - 4) //ptr to cstick asdi function input
#define CSTICK_ASDI_COUNT_LOC   (CSTICK_ASDI_PTR_LOC - 4) //how many times cstick asdi was run this frame
#define SYNC_DATA_BUFFER   (CSTICK_ASDI_COUNT_LOC - 4 * 7) //7 is the highest amount of words possible
#define SYNC_DATA_LENGTH   (SYNC_DATA_BUFFER - 4) //how long in bytes the sync data actually is
#define SET_FLOAT_REG_TEMP_MEM  (SYNC_DATA_LENGTH - 8) //used to set floating register and retrieve in value
#define LIGHT_PRESS_TEMP_MEM   (SET_FLOAT_REG_TEMP_MEM - 4) //stores light press during replay
#define CURRENT_FRAME_INPUT_BUFFER   (LIGHT_PRESS_TEMP_MEM - 0x44) //holds inputs that are to be recorded this frame
#define LAST_FRAME_INPUT_BUFFER   (CURRENT_FRAME_INPUT_BUFFER - 0x44) //holds inputs that were recorded or played last frame
#define FRAME_INPUT_BUFFER_OFFSET   (LAST_FRAME_INPUT_BUFFER - 4) //holds current offset of both input buffers
#define SHOULD_STOP_RECORDING   (FRAME_INPUT_BUFFER_OFFSET - 4) //set to 1 if no more memory left for replay
#define B_HELD_FRAME_COUNTER_LOC   (SHOULD_STOP_RECORDING - 4) //counts how many frames z was held consecutively and records state of debug pause from before slo-mo
#define CURRENT_ALT_STAGE_INFO_LOC   (B_HELD_FRAME_COUNTER_LOC - 4) //contains alt stage, used to fix salty runback
#define IS_VERSUS_LOC   (CURRENT_ALT_STAGE_INFO_LOC - 4) //is 1 if in versus, else 0
#define RANDOM_1_TO_1_CPP_FLAG_LOC	(IS_VERSUS_LOC - 4) //flag to communicate with c++ code
#define END_OF_REPLAY_BUFFER   (RANDOM_1_TO_1_CPP_FLAG_LOC - 0x100) //tells when to stop recording inputs
///set END_OF_REPLAY_BUFFER to the last constant - 0x80 to ensure no memory leaks

///at end of MEM2
const int MENU_BLOCK_PTRS = 0x935ce300;
const int MENU_BUTTON_STRING_LOC = MENU_BLOCK_PTRS + 4 * 4;
const int MENU_CONTROL_STRING_LOC = MENU_BUTTON_STRING_LOC + 8 * 12;
const int TAG_IN_USE_LOC = MENU_CONTROL_STRING_LOC + 8 * 12;
const int REPLACE_NAME_OLD_TIME_LOC = TAG_IN_USE_LOC + 4;
const int REPLACE_NAME_TIME_ADDRESS = REPLACE_NAME_OLD_TIME_LOC + 4;
#define DISABLE_DPAD_ASL_STORAGE REPLACE_NAME_TIME_ADDRESS + 4 //4
#define GCC_BUTTON_STORAGE_LOC DISABLE_DPAD_ASL_STORAGE + 4 //8
#define WIIMOTE_CONVERTED_BUTTON_STORAGE_LOC GCC_BUTTON_STORAGE_LOC + 8 //8
#define WIIMOTE_CONVERSION_TABLE WIIMOTE_CONVERTED_BUTTON_STORAGE_LOC + 8 //16
#define WIICHUCK_CONVERSION_TABLE WIIMOTE_CONVERSION_TABLE + 16 //16
#define CLASSIC_CONVERSION_TABLE WIICHUCK_CONVERSION_TABLE + 16 //16
#define KAPPA_ITEM_FLAG CLASSIC_CONVERSION_TABLE + 16 //4
#define MAIN_BUFFER_PTR KAPPA_ITEM_FLAG + 4 //4
#define STRING_BUFFER MAIN_BUFFER_PTR + 4 //0x100, 0x935ce428
#define IASA_OVERLAY_MEM_PTR_LOC STRING_BUFFER + 0x100 //4
#define IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC IASA_OVERLAY_MEM_PTR_LOC + 4 //4
#define IASA_TERMINATE_OVERLAY_COMMAND_PTR_LOC IASA_TRIGGER_OVERLAY_COMMAND_PTR_LOC + 4 //4
#define IASA_STATE IASA_TERMINATE_OVERLAY_COMMAND_PTR_LOC + 4 //4
#define IS_IN_GAME_FLAG IASA_STATE + 4 //4
#define WRITE_SD_FILE_HEADER_LOC IS_IN_GAME_FLAG + 4 //0x18
#define ACTIVE_TAG_ID_BY_PORT WRITE_SD_FILE_HEADER_LOC + 0x18 //4
#define HEX_TO_ASCII_TABLE ACTIVE_TAG_ID_BY_PORT + 0x4 //0x10
#define COSTUME_PATH_ADDRESS_RESULT HEX_TO_ASCII_TABLE + 0x10 //4
#define CSTICK_TAUNT_SPECIAL_WORDS (COSTUME_PATH_ADDRESS_RESULT + 4) //
///reserved memory for storage end

///Control code constants start
#define BUTTON_MENU_SIZE 11
//#define CONTROL_MENU_BASE_SIZE 7
#define CONTROL_MENU_BASE_SIZE 8
#define MENU_WRITE_LOC 0x805b4c00
#define TAG_LIST_START_LOC 0x90172e20
#define TAG_LIST_SIZE 0x124
#define GCC_SETTINGS_TAG_OFFSET 0x14
#define TAG_TIME_OFFSET 0x10
#define MENU_FIRST_CREATION_TIME_LOC 0x90172e30
//state=0, pos=1, size=2, buttonPos=3, buttonCancel=4-5, newSize=6, newPos=7, reopenFlag=8, replaceTimeFlag=9, openFlag=10
const int MENU_STATE_INFO_OFFSET = 0x60;
const int MENU_SIZE_OFFSET = 0x6C;
const int MENU_INDEX_OFFSET = 0x70;
const int MENU_POS_OFFSET = 0x44;
//const int MENU_PORT_OFFSET = 0x47F80;
//const int MENU_PORT1_LOC = 0x81578BDC;
const int MENU_PORT_NUM_OFFSET = 0x57;
const int MENU_SELECTED_TAG_OFFSET = 0x164;
///Control code constants end


#define REPLAY_ALT_STAGE_STORAGE_LOC 0x91301f4a //half word that is used to store which alt stage was loaded
#define REPLAY_AUTO_L_CANCEL_SETTING 0x91301f4e //half word that stores current and recorded auto L-Cancel settings
#define IN_STAGE_SELECT_MENU_FLAG 0x91301f50 //byte flag == 1 when in stage select menu
#define REPLAY_ENDLESS_ROTATION_QUEUE 0x91301f54 //word that saves copy of endless rotation queue for replay

///addresses end

///constants start
///colors
#define RED 0xFF0000FF
#define BLUE 0x0066FFFF
#define ORANGE 0xFF9900FF
#define GREEN 0x33CC33FF
#define YELLOW 0xFFFF00FF
#define BLACK 0x000000FF
#define WHITE 0xFFFFFFFF
#define PURPLE 0x6E0094FF
#define TEAL 0x6DD0FFFF
///colors end
///primitive types
#define PRIMITIVE_LINE 0xB0
#define PRIMITIVE_QUAD 0x80
///button values
#define BUTTON_DL 0x1
#define BUTTON_DR 0x2
#define BUTTON_DD 0x4
#define BUTTON_DU 0x8
#define BUTTON_Z 0x10
#define BUTTON_R 0x20
#define BUTTON_L 0x40
#define BUTTON_A 0x100
#define BUTTON_B 0x200
#define BUTTON_X 0x400
#define BUTTON_Y 0x800
#define BUTTON_START 0x1000
#define BUTTON_DPAD 0xF
///button values end
#define BUTTON_PORT_OFFSET 0x40
#define WIIMOTE 0
#define WIICHUCK 1
#define CLASSIC 2
#define SP 1 //stack ptr reg
#define WORD_SIZE 32
#define BRANCH_IF_TRUE 0b01100
#define BRANCH_IF_FALSE 0b00100
#define BRANCH_ALWAYS 0b10100
#define MAX_IFS 15
#define MAX_LABELS 20
#define MAX_JUMPS 50
#define LESS 0
#define GREATER 1
#define EQUAL 2
#define LESS_OR_EQUAL 4
#define GREATER_OR_EQUAL 3
#define NOT_EQUAL 5
#define LESS_I 10
#define GREATER_I 11
#define EQUAL_I 12
#define LESS_OR_EQUAL_I 14
#define GREATER_OR_EQUAL_I 13
#define NOT_EQUAL_I 15
#define LESS_L 30
#define GREATER_L 31
#define EQUAL_L 32
#define LESS_OR_EQUAL_L 34
#define GREATER_OR_EQUAL_L 33
#define NOT_EQUAL_L 35
#define LESS_I_L 40
#define GREATER_I_L 41
#define EQUAL_I_L 42
#define LESS_OR_EQUAL_I_L 44
#define GREATER_OR_EQUAL_I_L 43
#define NOT_EQUAL_I_L 45
#define IS_ELSE 20
#define LT 0
#define GT 1
#define EQ 2
#define MAX_CSTICK_VALUE 0x55
#define CSTICK_COEFFICIENT 0.011775
#define TERMINATE_REPLAY_VALUE 0x06000000
///constants end

///variables start
static fstream WPtr;
static char OpHexBuffer[10] = {};//used for writing 8 char assembly hex to file
static u32 OpHex = 0;//for writing ops to file
static int IfStartPos[MAX_IFS] = {};
static int IfConditionArray[MAX_IFS] = {};
static int IfIndex = 0;
static int WhileConditionArray[MAX_IFS] = {};
static int WhileCompareArray[MAX_IFS] = {};
static int WhileIndex = 0;
static int ASMStartAddress = 0;
static int LabelPosArray[MAX_LABELS] = {};
static int LabelIndex = 0;
static int JumpLabelNumArray[MAX_JUMPS] = {};
static int JumpFromArray[MAX_JUMPS] = {};
static int JumpIndex = 0;
static vector<int> FPPushRecords;
static vector<int> CounterLoppRecords;
static vector<int> StackIteratorRecords;
///variables end
int HexToDec(char x);

void CodeStart(string FilePath);
void CodeEnd();

void MakeGCT(string TextFilePath, string OldGCTFilePath, string NewGCTFilePath);

int GetHexFromFloat(float Value);
int GetShiftNum(int endPos);
int GetOpSegment(int val, int size, int pos);
void WriteMenuTextToFile(string Text);
void WriteTextToFile(string Text);
void WriteIntToFile(int val);
void If(int Val1, int Comparision, int Val2);
void EndIf();
void Else();
void While(int Val1, int Comparision, int Val2);
void EndWhile();
void SetRegister(int Register, int value);
void SetRegister(int Register, string value);
void SetFloatingRegister(int FPReg, int TempReg1, int TempReg2, float Value);
void SetFloatingRegister(int FPReg, int TempReg, float Value);
void LoadWordToReg(int Register, int Address);
void LoadHalfToReg(int Register, int Address);
void LoadByteToReg(int Register, int Address);
void LoadWordToReg(int DestReg, int Reg, int Address);
void LoadHalfToReg(int DestReg, int Reg, int Address);
void LoadByteToReg(int DestReg, int Reg, int Address);
void ConvertIntToFloat(int SourceReg, int TempReg, int ResultReg);
void ASMStart(int BranchAddress);
void ASMEnd(int Replacement);
void ASMEnd();
void Label(int LabelNum);
int GetNextLabel();
void JumpToLabel(int LabelNum);
void CompleteJumps();
int CalcBranchOffset(int Location, int Target);
void StrCpy(int Destination, int Source, int Temp);
void GeckoStringWrite(string String, u32 Address);
void Gecko32BitWrite(int Address, int Value);
void Gecko8BitWrite(int Address, int Value, int NumTimes = 1);
void SetGeckoBaseAddress(int Address);
void SetGeckoPointerAddress(int Address);
void LoadIntoGeckoPointer(int Address);
void LoadIntoGeckoRegister(int Address, int Reg, int size);
void StoreGeckoRegisterAt(int Address, int Reg, int size, int repeats = 0);
void GeckoIf(u32 Address, int Comparison, int Value);
void GeckoEndIf();
//searches for byte, elementOffset is distance between elements, ResultReg returns index if found, else -1
//StartAddressReg ends with the address of the found element, or an address after the array
void FindInArray(int ValueReg, int StartAddressReg, int numberOfElements, int elementOffset, int ResultReg, int TempReg);
//searches for target, elementOffset is distance between elements, ResultReg returns index if found, else -1
//StartAddressReg ends with the address of the found element, or an address after the array
//ends when end marker is encountered
void FindInTerminatedArray(int ValueReg, int StartAddressReg, int endMarker, int elementOffset, int ResultReg, int TempReg, int searchSize);
void CallBrawlFunc(int Address);
//r3 returns ptr
void Allocate(int SizeReg, int Heap = 42);
void AllocateIfNotExist(int SizeReg, int AddressReg, int EmptyVal);
void Memmove(int DestReg, int SourceReg, int SizeReg);
void SetRegs(int StartReg, vector<int> values);
void SetArgumentsFromRegs(int StartReg, vector<int> ValueRegs);
void SetFloatingArgumentsFromRegs(int FPStartReg, vector<int> FPValueRegs);
void GXSetCullMode(int CullModeReg);
void GXClearVtxDesc();
void GXSetVtxDesc(int AttributeReg, int TypeReg);
void GXSetVtxAttrFmt(int vtxfmtReg, int attrReg, int countReg, int typeReg, int fracReg);
void GetCameraMtx(int StorageLocReg);
void GXSetCurrentMtx(int IDReg);
void GXLoadPosMtxImm(int MtxPtrReg, int IDReg);
void GXSetZMode(int CompareEnabledReg, int FuncTypeReg, int UpdateEnabledReg);
void GXSetLineWidth(int WidthReg, int TexOffsetReg);
void GXBegin(int TypeReg, int VtxAttrFmtReg, int NumVertsReg);
void GFDrawSetupCoord2D();
void GXDrawSetVtxColorPrimEnviroment();
void FreeMem(int AddressReg);
void FreeMemFromFighter(int AddressReg);
void FreeMemIfAllocd(int AddressReg, int EmptyVal);
void FreeAllocdArray(int AllocAddressReg);
void SaveMem(int LocationReg, int SizeReg, int SaveToReg, int Heap = 6);
void SaveAllocedMem(int AddressReg, int SaveToReg, int reg1, int reg2, bool SaveHeader = false, int Heap = 6);
void SaveRegisters();
void SaveRegisters(vector<int> FPRegs);
void SaveRegisters(int NumFPRegs);
void RestoreRegisters();
void Increment(int Reg);
void Decrement(int Reg);
void WriteStringToMem(string Text, int AddressReg);
void WriteVectorToMem(vector<int> Values, int AddressReg);
void WriteVectorToMem(vector<float> Values, int AddressReg);
void CounterLoop(int CounterReg, int startVal, int endVal, int stepVal);
void CounterLoopEnd();
void SprintF(int StrReg, vector<int> ValueRegs, int DestPtrReg = -1);
//can't have default, because overload ambiguity
void SprintF(int StrReg, vector<int> ValueRegs, vector<int> FPValueRegs, int DestPtrReg);
void ClampStick(int FPXValReg, int FPYValReg);
void ConvertIntStickValsToFloating(int StickXReg, int StickYReg, int FPXResultReg, int FPYResultReg, int FPTempReg);
void ConvertFloatingRegToInt(int FPReg, int ResultReg);
void ConvertFloatingRegToInt(int FPReg, int ResultReg, int TempFPReg);
void AddValueToByteArray(u32 value, vector<u8> &Array);
void AddValueToByteArray(u16 value, vector<u8> &Array);
void AddValueToByteArray(u8 value, vector<u8> &Array);
void AddValueToByteArray(int value, vector<u8> &Array);
void AddValueToByteArray(short value, vector<u8> &Array);
void AddValueToByteArray(char value, vector<u8> &Array);
void DrawPrimitive(int type, vector<float> Positions, vector<int> Colors, int VTXAttrFrmt);
void DrawPrimitive(int type, vector<float> Positions, int Color, int VTXAttrFrmt);
void LoadVal(int AddressReg, int size, int offset = 0, int ResultReg = 3);
void GetValueFromPtrPath(vector<int> Path, int StartingReg, int ResultReg = 3);
void GetValueFromPtrPath(int StartingAddress, vector<int> Path, int ResultReg = 3);
void AllocateStack(int size, int Address, int TempReg = 3);
void PushOnStack(int ValueReg, int StackReg, int TempReg = 3);
void PopOffStack(int ResultReg, int StatckReg, int TempReg = 3);
void IterateStack(int ResultReg, int StackReg, int TempReg);
void IterateStackEnd();
void AllocateVector(int size, int Address, int reg);
void ClearVector(int VectorReg);
void CopyVector(int SourceVector, int DestVector);
void IterateVector(int VectorReg, int IteratorReg, int TempReg1, int TempReg2, int StartReg = -1);
void ReverseIterateVector(int VectorReg, int IteratorReg, int TempReg1, int TempReg2, int StartReg = -1);
void RemoveFromVector(int VectorReg, int ValueReg);
void FindInVector(int VectorReg, int ValueReg, int ResultReg);
void ShiftVectorDown(int VectorReg, int StartReg);
void Randi(int ResultReg, int MaxReg = 3, int TempReg = 4);
//void RandomCapped(int HighReg, int reg1, int ResultReg = 3);
void WriteFileToSD(int PathReg, int SizeReg, int DataPtrReg);
void WriteFileToVF(int PathReg, int SizeReg, int DataPtrReg);
void IfInGame(int reg = 3, bool condition = true);
void ClearBitsFromMemory(short BitsToClear, int Address);
void ClearBitsFromMemory(short BitsToClear, int AddressReg, int Offset = 0);
void GetSceneNum(int ResultReg);
void IfInVersus(int reg);
void LoadFile(string filePath, int destination, int reg1, int reg2, bool loadFromSD = true);
void constrainFloat(int floatReg, int tempFReg, int tempReg, float min, float max);
void constrainFloatDynamic(int floatReg, int minFReg, int maxFReg);
void modifyInstruction(int instructionReg, int addressReg);
void IfInSSE(int reg1, int reg2);
void IfNotInSSE(int reg1, int reg2);

void ABS(int DestReg, int SourceReg, int tempReg);
void ADD(int DestReg, int SourceReg1, int SourceReg2);
void ADDI(int DestReg, int SourceReg, int Immediate);
void ADDIS(int DestReg, int SourceReg, int Immediate);
void AND(int DestReg, int SourceReg1, int SourceReg2);
void ANDC(int DestReg, int SourceReg1, int SourceReg2);
void ANDI(int DestReg, int SourceReg, int Immediate);
void ANDIS(int DestReg, int SourceReg, int Immediate);
void B(int JumpDist);
void BA(int Address);
void BC(int JumpDist, int BranchCondition, int ConditionBit);
void BCTR();
void BCTRL();
void BLA(int Address);
void BLR();
void CMP(int Reg1, int Reg2, int CondField);
void CMPI(int Reg, int Immediate, int CondField);
void CMPL(int Reg1, int Reg2, int CondField);
void CMPLI(int Reg, int Immediate, int CondField);
void CNTLZW(int DestReg, int SourceReg);
//if sourceReg1 == r0, not used
void DCBF(int SourceReg1, int SourceReg2);
void DCBST(int SourceReg1, int SourceReg2);
void DIVW(int DestReg, int DividendReg, int DivisorReg);
void DIVWU(int DestReg, int DividendReg, int DivisorReg);
void EQV(int DestReg, int SourceReg1, int SourceReg2);
void EXTSB(int DestReg, int SourceReg);
void FABS(int DestReg, int SourceReg);
void FADD(int DestReg, int SourceReg1, int SourceReg2);
void FADDS(int DestReg, int SourceReg1, int SourceReg2);
void FCMPU(int FPReg1, int FPReg2, int CondField);
void FMR(int DestReg, int SourceReg);
void FCTIW(int DestReg, int SourceReg);
void FCTIWZ(int DestReg, int SourceReg);
void FDIV(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
void FDIVS(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
void FMUL(int DestReg, int SourceReg1, int SourceReg2);
void FMULS(int DestReg, int SourceReg1, int SourceReg2);
void FNEG(int DestReg, int SourceReg);
void FRES(int DestReg, int SourceReg);
void FRSP(int DestReg, int SourceReg);
void FRSQRTE(int DestReg, int SourceReg);
void FSQRT(int FPDestReg, int FPSourceReg);
void FSUB(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
void FSUBS(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
//if SOurceReg1 == r0, it is not used
void ICBI(int SourceReg1, int SourceReg2);
void ISYNC();
void LBA(int DestReg, int AddressReg, int Immediate);
void LBAU(int DestReg, int AddressReg, int Immediate);
void LBAUX(int DestReg, int AddressReg1, int AddressReg2);
void LBAX(int DestReg, int AddressReg1, int AddressReg2);
void LBZ(int DestReg, int AddressReg, int Immediate);
void LBZU(int DestReg, int AddressReg, int Immediate);
void LBZUX(int DestReg, int AddressReg1, int AddressReg2);
void LBZX(int DestReg, int AddressReg1, int AddressReg2);
void LFD(int DestReg, int AddressReg, int Immediate);
void LFS(int DestReg, int AddressReg, int Immediate);
void LFSU(int DestReg, int AddressReg, int Immediate);
void LFSUX(int DestReg, int AddressReg, int AddressReg2);
void LFSX(int DestReg, int AddressReg, int AddressReg2);
void LHZ(int DestReg, int AddressReg, int Immediate);
void LHZU(int DestReg, int AddressReg, int Immediate);
void LHZUX(int DestReg, int AddressReg1, int AddressReg2);
void LHZX(int DestReg, int AddressReg1, int AddressReg2);
void LWZ(int DestReg, int AddressReg, int Immediate);
void LWZU(int DestReg, int AddressReg, int Immediate);
void LWZUX(int DestReg, int AddressReg1, int AddressReg2);
void LWZX(int DestReg, int AddressReg1, int AddressReg2);
void LMW(int StartReg, int AddressReg, int Immediate);
void LSWI(int StartReg, int AddressReg, int numBytes);
void LSWX(int StartReg, int AddressReg1, int AddressReg2, int NumArgsReg);
void MFCTR(int TargetReg);
void MFLR(int TargetReg);
void MOD(int DestReg, int SourceReg1, int SourceReg2);
void MR(int DestReg, int SourceReg);
void MTCTR(int TargetReg);
void MTLR(int TargetReg);
void MTXER(int TargetReg);
void MULLI(int DestReg, int SourceReg, int Immediate);
void MULLW(int DestReg, int SourceReg1, int SourceReg2);
void NEG(int DestReg, int SourceReg);
void NOP();
void NOR(int DestReg, int SourceReg1, int SourceReg2);
void OR(int DestReg, int SourceReg1, int SourceReg2);
void ORC(int DestReg, int SourceReg1, int SourceReg2);
void ORI(int DestReg, int SourceReg, int Immediate);
void ORIS(int DestReg, int SourceReg, int Immediate);
void RLWINM(int DestReg, int SourceReg, int ShiftNum, int MaskStart, int MaskEnd);
void RLWNM(int DestReg, int SourceReg, int ShiftReg, int MaskStart, int MaskEnd);
void SRAWI(int DestReg, int SourceReg, int ShiftNum);
void STB(int SourceReg, int AddressReg, int Immediate);
void STB(int SourceReg, int AddressReg, int Immediate);
void STBU(int SourceReg, int AddressReg, int Immediate);
void STBUX(int SourceReg, int AddressReg1, int AddressReg2);
void STBX(int SourceReg, int AddressReg1, int AddressReg2);
void STFD(int SourceReg, int AddressReg, int Immediate);
void STFS(int SourceReg, int AddressReg, int Immediate);
void STFSU(int SourceReg, int AddressReg, int Immediate);
void STH(int SourceReg, int AddressReg, int Immediate);
void STHU(int SourceReg, int AddressReg, int Immediate);
void STHUX(int SourceReg, int AddressReg1, int AddressReg2);
void STHX(int SourceReg, int AddressReg1, int AddressReg2);
void STMW(int StartReg, int AddressReg,  int Immediate);
void STW(int SourceReg, int AddressReg, int Immediate);
void STWU(int SourceReg, int AddressReg, int Immediate);
void STWUX(int SourceReg, int AddressReg1, int AddressReg2);
void STWX(int SourceReg, int AddressReg1, int AddressReg2);
//DestReg = SourceReg1 - SourceReg2
void SUBF(int DestReg, int SourceReg1, int SourceReg2);
void SYNC();
void XOR(int DestReg, int SourceReg1, int SourceReg2);
void XORI(int DestReg, int SourceReg, int Immediate);
void XORIS(int DestReg, int SourceReg, int Immediate);