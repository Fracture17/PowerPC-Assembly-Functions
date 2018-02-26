#pragma once

#include "stdafx.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
using namespace std;

typedef unsigned int u32;

//set to 1 if debugging.  records positions every frame and compares them during replay
#define IS_DEBUGGING 0
#define NORMAL 0
#define PMEX 1
#define BUILD_TYPE NORMAL

//ROTC floating offsets
#define FS_20_0 -0x7920

//draw buffer offsets
#define DRAW_BUFFER_XPOS_OFFSET 0x2C
#define DRAW_BUFFER_YPOS_OFFSET 0x30

///addresses start

///Function addresses start
#define GET_MEM_ALLOCATOR 0x80024430
#define ALLOC 0x80204e5c
#define MEMMOVE 0x803f602c
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
///Function addresses end

///addresses maintained by Brawl start
#define IS_REPLAY_LOC 0x805BBFC0 //equals 1 if in replay, 2 if in match
#define REPLAY_BUFFER_END_ADDRESS 0x9134CA00 //end of replay buffer
#define BRAWL_REPLAY_PTR_LOC 0x805BBFE8 //address of pointer to where to store input data
#define FRAME_COUNTER_LOC 0x901812A4 //gives the frame count of the match, increments through debug pause
#define PLAY_INPUT_LOC_START 0x805BC068 //the location of P1's inputs.  Add 4 for the next player during playback
#define PLAY_BUTTON_LOC_START 0x805BAD04 //the location of P1's buttons.  Add 0x40 for the next player
#define WII_BASED_CONTROLLER_START 0x804F7880 //P1 wiimote loc, can't affect inputs from here
#define WII_BASED_CONTROLLER_PORT_OFFSET 0x9A0 //offset between wii controller ports
#define WII_BASED_CONTROLLER_TYPE_OFFSET 0x28 //offset from Wii contoller of byte containing type of controller
#if BUILD_TYPE == NORMAL
#define ALT_STAGE_VAL_LOC 0x815e8422 //half word that defines what alt stage should be loaded
//#define ALT_STAGE_VAL_LOC 0x800B9EA2 //half word that defines what alt stage should be loaded
#elif BUILD_TYPE == PMEX
#define ALT_STAGE_VAL_LOC 0x805858ba //half word that defines what alt stage should be loaded
#endif
#define REPLAY_BUFFER_BEGIN 0x91301c00 //start of the replay buffer
#define IS_DEBUG_PAUSED 0x805B8A08 //word that equals 1 if game is debug paused, 0x100 if frame advancing
#define IS_AUTO_L_CANCEL 0x9017f36f //byte that is 1 if auto L-cancel is on
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
#define END_OF_REPLAY_BUFFER   (CURRENT_ALT_STAGE_INFO_LOC - 0x100) //tells when to stop recording inputs
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
#define KAPPA_ITEM_FLAG WIICHUCK_CONVERSION_TABLE + 16 //4
#define DI_DRAW_ALLOC_PTR KAPPA_ITEM_FLAG + 4 //8 * 4 * 4
///Code Menu Start
//935ce494
#define CODE_MENU_CREATED DI_DRAW_ALLOC_PTR + 8 * 4 * 4 //4 (equals 1 if created)
#define CODE_MENU_CONTROL_FLAG CODE_MENU_CREATED + 4 //4
#define CODE_MENU_PAGE_ADDRESS CODE_MENU_CONTROL_FLAG + 4 //4
#define DRAW_SETTINGS_PTR_LOC CODE_MENU_PAGE_ADDRESS + 4 //4
#define OFF_TEXT_LOC DRAW_SETTINGS_PTR_LOC + 4 //4
#define ON_TEXT_LOC OFF_TEXT_LOC + 4 //4
#define CODE_MENU_SETTINGS ON_TEXT_LOC + 4 //4 * MenuID
#define CODE_MENU_END CODE_MENU_SETTINGS + 4 * MenuID //0 (just a marker)
///Code Menu End
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

///addresses end

