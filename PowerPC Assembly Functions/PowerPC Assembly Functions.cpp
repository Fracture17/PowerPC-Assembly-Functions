#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include <iostream>

//converts char hex digit to decimal
int HexToDec(char x)
{
	x = tolower(x);
	if (x <= '9')
	{
		return x - '0';
	}
	return x - 'a' + 10;
}

void CodeStart(string FilePath)
{
	WPtr.open(FilePath, fstream::out);
	if(!WPtr.is_open())
	{
		cout << "ERROR, output file not found\n";
		exit(0);
	}
}

void CodeEnd()
{
	CompleteJumps();

	WPtr.close();
}

void MakeGCT(string TextFilePath, string OldGCTFilePath, string NewGCTFilePath)
{
	fstream TextFilePtr(TextFilePath, fstream::in | fstream::binary);
	fstream OldGCTFilePtr(OldGCTFilePath, fstream::in | fstream::binary);
	fstream NewGCTFilePtr(NewGCTFilePath, fstream::out | fstream::binary);

	if (TextFilePtr.is_open() == false)
	{
		cout << "could not find text file\n";
		return;
	}
	if (OldGCTFilePtr.is_open() == false)
	{
		cout << "could not find source gct file\n";
		return;
	}
	if (NewGCTFilePtr.is_open() == false)
	{
		cout << "could not make new gct file\n";
		return;
	}

	string str = "";
	char input;
	while ((input = TextFilePtr.get()) != EOF)
	{
		input = (HexToDec(input) << 4) + HexToDec(TextFilePtr.get());
		str.push_back(input);
	}

	NewGCTFilePtr << OldGCTFilePtr.rdbuf();
	NewGCTFilePtr.seekg(-8, ios::end);
	for (auto x : str)
	{
		NewGCTFilePtr << x;
	}
	char end[8] = { 0xF0,0,0,0,0,0,0,0 };
	NewGCTFilePtr.write(end, 8);


	TextFilePtr.close();
	OldGCTFilePtr.close();
	NewGCTFilePtr.close();
}

int GetHexFromFloat(float Value)
{
	int *b = (int *)&Value;
	return *b;
}

int GetShiftNum(int endPos)
{
	return (WORD_SIZE - 1 - endPos);
}

int GetOpSegment(int val, int size, int pos)
{
	int mask = (1 << size) - 1;
	val &= mask;
	val <<= GetShiftNum(pos);
	return val;
}

//menu text has byte with how long the string is in front
void WriteMenuTextToFile(string Text)
{
	WriteTextToFile(((char) Text.size()) + Text);
}

void WriteTextToFile(string Text)
{
	string Output = "";
	for (int i = 0; i < Text.size(); i++) {
		sprintf(OpHexBuffer, "%02X", Text[i]);
		Output += OpHexBuffer[0];
		Output += OpHexBuffer[1];
	}
	/*for (int i = 0; i < Text.size() % 4; i++) {
		Output += "00";
	}*/
	const char* ptr = Output.c_str();
	WPtr << ptr;
}

//writes in hex
void WriteIntToFile(int val)
{
	sprintf(OpHexBuffer, "%08X", val);
	WPtr << OpHexBuffer;
}

//use preproccesor defined values for comparisions
//definitions with _I treat the third argument as an immediate
void If(int Val1, int Comparision, int Val2)
{
	if(IfIndex >= MAX_IFS)
	{
		cout << "ERROR, too many ifs\n";
		exit(0);
	}
	if (Comparision >= 40)
	{
		Comparision -= 40;
		CMPLI(Val1, Val2, 0);
	}
	else if (Comparision >= 30)
	{
		Comparision -= 30;
		CMPL(Val1, Val2, 0);
	}
	else if(Comparision >= 10)
	{
		Comparision -= 10;
		CMPI(Val1, Val2, 0);
	}
	else
	{
		CMP(Val1, Val2, 0);
	}
	IfStartPos[IfIndex] = WPtr.tellp();
	IfConditionArray[IfIndex] = Comparision;
	WriteIntToFile(0);
	IfIndex++;
}

