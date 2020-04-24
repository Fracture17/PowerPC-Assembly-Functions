#include "stdafx.h"
#include "Control Codes.h"

void ControlCodes()
{
	StartMatch();

	EndMatch();

	Draw();

	LoadCodeMenu();

	AddNewCharacterBuffer();

	DeleteCharacterBuffer();

	DeleteCharacterBufferOnTransform();

	//RestoreLimitedCharacters();

	if (PERCENT_SELECT_ACTIVATOR_P1_INDEX != -1) {
		//FixPercentSelector();
	}
}

void FixPercentSelector() {
	//can use r4-r6
	//r30 is ptr
	ASMStart(0x800e0ce4);

	int reg1 = 4;
	int reg2 = 5;
	int reg3 = 6;

	SetRegister(reg2, 0x69);
	STB(reg2, 30, 8); //flag

	//SetRegister(4, 0x100); //percent

	ASMEnd(); //set r4 to percent

	//can use r18 and up
	//r3 is ptr
	ASMStart(0x800e15a4);

	reg1 = 19;
	reg2 = 20;
	reg3 = 21;

	LBZ(reg1, 3, 8);
	If(reg1, EQUAL_I, 0x69); {
		Increment(reg1);
		SetRegister(4, 0x100);
		STB(reg1, 3, 8);
	}Else(); If(reg1, EQUAL_I, 0x6A); {
		SetRegister(4, 0);
		STB(4, 3, 8);
	}EndIf(); EndIf();

	ASMEnd(0x80030354); //lwz r0, r3, 0x354
}

void LoadCodeMenu()
{
	ASMStart(0x8002d4f4);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;

#if BUILD_TYPE == PROJECT_PLUS
	LoadFile("/menu3/data.cmnu", START_OF_CODE_MENU_HEADER, reg1, reg2, false);
#else
	LoadFile(MAIN_FOLDER + "/cm.bin", START_OF_CODE_MENU_HEADER, reg1, reg2);
#endif

	/*SetRegister(reg1, STRING_BUFFER);

	SetRegister(reg2, STRING_BUFFER + 0x18);
	STW(reg2, reg1, 0); //file path ptr

	SetRegister(reg2, 0);
	STW(reg2, reg1, 4);
	STW(reg2, reg1, 8);
	STW(reg2, reg1, 0x10);

	SetRegister(reg2, START_OF_CODE_MENU_HEADER);
	STW(reg2, reg1, 0xC); //storage loc

	SetRegister(reg2, -1);
	STW(reg2, reg1, 0x14);

	ADDI(reg2, reg1, 0x18);
	WriteStringToMem("legacyte/cm.bin\0"s, reg2);

	MR(3, reg1);
	CallBrawlFunc(0x8001cbf4); //readSDFile*/

	RestoreRegisters();
	ASMEnd(0x9421ffe0); //stwu sp, sp, -0x20
}

void setRotationQueuePlayers() {
	//1[0x90180fb8 + 4 + 0x5C * port] = 0 if not active
	//number seems to indicate what type of match it is

	//For each active port, check if in queue
	//if active and not in queue, add to second position, push others back
	//if not active and in queue, remove and push others forward

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	
	int portReg = 15;
	int portInfoReg = 14;

	GetSceneNum(reg1);
	If(reg1, EQUAL_I, 6); { //in replay
		LoadWordToReg(reg1, REPLAY_ENDLESS_ROTATION_QUEUE);
		SetRegister(reg2, ENDLESS_ROTATION_QUEUE_LOC);
		STW(reg1, reg2, 0);
	} Else(); {
		LoadWordToReg(reg1, INFINITE_FRIENDLIES_INDEX + Line::VALUE);
		If(reg1, EQUAL_I, 3); {
			SetRegister(portInfoReg, 0x90180fb8 + 4);
			//for each port:
			CounterLoop(portReg, 1, 5, 1); {
				//get if port is active
				LBZ(reg1, portInfoReg, 0);

				//get index of port in queue
				SetRegister(reg2, ENDLESS_ROTATION_QUEUE_LOC);
				FindInArray(portReg, reg2, 4, 1, reg3, reg4);

				//if active and not in queue, add to second position, push others back
				If(reg1, NOT_EQUAL_I, 0); { //active
					If(reg3, LESS_I, 0); { //not in queue
						//insert into queue
						SetRegister(reg2, ENDLESS_ROTATION_QUEUE_LOC);
						LBZ(reg4, reg2, 0);
						If(reg4, EQUAL_I, 0); { //insert into first porition
							//insert new port
							STB(portReg, reg2, 0);
						} Else(); {
							//shift others back
							LWZ(reg4, reg2, 1);
							RLWINM(reg4, reg4, 24, 8, 24); //shift right 8, only hold 2 bytes
							STW(reg4, reg2, 1);
							//insert new port
							STB(portReg, reg2, 1);
						} EndIf();
					} EndIf();
				} EndIf();

				//if not active and in queue, remove and push others forward
				If(reg1, EQUAL_I, 0); { //not active
					If(reg3, GREATER_OR_EQUAL_I, 0); { //in queue
						//delete from queue
						LWZ(reg4, reg2, 0); //reg 2 is address of port in queue
						RLWINM(reg4, reg4, 8, 0, 24); //<<8
						STW(reg4, reg2, 0);
					} EndIf();
				} EndIf();

				ADDI(portInfoReg, portInfoReg, 0x5C); //next port
			} CounterLoopEnd();
		} Else(); {
			SetRegister(reg1, 0);
			SetRegister(reg2, ENDLESS_ROTATION_QUEUE_LOC);
			STW(reg1, reg2, 0);
		} EndIf();
	} EndIf();
}

