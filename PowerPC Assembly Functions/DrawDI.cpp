#include "stdafx.h"
#include "DrawDI.h"

#define FP_REGISTERS int FPReg1 = 0, FPReg2 = 2, FPReg3 = 3, FPReg4 = 4, PX = 5, PY = 6, G = 7, FS = 8, MFS = 9, VX = 10, VY = 11, XA = 12, YA = 13, StaticFP1 = 14, StaticFP2 = 15, StaticFP3 = 16, TopBound = 17, LowBound = 18, LeftBound = 19, RightBound = 20;

void DrawDI()
{
	SetHitStart();

	AddToSDIBuffer();

	AddToASDIBuffer();

	SetTrajectoryBuffers();
}


void DrawTrajectories()
{
	//can't use r28 because Chase uses it in correctDamageVector
	//don't use r16 - r18 for same reason
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 27;
	int reg5 = 26;
	int reg6 = 25;
	int DIBufferReg = 24;
	int CounterReg = 23;
	int GraphicBufferReg = 22;
	int CharacterBufferReg = 21;
	int MainBufferReg = 20;
	int SignalReg = 18;

	FP_REGISTERS

	LoadWordToReg(reg1, IS_IN_GAME_FLAG);
	If(reg1, EQUAL_I, 1); {
		LoadWordToReg(reg1, DI_DRAW_INDEX + Line::VALUE);
		If(reg1, EQUAL_I, 1); {
			SetRegister(reg1, 20);
			SetupDrawLines(reg1);

			LoadWordToReg(reg2, IS_DEBUG_PAUSED);

			LoadWordToReg(MainBufferReg, MAIN_BUFFER_PTR);
			LWZU(CharacterBufferReg, MainBufferReg, 4);
			While(CharacterBufferReg, NOT_EQUAL_I, 0); {
				LWZ(reg1, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET);
				LWZ(reg1, reg1, 0);
				OR(reg1, reg1, reg2); //union of debug state and hitstun
				If(reg1, GREATER_I, 0); {
					//in hitstun or debug paused
					LWZ(DIBufferReg, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
					If(reg2, NOT_EQUAL_I, 0); {
						//is debug paused
						LWZ(reg1, DIBufferReg, DI_BUFFER_NORMAL_START_OFFSET);
						ResetGraphicBuffer(reg1, BLUE);

						LWZ(reg1, CharacterBufferReg, CHR_BUFFER_INFO_PTR_OFFSET);
						LWZ(reg3, CharacterBufferReg, CHR_BUFFER_PORT_OFFSET);
						CallBrawlFunc(0x8004a750); //getIPSwitch
						LBZ(reg1, reg1, 7); //get controller num
						MULLI(reg3, reg3, 4);
						MULLI(reg1, reg1, BUTTON_PORT_OFFSET);
						SetRegister(reg4, GCC_CONTROL_STICK_X_START - BUTTON_PORT_OFFSET);
						ADDI(reg3, reg3, 10);
						LHZX(reg1, reg4, reg1); //get stick values
						STHX(reg1, 3, reg3);

						LWZ(reg1, CharacterBufferReg, CHR_BUFFER_FIGHTER_INPUT_PTR_OFFSET);
						MR(3, reg1);
						CallBrawlFunc(0x80048250); //update IPHuman
						LFS(XA, reg1, 0x10); //stickX
						LFS(YA, reg1, 0x14); //stickY

						LWZ(reg6, CharacterBufferReg, CHR_BUFFER_IP_SWITCH_PTR_OFFSET);
						LFS(StaticFP1, reg6, 8); //stickX
						LFS(StaticFP2, reg6, 0xC); //stickY
						STFS(XA, reg6, 8);
						STFS(YA, reg6, 0xC);
						LWZ(reg1, CharacterBufferReg, CHR_BUFFER_KB_VECTOR_PTR_OFFSET);
						LWZ(reg5, CharacterBufferReg, CHR_BUFFER_XVEL_OFFSET);
						LWZ(reg3, reg1, 8); //save xvel
						STW(reg5, reg1, 8);
						LWZ(reg5, CharacterBufferReg, CHR_BUFFER_YVEL_OFFSET);
						LWZ(reg4, reg1, 0xC); //save yvel
						STW(reg5, reg1, 0xC);
						SetRegister(3, 0x80b897bc);
						LWZ(4, MainBufferReg, -4);
						SetRegister(SignalReg, 0xC0DE);
						CallBrawlFunc(0x808778d8); //correctDamageVector
						//restore
						STW(reg3, reg1, 8);
						STW(reg4, reg1, 0xC);
						STFS(StaticFP1, reg6, 8);
						STFS(StaticFP2, reg6, 0xC);
					}EndIf();

					LWZU(GraphicBufferReg, DIBufferReg, DI_BUFFER_SDI_START_OFFSET);
					CounterLoop(CounterReg, 0, 4, 1); {
						DrawGraphicBuffer(GraphicBufferReg);
						LWZU(GraphicBufferReg, DIBufferReg, 4);
					}CounterLoopEnd();

					If(reg2, NOT_EQUAL_I, 0); {
						//debug draw
						DrawGraphicBuffer(GraphicBufferReg);
					}EndIf();
				}EndIf();

				LWZU(CharacterBufferReg, MainBufferReg, 8);
			}EndWhile();
		}EndIf();
	}EndIf();
}

void SetHitStart()
{
	//r30 = module ptr
	//r31 = damage log
	ASMStart(0x808761e8);

	WriteIntToFile(0x4e800421); //bctrl

	SaveRegisters();

	int reg1 = 29;
	int reg2 = 28;
	int reg3 = 27;
	int reg4 = 26;
	int reg5 = 25;
	int DIBufferReg = 15;
	int CharacterBufferReg = 14;

	FindCharacterBuffer(30, CharacterBufferReg);
	If(CharacterBufferReg, NOT_EQUAL_I, 0); {
		LWZ(reg1, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET);
		LWZ(reg1, reg1, 0);
		STW(reg1, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_OFFSET);

		LWZ(reg1, CharacterBufferReg, CHR_BUFFER_POS_PTR_OFFSET);
		LWZ(reg2, reg1, 0xC);
		LWZ(reg3, reg1, 0x10);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_XPOS_OFFSET);
		STW(reg3, CharacterBufferReg, CHR_BUFFER_YPOS_OFFSET);

		LWZ(reg1, 31, 0xC);
		STW(reg1, CharacterBufferReg, CHR_BUFFER_XVEL_OFFSET);

		LWZ(reg1, 31, 0x10);
		STW(reg1, CharacterBufferReg, CHR_BUFFER_YVEL_OFFSET);

		LWZ(DIBufferReg, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);

		LWZ(reg1, DIBufferReg, DI_BUFFER_SDI_START_OFFSET);
		ResetGraphicBuffer(reg1);

		LWZ(reg2, CharacterBufferReg, CHR_BUFFER_XPOS_OFFSET);
		LWZ(reg3, CharacterBufferReg, CHR_BUFFER_YPOS_OFFSET);
		AddToGraphicBuffer(reg1, reg2, false, reg3, false, false);

		LWZ(reg1, DIBufferReg, DI_BUFFER_ASDI_START_OFFSET);
		ResetGraphicBuffer(reg1);

		LWZ(reg1, DIBufferReg, DI_BUFFER_NORMAL_START_OFFSET);
		ResetGraphicBuffer(reg1, BLUE);

		LWZ(reg1, DIBufferReg, DI_BUFFER_CURRENT_START_OFFSET);
		ResetGraphicBuffer(reg1, YELLOW);
	} EndIf();

	RestoreRegisters();
	ASMEnd();
}

