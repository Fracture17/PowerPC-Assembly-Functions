#include "stdafx.h"
#include "DrawDI.h"

#define POSITION_BUFFER_SIZE 8 * 512


void CodeMenuTest()
{
	ASMStart(0x800b0888);

	ASMEnd();

	//ASMStart(0x800b0884);
	ASMStart(0x800b088c);
	SaveRegisters();

	int Reg1 = 31;
	int Reg2 = 30;

	SetRegister(Reg1, 0x80002800);
	LWZ(Reg2, Reg1, 4);
	If(Reg2, NOT_EQUAL_I, 0); //create

	SetRegs(3, { 1, 0x2A, 0x2B });
	CallBrawlFunc(0x800b8930); //create
	STW(3, Reg1, 0);

	SetRegs(4, { 0x200, 1 });
	CallBrawlFunc(0x800b8b08); //allocMsgBuffer

	LWZ(3, Reg1, 0);
	SetRegister(4, 0x928929e0);
	CallBrawlFunc(0x800b8c7c); //setMsgData

	SetRegister(Reg2, 0);
	STW(Reg2, Reg1, 4);

	EndIf(); //create

	LWZ(Reg2, Reg1, 0);
	If(Reg2, LESS_I, 0); //draw

	LWZ(3, Reg1, 0);
	LWZ(3, 3, 8);
	SetRegs(4, { 0, 9 });
	CallBrawlFunc(0x8006a964); //SetDefaultEnv



	CallBrawlFunc(0x80019FA4); //(getManager[cameraManager])

	CallBrawlFunc(0x80018DE4); //(setGX[camera])


	LWZ(3, Reg1, 0);
	LWZ(3, 3, 8);
	SetRegister(4, 0x01000000);
	STW(4, 3, 0x4C);
	SetRegister(4, GetHexFromFloat(0.0625));
	//SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x50);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x68);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x24);
	SetRegister(4, GetHexFromFloat(1));
	STW(4, 3, 0x28);
	SetRegister(4, GetHexFromFloat(0.973585));
	STW(4, 3, 0x58);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x5C);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x2C);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x30);
	SetRegister(4, GetHexFromFloat(0));
	STW(4, 3, 0x34);
	SetRegister(4, 1);
	STW(4, 3, 0x38);
	SetRegister(4, 1);
	STW(4, 3, 0x3C);
	SetRegister(4, RED);
	STW(4, 3, 8);
	SetRegister(4, RED);
	STW(4, 3, 0xC);
	SetRegister(4, RED);
	STW(4, 3, 0x10);
	SetRegister(4, RED);
	STW(4, 3, 0x14);
	SetRegister(4, 0x80497d7c);
	STW(4, 3, 0x48);
	SetRegister(4, 0);
	STW(4, 3, 0x40);


	float XPos = 0;
	for (char x : "Infinite Shields   ")
	{
		LWZ(3, Reg1, 0);
		LWZ(3, 3, 8);
		SetRegister(4, x);
		CallBrawlFunc(0x8006fe50); //printChar
	}

	SetRegister(4, 0xFFFFFFFF);
	STW(4, 3, 8);
	SetRegister(4, 0xFFFFFFFF);
	STW(4, 3, 0xC);
	SetRegister(4, 0xFFFFFFFF);
	STW(4, 3, 0x10);
	SetRegister(4, 0xFFFFFFFF);
	STW(4, 3, 0x14);

	for (char x : "P1   ")
	{
		LWZ(3, Reg1, 0);
		LWZ(3, 3, 8);
		SetRegister(4, x);
		CallBrawlFunc(0x8006fe50); //printChar
	}

	for (char x : "P2   ")
	{
		LWZ(3, Reg1, 0);
		LWZ(3, 3, 8);
		SetRegister(4, x);
		CallBrawlFunc(0x8006fe50); //printChar
	}

	for (char x : "P3   ")
	{
		LWZ(3, Reg1, 0);
		LWZ(3, 3, 8);
		SetRegister(4, x);
		CallBrawlFunc(0x8006fe50); //printChar
	}

	for (char x : "P4   ")
	{
		LWZ(3, Reg1, 0);
		LWZ(3, 3, 8);
		SetRegister(4, x);
		CallBrawlFunc(0x8006fe50); //printChar
	}

	EndIf(); //draw

	RestoreRegisters();
	ASMEnd(0x80010014);
}