void EndIf()
{
	IfIndex--;
	int holdPos = WPtr.tellp();
	WPtr.seekp(IfStartPos[IfIndex]);
	int BranchCond = BRANCH_IF_FALSE;
	if(IfConditionArray[IfIndex] == IS_ELSE)
	{
		BranchCond = BRANCH_ALWAYS;
	}
	else if(IfConditionArray[IfIndex] > 2)
	{
		BranchCond = BRANCH_IF_TRUE;
		IfConditionArray[IfIndex] -= 3;
	}
	BC(CalcBranchOffset(IfStartPos[IfIndex], holdPos), BranchCond, IfConditionArray[IfIndex]);
	WPtr.seekp(holdPos);
}

void Else()
{
	WriteIntToFile(0);
	EndIf();
	IfStartPos[IfIndex] = WPtr.tellp();
	IfStartPos[IfIndex] -= 8;
	IfConditionArray[IfIndex] = IS_ELSE;
	IfIndex++;
}

//use preproccesor defined values for comparisions
//definitions with _I treat the third argument as an immediate
void While(int Val1, int Comparision, int Val2)
{
	If(Val1, Comparision, Val2);

	if (Comparision >= 40)
	{
		OpHex = GetOpSegment(10, 6, 5); //cmpli
		OpHex |= GetOpSegment(Val2, 16, 31);
		Comparision -= 40;
		//CMPLI(Val1, Val2, 0);
	}
	else if (Comparision >= 30)
	{
		OpHex = GetOpSegment(31, 6, 5); //cmpl
		OpHex |= GetOpSegment(Val2, 5, 20);
		OpHex |= GetOpSegment(32, 10, 30);
		Comparision -= 30;
		//CMPL(Val1, Val2, 0);
	}
	else if(Comparision >= 10)
	{
		OpHex = GetOpSegment(11, 6, 5);//cmpi
		OpHex |= GetOpSegment(Val2, 16, 31);
		Comparision -= 10;
	}
	else
	{
		OpHex = GetOpSegment(31, 6, 5);//cmp
		OpHex |= GetOpSegment(Val2, 5, 20);
	}
	OpHex |= GetOpSegment(Val1, 5, 15);
	WhileCompareArray[WhileIndex] = OpHex;

	int BranchCond = BRANCH_IF_TRUE;
	if(Comparision > 2)
	{
		BranchCond = BRANCH_IF_FALSE;
		Comparision -= 3;
	}

	OpHex = GetOpSegment(16, 6, 5);
	OpHex |= GetOpSegment(BranchCond, 5, 10);
	OpHex |= GetOpSegment(Comparision, 5, 15);
	WhileConditionArray[WhileIndex] = OpHex;

	WhileIndex++;
}

void EndWhile()
{
	WhileIndex--;
	WriteIntToFile(WhileCompareArray[WhileIndex]);
	WhileConditionArray[WhileIndex] |= GetOpSegment(CalcBranchOffset(WPtr.tellp(), IfStartPos[IfIndex - 1] + 8), 14, 29);
	WriteIntToFile(WhileConditionArray[WhileIndex]);

	EndIf();
}

void SetRegister(int Register, int value)
{
	if ((value & 0xffff0000))
	{
		ADDIS(Register, 0, (value & 0xffff0000) >> 16);
		if (value & 0xffff)
		{
			ORI(Register, Register, (value & 0xffff));
		}
	}
	else
	{
		ADDI(Register, 0, (value & 0xffff));
		if (value & 0x8000)
		{
			ANDI(Register, Register, 0xFFFF);
		}
	}
}

void SetRegister(int Register, string value)
{
	int val = 0;
	for (int i = 0; i < value.size() && i < 4; i++)
	{
		val |= value[i] << ((3 - i) * 8);
	}
	SetRegister(Register, val);
}

void SetFloatingRegister(int FPReg, int TempReg1, int TempReg2, float Value)
{
	int *b = (int *)&Value;
	SetRegister(TempReg1, SET_FLOAT_REG_TEMP_MEM);

	SetRegister(TempReg2, *b);
	STW(TempReg2, TempReg1, 0);

	LFS(FPReg, TempReg1, 0);
}

void LoadWordToReg(int Register, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Register, 0, (Address & 0xFFFF0000) >> 16);
		LWZ(Register, Register, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Register, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LWZ(Register, Register, (Address & 0xFFFF));
	}
}

void LoadHalfToReg(int Register, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Register, 0, (Address & 0xFFFF0000) >> 16);
		LHZ(Register, Register, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Register, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LHZ(Register, Register, (Address & 0xFFFF));
	}
}

void LoadByteToReg(int Register, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Register, 0, (Address & 0xFFFF0000) >> 16);
		LBZ(Register, Register, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Register, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LBZ(Register, Register, (Address & 0xFFFF));
	}
}