void AddToSDIBuffer()
{
	//f0 = new xpos
	//f2 = new ypos
	//r30 = module ptr
	//can use r12, r3
	ASMStart(0x80876c84);
	SaveRegisters();

	FindCharacterBuffer(30, 12);
	If(12, NOT_EQUAL_I, 0); {

		STFS(0, 12, CHR_BUFFER_XPOS_OFFSET);
		STFS(2, 12, CHR_BUFFER_YPOS_OFFSET);

		LWZ(12, 12, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
		LWZ(12, 12, DI_BUFFER_SDI_START_OFFSET);
		AddToGraphicBuffer(12, 0, true, 2, true, true);

	} EndIf();

	RestoreRegisters();
	ASMEnd(0xd0410014); //stfs f2, sp, 0x14
}

void AddToASDIBuffer()
{
	//f1 = old xpos
	//f3 = old ypos
	//f0 + f1 = new xpos
	//f2 = new ypos
	//r31 = module ptr
	//use f3, r3, r12
	ASMStart(0x80876fec);
	SaveRegisters();

	int reg1 = 30;
	int reg2 = 29;
	int CharacterBufferReg = 15;
	int DIBufferReg = 14;

	FindCharacterBuffer(31, CharacterBufferReg);
	If(CharacterBufferReg, NOT_EQUAL_I, 0); {

		LWZ(DIBufferReg, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
		LWZ(reg1, DIBufferReg, DI_BUFFER_ASDI_START_OFFSET);
		AddToGraphicBuffer(reg1, 1, true, 3, true, true);

		FADDS(3, 0, 1);

		STFS(3, CharacterBufferReg, CHR_BUFFER_XPOS_OFFSET);
		STFS(2, CharacterBufferReg, CHR_BUFFER_YPOS_OFFSET);

		AddToGraphicBuffer(reg1, 3, true, 2, true, false);

	} EndIf();

	RestoreRegisters();
	ASMEnd(0xc0610018); //lfs f3, sp, 0x18
}

void SetTrajectoryBuffers()
{
	//r29 = module ptr
	ASMStart(0x80877b48);
	SaveRegisters(30); //reduce number saved later

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 28;
	int reg4 = 27;
	int reg5 = 26;
	int reg6 = 25;
	int DIBufferReg = 15;
	int CharacterBufferReg = 14;

	FP_REGISTERS

	FindCharacterBuffer(29, CharacterBufferReg);
	If(CharacterBufferReg, NOT_EQUAL_I, 0); {

		LoadByteToReg(reg3, IS_DEBUG_PAUSED + 3);

		LFS(G, CharacterBufferReg, CHR_BUFFER_GRAVITY_OFFSET);
		LFS(MFS, CharacterBufferReg, CHR_BUFFER_MFS_OFFSET);
		LFS(VX, CharacterBufferReg, CHR_BUFFER_XVEL_OFFSET);
		LFS(VY, CharacterBufferReg, CHR_BUFFER_YVEL_OFFSET);
		LWZ(DIBufferReg, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
		LWZ(reg1, DIBufferReg, DI_BUFFER_NORMAL_START_OFFSET);
		ResetGraphicBuffer(reg1, BLUE);

		CounterLoop(reg6, 0, 2, 1); {
			LFS(PX, CharacterBufferReg, CHR_BUFFER_XPOS_OFFSET);
			LFS(PY, CharacterBufferReg, CHR_BUFFER_YPOS_OFFSET);

			LWZ(reg2, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_OFFSET);
			SetFloatingRegister(FS, 3, 0);
			//LWZ(reg2, reg2, 0);
			CalcTrajectory(reg1, reg2);

			LWZ(reg1, CharacterBufferReg, CHR_BUFFER_KB_VECTOR_PTR_OFFSET);
			LFS(VX, reg1, 0x8);
			LFS(VY, reg1, 0xC);

			If(reg3, NOT_EQUAL_I, 0); {
				//is debug paused
				LWZ(reg1, DIBufferReg, DI_BUFFER_DEBUG_START_OFFSET);
				ResetGraphicBuffer(reg1, PURPLE);
				SetRegister(reg3, 0);
			}Else(); {
				If(reg6, EQUAL_I, 0);
				{
					LWZ(reg1, DIBufferReg, DI_BUFFER_CURRENT_START_OFFSET);
					ResetGraphicBuffer(reg1, YELLOW);
				}EndIf();
			}EndIf();
		}CounterLoopEnd();

	} EndIf();

	RestoreRegisters();
	ASMEnd(0xe3e10088); //psq_l stuff
}

void CreateDIBuffer(int CharacterBufferReg, int reg1, int reg2, int reg3)
{
	SetRegister(reg1, DI_BUFFER_SIZE + 0x10);
	Allocate(reg1);
	STW(3, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
	MR(reg1, 3);

	//allocate buffers
	ADDI(reg2, reg1, DI_BUFFER_ASDI_START_OFFSET);
	SetRegister(reg3, ASDI_BUFFER_SIZE);
	AllocateGraphicBuffer(reg3, reg2, PRIMITIVE_LINE, GREEN);

	IfInSSE(reg3, reg2); {
		SetRegister(reg3, SSE_SDI_BUFFER_SIZE);
	} Else(); {
		SetRegister(reg3, SDI_BUFFER_SIZE);
	} EndIf();
	ADDI(reg2, reg1, DI_BUFFER_SDI_START_OFFSET);
	AllocateGraphicBuffer(reg3, reg2, PRIMITIVE_LINE, ORANGE);

	IfInSSE(reg3, reg2); {
		SetRegister(reg3, SSE_TRAJECTORY_BUFFER_SIZE);
	} Else(); {
		SetRegister(reg3, TRAJECTORY_BUFFER_SIZE);
	} EndIf();

	ADDI(reg2, reg1, DI_BUFFER_NORMAL_START_OFFSET);
	AllocateGraphicBuffer(reg3, reg2, PRIMITIVE_LINE, BLUE);

	ADDI(reg2, reg1, DI_BUFFER_CURRENT_START_OFFSET);
	AllocateGraphicBuffer(reg3, reg2, PRIMITIVE_LINE, YELLOW);

	ADDI(reg2, reg1, DI_BUFFER_DEBUG_START_OFFSET);
	AllocateGraphicBuffer(reg3, reg2, PRIMITIVE_LINE, PURPLE);

	SetRegister(reg2, 0);
	STW(reg2, reg1, DI_BUFFER_SIZE); //set end for freeing
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

	STW(5, 3, GRAPHIC_BUFFER_PRIMITIVE_OFFSET); //primitive type
	ADDI(5, SizeReg, -8);
	STW(5, 3, GRAPHIC_BUFFER_MAX_SIZE_OFFSET); //max size
	STW(6, 3, GRAPHIC_BUFFER_COLOR_OFFSET); //color
	STW(4, 3, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //number of elements (0)
	STW(4, 3, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //should draw flag

	ADDI(4, 3, GRAPHIC_BUFFER_POS_START_OFFSET - 4);
	STW(4, 3, GRAPHIC_BUFFER_END_PTR_OFFSET); //end of position list


	/*STWU(5, 3, 4); //primitive type
	STWU(6, 3, 4); //color
	STWU(4, 3, 4); //number of elements (0)
	STWU(4, 3, 4); //should draw flag
	STWU(3, 3, -0x10); //end of position list*/
}

//keep draw flag at end to work
void ResetGraphicBuffer(int AddressReg)
{
	SetRegister(3, 0);
	STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //reset number of elements
	STW(3, AddressReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //turn off draw flag

	ADDI(3, AddressReg, GRAPHIC_BUFFER_POS_START_OFFSET - 4);
	STW(3, AddressReg, GRAPHIC_BUFFER_END_PTR_OFFSET); //end of position list

	//STWU(AddressReg, AddressReg, -GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //reset end of list ptr
}

//keep draw flag at end to work
void ResetGraphicBuffer(int AddressReg, int Color)
{
	SetRegister(3, Color);
	STW(3, AddressReg, GRAPHIC_BUFFER_COLOR_OFFSET);
	ResetGraphicBuffer(AddressReg);
	/*SetRegister(3, 0);
	STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //reset number of elements
	STWU(3, AddressReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //turn off draw flag
	STWU(AddressReg, AddressReg, -GRAPHIC_BUFFER_DRAW_FLAG_OFFSET); //reset end of list ptr*/
}

void AddToGraphicBuffer(int AddressReg, int XPosReg, bool isXFloat, int YPosReg, bool isYFloat, bool shouldSetDrawFlag)
{
	//LWZ(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //num elements
	LWZ(4, AddressReg, GRAPHIC_BUFFER_MAX_SIZE_OFFSET);
	LWZ(3, AddressReg, GRAPHIC_BUFFER_END_PTR_OFFSET);
	SUBF(3, 3, AddressReg);
	If(3, LESS_L, 4); {
		LWZ(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //num elements
		Increment(3);
		STW(3, AddressReg, GRAPHIC_BUFFER_NUM_ELEMS_OFFSET); //update num elements

		LWZ(3, AddressReg, GRAPHIC_BUFFER_END_PTR_OFFSET); //ptr to end of list
		if (isXFloat) { STFSU(XPosReg, 3, 4); }
		else { STWU(XPosReg, 3, 4); }
		if (isYFloat) { STFSU(YPosReg, 3, 4); }
		else { STWU(YPosReg, 3, 4); }
		STW(3, AddressReg, 0); //update end of list ptr
	} EndIf();

	if (shouldSetDrawFlag) {
		SetGraphicBufferDrawFlag(AddressReg, true);
	}
}

void SetupCharacterBuffer()
{
	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;

	SetRegister(reg1, CHARACTER_BUFFER_SIZE);
	Allocate(reg1);
	SetRegister(reg1, MAIN_BUFFER_PTR);
	STW(3, reg1, 0);

	SetRegister(reg1, 0);
	STW(reg1, 3, 0); //clear first slot
	STW(reg1, 3, 4); //clear first slot
}

void FreeDIBuffer(int BufferReg, int reg1)
{
	ADDI(reg1, BufferReg, -4);
	FreeAllocdArray(reg1);
	FreeMem(BufferReg);
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

	If(numFramesReg, GREATER_I, 500); {
		SetRegister(numFramesReg, 500);
	}EndIf();

	While(numFramesReg, GREATER_I, 0); {
		AddToGraphicBuffer(bufferReg, PX, true, PY, true, false);

		CalcNextPosition();

		IsOutOfBounds();
		If(3, EQUAL_I, 1); {
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

	SetRegister(3, 1);
	FCMPU(PY, TopBound, 0);
	BC(7, BRANCH_IF_FALSE, GT); //branch to next
	SetFloatingRegister(XA, 4, 2.4); //4 instructions
	FCMPU(VY, XA, 0);
	BC(8, BRANCH_IF_TRUE, GT); //branch to end
	//next
	FCMPU(PX, LeftBound, 0);
	BC(6, BRANCH_IF_TRUE, LT); //branch to end
	FCMPU(PX, RightBound, 0);
	BC(4, BRANCH_IF_TRUE, GT); //branch to end
	FCMPU(PY, LowBound, 0);
	BC(2, BRANCH_IF_TRUE, LT); //branch to end
	SetRegister(3, 0);
	//end
}

void SetGraphicBufferDrawFlag(int BufferPtrReg, bool value)
{
	SetRegister(3, value);
	STW(3, BufferPtrReg, GRAPHIC_BUFFER_DRAW_FLAG_OFFSET);
}

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