#pragma once
#include <map>
#include "PowerPC Assembly Functions.h"
#include "Code Menu.h"
#include "Control Codes.h"

const int SAVE_STATE_BUFFER_SIZE = 0x100;







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
void RestoreList(int AllocAddressReg);
void RestoreState();
void FreeArticles(int reg1, int reg2, int reg3);
void RestoreArticles(int reg1, int reg2, int reg3, int reg4);
void SetResourcePtrs();
void GetResourcePtrLocations(int DestVectorReg);
void SaveInstance(int StartReg, int SaveToReg, int TempReg1, int TempReg2, int Heap = 6);
void SaveList(int AddressReg, int SaveSizeReg, int SaveToReg, int TempReg1, int TempReg2);
void SaveStateCodes();
void TriggerAllocSaves();
void RecordSavedLocations();
void RemoveFreedLocation();
void SaveOrRestoreState();
void AddArticle();
void RemoveArticle();
void SaveState();
void SaveModule(int ModuleReg, int SavePtrReg, int StartOffset, int EndOffset, int reg1, int reg2);
void SaveSpecifiedMemory(int Address, int maxNum, int SaveReg, int reg1, int reg2, int reg3, int reg4, int Heap = 6);
void ClearSpecifiedMemory();
void temp1();