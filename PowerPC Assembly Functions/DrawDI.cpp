#include "stdafx.h"
#include "DrawDI.h"

#define FP_REGISTERS int FPReg1 = 0, FPReg2 = 2, FPReg3 = 3, FPReg4 = 4, PX = 5, PY = 6, G = 7, FS = 8, MFS = 9, VX = 10, VY = 11, XA = 12, YA = 13, StaticFP1 = 14, StaticFP2 = 15, StaticFP3 = 16, TopBound = 17, LowBound = 18, LeftBound = 19, RightBound = 20;

/*
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
*/
void DrawDI()
{
	int CounterReg = 31;
	int BufferPtrReg = 30;
	int Reg1 = 29;
	
	LoadWordToReg(Reg1, IS_IN_GAME_FLAG);
	If(Reg1, EQUAL_I, 1); //is in game

	LoadWordToReg(Reg1, CODE_MENU_SETTINGS);
	If(Reg1, EQUAL_I, 1); //toggle is on

	SetKnockbackTrajectoryBuffers();

	SetRegister(14, 20);
	SetupDrawLines(14);

	LoadWordToReg(BufferPtrReg, DI_DRAW_ALLOC_PTR);
	ADDI(BufferPtrReg, BufferPtrReg, MAIN_BUFFER_GRAPHICS_BUFFER_START_OFFSET - 4);
	CounterLoop(CounterReg, 0, 8 * 5, 1); {
		LWZU(Reg1, BufferPtrReg, 4);
		DrawGraphicBuffer(Reg1);
	}CounterLoopEnd();

	EndIf(); //toggle is on

	EndIf(); //is in game
}