void saveRotationQueueForReplay() {
	ASMStart(0x806d1770);

	int reg1 = 3;
	int reg2 = 4;

	GetSceneNum(reg1);
	If(reg1, EQUAL_I, 0xA); { //in versus
		LoadWordToReg(reg1, ENDLESS_ROTATION_QUEUE_LOC);
		SetRegister(reg2, REPLAY_ENDLESS_ROTATION_QUEUE);
		STW(reg1, reg2, 0);
	} EndIf();

	ASMEnd(0x4e800020); //blr
}

void StartMatch()
{
	saveRotationQueueForReplay();

	ASMStart(0x806cf15c);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;
	int reg8 = 24;
	int reg9 = 23;

	setRotationQueuePlayers();

	//randomize P1 character
	//should automatically work with ex builds
	/*LoadByteToReg(reg1, 0x806857f0 + 3); //number of characters
	Randi(reg2, reg1);
	SetRegister(reg1, 0x80680e80);
	LBZX(reg1, reg1, reg2);
	SetRegister(reg2, 0x90180FB8);
	STB(reg1, reg2, 0);
	SetRegister(reg1, 0);
	STB(reg1, reg2, 1); //set costume to 0*/


	//LimitCharacters();

	//allocate memory

	SetupCharacterBuffer();

	if (INFINITE_FRIENDLIES_INDEX != -1) {
		InfiniteFriendlies(reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
	}

	//set in game flag
	SetRegister(reg1, 1);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	RestoreRegisters();
	ASMEnd(0x3fa08070); //lis r29, 0x8070
}

void LimitCharacters() {
	//[80B8A6D0] + 0 based port = ptr
	//ptr + 0x44 = stuff
	//set stuff to 0x08010100 to stop fighter
	//upper bits are 0 if doesn't exist

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;
	int reg8 = 24;
	int reg9 = 23;

	IfInVersus(reg1);
	{
		//check if option selected

		LoadWordToReg(reg1, 0x80B8A6D0);
		LWZ(reg1, reg1, 0xC); //port 4
		SetRegister(reg2, 0x03010100);
		STW(reg2, reg1, 0x44);
	}EndIf();
}

void RestoreLimitedCharacters() {
	ASMStart(0x806d4c24);
	//r18 is 0 based port number
	//r3 is ptr
	//r3 + 0x44 == stuff to change
	//change to 0x03000100 to restore

	IfInVersus(12); {
		If(18, EQUAL_I, 3);
		{
			SetRegister(12, 0x03000100);
			STW(12, 3, 0x44);
		}EndIf();
	}EndIf();
	


	ASMEnd(0x8183003c); //lwz r12, 0x3C (r3)
}

void orderRotationQueueByMatchPlacing() {
	ASMStart(0x806d4c14);
	SaveRegisters({ 1 });

	//[[0x80623318 + 0x244 * port(0 based)] + 0x44] = stocks
	//[[0x80623318 + 0x244 * port(0 based)] + 0x34] (float)= stocks

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;

	int entryReg = 22;
	int moduleReg = 21;
	int stockPtrReg = 20;
	int numPortsReg = 19;
	int placementListReg = 18;
	int percentReg = 17;
	int stocksReg = 16;
	int portReg = 15;
	int queuePtrReg = 14;

	IfInVersus(reg1); {

		SetRegister(placementListReg, ENDLESS_ROTATION_PLACEMENT_LIST_LOC);
		CounterLoop(portReg, 1, 5, 1); {
			SetRegister(3, 0x80629a00);
			ADDI(4, portReg, -1);
			CallBrawlFunc(0x80815c40); //getEntryID

			If(3, GREATER_I, -1); {
				MR(entryReg, 3);
				//call getFighter
				MR(4, 3);
				SetRegister(3, 0x80629a00);
				SetRegister(5, -1);
				CallBrawlFunc(0x80814f20);
				LWZ(moduleReg, 3, 0x60);
				LWZ(moduleReg, moduleReg, 0xD8);

				//call getPercent
				LWZ(3, moduleReg, 0x38);
				SetRegister(4, 0);
				CallBrawlFunc(0x80769cb8); //getDamage
				ConvertFloatingRegToInt(1, percentReg);

				//call getStocks
				SetRegister(3, 0x80629a00);
				MR(4, entryReg);
				CallBrawlFunc(0x808159e4); //getOwner
				CallBrawlFunc(0x8081c540); //getStockCount
				NEG(stocksReg, 3);

				STB(stocksReg, placementListReg, 0);
				STH(percentReg, placementListReg, 1);
				STB(portReg, placementListReg, 3);
				ADDI(placementListReg, placementListReg, 4);
				ADDI(numPortsReg, numPortsReg, 1);
			} EndIf();
		} CounterLoopEnd();

		//getEntryID 0x80815c40
		//r3 = 0x80629a00
		//r4 = port

		//getFighter 80814f20

		/*SetRegister(placementListReg, ENDLESS_ROTATION_PLACEMENT_LIST_LOC);
		SetRegister(numPortsReg, 0);
		SetRegister(stockPtrReg, 0x80623318);

		//for each port in queue:
		SetRegister(queuePtrReg, ENDLESS_ROTATION_QUEUE_LOC);
		LBZ(portReg, queuePtrReg, 0);
		While(portReg, NOT_EQUAL_I, 0); {
			//get port stocks
			SetRegister(reg2, 0);
			LWZ(reg1, stockPtrReg, 0x20); //port
			ADDI(reg1, reg1, 1);
			If(reg1, EQUAL, portReg); {
				LWZ(reg3, stockPtrReg, 0);
				SetRegister(reg2, 1);
			} EndIf();
			LWZ(reg1, stockPtrReg, 0x20 + 0x244); //port
			ADDI(reg1, reg1, 1);
			If(reg1, EQUAL, portReg); {
				LWZ(reg3, stockPtrReg, 0x244);
				SetRegister(reg2, 1);
			} EndIf();

			If(reg2, EQUAL_I, 1); {
				LWZ(stocksReg, reg3, 0x44);
				NEG(stocksReg, stocksReg); //negate stocks

				//get port percent
				LFS(0, reg3, 0x34);
				ConvertFloatingRegToInt(0, percentReg);

				//store stocks as byte, percent as half, port num as byte
				STB(stocksReg, placementListReg, 0);
				STH(percentReg, placementListReg, 1);
				STB(portReg, placementListReg, 3);
				ADDI(placementListReg, placementListReg, 4);

				ADDI(numPortsReg, numPortsReg, 1);
			} EndIf();

			LBZU(portReg, queuePtrReg, 1);
		} EndWhile();*/

		//call qsort @ 0x803f8acc
		//r3 = start of list
		SetRegister(3, ENDLESS_ROTATION_PLACEMENT_LIST_LOC);
		//r4 = num elements
		MR(4, numPortsReg);
		//r5 = size of each element (4)
		SetRegister(5, 4);
		//r6 = ptr to compare func
		SetRegister(6, ENDLESS_ROTATION_COMP_FUNC_LOC);
		CallBrawlFunc(0x803f8acc);

		//MR(reg4, queuePtrReg);

		//re-create queue using sorted results
		SetRegister(queuePtrReg, ENDLESS_ROTATION_QUEUE_LOC - 1);
		SetRegister(placementListReg, ENDLESS_ROTATION_PLACEMENT_LIST_LOC + 3 - 4);
		While(numPortsReg, GREATER_I, 0); {
			LBZU(reg1, placementListReg, 4); //get port
			STBU(reg1, queuePtrReg, 1); //store port in new list
			ADDI(numPortsReg, numPortsReg, -1);
		} EndWhile();

		//shift 2nd position back
		SetRegister(reg2, 0);
		SetRegister(queuePtrReg, ENDLESS_ROTATION_QUEUE_LOC);
		FindInArray(reg2, queuePtrReg, 4, 1, reg1, reg3);

		SetRegister(reg1, ENDLESS_ROTATION_QUEUE_LOC);
		LBZ(reg3, reg1, 1); //byte to push back
		LWZ(reg2, reg1, 1);
		RLWINM(reg2, reg2, 8, 0, 24); //<<8
		STW(reg2, reg1, 1);
		STB(reg3, queuePtrReg, -1); //reg4 is address of first 0
	} EndIf();

	RestoreRegisters();
	ASMEnd(0x3a400000); //li r18, 0
}

void EndMatch()
{
	orderRotationQueueByMatchPlacing();

	//r3 is scene manager thing
	ASMStart(0x806d4850);
	SaveRegisters();

	int reg1 = 31;
	int reg2 = 30;
	int reg3 = 29;
	int reg4 = 28;
	int reg5 = 27;
	int reg6 = 26;
	int reg7 = 25;
	int reg8 = 24;
	int reg9 = 23;
	int NextSceneReg = 16;
	int MainBufferReg = 15;
	int oldSceneReg = 14;

	//clear in game flag
	SetRegister(reg1, 0);
	SetRegister(reg2, IS_IN_GAME_FLAG);
	STW(reg1, reg2, 0);

	//set random ASL flag
	SetRegister(reg6, RANDOM_ALTS_MATCH_START_FLAG);
	SetRegister(reg9, 1);
	STW(reg9, reg6, 0);

	//free memory
#ifdef IASA_OVERLAY
	FreeIASABuffers();
#endif

	//clear endless friendly rotation flags
	SetRegister(reg6, 0);
	SetRegister(reg9, P1_STOP_LOAD_FLAG_PTR_LOC);
	for (int i = 0; i < 4; i++) {
		LWZ(reg8, reg9, i * 4);
		If(reg8, NOT_EQUAL_I, 0); {
			LBZ(reg7, reg8, 0);
			ANDI(reg7, reg7, ~0x8); //clear flag
			STB(reg7, reg8, 0);
			STW(reg6, reg9, i * 4); //clear ptr
		} EndIf();
	}

	//oldSceneReg/NextSceneReg gets scene
	IfInVersus(oldSceneReg); {
		MR(NextSceneReg, oldSceneReg);
		LoadWordToReg(reg4, reg3, 0x80584084); //get current hold to pause flag

		if (AUTO_SKIP_TO_CSS_INDEX != -1) {
			LoadWordToReg(reg1, AUTO_SKIP_TO_CSS_INDEX + Line::VALUE);
			If(reg1, EQUAL_I, 1); {
				SetRegister(reg4, 0); //clear hold to pause
				SetRegister(NextSceneReg, 0xD);
			}EndIf();
		}

		//reload stage
		if (INFINITE_FRIENDLIES_INDEX != -1) {
			int Skip = GetNextLabel();
			LoadWordToReg(reg1, INFINITE_FRIENDLIES_INDEX + Line::VALUE);
			If(reg1, GREATER_I, 0); {
				LWZ(reg1, 3, 0x24);
				LWZ(reg1, reg1, 0x8C);
				If(reg1, EQUAL_I, 0x13); {
					JumpToLabel(Skip);
				}EndIf();
				If(reg1, EQUAL_I, 0x19); {
					JumpToLabel(Skip);
				}EndIf();

				LWZ(reg4, reg3, 0); //keep hold to puase flag

				SetRegister(reg1, 1);
				SetRegister(reg2, INFINITE_FRIENDLIES_FLAG_LOC);
				STW(reg1, reg2, 0);

				SetRegister(NextSceneReg, 1);
			}EndIf();

			Label(Skip);
		}

		SetRegister(reg2, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
		LoadHalfToReg(reg6, SALTY_RUNBACK_BUTTON_COMBO_LOC + 2);
		LoadHalfToReg(reg7, SKIP_RESULTS_BUTTON_COMBO_LOC + 2);
		SetRegister(reg9, 0);
		CounterLoop(reg1, 0, 8, 1); {
			LWZU(reg5, reg2, BUTTON_PORT_OFFSET);
			//salty runback
			AND(reg8, reg5, reg6);
			If(reg8, EQUAL, reg6); {
				SetRegister(reg9, 0x10);
			}EndIf();
			//skip to CSS
			AND(reg8, reg5, reg7);
			If(reg8, EQUAL, reg7); {
				SetRegister(reg9, 0x20);
			}EndIf();
		}CounterLoopEnd();

		SetRegister(reg5, INFINITE_FRIENDLIES_FLAG_LOC);
		SetRegister(reg6, 0);
		//skip to CSS
		LoadByteToReg(reg1, SKIP_RESULTS_BUTTON_COMBO_LOC);
		RLWINM(reg1, reg1, 1, 0, 31); //*=2
		XOR(reg1, reg1, reg9);
		If(reg1, EQUAL_I, 0x20); {
			SetRegister(NextSceneReg, 0xD);
			SetRegister(reg4, 0); //clear hold to pause
			STW(reg6, reg5, 0); //clear endless friendlies flag
		}EndIf();
		//salty runback
		LoadByteToReg(reg1, SALTY_RUNBACK_BUTTON_COMBO_LOC);
		XOR(reg1, reg1, reg9);
		If(reg1, EQUAL_I, 0x10); {
			SetRegister(NextSceneReg, 1);
			LWZ(reg4, reg3, 0); //keep hold to puase flag
			STW(reg6, reg5, 0); //clear endless friendlies flag
		}EndIf();

		//set next scene
		LoadWordToReg(reg1, 0x805b4fd8 + 0xd4);
		LWZ(reg1, reg1, 0x10);
		STW(NextSceneReg, reg1, 8);

		If(oldSceneReg, NOT_EQUAL, NextSceneReg); {
			SetRegister(reg2, 0);
			SetRegister(reg5, 0x80588003);
			SetRegister(reg6, 0x80588300);
			CounterLoop(reg1, 0, 4, 1); {
				STW(reg2, reg5, 153);
				STW(reg2, reg6, 104);
				ADDI(reg5, reg5, 0xA0);
				ADDI(reg6, reg6, 0x70);
			} CounterLoopEnd();
		} EndIf();

		STW(reg4, reg3, 0); //set hold to pause to whatever
	}EndIf();

	LoadWordToReg(MainBufferReg, MAIN_BUFFER_PTR);
	FreeMem(MainBufferReg);

	if (AUTO_SAVE_REPLAY_INDEX != -1) {
		If(oldSceneReg, EQUAL_I, 0xA); {
			SetRegister(reg3, 0);
			LoadWordToReg(reg1, reg2, AUTO_SAVE_REPLAY_FLAG_LOC);
			STW(reg3, reg2, 0); //clear flag
			If(reg1, EQUAL_I, 2); {
				SaveReplay();
			}EndIf();
		}EndIf();
		//can't trust registers after this
	}
	

	RestoreRegisters();
	ASMEnd(0x7c7f1b78); //mr r31, r3
}

void Draw()
{
	ASMStart(0x8000e588);
	vector<int> FPRegs(21);
	iota(FPRegs.begin(), FPRegs.end(), 0);
	SaveRegisters(FPRegs);

	//draw di
	if (DI_DRAW_INDEX != -1) {
		DrawTrajectories();
	}

	//IASA overlay
#ifdef IASA_OVERLAY
	EndOverlay();
#endif

	RestoreRegisters();
	ASMEnd(0x38210030); //addi sp, sp, 48
}

void DeleteCharacterBufferOnTransform()
{
	//r3 is thing
	//r4 is new module index
	//[r3 + 0xA] = old index
	ASMStart(0x808205bc);
	SaveRegisters(14);

	int BaseModuleTableReg = 31;
	int reg1 = 30;
	int reg2 = 29;
	int reg3 = 28;
	int reg4 = 27;
	int reg5 = 26;
	int ModulePtrReg = 25;
	int HeadOfFighterReg = 24;
	int CharacterBufferReg = 23;
	int ModuleIndexReg = 22;

	MR(ModuleIndexReg, 4);

	LBZ(reg3, 3, 0xA);
	ADDI(reg1, 3, 0x34);
	RLWINM(reg2, 4, 3, 0, 31); //<< 3
	RLWINM(reg3, reg3, 3, 0, 31); //<< 3
	LWZX(3, reg1, reg2); //get new module
	LWZX(4, reg1, reg3); //get old module
	LWZ(BaseModuleTableReg, 3, 0x60);
	MR(HeadOfFighterReg, 3);

	//delete old buffer
	int CharacterBufferPtrReg = 28;

	int Quit = GetNextLabel();

	LWZ(4, 4, 0x60);
	LoadWordToReg(CharacterBufferPtrReg, MAIN_BUFFER_PTR);
	LWZ(CharacterBufferReg, CharacterBufferPtrReg, 0);
	While(CharacterBufferReg, NOT_EQUAL, 4); {
		LWZU(CharacterBufferReg, CharacterBufferPtrReg, 8);
		If(CharacterBufferReg, EQUAL_I, 0); {
			JumpToLabel(Quit);
		}EndIf();
	}EndWhile();
	LWZ(CharacterBufferReg, CharacterBufferPtrReg, 4);

	//free mem
	if (DI_DRAW_INDEX != -1) {
		LWZ(reg1, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
		FreeDIBuffer(reg1, reg2);
	}

	FreeMem(CharacterBufferReg);

	//shift all elements down
	SetRegister(reg1, 1);
	While(reg1, NOT_EQUAL_I, 0); {
		LWZU(reg1, CharacterBufferPtrReg, 8);
		LWZ(reg2, CharacterBufferPtrReg, 4);
		STW(reg1, CharacterBufferPtrReg, -8);
		STW(reg2, CharacterBufferPtrReg, -4);
	}EndWhile();

	Label(Quit);

	RestoreRegisters();
	ASMEnd(0x9883000b); //stb r4, r3, 0xB
}

void AddNewCharacterBuffer()
{
	//r31 = entity ptr

	int BaseModuleTableReg = 31;
	int reg1 = 30;
	int reg2 = 29;
	int reg3 = 28;
	int reg4 = 27;
	int reg5 = 26;
	int ModulePtrReg = 25;
	int HeadOfFighterReg = 24;
	int CharacterBufferReg = 23;
	int EntityReg = 22;

	int IsPopoReg = 14;
	int Quit = GetNextLabel();

	ASMStart(0x8081f4b4);
	SaveRegisters(14);

	LoadWordToReg(28, IS_IN_GAME_FLAG);
	MR(EntityReg, 31);
	If(28, EQUAL_I, 1); {
		//is in game
		LBZ(reg1, EntityReg, 0xA);
		ADDI(reg2, EntityReg, 0x34);
		RLWINM(reg1, reg1, 3, 0, 31); //*=8
		LWZX(3, reg2, reg1);
		SetRegister(IsPopoReg, 0x10);

		int Start = GetNextLabel();
		Label(Start);

		//check if already exists
		LWZ(BaseModuleTableReg, 3, 0x60);
		LoadWordToReg(reg1, MAIN_BUFFER_PTR);
		LWZ(CharacterBufferReg, reg1, 0);
		While(CharacterBufferReg, NOT_EQUAL_I, 0); {
			If(CharacterBufferReg, EQUAL, BaseModuleTableReg); {
				JumpToLabel(Quit);
			}EndIf();
			LWZU(CharacterBufferReg, reg1, 8);
		}EndWhile();

		MR(HeadOfFighterReg, 3);
		LWZ(ModulePtrReg, BaseModuleTableReg, 0xD8);

		SetRegister(reg1, 0);
		FindEndOfCharacterBuffers(reg1, reg2);
		STW(BaseModuleTableReg, reg2, 0);
		STW(reg1, reg2, 8); //clear next slot
		STW(reg1, reg2, 0xC); //clear next slot

		SetRegister(reg1, CHR_BUFFER_SIZE + 0x20);
		Allocate(reg1);
		STW(3, reg2, 4);

		MR(CharacterBufferReg, 3);

		MR(3, HeadOfFighterReg);
		CallBrawlFunc(0x8083ae38); //getInput
		STW(3, CharacterBufferReg, CHR_BUFFER_FIGHTER_INPUT_PTR_OFFSET);
		SetRegister(3, 0x80629a00);
		CallBrawlFunc(0x80815ad0); //get player number
		STW(3, CharacterBufferReg, CHR_BUFFER_PORT_OFFSET);

		STW(HeadOfFighterReg, CharacterBufferReg, CHR_BUFFER_HEAD_OF_FIGHTER_OFFSET);

		MULLI(reg2, 3, 0x5C);
		SetRegister(reg1, CHARACTER_INFO_START_ADDRESS);
		ADD(reg1, reg1, reg2);
		STW(reg1, CharacterBufferReg, CHR_BUFFER_INFO_PTR_OFFSET);

		if (CHARACTER_SELECT_P1_INDEX != -1) {
			GetArrayValueFromIndex(CHARACTER_SWITCHER_ARRAY_LOC, 3, 0, 3, reg2); {
				LBZ(reg1, reg1, 0); //get char ID
				ADDI(reg5, reg2, Selection::SELECTION_LINE_OFFSETS_START + 3);
				FindInArray(reg1, reg5, CHARACTER_ID_LIST.size(), 4, reg3, reg4);
				STW(reg3, reg2, Line::VALUE);
				STW(reg3, reg2, Line::DEFAULT);
			}EndIf(); EndIf();
		}

		MR(3, BaseModuleTableReg);
		SetRegs(4, { 3023, 0 }); //gravity
		CallBrawlFunc(0x80796c6c); //get constant float core
		FNEG(1, 1);
		STFS(1, CharacterBufferReg, CHR_BUFFER_GRAVITY_OFFSET);

		MR(3, BaseModuleTableReg);
		SetRegs(4, { 3024, 0 }); //mfs
		CallBrawlFunc(0x80796c6c); //get constant float core
		FNEG(1, 1);
		STFS(1, CharacterBufferReg, CHR_BUFFER_MFS_OFFSET);

		LWZ(reg2, ModulePtrReg, 0x64);
		LWZ(reg2, reg2, 0x20);
		LWZ(reg2, reg2, 0xC);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_VARIABLES_ADDRESS_OFFSET); //variables start address
		LWZU(reg3, reg2, 0xE0);
		STW(reg3, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_OFFSET); //frames
		STW(reg2, CharacterBufferReg, CHR_BUFFER_HITSTUN_FRAMES_PTR_OFFSET); //ptr

		LWZ(reg2, ModulePtrReg, 0xC);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_POS_PTR_OFFSET);

		LWZ(reg3, ModulePtrReg, 0x7C);
		LWZ(reg2, reg3, 0x7C);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_KB_VECTOR_PTR_OFFSET);

		LWZ(reg2, ModulePtrReg, 0x5C);
		LWZ(reg2, reg2, 0x14C);
		STW(reg2, CharacterBufferReg, CHR_BUFFER_IP_SWITCH_PTR_OFFSET);

		//create buffers
		if (DI_DRAW_INDEX != -1) {
			CreateDIBuffer(CharacterBufferReg, reg2, reg3, reg4);
		}

		LWZ(reg1, EntityReg, 0x5C);
		If(reg1, EQUAL, IsPopoReg); {
			//is ice climbers, and first run through
			LWZ(3, EntityReg, 0x3C); //get nana module
			SetRegister(IsPopoReg, 0); //prevent loop next time
			JumpToLabel(Start);
		}EndIf();
	}EndIf();

	Label(Quit);

	RestoreRegisters();
	ASMEnd(0xcbe10058); //lfd f31, sp, 0x58
}