//Reg takes on the value of address
void LoadWordToReg(int DestReg, int Reg, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Reg, 0, (Address & 0xFFFF0000) >> 16);
		LWZU(DestReg, Reg, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Reg, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LWZU(DestReg, Reg, (Address & 0xFFFF));
	}
}

//Reg takes on the value of address
void LoadHalfToReg(int DestReg, int Reg, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Reg, 0, (Address & 0xFFFF0000) >> 16);
		LHZU(DestReg, Reg, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Reg, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LHZU(DestReg, Reg, (Address & 0xFFFF));
	}
}

//Reg takes on the value of address
void LoadByteToReg(int DestReg, int Reg, int Address)
{
	if ((Address & 0xFFFF) < 0x8000)
	{
		ADDIS(Reg, 0, (Address & 0xFFFF0000) >> 16);
		LBZU(DestReg, Reg, (Address & 0xFFFF));
	}
	else
	{
		ADDIS(Reg, 0, ((Address & 0xFFFF0000) >> 16) + 1);
		LBZU(DestReg, Reg, (Address & 0xFFFF));
	}
}

//takes an integer value from SourceReg and converts it into a 32 bit float that is stored in ResultReg
//the values of SourceReg and TempReg are overwritten
void ConvertIntToFloat(int SourceReg, int TempReg, int ResultReg)
{
	SetRegister(ResultReg, 0);

	If(SourceReg, NOT_EQUAL_I, 0); //is not 0

	If(SourceReg, LESS_I, 0); //is less than 0

	SetRegister(ResultReg, 0x80000000);
	NEG(SourceReg, SourceReg);

	EndIf(); //is less than 0 end

	//set mantissa
	CNTLZW(TempReg, SourceReg);
	ADDI(TempReg, TempReg, -8);
	RLWNM(SourceReg, SourceReg, TempReg, 9, 31);
	OR(ResultReg, ResultReg, SourceReg);

	//set exponent
	ADDI(TempReg, TempReg, -150); //LZ - ((32 - 8) + 126) = -exponent
	NEG(TempReg, TempReg);
	RLWINM(TempReg, TempReg, 23, 1, 0);
	OR(ResultReg, ResultReg, TempReg);

	EndIf(); //is not 0 end
}

//writes insert asm gecko code
//keeps track of how many lines are written
//writes extra 00000000 or 60000000 00000000 at end as necessary
//assumes ba = 0x80000000
void ASMStart(int BranchAddress)
{
	int OpWord = (0xC2 << 24);
	if(BranchAddress >= 0x81000000)
	{
		OpWord += 0x01000000;
	}
	OpWord += BranchAddress & 0xFFFFFF;
	WriteIntToFile(OpWord);
	WriteIntToFile(0);
	ASMStartAddress = WPtr.tellp();
}

void ASMEnd()
{
	int HoldPos = WPtr.tellp();
	int numLines = HoldPos - ASMStartAddress;
	numLines /= 8;
	if(numLines % 2)
	{
		//odd number of ops
		WriteIntToFile(0);
		HoldPos += 8;
	}
	else
	{
		//even number of ops
		WriteIntToFile(0x60000000);
		WriteIntToFile(0);
		HoldPos += 16;
	}
	numLines /= 2;
	numLines++;
	WPtr.seekp(ASMStartAddress - 8);
	WriteIntToFile(numLines);
	WPtr.seekp(HoldPos);
}

void ASMEnd(int Replacement)
{
	WriteIntToFile(Replacement);
	ASMEnd();
}

void Label(int LabelNum)
{
	if(LabelNum >= MAX_LABELS)
	{
		cout << "ERROR, too many labels\n";
		exit(0);
	}
	LabelPosArray[LabelNum] = WPtr.tellg();
}

int GetNextLabel()
{
	LabelIndex++;
	return (LabelIndex - 1);
}

void JumpToLabel(int LabelNum)
{
	if(JumpIndex >= MAX_JUMPS)
	{
		cout << "ERROR, too many jumps\n";
		exit(0);
	}
	JumpLabelNumArray[JumpIndex] = LabelNum;
	JumpFromArray[JumpIndex] = WPtr.tellp();
	WriteIntToFile(0);
	JumpIndex++;
}