void Draw()
{
	ASMStart(0x8000e588);
	vector<int> FPRegs(14);
	iota(FPRegs.begin(), FPRegs.end(), 0);
	SaveRegisters(FPRegs);

	SetRegister(14, 20);
	SetupDrawLines(14);

	DrawDI();

	RestoreRegisters();
	ASMEnd(0x38210030); //addi sp, sp, 48
}

void DrawDI()
{
	int CounterReg = 31;
	int BufferPtrReg = 30;
	int Reg1 = 29;

	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	If(Reg1, NOT_EQUAL_I, 0); //is in game

	CalcTrajectory();

	SetRegister(CounterReg, 0);
	SetRegister(BufferPtrReg, DI_DRAW_ALLOC_PTR - 4);
	While(CounterReg, LESS_I, 32); //draw loop

	LHZU(Reg1, BufferPtrReg, 4);
	If(Reg1, NOT_EQUAL_I_L, 0xCCCC); //exists

	LWZ(Reg1, BufferPtrReg, 0);
	DrawBuffer(Reg1);

	EndIf(); //exists

	ADDI(CounterReg, CounterReg, 1);
	EndWhile(); //draw loop

	EndIf(); //is in game
}

void DrawBuffer(int AddressReg)
{
	LWZU(5, AddressReg, 0x1C); //number
	If(5, GREATER_OR_EQUAL_I, 2); //enough nodes

	LWZ(3, AddressReg, -0xC); //type
	LWZ(4, AddressReg, -8);
	GXBegin(3, 4, 5);

	LWZ(3, AddressReg, 0); //number
	LWZ(4, AddressReg, -4); //color
	SetRegister(5, 0xCC010000);
	SetRegister(6, 0);
	While(3, GREATER_I, 0); //draw loop

	LWZU(7, AddressReg, 4); //x
	STW(7, 5, -0x8000);
	LWZU(7, AddressReg, 4); //y
	STW(7, 5, -0x8000);
	STW(6, 5, -0x8000);
	STW(4, 5, -0x8000);

	ADDI(3, 3, -1);
	EndWhile(); //draw loop

	EndIf(); //enough nodes
}

void AllocateGraphicBufferIfNotExist(int SizeReg, int AddressReg, int EmptyVal, int primType, int color)
{
	LHZ(3, AddressReg, 0);
	If(3, EQUAL_I_L, EmptyVal);

	Allocate(SizeReg);
	STW(3, AddressReg, 0);

	SetRegister(4, 0);
	SetRegister(5, primType);
	SetRegister(6, 1);
	SetRegister(7, color);
	STWU(4, 3, 4); //extra
	STWU(4, 3, 4); //extra
	STWU(4, 3, 4); //extra
	STWU(5, 3, 4); //primitive type
	STWU(6, 3, 4); //VTXDesc
	STWU(7, 3, 4); //color
	STWU(4, 3, 4); //number of elements (0)
	STWU(3, 3, -0x1C); //end of position list

	EndIf();
}

void ResetGraphicBuffer(int AddressReg)
{
	SetRegister(3, 0);
	STWU(3, AddressReg, 4); //reset flag
	STWU(3, AddressReg, 4); //reset flag
	STWU(3, AddressReg, 4); //reset flag
	STWU(3, AddressReg, 0x10); //reset number of elements
	STWU(AddressReg, AddressReg, -0x1C); //reset end of list ptr
}

void AddToGraphicBuffer(int AddressReg, int XPosReg, bool isXFloat, int YPosReg, bool isYFloat)
{
	LWZ(3, AddressReg, 0); //ptr to end of list
	if (isXFloat) { STFSU(XPosReg, 3, 4); }
	else { STWU(XPosReg, 3, 4); }
	if (isYFloat) { STFSU(YPosReg, 3, 4); }
	else { STWU(YPosReg, 3, 4); }
	STW(3, AddressReg, 0); //update end of list ptr
	LWZ(3, AddressReg, 0x1C); //num elements
	ADDI(3, 3, 1);
	STW(3, AddressReg, 0x1C); //update num elements
}

