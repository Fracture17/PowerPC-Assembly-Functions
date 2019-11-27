#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "DrawDI.h"
#include "IASA Overlay.h"
#include "Code Menu.h"
#include "Save States.h"

//constants
const int CHR_BUFFER_VARIABLES_ADDRESS_OFFSET = 0;
const int CHR_BUFFER_HITSTUN_FRAMES_OFFSET = CHR_BUFFER_VARIABLES_ADDRESS_OFFSET + 4;
const int CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET = CHR_BUFFER_HITSTUN_FRAMES_OFFSET + 4;
const int CHR_BUFFER_POS_PTR_OFFSET = CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET + 4;
const int CHR_BUFFER_KB_VECTOR_PTR_OFFSET = CHR_BUFFER_POS_PTR_OFFSET + 4;
const int CHR_BUFFER_IP_SWITCH_PTR_OFFSET = CHR_BUFFER_KB_VECTOR_PTR_OFFSET + 4;
const int CHR_BUFFER_FIGHTER_INPUT_PTR_OFFSET = CHR_BUFFER_IP_SWITCH_PTR_OFFSET + 4;
const int CHR_BUFFER_XVEL_OFFSET = CHR_BUFFER_FIGHTER_INPUT_PTR_OFFSET + 4;
const int CHR_BUFFER_YVEL_OFFSET = CHR_BUFFER_XVEL_OFFSET + 4;
const int CHR_BUFFER_XPOS_OFFSET = CHR_BUFFER_YVEL_OFFSET + 4;
const int CHR_BUFFER_YPOS_OFFSET = CHR_BUFFER_XPOS_OFFSET + 4;
const int CHR_BUFFER_GRAVITY_OFFSET = CHR_BUFFER_YPOS_OFFSET + 4;
const int CHR_BUFFER_MFS_OFFSET = CHR_BUFFER_GRAVITY_OFFSET + 4;
const int CHR_BUFFER_INFO_PTR_OFFSET = CHR_BUFFER_MFS_OFFSET + 4;
const int CHR_BUFFER_PORT_OFFSET = CHR_BUFFER_INFO_PTR_OFFSET + 4;
const int CHR_BUFFER_HEAD_OF_FIGHTER_OFFSET = CHR_BUFFER_PORT_OFFSET + 4;
const int CHR_BUFFER_SAVE_STATE_ACTION_OFFSET = CHR_BUFFER_HEAD_OF_FIGHTER_OFFSET + 4;
const int CHR_BUFFER_SAVE_STATE_SUB_ACTION_OFFSET = CHR_BUFFER_SAVE_STATE_ACTION_OFFSET + 4;
const int CHR_BUFFER_SAVE_STATE_SUB_ACTION_FRAME_OFFSET = CHR_BUFFER_SAVE_STATE_SUB_ACTION_OFFSET + 4;
const int CHR_BUFFER_SAVE_STATE_MEM_PTR_ARRAY_OFFSET = CHR_BUFFER_SAVE_STATE_SUB_ACTION_FRAME_OFFSET + 4; //0x18
//buffers
const int CHR_BUFFER_DI_BUFFER_PTR_OFFSET = CHR_BUFFER_SAVE_STATE_MEM_PTR_ARRAY_OFFSET + 0x18;
const int CHR_BUFFER_SIZE = CHR_BUFFER_DI_BUFFER_PTR_OFFSET + 4;

//value paths
const vector<int> COLOR_ID = { CHR_BUFFER_INFO_PTR_OFFSET, 5, 1 };
const vector<int> CHARACTER_ID = { CHR_BUFFER_INFO_PTR_OFFSET, 0, 1 };

static const int CHARACTER_INFO_START_ADDRESS = 0x90180FB8;

void Draw();
void DeleteCharacterBufferOnTransform();
void ChangeCharacterBufferModule();
void AddNewCharacterBuffer();
void DeleteCharacterBuffer();
void ControlCodes();
void FixPercentSelector();
void LoadCodeMenu();
void StartMatch();
void EndMatch();
void FindCharacterBuffer(int TargetReg, int ResultReg);
void FindEndOfCharacterBuffers(int TargetReg, int ResultReg);
void GetCharacterValue(int CharacterBufferReg, vector<int> ValuePath, int ResultReg = 3);
void InfiniteFriendlies(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9);
void GetLegalStagesArray(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9);
void AddLegalStagesToArray(int StageListReg, int LegalStageHighMaskReg, int LegalStageLowMaskReg, int ArrayReg, int PosReg, int reg2, int reg3, int reg4);
void LimitCharacters();
void RestoreLimitedCharacters();