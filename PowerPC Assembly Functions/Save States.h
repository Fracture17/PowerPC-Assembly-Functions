#pragma once
#include <map>
#include "PowerPC Assembly Functions.h"

//change TEMP stuff later
//#define TEMP_SAVE_STATE_PTR_LIST_LOC 0x80002800
//#define PTR_LIST_LOC 0x80002800
#define PTR_LIST_LOC 0x935ce700
#define FIRST_FIGHTER_INSTANCE_LOC 0x8123ab60
#define FIGHTER_INSTANCE_SIZE 0x52000

struct Module
{
	bool used;
	int start;
	int end;
	int size;

	Module(bool used, int start, int end)
	{
		this->used = used;
		this->start = start;
		this->end = end;
		size = end - start;
	}
};

void InitializeModules();
void RestoreCharacterAminObjects();
void SaveCharacterAnimObjects();

void RestoreMem(int MemPtrReg);
void RestoreState(int AllocAddressReg);
void SaveInstance(int StartReg, int SaveToReg, int TempReg1, int TempReg2);
void SaveList(int AddressReg, int SaveSizeReg, int SaveToReg, int TempReg1, int TempReg2);

void SaveStateCodes();
void TriggerAllocSaves();
void RecordSavedLocations();
void RemoveFreedLocation();
void SaveOrRestoreState();

void temp1();