void DrawGraphicBuffer(int AddressReg)
{
	LWZ(5, AddressReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET);
	If(5, EQUAL_I, 1); //draw flag set

	LWZ(5, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //number
	If(5, GREATER_OR_EQUAL_I, 2); //enough nodes

	LWZ(3, AddressReg, GRAPHIC_BUFFER_PRIMITIVE_OFFSET); //type
	SetRegister(4, 1); //attr num
	GXBegin(3, 4, 5);

	LWZ(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //number
	LWZ(4, AddressReg, GRAPHIC_BUFFER_COLOR_OFFSET); //color
	SetRegister(5, 0xCC010000);
	SetRegister(6, 0);
	ADDI(8, AddressReg, GRAPHIC_BUFFER_POS_START_OFFSET - 4);
	While(3, GREATER_I, 0); //draw loop

	LWZU(7, 8, 4); //x
	STW(7, 5, -0x8000);
	LWZU(7, 8, 4); //y
	STW(7, 5, -0x8000);
	STW(6, 5, -0x8000);
	STW(4, 5, -0x8000);

	Decrement(3);
	EndWhile(); //draw loop

	EndIf(); //enough nodes

	EndIf(); //draw flag set
}

//r3 returns beginning of buffer
void AllocateGraphicBuffer(int SizeReg, int AddressReg, int primType, int color)
{
	Allocate(SizeReg);
	STW(3, AddressReg, 0);

	SetRegister(4, 0);
	SetRegister(5, primType);
	SetRegister(6, color);
	STWU(5, 3, 4); //primitive type
	STWU(6, 3, 4); //color
	STWU(4, 3, 4); //number of elements (0)
	STWU(4, 3, 4); //should draw flag
	STWU(3, 3, -0x10); //end of position list
}

//keep draw flag at end to work
void ResetGraphicBuffer(int AddressReg)
{
	SetRegister(3, 0);
	STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //reset number of elements
	STWU(3, AddressReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //turn off draw flag
	STWU(AddressReg, AddressReg, -GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //reset end of list ptr
}

//keep draw flag at end to work
void ResetGraphicBuffer(int AddressReg, int Color)
{
	SetRegister(3, Color);
	STW(3, AddressReg, GRAPHIC_BUFFER_COLOR_OFFSET);
	SetRegister(3, 0);
	STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //reset number of elements
	STWU(3, AddressReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //turn off draw flag
	STWU(AddressReg, AddressReg, -GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //reset end of list ptr
}

void AddToGraphicBuffer(int AddressReg, int XPosReg, bool isXFloat, int YPosReg, bool isYFloat, bool shouldSetDrawFlag)
{
	LWZ(3, AddressReg, GRAPHIC_BUFFER_END_PTR_OFFSET); //ptr to end of list
	if (isXFloat) { STFSU(XPosReg, 3, 4); }
	else { STWU(XPosReg, 3, 4); }
	if (isYFloat) { STFSU(YPosReg, 3, 4); }
	else { STWU(YPosReg, 3, 4); }
	STW(3, AddressReg, 0); //update end of list ptr
	LWZ(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //num elements
	Increment(3);
	STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //update num elements

	if (shouldSetDrawFlag) {
		SetGraphicBufferDrawFlag(AddressReg, true);
	}
}

void SetupDIBuffer()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;

	SetRegister(reg1, MAIN_BUFFER_SIZE);
	Allocate(reg1);
	SetRegister(reg1, DI_DRAW_ALLOC_PTR);
	STW(3, reg1, 0);

	ADDI(reg1, 3, -4);
	ADDI(reg3, 3, MAIN_BUFFER_GRAPHICS_BUFFER_START_OFFSET);
	SetRegister(reg4, 0);
	SetRegister(reg5, SDI_BUFFER_SIZE);
	SetRegister(reg6, ASDI_BUFFER_SIZE);
	SetRegister(reg7, TRAJECTORY_BUFFER_SIZE);
	CounterLoop(reg2, 0, 8, 1); {
		AllocateGraphicBuffer(reg5, reg3, 0xB0, RED); //sdi
		ADDI(reg3, reg3, 4);
		AllocateGraphicBuffer(reg6, reg3, 0xB0, GREEN); //asdi
		ADDI(reg3, reg3, 4);
		AllocateGraphicBuffer(reg7, reg3, 0xB0, BLUE); //normal trajectory
		ADDI(reg3, reg3, 4);
		AllocateGraphicBuffer(reg7, reg3, 0xB0, YELLOW); //DI trajectory
		ADDI(reg3, reg3, 4);
		AllocateGraphicBuffer(reg7, reg3, 0xB0, PURPLE); //projected debug trajectory
		ADDI(reg3, reg3, 4);
		STWU(reg4, reg1, 4); //flags
	}CounterLoopEnd();
	STW(reg4, reg3, 0); //set last to 0 for freeing
}

void FreeDIBuffer()
{
	int reg1 = 31;
	int reg2 = 30;

	LoadWordToReg(reg1, DI_DRAW_ALLOC_PTR);
	ADDI(reg2, reg1, MAIN_BUFFER_GRAPHICS_BUFFER_START_OFFSET);
	FreeAllocdArray(reg2); //free graphics buffers
	FreeMem(reg1); //free main buffer
}

//numFramesReg is set to 0
void CalcTrajectory(int bufferReg, int numFramesReg)
{
	//[[r13 - 0x41A8] + 0x158] = left blast zone
	//+ 0x15C = right blast zone
	//+ 0x160 = top
	//+ 0x164 = bottom

	FP_REGISTERS

	LWZ(3, 13, -0x41A8);
	LFS(LeftBound, 3, 0x158);
	LFS(RightBound, 3, 0x15C);
	LFS(TopBound, 3, 0x160);
	LFS(LowBound, 3, 0x164);

	While(numFramesReg, GREATER_I, 0); {
		AddToGraphicBuffer(bufferReg, PX, true, PY, true, false);

		CalcNextPosition();

		IsOutOfBounds();
		If(3, EQUAL_I, 0); {
			SetRegister(3, RED);
			STW(3, bufferReg, GRAPHIC_BUFFER_COLOR_OFFSET);
		}EndIf();

		Decrement(numFramesReg);
	}EndWhile(); //calc pos loop

	SetGraphicBufferDrawFlag(bufferReg, true);
}

void IsOutOfBounds()
{
	FP_REGISTERS

	SetRegister(3, 0);
	FCMPU(PX, LeftBound, 0);
	BC(8, BRANCH_IF_TRUE, LT); //branch to end
	FCMPU(PX, RightBound, 0);
	BC(6, BRANCH_IF_TRUE, GT); //branch to end
	FCMPU(PY, TopBound, 0);
	BC(4, BRANCH_IF_TRUE, GT); //branch to end
	FCMPU(PY, LowBound, 0);
	BC(2, BRANCH_IF_TRUE, LT); //branch to end
	SetRegister(3, 1);
	//end
}

void SetGraphicBufferDrawFlag(int BufferPtrReg, bool value)
{
	SetRegister(3, value);
	STW(3, BufferPtrReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET);
}

void SetKnockbackTrajectoryBuffers()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int StickPtrReg = 26;
	int DebugBufferReg = 25;
	int DIBufferReg = 24;
	int NoDIBufferReg = 23;
	int ASDIBufferReg = 22;
	int SDIBufferReg = 21;
	int StateReg = 20;
	int StatePtr = 19;
	int GraphicBufferPtr = 18;
	int ModuleReg = 17;
	int HitstunFramesReg = 16;
	int HitstunLocReg = 15;
	int CharCounterReg = 14;

	FP_REGISTERS

	int NextFighter = GetNextLabel();

	SetRegister(HitstunLocReg, 0x901AE000 + 0xE0 - 0x5A0);
	SetRegister(StickPtrReg, GCC_CONTROL_STICK_X_START - 0x40);
	LoadWordToReg(StatePtr, DI_DRAW_ALLOC_PTR);
	ADDI(GraphicBufferPtr, StatePtr, MAIN_BUFFER_GRAPHICS_BUFFER_START_OFFSET - 4);
	ADDI(StatePtr, StatePtr, -4);

	CounterLoop(CharCounterReg, 0, 8, 1); {
		LWZU(StateReg, StatePtr, 4);
		LoadWordToReg(reg1, 0x80B87C28);
		ANDI(reg2, CharCounterReg, 1);
		If(reg2, EQUAL_I, 0); {
			ADDI(StickPtrReg, StickPtrReg, 0x40);
		}EndIf();
		RLWINM(reg2, CharCounterReg, 31, 30, 31); //>>1
		SetArgumentsFromRegs(3, { reg1, reg2 });
		CallBrawlFunc(0x80815c40); //getEntryID

		If(3, GREATER_OR_EQUAL_I, 0); {
			MR(4, 3);
			MR(3, reg1);
			SetRegister(5, 0);
			ANDI(reg2, CharCounterReg, 1);
			If(reg2, EQUAL_I, 0); {
				//main fighter
				ADDI(HitstunLocReg, HitstunLocReg, 0x5A0);
				CallBrawlFunc(0x80814f20); //getFighter
			}Else(); {
				//sub fighter
				ADDI(HitstunLocReg, HitstunLocReg, 0x2D0);
				CallBrawlFunc(0x80814fb8); //getSubFighter
			}EndIf();

			If(3, NOT_EQUAL_I, 0); {
				//if fighter or subfighter exists
				LWZ(HitstunFramesReg, HitstunLocReg, 0); //hitstun frames
				If(HitstunFramesReg, GREATER_I, 0); {
					LWZ(SDIBufferReg, GraphicBufferPtr, 4);
					LWZ(ASDIBufferReg, GraphicBufferPtr, 8);
					LWZ(NoDIBufferReg, GraphicBufferPtr, 0xC);
					LWZ(DIBufferReg, GraphicBufferPtr, 0x10);
					LWZU(DebugBufferReg, GraphicBufferPtr, 0x14);

					LWZ(ModuleReg, 3, 0x60);
					LWZ(ModuleReg, ModuleReg, 0xD8);

					LWZ(reg1, ModuleReg, 0x38); //soDamageModuleActor

					LWZ(reg3, ModuleReg, 0xC); //posture module
					LFS(PX, reg3, 0xC);
					LFS(PY, reg3, 0x10);

					LWZ(reg3, ModuleReg, 0x7C); //kineticGeneric module
					LWZ(reg4, reg3, 0x58);
					LFS(FS, reg4, 0xC);
					LFS(G, reg4, 0x10);
					LFS(MFS, reg4, 0x14);
					FNEG(MFS, MFS);

					LWZ(reg4, reg3, 0x7C);
					LFS(VX, reg4, 0x8);
					LFS(VY, reg4, 0xC);
					LFS(XA, reg4, 0x10); //XA
					LFS(YA, reg4, 0x14); //YA

					LWZ(reg3, reg4, 0x10); //XA
					LWZ(reg4, reg4, 0x14); //YA
					OR(reg4, reg3, reg4);
					If(reg4, EQUAL_I, 0); { //not moving yet
						//clear buffers
						ResetGraphicBuffer(ASDIBufferReg);
						ResetGraphicBuffer(NoDIBufferReg, BLUE);
						ResetGraphicBuffer(DIBufferReg, YELLOW);
					}EndIf();

					LWZ(reg3, ModuleReg, 0x44); //stop module
					LBZ(reg3, reg3, 0x1C);
					If(reg3, NOT_EQUAL_I, 0); {
						//in hitlag
						LoadByteToReg(reg2, LAST_DEBUG_STATE + 3);
						If(reg2, EQUAL_I, 0); {
							If(StateReg, NOT_EQUAL_I, HITSTUN_AND_HITLAG_STATE); { //first hitlag frame
								SetRegister(StateReg, HITSTUN_AND_HITLAG_STATE);
								ResetGraphicBuffer(SDIBufferReg);
							}EndIf();

							AddToGraphicBuffer(SDIBufferReg, PX, true, PY, true, true);
						}EndIf();
						AddToGraphicBuffer(ASDIBufferReg, PX, true, PY, true, false);
					}Else(); If(reg4, NOT_EQUAL_I, 0); {
						If(StateReg, NOT_EQUAL_I, HITSTUN_STATE); {
							SetRegister(StateReg, HITSTUN_STATE);
							//get start pos
							FSUBS(PX, PX, VX);
							FSUBS(PY, PY, VY);
							FSUBS(PY, PY, FS);
							//asdi
							AddToGraphicBuffer(ASDIBufferReg, PX, true, PY, true, true);
							//di
							//save changing fpregs
							FMR(StaticFP1, PX);
							FMR(StaticFP2, PY);
							FMR(StaticFP3, FS);
							MR(reg5, HitstunFramesReg);
							FSUBS(VX, VX, XA);
							FSUBS(VY, VY, YA);
							CalcTrajectory(DIBufferReg, reg5);
							//no di
							//restore changed fpregs
							FMR(PX, StaticFP1);
							FMR(PY, StaticFP2);
							FMR(FS, StaticFP3);
							LFS(VX, reg1, 0xC8);
							LFS(VY, reg1, 0xCC);
							CalcTrajectory(NoDIBufferReg, HitstunFramesReg);
						}EndIf();
					}EndIf(); EndIf();

					LoadWordToReg(reg2, IS_DEBUG_PAUSED);
					If(reg2, NOT_EQUAL_I, 0); {
						//is in debug pause
						ResetGraphicBuffer(DebugBufferReg, PURPLE);

						//get hitstun frames
						LFS(FPReg1, reg1, 0xD8);
						ConvertFloatingRegToInt(FPReg1, HitstunFramesReg);

						LFS(StaticFP1, reg1, 0xC8); //VX
						LFS(StaticFP2, reg1, 0xCC); //VY
						FMR(1, StaticFP2);
						FMR(2, StaticFP1);
						//call atan, takes f1, f2, changes all volitile fpregs, returns angle in radians in f1
						CallBrawlFunc(0x803fd0b0);
						FMR(StaticFP3, 1); //V angle
						//LFS(StaticFP3, reg1, 0xD0);
						//LFS(FPReg1, reg1, 0xD4);
						//FNEG(FPReg1, FPReg1);
						//FMULS(StaticFP3, StaticFP3, FPReg1);

						LBZ(reg3, StickPtrReg, 0); //SX
						LBZ(reg2, StickPtrReg, 1); //SY
						ConvertIntStickValsToFloating(reg3, reg2, XA, YA, FPReg3);
						ClampStick(XA, YA);
						FMULS(FPReg3, XA, XA); //SX^2
						FMULS(FPReg4, YA, YA); //SY^2
						FADDS(FPReg3, FPReg3, FPReg4); //SX^2 + SY^2 = S^2
						FRSQRTE(FPReg3, FPReg3); //1 / S
						FRES(FPReg3, FPReg3); //S

						SetFloatingRegister(FPReg4, 3, 1.0025);
						FABS(FPReg1, FPReg3); //|S|
						FCMPU(FPReg1, FPReg4, 0); //if |S| > 1.0025
						BC(4, BRANCH_IF_FALSE, GT);
						FDIVS(FPReg4, FPReg4, FPReg3); //1.0025 / |S|
						FMULS(XA, XA, FPReg4);
						FMULS(YA, YA, FPReg4);
						//branch to here

						LFS(FPReg1, reg1, 0xBC);
						SetFloatingRegister(FPReg2, 3, 0.03);
						FMULS(FPReg1, FPReg1, FPReg2);

						//FMULS(FPReg1, StaticFP1, StaticFP1); //VX^2
						//FMULS(FPReg2, StaticFP2, StaticFP2); //VY^2
						//FADDS(VX, FPReg1, FPReg2); //VX^2 + VY^2
						//FMR(1, VX);
						//CallBrawlFunc(0x8003db58); //rsqrt
						//FMULS(FPReg1, 1, VX); //V^2 * (1 / V) = V

						FMULS(StaticFP2, XA, StaticFP2); //SX * VY
						FMULS(StaticFP1, YA, StaticFP1); //SY * VX
						FSUBS(StaticFP1, StaticFP1, StaticFP2); //-(SX * VY) + (SY * VX) = cross
						//FSUBS(StaticFP1, StaticFP2, StaticFP1); //(SX * VY) - (SY * VX) = cross
						FDIVS(StaticFP1, StaticFP1, FPReg1); //|cross| / V = val

						//cap value (apparently don't need to)


						FABS(FPReg2, StaticFP1); //|val|
						FMULS(StaticFP1, StaticFP1, FPReg2);
						SetFloatingRegister(FPReg2, 3, 0.314159265);
						FMULS(StaticFP1, StaticFP1, FPReg2);
						FADDS(StaticFP1, StaticFP1, StaticFP3); //V angle + val
						FMR(StaticFP2, FPReg1); //move V
						//call sin
						FMR(1, StaticFP1);
						CallBrawlFunc(0x804009e0);
						FMULS(StaticFP3, 1, StaticFP2); //sin(V angle + val) * V
						//call cos
						FMR(1, StaticFP1);
						CallBrawlFunc(0x804004d8);
						FMULS(VX, 1, StaticFP2); //cos(V angle + val) * V
						FMR(VY, StaticFP3);

						LWZ(reg1, ASDIBufferReg, GRAPHIC_BUFFER_END_PTR_OFFSET);
						LFS(PX, reg1, -4);
						LFS(PY, reg1, 0);

						LWZ(reg3, ModuleReg, 0x7C); //kineticGeneric module
						LWZ(reg4, reg3, 0x58);
						LFS(G, reg4, 0x10);
						FMR(FS, G); //always this when first hit
						LFS(MFS, reg4, 0x14);
						FNEG(MFS, MFS);

						CalcTrajectory(DebugBufferReg, HitstunFramesReg);
					}Else(); {
						SetGraphicBufferDrawFlag(DebugBufferReg, false);
					}EndIf();

					JumpToLabel(NextFighter);
				}EndIf();
			}EndIf();
		}EndIf();

		SetRegister(StateReg, OTHER_STATE);
		//clear draw flags
		CounterLoop(reg1, 0, 5, 1); {
			LWZU(reg2, GraphicBufferPtr, 4);
			SetGraphicBufferDrawFlag(reg2, false);
		}CounterLoopEnd();

		Label(NextFighter);
		STW(StateReg, StatePtr, 0); //store state
	}CounterLoopEnd();
}

/*
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
	FMULS(FPReg1, VX, VX); //VX^2
	FMULS(FPReg2, VY, VY); //VY^2
	FADDS(FPReg1, FPReg1, FPReg2); //VX^2 + VY^2
	
	FMR(FPReg4, 0);
	FMR(FPReg3, 1);
	FMR(1, FPReg1);
	CallBrawlFunc(0x80400b48); //sqrt
	//use rsqrt at 0x8003db58
	//takes f0, changes f0 - f4
	//returns 1 / root(x)
	//x * (1 / root(x)) == root(x)
	FMR(FPReg1, 1);
	FMR(0, FPReg4);
	FMR(1, FPReg3);
	SetRegister(Reg1, GetHexFromFloat(-0.051));
	STW(Reg1, 1, -4);
	LFS(FPReg2, 1, -4); //-0.051
	FADD(FPReg2, FPReg1, FPReg2); //Vi - 0.051
	FDIVS(FPReg1, FPReg2, FPReg1); //Vf / Vi
	SetRegister(Reg1, GetHexFromFloat(-1));
	STW(Reg1, 1, -4);
	LFS(FPReg2, 1, -4); //-1
	FADDS(FPReg1, FPReg1, FPReg2); //Vf/Vi - 1
	FMULS(XA, VX, FPReg1); //VX * Vm
	FMULS(YA, VY, FPReg1); //VY * Vm

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
*/

//updates all input to be that of the next frame
void CalcNextPosition()
{
	FP_REGISTERS

	CalcBrakeVectors();

	FADDS(VX, VX, XA);
	FADDS(VY, VY, YA);
	FADDS(PX, PX, VX);
	FADDS(PY, PY, FS);
	FADDS(PY, PY, VY);
	FADDS(FS, FS, G);

	//check if FallSpeed is within bounds, and set it to MaxFallSpeed if it isn't
	FCMPU(FS, MFS, 0);
	BC(2, 0b00100, 0); //branch 2 instructions
	FMR(FS, MFS);
}

void CalcBrakeVectors()
{
	FP_REGISTERS

	FMULS(FPReg1, VX, VX); //VX^2
	FMULS(FPReg2, VY, VY); //VY^2
	FADDS(XA, FPReg1, FPReg2); //VX^2 + VY^2
	
	FMR(1, XA);
	CallBrawlFunc(0x8003db58); //rsqrt
	//takes f1, changes f0 - f4
	//x * (1 / root(x)) == root(x)
	FMULS(FPReg1, 1, XA); //V^2 * (1 / V) = V

	SetFloatingRegister(FPReg2, 3, -0.051);
	FADDS(FPReg2, FPReg1, FPReg2); //Vi - 0.051
	FDIVS(FPReg1, FPReg2, FPReg1); //Vf / Vi
	SetFloatingRegister(FPReg2, 3, -1);
	FADDS(FPReg1, FPReg1, FPReg2); //Vf/Vi - 1
	FMULS(XA, VX, FPReg1); //VX * Vm
	FMULS(YA, VY, FPReg1); //VY * Vm
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