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
using namespace std;

int main()
{
	string OutputTextPath = "C:\\Users\\john\\Desktop\\ASM Testing\\ASM Dest.txt";
	
	CodeStart(OutputTextPath);
	//place all ASM code here

	//ReplayFix();

	//NameIsFound();

	//MenuControlCodes();

	//StopStartAltFunctions();

	//temp1();

	//SaveStateCodes();

	//SaveOrRestoreState();

	//StopPokemonTrainerSwitch();

	//StopDPadOnSSS();

	//ConvertButtons();

	//ItemSpawnControl();

	//ClearASLData();

	//SetTeamAttackTraining();

	//test();

	//PositionTrace();

	//UpdatePosTest();

	//DrawDI();

	//Draw();

	CodeMenu();

	//FixTr4shTeamToggle();

	//ControlCodes();

	CodeEnd();
	
	//string InputGCTPath = "C:\\Users\\john\\Desktop\\ASM Testing\\Good GCT\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.0\\Legacy TE 1.0\\LegacyTE\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.1 Wii\\Legacy TE 1.1\\codes\\RSBE01.gct";
	string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.11 Wii\\Legacy TE 1.11 Wii\\LegacyTE\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\legacyxp-master-f7b8d9b5cedaa8cb1f5740a568543a9dbfadd39d\\legacyxp-master-f7b8d9b5cedaa8cb1f5740a568543a9dbfadd39d\\Smash Bros LXP\\Legacy XP\\SD\\LegacyXP\\RSBEXP.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\legacyte-master-e8253b1ae029829e218c47da95806adf70b461e9\\legacyte-master-e8253b1ae029829e218c47da95806adf70b461e9\\Smash Bros Legacy TE\\Legacy TE\\LegacyTE\\RSBE01.gct";
	string OutputGCTPath = "C:\\Program Files (x86)\\WinImage\\RSBE01.gct";
	MakeGCT(OutputTextPath, InputGCTPath, OutputGCTPath); //remove if you don't want to append code
	return 0;
}