void CalcTrajectory()
{
	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;
	int Reg5 = 27;
	int BufferSizeReg = 21;
	int PosDataPtrReg = 20;
	int PositionBufferPtrReg = 19;
	int CharCounterReg = 18;
	int ColorReg = 17;
	int HitstunFramesReg = 16;
	int HitstunLocReg = 15;
	int ModuleReg = 14;

	//floating regs
	int PX = 0;
	int PY = 1;
	int G = 2;
	int FS = 3;
	int MFS = 4;
	int VX = 5;
	int VY = 6;
	int XA = 7;
	int YA = 8;
	int PosCompFPReg = 9;
	int FPReg1 = 10;
	int FPReg2 = 11;
	int FPReg3 = 12;
	int FPReg4 = 13;


	SetRegister(BufferSizeReg, POSITION_BUFFER_SIZE);
	SetRegister(PositionBufferPtrReg, DI_DRAW_ALLOC_PTR);
	SetRegister(HitstunLocReg, 0x901AE000 + 0xE0 - 0x5A0);
	SetRegister(CharCounterReg, 0);
	While(CharCounterReg, LESS_I, 8); //character loop

	LoadWordToReg(Reg1, 0x80B87C28);
	RLWINM(Reg2, CharCounterReg, 31, 30, 31); //>>1
	SetArgumentsFromRegs(3, { Reg1, Reg2 });
	CallBrawlFunc(0x80815c40); //getEntryID

	If(3, GREATER_OR_EQUAL_I, 0); //fighter exists

	ADDI(4, 3, 0);
	ADDI(3, Reg1, 0);
	SetRegister(5, 0);
	ANDI(Reg2, CharCounterReg, 1);
	If(Reg2, EQUAL_I, 0); //is main fighter
	ADDI(HitstunLocReg, HitstunLocReg, 0x5A0);
	CallBrawlFunc(0x80814f20); //getFighter
	Else(); //is main fighter
	ADDI(HitstunLocReg, HitstunLocReg, 0x2D0);
	CallBrawlFunc(0x80814fb8); //getSubFighter
	EndIf(); //is main fighter

	If(3, NOT_EQUAL_I, 0); //fighter or subFighter exists

	LWZ(HitstunFramesReg, HitstunLocReg, 0); //hitstun frames
	If(HitstunFramesReg, GREATER_I, 0); //in hitstun

	LWZ(ModuleReg, 3, 0x60);

	LWZ(ModuleReg, ModuleReg, 0xD8);
	LWZ(Reg3, ModuleReg, 0xC); //posture module
	LFS(PX, Reg3, 0xC);
	LFS(PY, Reg3, 0x10);

	LWZ(Reg3, ModuleReg, 0x7C); //kineticGeneric module
	LWZ(Reg4, Reg3, 0x58);
	LFS(FS, Reg4, 0xC);
	LFS(G, Reg4, 0x10);
	LFS(MFS, Reg4, 0x14);

	LWZ(Reg4, Reg3, 0x7C);
	LFS(VX, Reg4, 0x8);
	LFS(VY, Reg4, 0xC);
	LFS(XA, Reg4, 0x10);
	LFS(YA, Reg4, 0x14);

	FNEG(MFS, MFS);

	LWZ(Reg3, ModuleReg, 0x44); //stop module
	LBZ(Reg3, Reg3, 0x1C);
	If(Reg3, NOT_EQUAL_I, 0); //in hitlag

	//delete non SDI buffers
	SetRegister(Reg1, 0);
	ADDI(Reg2, PositionBufferPtrReg, 4);
	While(Reg1, LESS_I, 3); //delete loop

	FreeMemIfAllocd(Reg2, 0xCCCCCCCC);

	ADDI(Reg2, Reg2, 4);
	ADDI(Reg1, Reg1, 1);
	EndWhile(); //delete loop

	AllocateGraphicBufferIfNotExist(BufferSizeReg, PositionBufferPtrReg, 0xCCCCCCCC, 0xB0, RED);
	LWZ(Reg1, PositionBufferPtrReg, 0); //SDI buffer
	LWZ(Reg2, Reg1, 0xC); //flag
	If(Reg2, EQUAL_I, 1); //reset SDI
	ResetGraphicBuffer(Reg1);
	EndIf(); //reset SDI

	AddToGraphicBuffer(Reg1, PX, true, PY, true);

	STFS(VX, Reg1, 4);
	STFS(VY, Reg1, 8);

	Else(); //in hitlag

	LHZ(Reg1, PositionBufferPtrReg, 0xC); //hitstun buffer
	If(Reg1, EQUAL_I_L, 0xCCCC); //buffer does not exist

	SetRegister(Reg1, 0);
	ADDI(Reg2, PositionBufferPtrReg, 0);
	While(Reg1, LESS_I, 4); //create buffer loop

	AllocateGraphicBufferIfNotExist(BufferSizeReg, Reg2, 0xCCCCCCCC, 0xB0, GREEN);

	ADDI(Reg2, Reg2, 4);
	ADDI(Reg1, Reg1, 1);
	EndWhile(); //create buffer loop

	LWZ(Reg1, PositionBufferPtrReg, 0); //SDI
	SetRegister(Reg2, 1);
	STW(Reg2, Reg1, 0xC); //SDI flag
	LWZ(Reg2, Reg1, 0x1C); //num elems
	If(Reg2, EQUAL_I, 0); //just created

	AddToGraphicBuffer(Reg1, PX, true, PY, true);
	STFS(VX, Reg1, 4);
	STFS(VY, Reg1, 8);

	EndIf(); //just created

	LWZ(Reg2, Reg1, 0); //end ptr
	LWZ(Reg3, Reg2, -4); //X
	LWZ(Reg4, Reg2, 0); //Y

	LWZ(Reg1, PositionBufferPtrReg, 4); //ASDI
	AddToGraphicBuffer(Reg1, Reg3, false, Reg4, false); //pos at end of hitlag
	AddToGraphicBuffer(Reg1, PX, true, PY, true); //current pos

	//save changed fpregs
	FMR(FPReg1, PX);
	FMR(FPReg2, PY);
	FMR(FPReg3, FS);

	LWZ(Reg1, PositionBufferPtrReg, 8); //DI buffer
	STW(HitstunFramesReg, Reg1, 0x1C); //set numm elems
	SetRegister(Reg2, YELLOW);
	STW(Reg2, Reg1, 0x18);
	SetRegister(Reg2, 0);
	LWZ(Reg3, Reg1, 0);
	While(Reg2, LESS, HitstunFramesReg); //calc pos loop

	STFSU(PX, Reg3, 4);
	STFSU(PY, Reg3, 4);

	CalcNextPosition(PX, PY, G, FS, MFS, VX, VY, XA, YA);

	ADDI(Reg2, Reg2, 1);
	EndWhile(); //calc pos loop

	//restore changed fpregs
	FMR(PX, FPReg1);
	FMR(PY, FPReg2);
	FMR(FS, FPReg3);

	//load VX, VY
	LWZ(Reg1, PositionBufferPtrReg, 0); //SDI
	LFS(VX, Reg1, 4);
	LFS(VY, Reg1, 8);

	//calc XA, YA
	FMUL(FPReg1, VX, VX); //VX^2
	FMUL(FPReg2, VY, VY); //VY^2
	FADD(FPReg1, FPReg1, FPReg2); //VX^2 + VY^2
	FMR(FPReg4, 0);
	FMR(FPReg3, 1);
	FMR(1, FPReg1);
	CallBrawlFunc(0x80400b48); //sqrt
	FMR(FPReg1, 1);
	FMR(0, FPReg4);
	FMR(1, FPReg3);
	SetRegister(Reg1, GetHexFromFloat(-0.051));
	STW(Reg1, 1, -4);
	LFS(FPReg2, 1, -4); //-0.051
	FADD(FPReg2, FPReg1, FPReg2); //Vi - 0.051
	FDIV(FPReg1, FPReg2, FPReg1); //Vf / Vi
	SetRegister(Reg1, GetHexFromFloat(-1));
	STW(Reg1, 1, -4);
	LFS(FPReg2, 1, -4); //-1
	FADD(FPReg1, FPReg1, FPReg2); //Vf/Vi - 1
	FMUL(XA, VX, FPReg1); //VX * Vm
	FMUL(YA, VY, FPReg1); //VY * Vm

	LWZ(Reg1, PositionBufferPtrReg, 0xC); //no DI buffer
	STW(HitstunFramesReg, Reg1, 0x1C); //set numm elems
	SetRegister(Reg2, BLUE);
	STW(Reg2, Reg1, 0x18);
	SetRegister(Reg2, 0);
	LWZ(Reg3, Reg1, 0);
	While(Reg2, LESS, HitstunFramesReg); //calc pos loop

	STFSU(PX, Reg3, 4);
	STFSU(PY, Reg3, 4);

	CalcNextPosition(PX, PY, G, FS, MFS, VX, VY, XA, YA);

	ADDI(Reg2, Reg2, 1);
	EndWhile(); //calc pos loop

	EndIf(); //buffer does not exist
	EndIf(); //in hitlag

	ADDI(PositionBufferPtrReg, PositionBufferPtrReg, 0x10);

	//jump to end of loop
	int DeleteSkip = GetNextLabel();
	JumpToLabel(DeleteSkip);

	EndIf(); //in hitstun
	EndIf(); //fighter or subFighter exists
	EndIf(); //fighter exists

	SetRegister(Reg1, 0);
	While(Reg1, LESS_I, 4); //delete loop

	FreeMemIfAllocd(PositionBufferPtrReg, 0xCCCCCCCC);

	ADDI(PositionBufferPtrReg, PositionBufferPtrReg, 4);
	ADDI(Reg1, Reg1, 1);
	EndWhile(); //delete loop

	Label(DeleteSkip);

	ADDI(CharCounterReg, CharCounterReg, 1);
	EndWhile(); //character loop
}