void DeleteCharacterBuffer()
{
	//r4 is module ptr
	//can use r31
	ASMStart(0x8082f3f4);
	LoadWordToReg(31, IS_IN_GAME_FLAG);
	If(31, EQUAL_I, 1); {
		SaveRegisters();

		int CharacterBufferReg = 31;
		int reg1 = 30;
		int reg2 = 29;
		int CharacterBufferPtrReg = 28;

		int Quit = GetNextLabel();

		LWZ(4, 4, 0x60);
		LoadWordToReg(CharacterBufferPtrReg, MAIN_BUFFER_PTR);
		LWZ(CharacterBufferReg, CharacterBufferPtrReg, 0);
		While(CharacterBufferReg, NOT_EQUAL, 4); {
			LWZU(CharacterBufferReg, CharacterBufferPtrReg, 8);
			If(CharacterBufferReg, EQUAL_I, 0); {
				JumpToLabel(Quit);
			}EndIf();
		}EndWhile();
		LWZ(CharacterBufferReg, CharacterBufferPtrReg, 4);

		//free mem
		if (DI_DRAW_INDEX != -1) {
			LWZ(reg1, CharacterBufferReg, CHR_BUFFER_DI_BUFFER_PTR_OFFSET);
			FreeDIBuffer(reg1, reg2);
		}

		FreeMem(CharacterBufferReg);

		//shift all elements down
		SetRegister(reg1, 1);
		While(reg1, NOT_EQUAL_I, 0); {
			LWZU(reg1, CharacterBufferPtrReg, 8);
			LWZ(reg2, CharacterBufferPtrReg, 4);
			STW(reg1, CharacterBufferPtrReg, -8);
			STW(reg2, CharacterBufferPtrReg, -4);
		}EndWhile();

		Label(Quit);

		RestoreRegisters();
	}EndIf();
	ASMEnd(0x3be00000); //li r31, 0
}


