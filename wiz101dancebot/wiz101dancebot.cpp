// Christopher R. Fischer
// 10/15/2017
// VCC / FHS
// Wizard101 Gaming Bot.
// Copyright 2017. Please do not redistrute or use outside of written authorization.

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
/*
Commented 10/16/2017 - 3:13. Feel free to revert to the commit before this and get the comments that were originally made with the program!
*/

// Using Namespace makes it so we don't have to reference to the standard library in C++, so instead of writing std::string every time we can just write string and C++ will realize that if it doesn't know what that is to look in the std library.
using namespace std;


// Offsets refer to the individual locations in RAM (memory) where values of interest are located. Below is a list of the values.
const int offsets[30] = { (int)(0x00003964), ((int)0x0000266C), 0, (int)0x1F14, (int)0x1EF8, (int)0x9200,};
// CurrentRow, CurrentColumn, CurrentTime, CurrentScore, CurrentLevel, BaseRowColum

// Base Address referes to the base address of the DLL we are referencing.
int baseAddress = 0;

// Speed refers to the delay in between clicks that are accurate, as in clicks that would "score" us points by matching cards.
int speed = 200;

// This boolean, implemented across the entire program, is what switches v1 from v2. By enabling this variable, you introduce intentional misclicks which makes the bot more difficult to detect.
bool errors = true;



// Function that generates a random number based off of the time. Lowbound means the lowest possible value this number can generate, 
// highbound would be the highest number you can generate.
int getRandNum(int lowbound, int highbound) {
	srand(time(NULL));
	return lowbound + (rand() % highbound);
}




int readNoPointer(HANDLE w101, int offset) {
	// Access pointer stored at arbitrary location, retrieve current memory address, and retrieve value.
	// This function is used when we don't need to access a pointer. This is unusual for the current setup.
	// Such applications include calculating the addresses by hand.
	UINT_PTR PointerAddress = (UINT_PTR)(offset + baseAddress);
	// (DWORD) is a type neutral reference to a 4 byte value in RAM.
	DWORD CorrectAddress;
	DWORD Value;
	// This is a Microsoft Function that reads memory directly from the function.
	ReadProcessMemory(w101, (void*)PointerAddress, &Value, sizeof(DWORD), 0);
	return Value;
}



int readAddress(HANDLE w101, int offset) {
	// Access pointer stored at arbitrary location, retrieve current memory address, and retrieve value.
	UINT_PTR PointerAddress = (UINT_PTR)(offset + baseAddress);
	DWORD CorrectAddress;
	DWORD Value;
	// The extra ReadProcessMemory allows us to access an address, store the value of that address (which is presumbably another address), and then access THAT address.
	ReadProcessMemory(w101, (void*)PointerAddress, &CorrectAddress, sizeof(DWORD), 0);
	ReadProcessMemory(w101, (void*)CorrectAddress, &Value, sizeof(DWORD), 0);
	return Value;
}



int writeAddress(HANDLE w101, int offset, DWORD value) {
	//Access Pointer and write at arbritray memory.
	UINT_PTR adr = (UINT_PTR)(offset + baseAddress);
	DWORD CorrectAddress;
	DWORD ValueInMemory;
	// We read in the desintation memory address, save the old value, write the new value, then return the old value.
	ReadProcessMemory(w101, (void*)adr, &CorrectAddress, sizeof(DWORD), 0);
	ReadProcessMemory(w101, (void*)CorrectAddress, &ValueInMemory, sizeof(DWORD), 0);
	WriteProcessMemory(w101, (void*)CorrectAddress, &value, sizeof(DWORD), 0);
	return ValueInMemory;
}