///constants start
///colors
#define RED 0xFF0000FF
#define BLUE 0x0066FFFF
#define ORANGE 0x33CC33FF //still green rn
#define GREEN 0x33CC33FF
#define YELLOW 0xFFFF00FF
#define BLACK 0x000000FF
#define WHITE 0xFFFFFFFF
///colors end
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
void GeckoStringWrite(char *Buffer, u32 NumBytes, u32 Address);
void SetGeckoBaseAddress(int Address);
void SetGeckoPointerAddress(int Address);
//searches for byte, elementOffset is distance between elements, ResultReg returns index if found, else -1
//StartAddressReg ends with the address of the found element, or an address after the array
void FindInArray(int ValueReg, int StartAddressReg, int numberOfElements, int elementOffset, int ResultReg, int TempReg);
void CallBrawlFunc(int Address);
//r3 returns ptr
void Allocate(int SizeReg);
void AllocateIfNotExist(int SizeReg, int AddressReg, int EmptyVal);
void Memmove(int DestReg, int SourceReg, int SizeReg);
void SetRegs(int StartReg, vector<int> values);
void SetArgumentsFromRegs(int StartReg, vector<int> ValueRegs);
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
void FreeMemIfAllocd(int AddressReg, int EmptyVal);
void FreeAllocdArray(int AllocAddressReg);
void SaveMem(int LocationReg, int SizeReg, int SaveToReg);
void SaveRegisters();
void SaveRegisters(vector<int> FPRegs);
void RestoreRegisters();
void Increment(int Reg);
void Decrement(int Reg);
void WriteStringToMem(string Text, int AddressReg);

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
void BCTRL();
void BLA(int Address);
void BLR();
void CMP(int Reg1, int Reg2, int CondField);
void CMPI(int Reg, int Immediate, int CondField);
void CMPL(int Reg1, int Reg2, int CondField);
void CMPLI(int Reg, int Immediate, int CondField);
void CNTLZW(int DestReg, int SourceReg);
void DIVW(int DestReg, int DividendReg, int DivisorReg);
void DIVWU(int DestReg, int DividendReg, int DivisorReg);
void EQV(int DestReg, int SourceReg1, int SourceReg2);
void EXTSB(int DestReg, int SourceReg);
void FADD(int DestReg, int SourceReg1, int SourceReg2);
void FADDS(int DestReg, int SourceReg1, int SourceReg2);
void FCMPU(int FPReg1, int FPReg2, int CondField);
void FMR(int DestReg, int SourceReg);
void FCTIW(int DestReg, int SourceReg);
void FDIV(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
void FMUL(int DestReg, int SourceReg1, int SourceReg2);
void FNEG(int DestReg, int SourceReg);
void FRSP(int DestReg, int SourceReg);
void FSUB(int FPDestReg, int FPSourceReg1, int FPSourceReg2);
void LBZ(int DestReg, int AddressReg, int Immediate);
void LBZU(int DestReg, int AddressReg, int Immediate);
void LBZUX(int DestReg, int AddressReg1, int AddressReg2);
void LBZX(int DestReg, int AddressReg1, int AddressReg2);
void LFD(int DestReg, int AddressReg, int Immediate);
void LFS(int DestReg, int AddressReg, int Immediate);
void LHZ(int DestReg, int AddressReg, int Immediate);
void LHZU(int DestReg, int AddressReg, int Immediate);
void LHZUX(int DestReg, int AddressReg1, int AddressReg2);
void LHZX(int DestReg, int AddressReg1, int AddressReg2);
void LWZ(int DestReg, int AddressReg, int Immediate);
void LWZU(int DestReg, int AddressReg, int Immediate);
void LWZUX(int DestReg, int AddressReg1, int AddressReg2);
void LWZX(int DestReg, int AddressReg1, int AddressReg2);
void LMW(int StartReg, int AddressReg, int Immediate);
void MFCTR(int TargetReg);
void MFLR(int TargetReg);
void MTCTR(int TargetReg);
void MTLR(int TargetReg);
void MULLI(int DestReg, int SourceReg, int Immediate);
void NEG(int DestReg, int SourceReg);
void NOP();
void NOR(int DestReg, int SourceReg1, int SourceReg2);
void OR(int DestReg, int SourceReg1, int SourceReg2);
void ORC(int DestReg, int SourceReg1, int SourceReg2);
void ORI(int DestReg, int SourceReg, int Immediate);
void ORIS(int DestReg, int SourceReg, int Immediate);
void RLWINM(int DestReg, int SourceReg, int ShiftNum, int MaskStart, int MaskEnd);
void RLWNM(int DestReg, int SourceReg, int ShiftReg, int MaskStart, int MaskEnd);
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
void XOR(int DestReg, int SourceReg1, int SourceReg2);
void XORI(int DestReg, int SourceReg, int Immediate);
void XORIS(int DestReg, int SourceReg, int Immediate);