//r3 has address of chr buffer ptr
void FindCharacterBuffer(int TargetReg, int ResultReg)
{
	LoadWordToReg(3, MAIN_BUFFER_PTR);
	SetRegister(4, 0x80000000);
	If(3, LESS_L, 4); {
		SetRegister(4, 0xA0000000);
		If(3, GREATER_L, 4); {
			SetRegister(3, 0);
		} EndIf();
	} EndIf();

	If(3, NOT_EQUAL_I, 0); {
		LWZ(ResultReg, 3, 0);
		While(ResultReg, NOT_EQUAL, TargetReg); {
			LWZU(ResultReg, 3, 8);
		}EndWhile();
		LWZ(ResultReg, 3, 4);
	}EndIf();

	/*LoadWordToReg(ResultReg, MAIN_BUFFER_PTR);
	LWZ(3, ResultReg, 0);
	While(3, NOT_EQUAL, TargetReg); {
		LWZU(3, ResultReg, 8);
	}EndWhile();
	LWZ(ResultReg, ResultReg, 4);*/
}

void FindEndOfCharacterBuffers(int TargetReg, int ResultReg)
{
	LoadWordToReg(ResultReg, MAIN_BUFFER_PTR);
	LWZ(3, ResultReg, 0);
	While(3, NOT_EQUAL, TargetReg); {
		LWZU(3, ResultReg, 8);
	}EndWhile();
}

