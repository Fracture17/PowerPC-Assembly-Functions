#include "stdafx.h"
#include "Save States.h"

map<string, Module> ModuleList;

void InitializeModules()
{
	ModuleList.emplace("Resource", Module(true, 0x812d34bc, 0x812d36c8));
	ModuleList.emplace("Model", Module(true, 0x812d36c8, 0x812d39b8));
	ModuleList.emplace("Motion", Module(false, 0x812d39b8, 0x812d3b64));
	ModuleList.emplace("Posture", Module(true, 0x812d3b64, 0x812d3c2c));
	ModuleList.emplace("Ground", Module(true, 0x812d3c2c, 0x812d3c84));
	ModuleList.emplace("Situation", Module(true, 0x812d3c84, 0x812d3cf0));
	ModuleList.emplace("CollisionAttack", Module(true, 0x812d4350, 0x812d4ce4));
	ModuleList.emplace("Status", Module(true, 0x812d7c98, 0x812d7d48));
	ModuleList.emplace("Kinetic", Module(true, 0x812d7d48, 0x812d7d48 + 0x500));
	ModuleList.emplace("Variables", Module(true, 0x901AE000, 0x901AE2C8));
}

void temp1()
{
	InitializeModules();

	//r9 has released inputs

	ASMStart(0x8004ac24);

	int Reg1 = 14;
	int Reg2 = 15;
	int Reg3 = 16;
	int Reg4 = 17;
	int Reg5 = 18;
	int Reg6 = 19;

	ANDI(Reg2, 9, 0xF);
	If(Reg2, GREATER_I, 0); //DPad pressed

	SaveRegs({ 3, 4 });
	SetRegister(Reg1, TEMP_SAVE_STATE_PTR_LIST_LOC);

	If(Reg2, EQUAL_I, 8); //DUp
	//load state
	int i = 0;
	for (auto x : ModuleList)
	{
		auto module = x.second;
		if (module.used)
		{
			LWZ(4, Reg1, i);
			If(4, NOT_EQUAL_I, 0); //saved
			SetRegister(3, module.start);
			Memmove(module.size);
			EndIf(); //saved
			i += 4;
		}
	}

	//load action state
	LWZ(4, Reg1, i);

	//change action
	SetRegister(3, 0x812d7c98);
	Push(5);
	SetRegister(5, 0x812d2d24);
	CallBrawlFunc(0x8077f9c4, true);
	Pop();

	//set frame
	/*LWZ(Reg3, Reg1, i + 8);
	SetRegister(Reg4, 0x812d39f8);
	STW(Reg3, Reg4, 0);*/

	EndIf(); //DUp

	If(Reg2, EQUAL_I, 4); //DDown

	//save modules
	i = 0;
	for (auto x : ModuleList)
	{
		auto module = x.second;
		if (module.used)
		{
			Allocate(module.size);
			STW(3, Reg1, i);
			SetRegister(4, module.start);
			Memmove(module.size);
			i += 4;
		}
	}

	//save action state
	LoadWordToReg(Reg3, 0x812d7ccc);
	STW(Reg3, Reg1, i);

	//save subaction
	LoadWordToReg(Reg3, 0x812d3a10);
	STW(Reg3, Reg1, i + 4);

	//save frame
	LoadWordToReg(Reg3, 0x812d39f8);
	STW(Reg3, Reg1, i + 8);

	EndIf(); //DDown

	RestoreRegs(2);

	EndIf(); //DPad pressed

	ASMEnd(0x4e800020); //blr
}