void CompleteJumps()
{
	int holdPos = WPtr.tellp();
	for(int i = 0; i < JumpIndex ; i++)
	{
		WPtr.seekp(JumpFromArray[i]);
		B(CalcBranchOffset(JumpFromArray[i], LabelPosArray[JumpLabelNumArray[i]]));
	}
	WPtr.seekp(holdPos);
}

int CalcBranchOffset(int Location, int Target)
{
	return (Target - Location) / 8;
}

//copies null-terminated string of bytes from address in Source to address in Destination
//Destination and Source regs are set to the addresses of their respective null bytes, Temp is zeroed
void StrCpy(int Destination, int Source, int Temp)
{
	LBZ(Temp, Source, 0);
	STB(Temp, Destination, 0);
	While(Temp, NOT_EQUAL_I, 0); //not ended

	LBZU(Temp, Source, 1);
	STBU(Temp, Destination, 1);

	EndWhile(); //not ended
}

void GeckoStringWrite(char *Buffer, u32 NumBytes, u32 Address)
{
	if (Address > 0x81000000)
	{
		SetGeckoBaseAddress(Address & 0xFE000000);
	}
	u32 Op = Address & 0x1FFFFFF;
	Op += 0x06000000;
	WriteIntToFile(Op);
	WriteIntToFile(NumBytes);
	int index = 0;
	u32 Word = 0;

	while (index < NumBytes)
	{
		if (Buffer[index] == '|')
		{
			Buffer[index] = 0;
		}
		Word += Buffer[index] << (8 * (3 - (index % 4)));
		index++;
		if (index % 4 == 0)
		{
			WriteIntToFile(Word);
			Word = 0;
		}
	}

	if (Address > 0x81000000)
	{
		SetGeckoBaseAddress(0x80000000);
	}
}

void SetGeckoBaseAddress(int Address)
{
	WriteIntToFile(0x42000000);
	WriteIntToFile(Address);
}

void SetGeckoPointerAddress(int Address)
{
	WriteIntToFile(0x4A000000);
	WriteIntToFile(Address);
}

void FindInArray(int ValueReg, int StartAddressReg, int numberOfElements, int elementOffset, int ResultReg, int TempReg)
{
	int EndOfSearch = GetNextLabel();

	SetRegister(ResultReg, 0);
	LBZ(TempReg, StartAddressReg, 0);
	While(ResultReg, LESS_I, numberOfElements); //search loop
	
	If(TempReg, EQUAL, ValueReg); //found target
	
	JumpToLabel(EndOfSearch);

	EndIf(); //found target

	LBZU(TempReg, StartAddressReg, elementOffset);
	ADDI(ResultReg, ResultReg, 1);

	EndWhile(); //search loop

	SetRegister(ResultReg, -1); //not found

	Label(EndOfSearch);
}

void CallBrawlFunc(int Address) {
	SetRegister(0, Address);
	MTCTR(0);
	BCTRL();
}

//r3 returns ptr to memory
void Allocate(int SizeReg)
{
	//SetRegister(3, 0x2A);
	SetRegister(3, 6); //network
	//SetRegister(3, 18); //fighter1 resource
	CallBrawlFunc(GET_MEM_ALLOCATOR); //call getMemAllocator
	//set size
	ADDI(4, SizeReg, 0);
	CallBrawlFunc(ALLOC); //call allocator
}

//allocates a buffer of size SizeReg and stores its address at AddressReg if
//value at addressReg equals EmptyVal
void AllocateIfNotExist(int SizeReg, int AddressReg, int EmptyVal)
{
	LHZ(3, AddressReg, 0);
	If(3, EQUAL_I_L, EmptyVal);
	Allocate(SizeReg);
	STW(3, AddressReg, 0);
	EndIf();
}

//r3 = Dest, r4 = Source, r5 = size
void Memmove(int DestReg, int SourceReg, int SizeReg)
{
	if (DestReg != 3) { ADDI(3, DestReg, 0); }
	if (SourceReg != 4) { ADDI(4, SourceReg, 0); }
	if (SizeReg != 5) { ADDI(5, SizeReg, 0); }
	CallBrawlFunc(MEMMOVE); //Memmove
}

void SaveRegisters()
{
	SaveRegisters({});
}