void GetCharacterValue(int CharacterBufferReg, vector<int> ValuePath, int ResultReg)
{
	GetValueFromPtrPath(ValuePath, CharacterBufferReg, ResultReg);
}

void InfiniteFriendlies(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9)
{
	//int Quit = GetNextLabel();

	LoadWordToReg(reg1, reg2, INFINITE_FRIENDLIES_FLAG_LOC);
	If(reg1, EQUAL_I, 1); {
		SetRegister(reg1, 0);
		STW(reg1, reg2, 0); //clear flag

		/*SetRegister(reg2, PLAY_BUTTON_LOC_START - BUTTON_PORT_OFFSET);
		CounterLoop(reg1, 0, 4, 1); {
			LWZU(reg3, reg2, BUTTON_PORT_OFFSET);
			//salty runback
			ANDI(reg4, reg3, BUTTON_L | BUTTON_R | BUTTON_A | BUTTON_Y);
			If(reg4, EQUAL_I, BUTTON_L | BUTTON_R | BUTTON_A | BUTTON_Y); {
				JumpToLabel(Quit);
			}EndIf();
		}CounterLoopEnd();*/

		LoadWordToReg(reg1, INFINITE_FRIENDLIES_INDEX + Line::VALUE);
		If(reg1, GREATER_OR_EQUAL_I, 2); {
			//random stage
			GetLegalStagesArray(reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
			If(reg4, EQUAL_I, 0); {
				//provide default
				STW(reg4, reg3, 0);
				SetRegister(reg4, 1);
			}EndIf();
			MR(3, reg4);
			CallBrawlFunc(0x8003fc7c); //randi
			//RandomCapped(reg4, reg1);
			LoadWordToReg(reg2, 0x805a00e0);
			LBZX(3, reg3, 3);
			LWZ(reg2, reg2, 8);
			CallBrawlFunc(0x800af614); //exchangeMuStageForScStage
			STH(3, reg2, 0x1A);
		}EndIf();

		LoadWordToReg(reg2, 0x805a00e0);
		LBZX(3, reg3, 3);
		LWZ(reg2, reg2, 8);
		LHZ(3, reg2, 0x1A); //stage ID
		ADDI(4, reg2, 0x1C);
		ADDI(5, reg2, 0x5C);
		SetRegister(6, 0);
		CallBrawlFunc(0x8010f960); //stGetStageParameter
	}EndIf();

	//Label(Quit);
}


#if BUILD_TYPE != PROJECT_PLUS
//reg4 contains total size
//reg3 contains start address
void GetLegalStagesArray(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9)
{
	SetRegister(reg1, 0x806b9298);
	LoadWordToReg(reg2, 0x805A00E0);
	LWZ(reg2, reg2, 0x24);
	ADDI(reg2, reg2, 2064);
	SetRegister(reg3, STRING_BUFFER);
	SetRegister(reg4, 0);
	LWZ(reg8, reg2, 0x24);
	LWZ(reg9, reg2, 0x20);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
	ADDI(reg1, reg1, 8);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
}

void AddLegalStagesToArray(int StageListReg, int LegalStageHighMaskReg, int LegalStageLowMaskReg, int ArrayReg, int PosReg, int reg2, int reg3, int reg4)
{
	LBZ(4, StageListReg, 4); //num stages
	LWZ(reg2, StageListReg, 0); //list ptr
	SetRegister(3, 1);
	While(4, GREATER_I, 0); {
		Decrement(4);
		LBZX(reg3, reg2, 4);
		If(reg3, GREATER_OR_EQUAL_I, 31); {
			ADDI(reg4, reg3, -23);
			RLWNM(reg4, 3, reg4, 0, 31);
			AND(reg4, reg4, LegalStageHighMaskReg);
		}Else(); {
			RLWNM(reg4, 3, reg3, 0, 31);
			AND(reg4, reg4, LegalStageLowMaskReg);
		}EndIf();
		
		If(reg4, NOT_EQUAL_I, 0); {
			If(reg3, NOT_EQUAL_I, 0x14); {
				If(reg3, NOT_EQUAL_I, 0xE); {
					STBX(reg3, ArrayReg, PosReg);
					Increment(PosReg);
				}EndIf();
			}EndIf();
		}EndIf();
	}EndWhile();
}

#else

//reg4 contains total size
//reg3 contains start address
void GetLegalStagesArray(int reg1, int reg2, int reg3, int reg4, int reg5, int reg6, int reg7, int reg8, int reg9)
{
	LoadWordToReg(reg2, 0x805A00E0);
	LWZ(reg2, reg2, 0x24);
	ADDI(reg2, reg2, 2064);
	SetRegister(reg3, STRING_BUFFER);
	SetRegister(reg4, 0);
	LWZ(reg8, reg2, 0x24);
	LWZ(reg9, reg2, 0x20);

	LoadWordToReg(reg1, 0x80495D04);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
	LoadWordToReg(reg1, 0x80495D08);
	AddLegalStagesToArray(reg1, reg9, reg8, reg3, reg4, reg5, reg6, reg7);
}

void AddLegalStagesToArray(int StageListReg, int LegalStageHighMaskReg, int LegalStageLowMaskReg, int ArrayReg, int PosReg, int reg2, int reg3, int reg4)
{
	SetRegister(4, 0x16); //num stages
	SetRegister(3, 1);
	While(4, GREATER_I, 0); {
		Decrement(4);
		LBZX(reg3, StageListReg, 4);
		If(reg3, GREATER_OR_EQUAL_I, 31); {
			ADDI(reg4, reg3, -23);
			RLWNM(reg4, 3, reg4, 0, 31);
			AND(reg4, reg4, LegalStageHighMaskReg);
		}Else(); {
			RLWNM(reg4, 3, reg3, 0, 31);
			AND(reg4, reg4, LegalStageLowMaskReg);
		}EndIf();

		If(reg4, NOT_EQUAL_I, 0); {
			If(reg3, NOT_EQUAL_I, 0x14); {
				If(reg3, NOT_EQUAL_I, 0xE); {
					STBX(reg3, ArrayReg, PosReg);
					Increment(PosReg);
				}EndIf();
			}EndIf();
		}EndIf();
	}EndWhile();
}
#endif