

#include "stdafx.h"
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#define IMC_GETOPENSTATUS 0x0005
#pragma comment(lib, "imm32.lib")
using namespace std;

int getInputMethod() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		DWORD threadID = GetWindowThreadProcessId(hwnd, NULL);
		HKL currentLayout = GetKeyboardLayout(threadID);
		unsigned int x = (unsigned int)currentLayout & 0x0000FFFF;
		return ((int)x);
	}
	return 0;
}

void switchInputMethod(int locale) {
	HWND hwnd = GetForegroundWindow();
	LPARAM currentLayout = ((LPARAM)locale);
	PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, currentLayout);
}

bool isNowChineseStatus() {
	HWND hIME = ImmGetDefaultIMEWnd(GetForegroundWindow());
	// 获取搜狗输入法状态https://www.zhihu.com/question/529237445
	LRESULT status = SendMessage(hIME, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0);
	return status;
}
#include <fstream>
void saveChineseStatus() {
	fstream saveFile = fstream("./ChineseState.txt", ios::out | ios::trunc);
	saveFile << isNowChineseStatus();
	saveFile.close();
}
void CONTROL_SPACE() {
	//keybd_event(VK_SHIFT, 0, 0, 0);
//keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

	INPUT inputs[4] = {};
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_SPACE;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_SPACE;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}
void loadChineseStatus() {
	fstream saveFile = fstream("./ChineseState.txt", ios::in);
	bool wasChineseStatus = 0;
	saveFile >> wasChineseStatus;
	saveFile.close();
	if (wasChineseStatus != isNowChineseStatus()) {
		Sleep(30);
		CONTROL_SPACE();
		Sleep(50);
		if (wasChineseStatus != isNowChineseStatus()) {
			keybd_event(VK_SHIFT, 0, 0, 0);
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		}
	}
	
}
int main(int argc, char** argv)
{
	if (argc == 1) {
		int imID = getInputMethod();
		printf("%d\n", imID);
	}
	else {
		int locale = atoi(argv[1]);
		if (locale == 1033)
			saveChineseStatus();
		switchInputMethod(locale);
		if (locale != 1033)
			loadChineseStatus();
	}

	return 0;
}