void SaveRegisters(vector<int> FPRegs)
{
	FPPushRecords = FPRegs;
	int stackSize = 29 * 4 + FPRegs.size() * 8 + 8 + 8;
	STW(0, 1, -4);
	MFLR(0);
	STW(0, 1, 4);
	MFCTR(0);
	STW(0, 1, -8);

	int offset = -8;
	for (int x : FPRegs) {
		offset -= 8;
		STFD(x, 1, offset);
	}

	STWU(1, 1, -stackSize);
	STMW(3, 1, 8);
}

void RestoreRegisters()
{
	int stackSize = 29 * 4 + FPPushRecords.size() * 8 + 8 + 8;

	LMW(3, 1, 8);
	ADDI(1, 1, stackSize);

	int offset = -8;
	for (int x : FPPushRecords) {
		offset -= 8;
		LFD(x, 1, offset);
	}
	
	LWZ(0, 1, -8);
	MTCTR(0);
	LWZ(0, 1, 4);
	MTLR(0);
	LWZ(0, 1, -4);
}

void SetRegs(int StartReg, vector<int> values)
{
	for (int x : values) {
		SetRegister(StartReg, x);
		StartReg++;
		if (StartReg > 31) {
			cout << "Too many values\n";
			exit(0);
		}
	}
}

void SetArgumentsFromRegs(int StartReg, vector<int> ValueRegs)
{
	for (int x : ValueRegs) {
		if (x != StartReg) {
			ADDI(StartReg, x, 0);
		}
		StartReg++;
		if (StartReg > 31) {
			cout << "Too many values\n";
			exit(0);
		}
	}
}

void GXSetCullMode(int CullModeReg)
{
	if (CullModeReg != 3) { ADDI(3, CullModeReg, 0); }
	CallBrawlFunc(GX_SET_CULL_MODE);
}

void GXClearVtxDesc()
{
	CallBrawlFunc(GX_CLEAR_VTX_DESC);
}

void GXSetVtxDesc(int AttributeReg, int TypeReg)
{
	if (AttributeReg != 3) { ADDI(3, AttributeReg, 0); }
	if (TypeReg != 4) { ADDI(4, TypeReg, 0); }
	CallBrawlFunc(GX_SET_VTX_DESC);
}

void GXSetVtxAttrFmt(int vtxfmtReg, int attrReg, int countReg, int typeReg, int fracReg)
{
	SetArgumentsFromRegs(3, { vtxfmtReg, attrReg, countReg, typeReg, fracReg });
	CallBrawlFunc(GX_SET_VTX_ATTRIBUTE_FORMAT);
}

//copies the camera mtx pointed to by r3 into memory pointed to by r4
//r4 is unchanged
void GetCameraMtx(int StorageLocReg)
{
	ADDI(3, 13, -0x4170); //get camera obj
	if (StorageLocReg != 4) { ADDI(4, StorageLocReg, 0); }
	CallBrawlFunc(GET_CAMERA_MTX);
}

void GXSetCurrentMtx(int IDReg)
{
	if (IDReg != 3) { ADDI(3, IDReg, 0); }
	CallBrawlFunc(GX_SET_CURRENT_MTX);
}

void GXLoadPosMtxImm(int MtxPtrReg, int IDReg)
{
	SetArgumentsFromRegs(3, { MtxPtrReg, IDReg });
	CallBrawlFunc(GX_LOAD_POS_MTX_IMM);
}

void GXSetZMode(int CompareEnabledReg, int FuncTypeReg, int UpdateEnabledReg)
{
	SetArgumentsFromRegs(3, { CompareEnabledReg, FuncTypeReg, UpdateEnabledReg });
	CallBrawlFunc(GX_SET_Z_MODE);
}

void GXSetLineWidth(int WidthReg, int TexOffsetReg)
{
	SetArgumentsFromRegs(3, { WidthReg, TexOffsetReg });
	CallBrawlFunc(GX_SET_LINE_WIDTH);
}

void GXBegin(int TypeReg, int VtxAttrFmtReg, int NumVertsReg)
{
	SetArgumentsFromRegs(3, { TypeReg, VtxAttrFmtReg, NumVertsReg });
	CallBrawlFunc(GX_BEGIN);
}

void GFDrawSetupCoord2D()
{
	CallBrawlFunc(GF_DRAW_SETUP_COORD_2D);
}

void GXDrawSetVtxColorPrimEnviroment()
{
	CallBrawlFunc(GX_DRAW_SET_VTX_COLOR_PRIM_ENVIROMENT);
}