// Adapted from Microsoft's Devolper Network Open Library, thanks for the function!
// PS Don't ask me how this works, I've changed what I need to and quite frankly I'm happy it works, if you'd like to read more about it 
// you can read it from it's location, https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx
int GetBaseAddress(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{				
				if (_tcscmp(szModName, _T("C:\\ProgramData\\KingsIsle Entertainment\\Wizard101\\Bin\\MG_concentration.dll")) == 0) {
					//_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
					return (int)hMods[i];
				}
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
	// We know it wasn't found!
	printf("\n\nFATAL ERROR: Concentration Game not loaded in! To retry, type anything and press enter.");
	printf("\nPress any key to continue.\n");
	char arr;
	cin >> arr;
	return GetBaseAddress(processID);
}




// Function shamelessly adapted from SE. This is designed to take in the X and Y co-ordinate from and send a click to that co-ordinate,
// which is harder then it should be.
void mouse_move(int x, int y) {
	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = x; // desired X coordinate
	Inputs[0].mi.dy = y; // desired Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(3, Inputs, sizeof(INPUT));
}



void click(int x, int y) {
	// This function takes in positions on the board and sends hardware events ("clicks");
	// basex and basey are the start of the board on my screen. X and Y are both variables
	// that represent row and column of the destination on the board, respectively.
	
	//printf("click card %d %d \t", x, y);
	int basex = 789;
	int basey = 349;
	
	// Here, we start at the very top of the board, and each card is 64x64, so we multiply by the row/column of our destination
	// card, which puts us in the top left corner. We then randomly get a number between 10-54 which determains the offsets we 
	// add to where we click, allowing it to be any random pixel in the card (with the exception of the top or right 10, when you get
	// too close to the boarder, the game can see that input as ambiguitous.
	int bx = basex + (x * 64) + getRandNum(10, 44);
	int by = basey + (y * 64) + getRandNum(10, 44);
	bx= bx * (65535 / 1920 );
	by= by * (65535 / 1080 );
	//printf("click cord %d %d \n", bx, by);
	mouse_move(bx, by);
}
int retrieveValue(int row, int column, HANDLE w101) {
	// For every ROW past 0, we add 28 hex (40 dec).
	// For every COLUMN past 0, we add F0 hex, 240 dec
	int off = offsets[5] + (row*40)+(column*240);
	//printf("Calculated Offset - %d\n", off);
	return readNoPointer(w101, off);
}
void winRound(HANDLE w101) {
	// This is cheating because we trick the game into thinking there is only one column/row, and with it's poorly written logic it determains that the round is over after the last click (probably a edge case trigger).
	// In the real world, this functions use exists primarily because in memory the array is stored in relative co-ordinates.
	// So, in the first few rounds, we can't accurately predict where to click because (annoyingly) the cards, if not taking up the whole board, aren't in the same place every time.
	// The solution is to use this function, set the round to win, and send clicks to each and every box until we blindly hit the right one.
	// Again, I'm not a fan of this either, but we can email KI later with our suggestions on how they write their proprietary code.
	writeAddress(w101, 0, (DWORD)1);
	writeAddress(w101, 1, (DWORD)1);
}
void findPairs(HANDLE w101) {
	// Let's find a bunch of pairs. First, we should set up our int array.
	int numRow = readAddress(w101, offsets[0]);
	int numCol = readAddress(w101, offsets[1]);
	// C requires constant numbers, and the board has a maximum of 7 x 7.
	// 9 is used as a buffer to make my life easier as a programmer.
	int board[9][9];

	// Cool, now that we have a board, let's fill it with values.
	// This for loop makes it so that for every individual element it is correctly filled with the integer value of the card from memory.
	for (int currentRow = 0; currentRow < numRow; currentRow++) {
		for (int currentColumn = 0; currentColumn < numCol; currentColumn++) {
			board[currentRow][currentColumn] = retrieveValue(currentRow, currentColumn, w101);
			// Leaving these prints in would print the board (duh!)
			printf("%4d ", board[currentRow][currentColumn]);
		}
		printf("\n");
	}
	// offrow/offcol stand for offsetForRow and offsetForColumn. These values exist because at the beginning of some games, as the video for v2
	// demonstrates, the board does not fill up the screen so we need a human to tell us where to the board starts.
	int offrow = 0;
	int offcol = 0;
	if (numCol != 7 || numRow != 6) {
		// Not a full board drawn.
		printf("WARNING: Incomplete board detected, please input 0,0 location offsets.\n");
		scanf("%d %d", &offrow, &offcol);
		printf("\n");
	}
	
	// Now that we have the board, let's locate our cards (in range). 
	// Found basically means, did you find this card? It is here because when we print later, we only want to print out the value of cards
	// that exist in the board.
	bool found;
	
	for (int i = 0; i <= 40; i++) {
		found = false;
		// Set this card as not found, because we don't want to print out or try to click non-existant cards.	
		for (int currentRow = 0; currentRow < numRow; currentRow++) {
			for (int currentColumn = 0; currentColumn < numCol; currentColumn++) {
				//If the current card we're over is equal to the integer value of the card we're looking for, we should click it.
				if (board[currentRow][currentColumn] == i) {
					// If this is the first of it's kind, we want to seet found equal to true and print the card index.
					if (!found) {
						printf("Card %2d: ", i);
						found = true;
					}
					click(currentRow+offrow, currentColumn+offcol);
					Sleep(speed+getRandNum(5,10));
				}
				if (getRandNum(0, 5) == 1 && errors) {
					// 20.0% chance of a misclick or random click 
					click(currentColumn, currentRow);
				}
			}
		}
		if (found) {
			// Print a newline for spacing.
			printf("\n");
		}
		if (errors)
		{
			// basically, after every row we click the cards in the board. This behavior has adapted because
			// it makes it more difficult to detect.
			for (int currentRow = 0; currentRow < numRow; currentRow++) {
				for (int currentColumn = 0; currentColumn < numCol; currentColumn++) {
					click(currentRow+offrow, currentColumn+offcol);
				}
			}
		}
		Sleep(20);
	}
	// The bot is giving the game time to draw the new board.
	Sleep(1500);
}
int main() {
	// Find the game's window.
	HWND wizard101 = FindWindow(NULL, "Wizard101");
	DWORD PID;
	GetWindowThreadProcessId(wizard101, &PID);
	
	// Let's get the delay from a human.
	printf("Please enter the speed multiplier (delay): ");
	scanf("%d", &speed);
	printf("\n");
	
	// Open the game's memory to us, allowing us to modify it.
	HANDLE w101 = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, PID);
	while (true) {
		// Is the module still loaded?
		baseAddress = GetBaseAddress(PID);
		// Print a status update. I never did bother to find the offset for time.
		printf("Time: d // Row Count: %d // Column Count: %d // Current Level: %d // Current Score: %d\n", readAddress(w101, offsets[0]), readAddress(w101, offsets[1]), readAddress(w101, offsets[4]), readAddress(w101, offsets[3]));
		findPairs(w101);
		Sleep(500);
	}
	system("pause");
    return 0;
}
