#include "Windows.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <conio.h>

#include "globals.h"
#include "terminal.h"
#include "..\EDR\DriverCommon.h"

//set no warnings for this file since it's just a demo C4996 
#define _CRT_SECURE_NO_WARNINGS 

Terminal term;
HANDLE g_hDevice = INVALID_HANDLE_VALUE;


void PageMonitor() {
	term.Clear();
	term.DrawBorder(2, 2, term.Width() - 4, term.Height() - 6);
	term.PrintAt(4, 2, L" [ LIVE MONITOR ] ", COL_NORMAL);
	term.PrintAt(4, 4, L"Connecting to DumbEDR kernel port...", COL_DIM);
	// TODO: monitor.cpp will go here
	if (g_hDevice == INVALID_HANDLE_VALUE) {
		term.PrintAt(4, 5, L"Failed to connect! Is driver loaded?", COL_DIM);
		_getch();
		return;
	}

	int row = 7;
	char buffer[512];
	DWORD bytesRead;

	term.PrintAt(4, 6, L"Listening for events... (ESC to exit)", COL_MUTED);

	while (true) {
		if (_kbhit() && _getch() == 27) break; // ESC to exit

		EDR_EVENT ev = {};
		DWORD bytesReturned = 0;

		if (DeviceIoControl(
			g_hDevice,
			IOCTL_BLUESTREET_SEND_DATA,
			nullptr,
			0,
			&ev,
			sizeof(ev),
			&bytesReturned,
			nullptr) && bytesReturned == sizeof(ev))
		{
			wchar_t line[600] = {};

			if (ev.Type == EdrEventProcessCreate)
			{
				swprintf_s(line, L"[CREATE] PID=%lu PPID=%lu %ls", ev.Pid, ev.Ppid, ev.ImagePath);
			}
			else if (ev.Type == EdrEventProcessExit)
			{
				swprintf_s(line, L"[EXIT] PID=%lu", ev.Pid);
			}
			else
			{
				swprintf_s(line, L"[TYPE=%d] PID=%lu", (int)ev.Type, ev.Pid);
			}

			term.PrintAt(4, row++, line, COL_NORMAL);
			if (row >= term.Height() - 7) row = 7;
		}

		Sleep(100);
	}

	CloseHandle(g_hDevice);



	term.PrintAt(4, 6, L"Press any key to return.", COL_MUTED);
	_getch();
}

void PageLastEvents() {
	term.Clear();
	term.DrawBorder(2, 2, term.Width() - 4, term.Height() - 6);
	term.PrintAt(4, 2, L" [ LAST EVENTS ] ", COL_NORMAL);
	term.PrintAt(4, 4, L"No events captured yet.", COL_DIM);
	term.PrintAt(4, 6, L"Press any key to return.", COL_MUTED);
	_getch();
}

void PageAbout() {
	term.Clear();
	term.DrawBorder(2, 2, term.Width() - 4, term.Height() - 6);
	term.PrintAt(4, 2, L" [ ABOUT ] ", COL_NORMAL);
	term.PrintAt(4, 4, L"DumbEDR v0.1", COL_NORMAL);
	term.PrintAt(4, 5, L"A minimal Windows kernel monitoring tool.", COL_DIM);
	term.PrintAt(4, 6, L"Kernel driver + User-mode client.", COL_DIM);
	term.PrintAt(4, 8, L"Press any key to return.", COL_MUTED);
	_getch();
}



int main()
{
	term.Init();


	//DriverInit(&g_hDevice);
	g_hDevice = CreateFileW(L"\\\\.\\DumbEDR", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hDevice == INVALID_HANDLE_VALUE) {
		wchar_t err[64];
		swprintf(err, 64, L"CreateFile FAILED: error %lu", GetLastError());
		term.PrintAt(4, 5, err, COL_DIM); 
		_getch();

	}
	else
	{
		term.PrintAt(4, 5, L"Handle opened OK!", COL_NORMAL);
		_getch();
	}


	std::vector<MenuItem> mainMenu = {
		{ L"1", L"Start Live Monitor"   },
		{ L"2", L"View Last Events"     },
		{ L"3", L"Filter by PID"        },
		{ L"4", L"Save Log to File"     },
		{ L"5", L"About"                },
		{ L"0", L"Exit"                 },
	};

	while (true) {
		int choice = term.ShowMenu(L"MAIN MENU", mainMenu);
		switch (choice) {
		case  0: PageMonitor();    break;
		case  1: PageLastEvents(); break;
		case  4: PageAbout();      break;
		case  5: // Exit
		case -1:
			term.Clear();
			term.PrintCentered(term.Height() / 2, L"Goodbye.", COL_DIM);
			Sleep(800);
			return 0;
		}
	}
}