void FreeMem(int AddressReg)
{
	if (AddressReg != 3) { ADDI(3, AddressReg, 0); }
	CallBrawlFunc(GF_POOL_FREE);
}

//EmptyVal is expected value of upper 16 bits if memory is not allocated
//stores value of EmptyVal into AddressReg
void FreeMemIfAllocd(int AddressReg, int EmptyVal)
{
	LHZ(3, AddressReg, 0);
	If(3, NOT_EQUAL_I_L, EmptyVal);
	LWZ(3, AddressReg, 0);
	FreeMem(3);
	SetRegister(3, EmptyVal);
	STW(3, AddressReg, 0);
	EndIf();
}

//calls free on a null teminated array of ptrs to allocd memory
void FreeAllocdArray(int AllocAddressReg)
{
	LWZU(3, AllocAddressReg, 4);
	While(3, NOT_EQUAL_I, 0);

	FreeMem(3);
	LWZU(3, AllocAddressReg, 4);

	EndWhile();
}

//location is where to save from, size is how much, saveTo is where to store the alloc ptr - 4
void SaveMem(int LocationReg, int SizeReg, int SaveToReg)
{
	ADDI(5, SizeReg, 8);
	Allocate(5);
	STWU(3, SaveToReg, 4);
	STW(LocationReg, 3, 0);
	STW(SizeReg, 3, 4);
	ADDI(3, 3, 8);
	Memmove(3, LocationReg, SizeReg);
}

void Increment(int Reg)
{
	ADDI(Reg, Reg, 1);
}

void Decrement(int Reg)
{
	ADDI(Reg, Reg, -1);
}

void WriteStringToMem(string Text, int AddressReg)
{
	ADDI(4, AddressReg, -4);
	for (int i = 0; i < Text.size() - 1; i += 4) {
		SetRegister(3, Text.substr(i, min(4, (int) Text.size() - i)));
		STWU(3, 4, 4);
	}
}

void ADD(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg1, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(266, 9, 30);
	WriteIntToFile(OpHex);
}

