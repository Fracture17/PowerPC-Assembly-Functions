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
	ModuleList.emplace("Animation", Module(true, 0x812d6d8c, 0x812d7c98));
	ModuleList.emplace("Status", Module(true, 0x812d7c98, 0x812d7d48));
	ModuleList.emplace("Kinetic", Module(true, 0x812d7d48, 0x812d7d48 + 0x500));
	ModuleList.emplace("Variables", Module(true, 0x901AE000, 0x901AE2C8));
}

#if 0
void SaveStateCodes()
{
	RecordSavedLocations();

	RemoveFreedLocation();

	SaveOrRestoreState();

	TriggerAllocSaves();
}

void TriggerAllocSaves()
{
	ASMStart(0x8082f35c); //use r7, r6
	SetRegister(7, PTR_LIST_LOC - 4);
	SetRegister(6, 1);
	STW(6, 7, 0);
	ASMEnd(0x80df000c); //lwz r6, r31, 0xC

	ASMStart(0x8082f370); //use r3, r4
	SetRegister(3, PTR_LIST_LOC - 4);
	SetRegister(4, 0);
	STW(4, 3, 0);
	ASMEnd(0x7fc3f378); //mr r3, r30

	ASMStart(0x80026440); //use r31, r3
	SetRegister(31, PTR_LIST_LOC - 4);
	SetRegister(3, 1);
	STW(3, 31, 0);
	ASMEnd(0x8bfe0010); //lbz r3, r31, 0x10

	ASMStart(0x80026454); //use r31, r30
	SetRegister(31, PTR_LIST_LOC - 4);
	SetRegister(30, 0);
	STW(30, 31, 0);
	ASMEnd(0x83e1000c); //lwz r31, sp, 0xC
}

//records the addresses of all allocated memory for characters
void RecordSavedLocations()
{
	//r3 contains address
	//ASMStart(0x80026450);
	ASMStart(0x80025ed8);
	SaveSpecialRegs();
	SaveAllRegs();

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;

	LoadWordToReg(Reg1, IS_REPLAY_LOC);
	If(Reg1, NOT_EQUAL_I, 0); //is in game
	LoadWordToReg(Reg1, PTR_LIST_LOC - 4);
	If(Reg1, EQUAL_I, 1); //should save

	SetRegister(Reg2, 0x80002804);
	LWZ(Reg1, Reg2, 0); //contains end of ptr list
	If(Reg1, EQUAL_I, 0); //empty list
	ADDI(Reg1, Reg2, 0);
	EndIf(); //empty list
	STWU(3, Reg1, 4);
	STW(Reg1, Reg2, 0);

	EndIf(); //should save
	EndIf(); //is in game
	
	RestoreRegs(29);
	RestoreSpecialRegs();
	ASMEnd(0x4e800020); //blr
	//ASMEnd(0x9bfe0010); //stb r31, r30, 0x10
}

void RemoveFreedLocation()
{
	ASMStart(0x80025f2c);
	SaveSpecialRegs();
	SaveAllRegs();

	int Reg1 = 31;
	int Reg2 = 30;
	int Reg3 = 29;
	int Reg4 = 28;


	SetRegister(Reg1, 0x80002804);
	LWZ(Reg2, Reg1, 0); //end of list
	SetRegister(Reg4, 1);
	//I think if the list is empty it will still work, because 0 is less (logically) than the address
	While(Reg1, LESS_L, Reg2); //check for match

	LWZU(Reg3, Reg1, 4);
	If(Reg4, EQUAL_I, 0); //shift back
	STW(Reg3, Reg1, -4);
	EndIf(); //shift back

	If(Reg3, EQUAL, 3); //match
	SetRegister(Reg4, 0);
	EndIf(); //match

	EndWhile(); //check for match

	If(Reg4, EQUAL_I, 0); //match was found
	STW(Reg4, Reg1, 0);
	ADDI(Reg1, Reg1, -4);
	SetRegister(Reg2, 0x80002804);
	STW(Reg1, Reg2, 0);
	EndIf(); //match was found

	RestoreRegs(29);
	RestoreSpecialRegs();
	ASMEnd(0x9421ffe0); //stwu sp, sp, -0x20
}

