#include "stdafx.h"
#include "ReplayFix.h"
#include "Controls.h"
#include "StopStartAlts.h"
#include "Save States.h"
using namespace std;

int main()
{
	string OutputTextPath = "C:\\Users\\john\\Desktop\\ASM Testing\\ASM Dest.txt";
	
	CodeStart(OutputTextPath);
	//place all ASM code here

	//ReplayFix();

	//MenuControlCodes();

	//StopStartAltFunctions();

	temp1();

	CodeEnd();
	
	//string InputGCTPath = "C:\\Users\\john\\Desktop\\ASM Testing\\Good GCT\\RSBE01.gct";
	//string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.0\\Legacy TE 1.0\\LegacyTE\\RSBE01.gct";
	string InputGCTPath = "C:\\Users\\john\\Downloads\\Legacy TE 1.1 Wii\\Legacy TE 1.1\\codes\\RSBE01.gct";
	string OutputGCTPath = "C:\\Program Files (x86)\\WinImage\\RSBE01.gct";
	MakeGCT(OutputTextPath, InputGCTPath, OutputGCTPath); //remove if you don't want to append code
	return 0;
}