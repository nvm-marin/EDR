#include "Windows.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <conio.h>

#include "globals.h"
#include "terminal.h"


Terminal term;


void PageMonitor() {
	term.Clear();
	term.DrawBorder(2, 2, term.Width() - 4, term.Height() - 6);
	term.PrintAt(4, 2, L" [ LIVE MONITOR ] ", COL_NORMAL);
	term.PrintAt(4, 4, L"Connecting to DumbEDR kernel port...", COL_DIM);
	// TODO: monitor.cpp will go here
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