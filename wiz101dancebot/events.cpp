#include "stdafx.h"
#include "events.h"
#include <windows.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>



events::events()
{
	events::testing = false;
}


events::~events()
{
}

void events::setTestingTrue() {
	events::testing = true;
}

int events::getRandNum(int lowbound, int highbound) {
	srand(time(NULL));
	return lowbound + (rand() % highbound);
}

void events::mouse_move_double(int x, int y) {
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
	SendInput(3, Inputs, sizeof(INPUT));
}

void events::event_score_continue() {
	if (events::testing) {
		printf("Events score continue called!\n");
	}
	mouse_move_double(((65535 / 1920)*(900 + getRandNum(0, 120))), ((65535 / 1080)* (315 + getRandNum(0, 15))));
	mouse_move_double(((65535 / 1920)*(900 + getRandNum(0, 120))), ((65535 / 1080)* (315 + getRandNum(0, 15))));
	Sleep(1500);
}

void events::event_play_continue() {
	if (events::testing) {
		printf("Events play continue called!\n");
	}
	mouse_move_double(((65535 / 1920)*(900 + getRandNum(0, 100))), ((65535 / 1080)* (781 + getRandNum(0, 24))));
	mouse_move_double(((65535 / 1920)*(900 + getRandNum(0, 100))), ((65535 / 1080)* (781 + getRandNum(0, 24))));
	Sleep(1000);
}

void events::event_game_end() {
	events::event_score_continue();
	if (events::testing) {
		printf("Events game end called!\n");
	}
	mouse_move_double(((65535 / 1920)*(1305 + getRandNum(0, 30))), ((65535 / 1080)* (785 + getRandNum(0, 30))));
	mouse_move_double(((65535 / 1920)*(1305 + getRandNum(0, 30))), ((65535 / 1080)* (785 + getRandNum(0, 30))));
	Sleep(5000);
}

void events::event_anti_afk() {
	if (events::testing) {
		printf("Events anti afk called!\n");
	}
	// We want to move 8 times.
	for (int i = 0; i < 8; i++) {
		if (events::testing) {
			printf("Pressed L!\n");
		}
		INPUT input;
		WORD leftkey = VK_LEFT; 
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(leftkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = leftkey;
		input.ki.dwFlags = 0;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
	}
	Sleep(1000);
	event_move_x();
}
void events::event_move_x() {
	if (events::testing) {
		printf("Event Move X!\n");
	}
	for (int i = 0; i < 3; i++) {
		if (events::testing) {
			printf("Pressed X!\n");
		}
		INPUT input;
		WORD xkey = 0x58;
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(xkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = xkey;
		input.ki.dwFlags = 0;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
	}
	event_click_conj();
}

void events::event_click_conj() {
	if (events::testing) {
		printf("Event Click Conj called!\n");
	}
	mouse_move_double(((65535 / 1920)*(1135 + getRandNum(0, 45))), ((65535 / 1080)* (390 + getRandNum(0, 100))));
	mouse_move_double(((65535 / 1920)*(1135 + getRandNum(0, 45))), ((65535 / 1080)* (390 + getRandNum(0, 100))));
}

void events::event_game_start() {
	if (events::testing) {
		printf("Event Game Start called!\n");
	}
	event_score_continue();
	event_play_continue();
}