void ADDI(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(14, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void ADDIS(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(15, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void AND(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(28, 10, 30);
	WriteIntToFile(OpHex);
}

void ANDC(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(60, 10, 30);
	WriteIntToFile(OpHex);
}

void ANDI(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(28, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void ANDIS(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(29, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void B(int JumpDist)
{
	OpHex = GetOpSegment(18, 6, 5);
	OpHex |= GetOpSegment(JumpDist, 24, 29);
	WriteIntToFile(OpHex);
}

void BA(int Address)
{
	Address /= 4;
	OpHex = GetOpSegment(18, 6, 5);
	OpHex |= GetOpSegment(Address, 24, 29);
	OpHex |= 2;
	WriteIntToFile(OpHex);
}

//distance/4, branch if true/false, bit to check
void BC(int JumpDist, int BranchCondition, int ConditionBit)
{
	OpHex = GetOpSegment(16, 6, 5);
	OpHex |= GetOpSegment(BranchCondition, 5, 10);
	OpHex |= GetOpSegment(ConditionBit, 5, 15);
	OpHex |= GetOpSegment(JumpDist, 14, 29);
	WriteIntToFile(OpHex);
}

void BCTRL()
{
	WriteIntToFile(0x4e800421);
}

void BL(int JumpDist)
{
	OpHex = GetOpSegment(18, 6, 5);
	OpHex |= GetOpSegment(JumpDist, 24, 29);
	OpHex |= 1;
	WriteIntToFile(OpHex);
}

void BLA(int Address)
{
	Address /= 4;
	OpHex = GetOpSegment(18, 6, 5);
	OpHex |= GetOpSegment(Address, 24, 29);
	OpHex |= 3;
	WriteIntToFile(OpHex);
}

void BLR()
{
	WriteIntToFile(0x4e800020);
}

//might be broken for some reason
void CMP(int Reg1, int Reg2, int CondField)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(CondField, 3, 8);
	OpHex |= GetOpSegment(Reg1, 5, 15);
	OpHex |= GetOpSegment(Reg2, 5, 20);
	WriteIntToFile(OpHex);
}

void CMPI(int Reg, int Immediate, int CondField)
{
	OpHex = GetOpSegment(11, 6, 5);
	OpHex |= GetOpSegment(CondField, 3, 8);
	OpHex |= GetOpSegment(Reg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void CMPL(int Reg1, int Reg2, int CondField)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(CondField, 3, 8);
	OpHex |= GetOpSegment(Reg1, 5, 15);
	OpHex |= GetOpSegment(Reg2, 5, 20);
	OpHex |= GetOpSegment(32, 10, 30);
	WriteIntToFile(OpHex);
}

void CMPLI(int Reg, int Immediate, int CondField)
{
	OpHex = GetOpSegment(10, 6, 5);
	OpHex |= GetOpSegment(CondField, 3, 8);
	OpHex |= GetOpSegment(Reg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void CNTLZW(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(26, 10, 30);
	WriteIntToFile(OpHex);
}

void DIVW(int DestReg, int DividendReg, int DivisorReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(DividendReg, 5, 15);
	OpHex |= GetOpSegment(DivisorReg, 5, 20);
	OpHex |= GetOpSegment(491, 9, 30);
	WriteIntToFile(OpHex);
}

void DIVWU(int DestReg, int DividendReg, int DivisorReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(DividendReg, 5, 15);
	OpHex |= GetOpSegment(DivisorReg, 5, 20);
	OpHex |= GetOpSegment(459, 9, 30);
	WriteIntToFile(OpHex);
}

void EQV(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(284, 10, 30);
	WriteIntToFile(OpHex);
}

void EXTSB(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(954, 10, 30);
	WriteIntToFile(OpHex);
}

void FADD(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg1, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(21, 5, 30);
	WriteIntToFile(OpHex);
}

void FADDS(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(59, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg1, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(21, 9, 30);
	WriteIntToFile(OpHex);
}

void FCMPU(int FPReg1, int FPReg2, int CondField)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(CondField, 3, 8);
	OpHex |= GetOpSegment(FPReg1, 5, 15);
	OpHex |= GetOpSegment(FPReg2, 5, 20);
	WriteIntToFile(OpHex);
}

void FCTIW(int SourceReg, int DestReg)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 20);
	OpHex |= GetOpSegment(14, 10, 30);
	WriteIntToFile(OpHex);
}

void FDIV(int FPDestReg, int FPSourceReg1, int FPSourceReg2)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(FPDestReg, 5, 10);
	OpHex |= GetOpSegment(FPSourceReg1, 5, 15);
	OpHex |= GetOpSegment(FPSourceReg2, 5, 20);
	OpHex |= GetOpSegment(18, 5, 30);
	WriteIntToFile(OpHex);
}

void FMR(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 20);
	OpHex |= GetOpSegment(72, 10, 30);
	WriteIntToFile(OpHex);
}

void FMUL(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg1, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 25);
	OpHex |= GetOpSegment(25, 5, 30);
	WriteIntToFile(OpHex);
}

void FNEG(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 20);
	OpHex |= GetOpSegment(40, 10, 30);
	WriteIntToFile(OpHex);
}

//Rounds a 64-bit, double precision floating-point operand to single precision
//places result in a floating-point register
void FRSP(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(63, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 20);
	OpHex |= GetOpSegment(12, 10, 30);
	WriteIntToFile(OpHex);
}

void FSUB(int FPDestReg, int FPSourceReg1, int FPSourceReg2)
{
	OpHex = GetOpSegment(59, 6, 5);
	OpHex |= GetOpSegment(FPDestReg, 5, 10);
	OpHex |= GetOpSegment(FPSourceReg1, 5, 15);
	OpHex |= GetOpSegment(FPSourceReg2, 5, 20);
	OpHex |= GetOpSegment(20, 5, 30);
	WriteIntToFile(OpHex);
}

void LBZ(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(34, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LBZU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(35, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LBZUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(119, 10, 30);
	WriteIntToFile(OpHex);
}

void LBZX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(87, 10, 30);
	WriteIntToFile(OpHex);
}

void LFD(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(50, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LFS(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(48, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LHZ(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(40, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LHZU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(41, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LHZUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(331, 10, 30);
	WriteIntToFile(OpHex);
}

void LHZX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(279, 10, 30);
	WriteIntToFile(OpHex);
}

void LWZ(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(32, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LWZU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(33, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void LWZUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(55, 10, 30);
	WriteIntToFile(OpHex);
}

void LWZX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(23, 10, 30);
	WriteIntToFile(OpHex);
}

void LMW(int StartReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(46, 6, 5);
	OpHex |= GetOpSegment(StartReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void MFCTR(int TargetReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(TargetReg, 5, 10);
	OpHex |= GetOpSegment(9 << 5, 10, 20); //spr
	OpHex |= GetOpSegment(339, 10, 30);
	WriteIntToFile(OpHex);
}

void MFLR(int TargetReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(TargetReg, 5, 10);
	OpHex |= GetOpSegment(8 << 5, 10, 20); //spr
	OpHex |= GetOpSegment(339, 10, 30);
	WriteIntToFile(OpHex);
}

void MTCTR(int TargetReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(TargetReg, 5, 10);
	OpHex |= GetOpSegment(9 << 5, 10, 20); //spr
	OpHex |= GetOpSegment(467, 10, 30);
	WriteIntToFile(OpHex);
}

void MTLR(int TargetReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(TargetReg, 5, 10);
	OpHex |= GetOpSegment(8 << 5, 10, 20); //spr
	OpHex |= GetOpSegment(467, 10, 30);
	WriteIntToFile(OpHex);
}

void MULLI(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(7, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void NEG(int DestReg, int SourceReg)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg, 5, 15);
	OpHex |= GetOpSegment(104, 9, 30);
	WriteIntToFile(OpHex);
}

void NOP()
{
	ORI(0, 0, 0);
}

void NOR(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(124, 10, 30);
	WriteIntToFile(OpHex);
}

void OR(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(444, 10, 30);
	WriteIntToFile(OpHex);
}

void ORC(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(412, 10, 30);
	WriteIntToFile(OpHex);
}

void ORI(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(24, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void ORIS(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(25, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void RLWINM(int DestReg, int SourceReg, int ShiftNum, int MaskStart, int MaskEnd)
{
	OpHex = GetOpSegment(21, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(ShiftNum, 5, 20);
	OpHex |= GetOpSegment(MaskStart, 5, 25);
	OpHex |= GetOpSegment(MaskEnd, 5, 30);
	WriteIntToFile(OpHex);
}

void RLWNM(int DestReg, int SourceReg, int ShiftReg, int MaskStart, int MaskEnd)
{
	OpHex = GetOpSegment(23, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(ShiftReg, 5, 20);
	OpHex |= GetOpSegment(MaskStart, 5, 25);
	OpHex |= GetOpSegment(MaskEnd, 5, 30);
	WriteIntToFile(OpHex);
}

void STB(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(38, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STBU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(39, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STBUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(247, 10, 30);
	WriteIntToFile(OpHex);
}

void STBX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(215, 10, 30);
	WriteIntToFile(OpHex);
}

void STFD(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(54, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STFS(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(52, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STFSU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(53, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STH(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(44, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STHU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(45, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STHUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(439, 10, 30);
	WriteIntToFile(OpHex);
}

void STHX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(407, 10, 30);
	WriteIntToFile(OpHex);
}

void STMW(int StartReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(47, 6, 5);
	OpHex |= GetOpSegment(StartReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STW(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(36, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STWU(int DestReg, int AddressReg, int Immediate)
{
	OpHex = GetOpSegment(37, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void STWUX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(183, 10, 30);
	WriteIntToFile(OpHex);
}

void STWX(int DestReg, int AddressReg1, int AddressReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(AddressReg1, 5, 15);
	OpHex |= GetOpSegment(AddressReg2, 5, 20);
	OpHex |= GetOpSegment(151, 10, 30);
	WriteIntToFile(OpHex);
}

//DestReg = SourceReg1 - SourceReg2
void SUBF(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(DestReg, 5, 10);
	OpHex |= GetOpSegment(SourceReg2, 5, 15);
	OpHex |= GetOpSegment(SourceReg1, 5, 20);
	OpHex |= GetOpSegment(40, 9, 30);
	WriteIntToFile(OpHex);
}

void XOR(int DestReg, int SourceReg1, int SourceReg2)
{
	OpHex = GetOpSegment(31, 6, 5);
	OpHex |= GetOpSegment(SourceReg1, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(SourceReg2, 5, 20);
	OpHex |= GetOpSegment(316, 10, 30);
	WriteIntToFile(OpHex);
}

void XORI(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(26, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}

void XORIS(int DestReg, int SourceReg, int Immediate)
{
	OpHex = GetOpSegment(27, 6, 5);
	OpHex |= GetOpSegment(SourceReg, 5, 10);
	OpHex |= GetOpSegment(DestReg, 5, 15);
	OpHex |= GetOpSegment(Immediate, 16, 31);
	WriteIntToFile(OpHex);
}