//updates all input to be that of the next frame
//all inputs are floating Regs
void CalcNextPosition(int PX, int PY, int G, int FS, int MFS, int VX, int VY, int XA, int YA)
{
	FADD(PX, PX, VX);
	FADD(VX, VX, XA);
	FADD(PY, PY, FS);
	FADD(PY, PY, VY);
	FADD(VY, VY, YA);
	FADD(FS, FS, G);

	//check if FallSpeed is within bounds, and set it to MaxFallSpeed if it isn't
	FCMPU(FS, MFS, 0);
	BC(2, 0b00100, 0); //branch 2 instructions
	FMR(FS, MFS);
}

void SetupDrawLines(int LineWidthReg)
{
	CallBrawlFunc(0x80019FA4); //(getManager[cameraManager])

	CallBrawlFunc(0x80018DE4); //(setGX[camera])

	CallBrawlFunc(0x8001A5C0); //gfDrawSetVtxColorPrimEnvrment

	SetRegister(3, 1);
	SetRegister(4, 3);
	SetRegister(5, 0);
	GXSetZMode(3, 4, 5);

	SetRegister(3, 7);
	SetRegister(4, 0);
	SetRegister(5, 1);
	SetRegister(6, 7);
	SetRegister(7, 0);
	CallBrawlFunc(0x801f3fd8); //GXSetAlphaCompare

	SetRegister(4, 0);
	GXSetLineWidth(LineWidthReg, 4);
}

void DrawVerticies(int NumVerticiesReg, int AddressReg, int ColorReg)
{
	SetRegister(3, 0);
	SetRegister(4, 0xCC010000);
	ADDI(5, AddressReg, -4);
	SetRegister(6, 0);
	While(3, LESS, NumVerticiesReg); //draw loop

	LWZU(7, 5, 4);
	STW(7, 4, -0x8000); //x
	LWZU(7, 5, 4);
	STW(7, 4, -0x8000); //y
	STW(6, 4, -0x8000); //z
	STW(ColorReg, 4, -0x8000); //color

	ADDI(3, 3, 1);
	EndWhile(); //draw loop
}