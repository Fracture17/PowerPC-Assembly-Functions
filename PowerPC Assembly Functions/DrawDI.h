#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"

void SetupDrawLines(int LineWidthReg);
void DrawVerticies(int NumVerticiesReg, int AddressReg, int ColorReg);
void DrawDI();
void CalcNextPosition(int PX, int PY, int G, int FS, int MFS, int VX, int VY, int XA, int YA);
void AllocateGraphicBufferIfNotExist(int SizeReg, int AddressReg, int EmptyVal, int primType, int color);
void Draw();
void DrawBuffer(int AddressReg);
void ResetGraphicBuffer(int AddressReg);
void AddToGraphicBuffer(int AddressReg, int XPosReg, bool isXFloat, int YPosReg, bool isYFloat);
void CalcTrajectory();