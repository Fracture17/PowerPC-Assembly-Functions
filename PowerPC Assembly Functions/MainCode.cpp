#include "stdafx.h"
#include "ReplayFix.h"
#include "Controls.h"
#include "StopStartAlts.h"
#include "Save States.h"
#include "Miscellaneous Code.h"
#include "StopDPadOnSSS (except Wiimote).h"
#include "DrawDI.h"
#include "Code Menu.h"
#include "IASA Overlay.h"
#include "Control Codes.h"
#include "Last Character Auto Select.h"
#include "Tag Based Costumes.h"
#include "Light Shield.h"
#include "IkeClimbers.h"
#include "AIDisplay.h"
#include "C++Injection.h"
//#include "FPS Display.h"
using namespace std;



int main()
{
	string OutputTextPath = "C:\\Users\\johno\\Documents\\Modding\\Dolphin\\SD_Raws\\ASM.txt";
	
	CodeStart(OutputTextPath);
	//place all ASM code here

	//ReplayFix();

	//NameIsFound();

	//MenuControlCodes();

	//StopStartAltFunctions();

	//StopPokemonTrainerSwitch();

	//StopDPadOnSSS();

	//ConvertButtons();

	//ItemSpawnControl();

	//ClearASLData();

	//SetTeamAttackTraining();

	//LXPGreenOverlayFix();

	//CodeMenu(); tagBasedCostumes();

	//musicPercentCode();

	//DoubleFighterTest();

	//UCF();

	//CStickSlowFix();

	//FixStickyRAlts();

	//SelectLastCharacter();

	//FixTr4shTeamToggle();

	//cstickTiltTest();

	//FPSDisplay();

	//CStickTiltFix();

	//DBZModeTest();

	//slipperyTechs();

	//lightShield();

	//IkeClimbers();

	//fixStanimaTextBug();

	//AIDisplay();

	loadCppCodes(); writeInjectionsRepeat();

	CodeEnd();



	//string InputGCTPath = "C:\\Users\\john\\Desktop\\ASM Testing\\Good GCT\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.0\\Legacy TE 1.0\\LegacyTE\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.11 Wii\\Legacy TE 1.11 Wii\\LegacyTE\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE\\LegacyTE\\RSBE01.gct"s;
#if BUILD_TYPE == PROJECT_PLUS
	string InputGCTPath = "C:\\Users\\john\\Downloads\\v1.06b DEV BUILD WII OFFLINE\\Project+\\RSBE01.gct";
#elif BUILD_TYPE == NORMAL
	string InputGCTPath = "C:\\Users\\john\\Downloads\\legacyte-master-Smash Bros Legacy TE-Legacy TE\\Smash Bros Legacy TE\\Legacy TE\\LegacyTE\\RSBE01.gct";
#endif
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\LXPL 2.1 v1 [Wii]\\LegacyLT\\RSBE01.gct";
	string OutputGCTPath = R"(C:\Users\john\Documents\Dolphin Emulator\Wii\RSBE01.gct)";
	//MakeGCT(OutputTextPath, InputGCTPath, OutputGCTPath); //remove if you don't want to append code
	return 0;
}