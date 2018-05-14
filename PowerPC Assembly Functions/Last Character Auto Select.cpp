#include "stdafx.h"
#include "Last Character Auto Select.h"

void SelectLastCharacter()
{
	SetCharacter();

	SetCorrectColor();
}

void SetCharacter()
{
	//r3 is port num
	//r30 is putCoin argument
	//[r30 + 0xA0] is setHand argument
	//r26 is setCharKind argument
	ASMStart(0x80689b18);
	SaveRegisters();

	//get data from port num

	LWZ(5, 26, 0x1B8);
	ANDI(5, 5, ~1);
	If(5, NOT_EQUAL_I, 0x28); {
		//is not equal to 0x28 or 0x29
		LWZ(3, 30, 0xA0);
		SetRegister(4, 0);
		CallBrawlFunc(0x8069ecac); //setHand

		MR(3, 30);
		CallBrawlFunc(0x8069d1e8); //putCoin

		MR(3, 26);
		LWZ(4, 26, 0x1B8); //move char ID into r4
		SetRegister(5, 0xC0DE); //signal for costume
		CallBrawlFunc(0x80696f60); //setCharKind
	}EndIf();

	RestoreRegisters();
	ASMEnd(0x7f03c378); //mr r3, r24
}

void SetCorrectColor()
{
	ASMStart(0x80696f8c);

	If(5, EQUAL_I_L, 0xC0DE); {
		LWZ(29, 3, 0x1BC);
	}Else(); {
		SetRegister(29, 0);
	}EndIf();

	ASMEnd();
}




//80682eb4
//806939c4