void SaveOrRestoreState()
{
	//r9 has released inputs

	ASMStart(0x8004ac24);

	int Reg1 = 14;
	int Reg2 = 15;
	int Reg3 = 16;
	int Reg4 = 17;
	int Reg5 = 18;
	int Reg6 = 19;

	ANDI(Reg6, 9, 0xF);
	If(Reg6, GREATER_I, 0); //DPad pressed

	SaveSpecialRegs();
	SaveAllRegs();

	SetRegister(Reg3, 0x80002800);

	If(Reg6, EQUAL_I, 8); //DUp

	LWZ(Reg3, Reg3, 0);
	RestoreCharacterState(Reg3);

	EndIf(); //DUp

	If(Reg6, EQUAL_I, 4); //DDown

	/*SetRegister(Reg1, 0x80002804);
	SetRegister(Reg2, 1);
	STW(Reg2, Reg1, -8); //stop allocations from being saved
	*/
	SetRegister(Reg2, 0x1000);
	Allocate(Reg2);
	STW(3, Reg3, 0);
	ADDI(Reg4, 3, 0);

	LWZU(Reg2, Reg1, 4);
	While(Reg2, NOT_EQUAL_I, 0); //save loop

	ADDI(Reg2, Reg2, -0x20); //get start
	LWZ(Reg3, Reg2, 4); //get size
	SaveMem(Reg2, Reg3, Reg4);
	ADDI(Reg4, Reg4, 4);
	LWZU(Reg2, Reg1, 4);

	EndWhile(); //save loop

	ADDI(Reg3, Reg4, 0);

	//save variables
	SetRegister(Reg1, 0x901AE000);
	SetRegister(Reg2, 0x2C8);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save system thing
	SetRegister(Reg1, 0x8062fb40);
	SetRegister(Reg2, 0x806312ac - 0x8062fb40);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save FAT index
	SetRegister(Reg1, 0x80b879b4);
	SetRegister(Reg2, 4);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save other modules
	SetRegister(Reg1, 0x80b8516c);
	SetRegister(Reg2, 0x80b8545c - 0x80b8516c);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save subaction table thing
	SetRegister(Reg1, 0x806312f0); //start of this is very close to end of system thing
	SetRegister(Reg2, 0x8063ce70 - 0x806312f0);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save animation object ptr things
	SetRegister(Reg1, 0x91c0ac84);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save instance (new)
	SetRegister(Reg1, 0x8128cb60);
	SetRegister(Reg2, 0x94);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//Reg5 is the head of the array
	//for P2 Ike its 0x812d397c
	/*SetRegister(Reg5, 0x812d397c);
	LWZ(Reg4, Reg5, 0x8); //get length
	RLWINM(Reg4, Reg4, 12, 19, 31);
	ADDI(Reg5, Reg5, 4);
	While(Reg4, GREATER_I, 0); //save loop

	LWZU(Reg1, Reg5, 8);
	ADDI(Reg1, Reg1, -0x20); //get start
	LWZ(Reg2, Reg1, 4); //get size
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	
	ADDI(Reg4, Reg4, -1);
	EndWhile(); //save loop
	*/
	/*SetRegister(Reg1, 0x812d1860);
	SetRegister(Reg2, 0x812deb60 - 0x812d1860);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	*/
	//p4 snake

	//save instance
	/*SetRegister(Reg1, 0x81330b60);
	SetRegister(Reg2, 0x52000);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	*/
	//save animation object ptr things
	SetRegister(Reg1, 0x926e4304);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//variables
	SetRegister(Reg1, 0x901ae870);
	SetRegister(Reg2, 0x2C8);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);


	
	SetRegister(Reg1, 0);
	STW(Reg1, Reg3, 0); //set last word to 0
	/*SetRegister(Reg2, 0x80002800 - 4);
	STW(Reg1, Reg2, 0); //let allocations be saved again
	*/
	EndIf(); //DDown

	RestoreRegs(29);
	RestoreSpecialRegs();

	EndIf(); //DPad pressed

	ASMEnd(0x4e800020); //blr
}
#endif



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

	ANDI(Reg6, 9, 0xF);
	If(Reg6, GREATER_I, 0); //DPad pressed

	SaveRegisters();

	SetRegister(Reg3, 0x80002800);

	If(Reg6, EQUAL_I, 8); //DUp

	/*LoadWordToReg(4, 0x80002800 - 4);
	RestoreMem(4);

	SetRegister(Reg3, 0x80002800 - 4);
	SetRegister(Reg1, 0x812d0860);
	SetRegister(Reg2, 0x1000);
	SaveMem(Reg1, Reg2, Reg3);


	SetRegister(Reg3, 0x80002800);*/

	RestoreState(Reg3);

	

	/*SetRegister(Reg1, 0x812d0860 - 1);
	LoadWordToReg(Reg2, 0x80002800 - 4);
	ADDI(Reg2, Reg2, 7);
	SetRegister(Reg3, 0);
	While(Reg3, LESS_I, 0x1000); //check loop
	LBZU(3, Reg1, 1);
	LBZU(4, Reg2, 1);
	If(3, NOT_EQUAL, 4); //wrong
	NOP(); NOP(); NOP(); NOP(); NOP();
	EndIf(); //wrong
	ADDI(Reg3, Reg3, 1);
	EndWhile(); //check loop
	*/

	EndIf(); //DUp

	If(Reg6, EQUAL_I, 4); //DDown

	//get animobject ptr
	//get size of array
	//add size to constant number of saves + 1 to get total number of saves
	//allocate enough memory for all saves, and store address
	//store 0 at end
	//store data from objects
	//store static data

	/*SetRegister(Reg3, 0x80002800 - 4);
	SetRegister(Reg1, 0x8128cb60);
	SetRegister(Reg2, 0x52000);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	*/

	//copy modules from 0x91c0ac84 to 0x91c0ac98
	//check for 0
	SetRegister(Reg5, 0x91c0ac84 - 4);
	SetRegister(Reg4, 5);
	While(Reg4, GREATER_I, 0); //save loop

	LWZU(Reg1, Reg5, 4);
	If(Reg1, NOT_EQUAL_I, 0);
	ADDI(Reg1, Reg1, -0x20); //get start
	LWZ(Reg2, Reg1, 4); //get size
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	EndIf();

	ADDI(Reg4, Reg4, -1);
	EndWhile(); //save loop

	LoadWordToReg(Reg1, 0x812d39f4);
	ADDI(Reg1, Reg1, -0x20); //location
	LWZ(Reg2, Reg1, 4); //size
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//stage
	/*SetRegister(Reg1, 0x814ce460);
	SetRegister(Reg2, 0x80100);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);*/

	//save variables
	SetRegister(Reg1, 0x901AE000);
	SetRegister(Reg2, 0x2C8);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save system thing
	SetRegister(Reg1, 0x8062fb40);
	SetRegister(Reg2, 0x806312ac - 0x8062fb40);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save FAT index
	SetRegister(Reg1, 0x80b879b4);
	SetRegister(Reg2, 4);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save other modules
	SetRegister(Reg1, 0x80b8516c);
	SetRegister(Reg2, 0x80b8545c - 0x80b8516c);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save subaction table thing
	SetRegister(Reg1, 0x806312f0); //start of this is very close to end of system thing
	SetRegister(Reg2, 0x8063ce70 - 0x806312f0);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save animation object ptr things
	SetRegister(Reg1, 0x91c0ac84);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save instance (new)
	SetRegister(Reg1, 0x8128cb60);
	SetRegister(Reg2, 0x94);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	
	//Reg5 is the head of the array
	//for P2 Ike its 0x812d397c
	SetRegister(Reg5, 0x812d397c);
	LWZ(Reg4, Reg5, 0x8); //get length
	RLWINM(Reg4, Reg4, 12, 19, 31);
	ADDI(Reg5, Reg5, 4);
	While(Reg4, GREATER_I, 0); //save loop

	LWZU(Reg1, Reg5, 8);
	ADDI(Reg1, Reg1, -0x20); //get start
	LWZ(Reg2, Reg1, 4); //get size
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);
	
	ADDI(Reg4, Reg4, -1);
	EndWhile(); //save loop
	
	SetRegister(Reg1, 0x812d1860);
	SetRegister(Reg2, 0x812deb60 - 0x812d1860);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);


	//save whole thing
	SetRegister(Reg1, 0x8128cb60);
	SetRegister(Reg2, 0x52000);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//p4 snake

	//save instance
	SetRegister(Reg1, 0x81330b60);
	SetRegister(Reg2, 0x52000);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//save animation object ptr things
	SetRegister(Reg1, 0x926e4304);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);

	//variables
	SetRegister(Reg1, 0x901ae870);
	SetRegister(Reg2, 0x2C8);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg3, Reg3, 4);


	
	SetRegister(Reg1, 0);
	STW(Reg1, Reg3, 0); //set last word to 0

	EndIf(); //DDown

	RestoreRegisters();

	EndIf(); //DPad pressed

	ASMEnd(0x4e800020); //blr
}

