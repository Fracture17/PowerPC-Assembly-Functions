#pragma once
#include "stdafx.h"

const int FORCE_COSTUME_LOAD_FLAG = 0x6969;

void tagBasedCostumes();
void recordActiveTags();
void setTagCostume();
void reloadCostumeAfterTagSelect();
void teamBattleTagReload();
void updateTagsWhenOptionChanged();