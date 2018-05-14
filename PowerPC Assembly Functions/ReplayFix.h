#pragma once
#include "PowerPC Assembly Functions.h"

void ReplayFix();
void MakeBrawlSaveWholeReplay();
void SetupRecord();
void SaveSyncData();
void AlignSyncDuringPlayback();
void SetupPlayback();
void RecordInput();
void PlayInput();
void CStickASDIFix();
void StopBrawlReplay();
void StopRecording();
void AlternateStageFix();
void MemCardCheckSkip();
void FastForward();
void FixDebugPause();
void StopFalseCorruption();
void ClearASLData();
void FixSinglePlayer();