void SaveOrRestoreState()
{
	ASMStart(0x8004ac24);

	int Reg1 = 14;
	int Reg2 = 15;
	int Reg3 = 16;
	int Reg4 = 17;
	int Reg5 = 18;
	int Reg6 = 19;
	//not ganenteed safe until saved
	int Reg7 = 20;
	int Reg8 = 21;
	int Reg9 = 22;
	int Reg10 = 23;

	SaveRegisters();

	SetRegister(Reg3, PTR_LIST_LOC);
	LoadWordToReg(Reg6, IS_REPLAY_LOC);
	If(Reg6, NOT_EQUAL_I, 0); //basic check if is in match

	ANDI(Reg6, 9, 0xF);
	If(Reg6, GREATER_I, 0); //DPad pressed

	If(Reg6, EQUAL_I, 8); //DUp

	LWZ(Reg4, Reg3, 4);
	If(Reg4, EQUAL_I, 0xFF); //state to restore exists
	LWZ(Reg3, Reg3, 0);
	RestoreState(Reg3);
	EndIf(); //state to restore exists

	EndIf(); //DUp

	If(Reg6, EQUAL_I, 4); //DDown

	//to save animation ptr things:
	//Get module ptr
	//module ptr + 4 -> + 0xC -> + 0x124 is start location

	//to save hitbox info:
	//Get module ptr
	//module ptr + 0x1C -> + 0x64 -> is start
	//start + 0x8 (rotate left 12 and take last 12 bits for size)
	//start + 0x2C + (index * 0x78) -> is start of save location
	//start + 0x30 + (index * 0x78) -> (half) * 0x40 is size of save
	//check if location != 0 before saving

	LWZ(Reg1, Reg3, 4);
	If(Reg1, EQUAL_I, 0xFF); //state already exists
	LWZ(Reg1, Reg3, 0);
	FreeAllocdArray(Reg1);
	LWZ(Reg1, Reg3, 0);
	FreeMem(Reg1);
	EndIf(); //state already exists

	//create ptr list
	SetRegister(Reg1, 0x4000);
	Allocate(Reg1);
	STW(3, Reg3, 0);
	ADDI(Reg3, 3, 0);

	//save fighter instances
	SetRegister(Reg1, 0x8128cb60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);
	SetRegister(Reg1, 0x8128cb60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);
	SetRegister(Reg1, 0x812deb60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);
	SetRegister(Reg1, 0x81330b60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);

	//save variables
	SetRegister(Reg2, 0x2C8);
	SetRegister(Reg1, 0x901AE000);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg1, Reg1, 0x870);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg1, Reg1, 0x870);
	SaveMem(Reg1, Reg2, Reg3);
	ADDI(Reg1, Reg1, 0x870);
	SaveMem(Reg1, Reg2, Reg3);

	//path to module ptr
	LoadWordToReg(Reg1, 0x80629a00 + 0x154);
	LWZ(Reg1, Reg1, 0);

	SetRegister(Reg2, 0);
	While(Reg2, LESS_I, 4); //character loop

	LWZ(Reg4, Reg1, 0x4);
	If(Reg4, NOT_EQUAL_I, -1); //character exists

	LWZ(Reg4, Reg1, 0x34);
	LWZ(Reg4, Reg4, 0x60);
	LWZ(Reg4, Reg4, 0xD8); //module list start

	//to save animation ptr things:
	//Get module ptr
	//module ptr + 4 -> + 0xC -> + 0x124 is start location
	LWZ(Reg5, Reg4, 4);
	LWZ(Reg5, Reg5, 0xC);
	ADDI(Reg5, Reg5, 0x124);
	SetRegister(Reg6, 0x14);
	SaveMem(Reg5, Reg6, Reg3);

	//to save hitbox info:
	//Get module ptr
	//module ptr + 0x1C -> + 0x64 -> is start
	//start + 0x8 (rotate left 12 and take last 12 bits for size)
	//start + 0x2C + (index * 0x78) -> is start of save location
	//start + 0x30 + (index * 0x78) -> (half) * 0x40 is size of save
	//check if location != 0 before saving
	LWZ(Reg5, Reg4, 0x1C);
	LWZ(Reg5, Reg5, 0x64);
	LWZ(Reg6, Reg5, 0x8);
	RLWINM(Reg6, Reg6, 12, 20, 31); //list size
	While(Reg6, GREATER_I, 0); //save loop

	ADDI(Reg7, Reg5, 0xC);
	SetRegister(Reg8, 0x28);
	SaveList(Reg7, Reg8, Reg3, Reg9, Reg10);

	ADDI(Reg7, Reg5, 0x28);
	SetRegister(Reg8, 0x40);
	SaveList(Reg7, Reg8, Reg3, Reg9, Reg10);

	ADDI(Reg7, Reg5, 0x34);
	SetRegister(Reg8, 0x2C);
	SaveList(Reg7, Reg8, Reg3, Reg9, Reg10);

	/*LWZ(Reg7, Reg5, 0x2C); //start
	If(Reg7, NOT_EQUAL_I, 0); //memory is reserved

	LHZ(Reg8, Reg5, 0x30);
	MULLI(Reg8, Reg8, 0x40); //save size
	SaveMem(Reg7, Reg8, Reg3);

	EndIf(); //memory is reserved
	*/
	ADDI(Reg5, Reg5, 0x78);
	ADDI(Reg6, Reg6, -1);
	EndWhile(); //save loop

	EndIf(); //character exists
	
	ADDI(Reg1, Reg1, 0x244);
	ADDI(Reg2, Reg2, 1);
	EndWhile(); //character loop

	//save info instance
	/*SetRegister(Reg1, 0x81601960);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);*/

	//save system
	/*SetRegister(Reg1, 0x80611f60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);*/

	//save system thing
	SetRegister(Reg1, 0x8062fb40);
	SetRegister(Reg2, 0x806312ac - 0x8062fb40);
	SaveMem(Reg1, Reg2, Reg3);

	//save FAT index
	SetRegister(Reg1, 0x80b879b4);
	SetRegister(Reg2, 4);
	SaveMem(Reg1, Reg2, Reg3);

	//save other modules
	SetRegister(Reg1, 0x80b8516c);
	SetRegister(Reg2, 0x80b8545c - 0x80b8516c);
	SaveMem(Reg1, Reg2, Reg3);

	//save subaction table thing
	SetRegister(Reg1, 0x806312f0); //start of this is very close to end of system thing
	SetRegister(Reg2, 0x8063ce70 - 0x806312f0);
	SaveMem(Reg1, Reg2, Reg3);

	//trace through module to get hitboxes

	//save animation object ptr things
	/*SetRegister(Reg1, 0x91c0ac84);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);*/

	//save instance (new)
	/*SetRegister(Reg1, 0x8128cb60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);*/

	//p4 snake

	//save instance
	/*SetRegister(Reg1, 0x81330b60);
	SaveInstance(Reg1, Reg3, Reg4, Reg5);*/

	//save animation object ptr things
	/*SetRegister(Reg1, 0x926e4304);
	SetRegister(Reg2, 0x14);
	SaveMem(Reg1, Reg2, Reg3);*/

	
	SetRegister(Reg1, 0);
	STW(Reg1, Reg3, 4); //set last word to 0

	SetRegister(Reg3, PTR_LIST_LOC);
	SetRegister(Reg1, 0xFF);
	STW(Reg1, Reg3, 4); //set state saved flag

	EndIf(); //DDown

	EndIf(); //DPad pressed

	Else(); //basic check if is in match

	LWZ(Reg1, Reg3, 4);
	If(Reg1, EQUAL_I, 0xFF); //state exists

	LWZ(Reg1, Reg3, 0);
	FreeAllocdArray(Reg1);
	LWZ(Reg1, Reg3, 0);
	FreeMem(Reg1);
	SetRegister(Reg1, 0);
	STW(Reg1, Reg3, 4);

	EndIf(); //state exists

	EndIf(); //basic check if is in match

	RestoreRegisters();

	ASMEnd(0x4e800020); //blr
}

