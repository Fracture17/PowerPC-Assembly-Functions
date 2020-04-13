#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Code Menu.h"
#include "Control Codes.h"

//graphic buffer constants
#define GRAPHIC_BUFFER_END_PTR_OFFSET 0
#define GRAPHIC_BUFFER_PRIMITIVE_OFFSET 4
#define GRAPHIC_BUFFER_COLOR_OFFSET 8
#define GRAPHIC_BUFFER_NUM_ELEMS_OFFSET 0xC
#define GRAPHIC_BUFFER_MAX_SIZE_OFFSET 0x10
#define GRAPHIC_BUFFER_DRAW_FLAG_OFFSET 0x14
#define GRAPHIC_BUFFER_POS_START_OFFSET 0x18
#define GRAPHIC_BUFFER_OVERHEAD_SIZE 0x18

#define TRAJECTORY_BUFFER_SIZE 8 * 512 + GRAPHIC_BUFFER_OVERHEAD_SIZE
#define SDI_BUFFER_SIZE 8 * 512 + GRAPHIC_BUFFER_OVERHEAD_SIZE
#define SSE_TRAJECTORY_BUFFER_SIZE 8 * 80 + GRAPHIC_BUFFER_OVERHEAD_SIZE
#define SSE_SDI_BUFFER_SIZE 8 * 80 + GRAPHIC_BUFFER_OVERHEAD_SIZE
#define ASDI_BUFFER_SIZE 8 * 3 + GRAPHIC_BUFFER_OVERHEAD_SIZE
#define MAIN_BUFFER_GRAPHICS_BUFFER_START_OFFSET 8 * 4
#define CHARACTER_BUFFER_SIZE (8 + 1) * 4 * 2

const int DI_BUFFER_SDI_START_OFFSET = 0;
const int DI_BUFFER_ASDI_START_OFFSET = DI_BUFFER_SDI_START_OFFSET + 4;
const int DI_BUFFER_NORMAL_START_OFFSET = DI_BUFFER_ASDI_START_OFFSET + 4;
const int DI_BUFFER_CURRENT_START_OFFSET = DI_BUFFER_NORMAL_START_OFFSET + 4;
const int DI_BUFFER_DEBUG_START_OFFSET = DI_BUFFER_CURRENT_START_OFFSET + 4;
const int DI_BUFFER_SIZE = DI_BUFFER_DEBUG_START_OFFSET + 4;

#define OTHER_STATE 0
#define HITSTUN_AND_HITLAG_STATE 1
#define HITSTUN_STATE 2

void SetupDrawLines(int LineWidthReg);
void DrawVerticies(int NumVerticiesReg, int AddressReg, int ColorReg);
void DrawDI();
void DrawTrajectories();
void SetHitStart();
void AddToSDIBuffer();
void AddToASDIBuffer();
void SetTrajectoryBuffers();
void CreateDIBuffer(int CharacterBufferReg, int reg1, int reg2, int reg3);
void CalcNextPosition();
void CalcBrakeVectors();
void AllocateGraphicBuffer(int SizeReg, int AddressReg, int primType, int color);
void DrawGraphicBuffer(int AddressReg);
void ResetGraphicBuffer(int AddressReg);
void ResetGraphicBuffer(int AddressReg, int Color);
void AddToGraphicBuffer(int AddressReg, int XPosReg, bool isXFloat, int YPosReg, bool isYFloat, bool shouldSetDrawFlag);
void SetupCharacterBuffer();
void FreeDIBuffer(int BufferReg, int reg1);
void CalcTrajectory(int bufferReg, int numFramesReg);
void IsOutOfBounds();
void SetGraphicBufferDrawFlag(int BufferPtrReg, bool value);