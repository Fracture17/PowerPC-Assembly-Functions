#pragma once
#include <map>
#include "PowerPC Assembly Functions.h"

//change TEMP stuff later
#define TEMP_SAVE_STATE_PTR_LIST_LOC 0x80002800

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

void temp1();