//TempReg1 ends up pointing to the start of the next instance
void SaveInstance(int StartReg, int SaveToReg, int TempReg1, int TempReg2)
{
	SetRegister(TempReg1, 0x94);
	SaveMem(StartReg, TempReg1, SaveToReg);

	ADDI(TempReg1, StartReg, 0x80);
	LWZ(3, TempReg1, 4);
	LWZUX(3, TempReg1, 3);
	While(3, EQUAL, StartReg); //save loop

	LBZ(3, TempReg1, 0x13);
	If(3, EQUAL_I, 1); //freed memory

	SetRegister(TempReg2, 0x20); //alloc size

	Else(); //freed memory

	LWZ(TempReg2, TempReg1, 4); //alloc size

	EndIf(); //freed memory

	SaveMem(TempReg1, TempReg2, SaveToReg);
	LWZ(3, TempReg1, 4);
	LWZUX(3, TempReg1, 3);

	EndWhile(); //save loop
}

//Memory to restore must start with origanal location followed by size
//MemPtrReg must point to start (origonal location value)
//don't use r3 or r5
void RestoreMem(int MemPtrReg)
{
	LWZ(3, MemPtrReg, 0); //destination
	LWZ(5, MemPtrReg, 4); //size
	ADDI(4, MemPtrReg, 8); //source
	Memmove(3, 4, 5);
}

//AllocAddressReg is changed to point to the end of the list
void RestoreState(int AllocAddressReg)
{
	LWZU(4, AllocAddressReg, 4);
	While(4, NOT_EQUAL_I, 0);

	RestoreMem(4);
	LWZU(4, AllocAddressReg, 4);

	EndWhile();
}

//traverses through a linked list and saves all the nodes
void SaveList(int AddressReg, int SaveSizeReg, int SaveToReg, int TempReg1, int TempReg2)
{
	LWZ(TempReg1, AddressReg, 0);
	LHZ(TempReg2, AddressReg, 0xA);
	ADDI(TempReg2, TempReg2, 4);
	While(TempReg1, NOT_EQUAL_I, 0); //save loop

	SaveMem(TempReg1, SaveSizeReg, SaveToReg);
	LWZX(TempReg1, TempReg1, TempReg2);

	EndWhile(); //save loop
}

/*
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

	Push(0);
	SaveAllRegs();

	//SaveRegs({ 3, 4 });
	SetRegister(Reg1, TEMP_SAVE_STATE_PTR_LIST_LOC);

	If(Reg2, EQUAL_I, 8); //DUp


	//restore ptr thing
	LWZ(Reg4, Reg1, 4);
	SetRegister(Reg3, 0x91C0AC88);
	STW(Reg4, Reg3, 0);

	//restore variables
	LWZ(4, Reg1, 8);
	SetRegister(3, 0x901AE000);
	Memmove(0x2C8);

	//restore system thing
	LWZ(4, Reg1, 0xC);
	SetRegister(3, 0x8062fb40);
	Memmove(0x806312ac - 0x8062fb40);

	//restore FAT index
	LWZ(4, Reg1, 0x10);
	SetRegister(Reg3, 0x80b879b4);
	STW(4, Reg3, 0);

	//restore other modules
	LWZ(4, Reg1, 0x14);
	SetRegister(3, 0x80b8516c);
	Memmove(0x80b8545c - 0x80b8516c);

	//restore subaction table thing
	LWZ(4, Reg1, 0x1C);
	SetRegister(3, 0x806312f0);
	Memmove(0x8063ce70 - 0x806312f0);

	//restore animation object ptr things
	LWZ(4, Reg1, 0x20);
	SetRegister(3, 0x91c0ac84);
	Memmove(0x14);

	//restore instance (new)
	LWZ(4, Reg1, 0x24);
	SetRegister(3, 0x8128cb60);
	Memmove(0x94);

	RestoreCharacterAminObjects();

	LWZ(4, Reg1, 0x2C);
	SetRegister(3, 0x812d1860);
	//SetRegister(3, 0x812d21a0);
	Memmove(0x812deb60 - 0x812d1860);
	//Memmove(0x812deb60 - 0x812d21a0);


	EndIf(); //DUp

	If(Reg2, EQUAL_I, 4); //DDown

	//save ptr thing
	LoadWordToReg(Reg4, 0x91C0AC88);
	STW(Reg4, Reg1, 4);

	//save variables
	Allocate(0x2C8);
	STW(3, Reg1, 8);
	SetRegister(4, 0x901AE000);
	Memmove(0x2C8);

	//save system thing
	Allocate(0x806312ac - 0x8062fb40);
	STW(3, Reg1, 0xC);
	SetRegister(4, 0x8062fb40);
	Memmove(0x806312ac - 0x8062fb40);

	//save FAT index
	LoadWordToReg(Reg3, 0x80b879b4);
	STW(Reg3, Reg1, 0x10);

	//save other modules
	Allocate(0x80b8545c - 0x80b8516c);
	STW(3, Reg1, 0x14);
	SetRegister(4, 0x80b8516c);
	Memmove(0x80b8545c - 0x80b8516c);

	//save action state
	LoadWordToReg(Reg3, 0x812d7ccc);
	STW(Reg3, Reg1, 0x18);

	//save subaction table thing
	Allocate(0x8063ce70 - 0x806312f0);
	STW(3, Reg1, 0x1C);
	SetRegister(4, 0x806312f0);
	Memmove(0x8063ce70 - 0x806312f0);

	//save animation object ptr things
	Allocate(0x14);
	STW(3, Reg1, 0x20);
	SetRegister(4, 0x91c0ac84);
	Memmove(0x14);

	//save instance (new)
	Allocate(0x94);
	STW(3, Reg1, 0x24);
	SetRegister(4, 0x8128cb60);
	Memmove(0x94);

	SaveCharacterAnimObjects();
	
	//Allocate(0x812deb60 - 0x812d21a0);
	Allocate(0x812deb60 - 0x812d1860);
	STW(3, Reg1, 0x2C);
	SetRegister(4, 0x812d1860);
	//SetRegister(4, 0x812d21a0);
	Memmove(0x812deb60 - 0x812d1860);
	//Memmove(0x812deb60 - 0x812d21a0);


	EndIf(); //DDown

	RestoreRegs();

	//RestoreRegs(2);

	EndIf(); //DPad pressed

	ASMEnd(0x4e800020); //blr
}

void RestoreCharacterAminObjects()
{
	//SaveAllRegs();
	int Reg1 = 7;
	int Reg2 = 8;
	int Reg3 = 9;

	LoadWordToReg(4, TEMP_SAVE_STATE_PTR_LIST_LOC + 0x28); //get object list address
	LWZ(Reg2, 4, 0); //get number of objects
	ADDI(4, 4, 4);
	While(Reg2, GREATER_I, 0); //loop over objects

	LWZ(3, 4, 0x14); //get destination
	LWZ(5, 4, 4); //get size

	//Memmove
	SaveRegs({ 4, 5, 6, 7, 8 });
	CallBrawlFunc(0x803f602c, true); //Memmove
	RestoreRegs(5);

	ADD(4, 4, 5);
	ADDI(Reg2, Reg2, -1);
	EndWhile(); //loop over objects

	//RestoreRegs(29);
}

void SaveCharacterAnimObjects()
{
	//SaveAllRegs();
	//temporary registers
	int LengthReg = 6;
	int CounterReg = 7;
	int StartReg = 8;
	int SizeReg = 9;
	int AddressReg = 10;
	int TempReg1 = 11;
	//temporary start address
	SetRegister(StartReg, 0x812d397c);
	
	LWZ(LengthReg, StartReg, 0x8); //get size
	RLWINM(LengthReg, LengthReg, 12, 19, 31);
	SetRegister(CounterReg, 0);
	SetRegister(SizeReg, 4);
	//MULLI(SizeReg, LengthReg, 0x20); //0x20 is extra overhead per object, more is for keeping track
	ADDI(AddressReg, StartReg, 0x4);
	While(CounterReg, LESS, LengthReg); //get total size

	LWZU(TempReg1, AddressReg, 8);
	LWZ(TempReg1, TempReg1, -0x1C);
	ADD(SizeReg, SizeReg, TempReg1);

	ADDI(CounterReg, CounterReg, 1);
	EndWhile(); //get total size

	//Allocate
	Push(LengthReg);
	Push(StartReg);
	SetRegister(3, 0x2A);
	CallBrawlFunc(0x80024430, true); //call getMemAllocator
	ADDI(4, SizeReg, 0); //set size
	CallBrawlFunc(0x80204e5c, true); //call allocator
	SetRegister(TempReg1, TEMP_SAVE_STATE_PTR_LIST_LOC);
	STW(3, TempReg1, 0x28);
	Pop(StartReg);
	Pop(LengthReg);
	STW(LengthReg, 3, 0); //save number of entries
	ADDI(3, 3, 4);

	ADDI(AddressReg, StartReg, 0x4);
	SetRegister(CounterReg, 0);
	While(CounterReg, LESS, LengthReg); //store data

	LWZU(TempReg1, AddressReg, 8); //get address of object
	ADDI(4, TempReg1, -0x20); //include start of header
	LWZ(5, 4, 4); //get size

	//Memmove
	SaveRegs({3, 5, 6, 7});
	CallBrawlFunc(0x803f602c, true); //Memmove
	RestoreRegs(4);

	ADD(3, 3, 5);
	ADDI(CounterReg, CounterReg, 1);
	EndWhile(); //store data

	//RestoreRegs(29);
}
*/
