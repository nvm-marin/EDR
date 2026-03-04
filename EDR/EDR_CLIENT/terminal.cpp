#include "terminal.h"
#include <iostream>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

Terminal::Terminal()
	: m_hOut(GetStdHandle(STD_OUTPUT_HANDLE))
	, m_hIn(GetStdHandle(STD_INPUT_HANDLE))
	, m_Width(80)
	, m_Height(24)
{
}

void Terminal::Init()
{
	SetConsoleTitleW(L"DumbEDR :>");
	
	_setmode(_fileno(stdout), _O_U16TEXT);

	SMALL_RECT rect = { 0,0,99,34 };
	SetConsoleWindowInfo(m_hOut, TRUE, &rect); //Sets the current size and position of a console screen buffer's window.

	COORD size = { 100, 35 };
	SetConsoleScreenBufferSize(m_hOut, size);

	GetConsoleSize();
	Clear();
}

void Terminal::GetConsoleSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(m_hOut, &csbi))
	{
		m_Width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		m_Height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}
}

void Terminal::Clear()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hOut, &csbi);
	DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
	COORD Origin = { 0, 0 };
	DWORD written;
	FillConsoleOutputCharacterW(m_hOut, ' ', size, Origin, &written);
	FillConsoleOutputAttribute(m_hOut, csbi.wAttributes, size, Origin, &written);
	SetConsoleCursorPosition(m_hOut, Origin);
}


void Terminal::HideCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
	SetConsoleCursorInfo(m_hOut, &cursorInfo);
}


void Terminal::ShowCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 10, TRUE };
	SetConsoleCursorInfo(m_hOut, &cursorInfo);
}

void Terminal::SetColor(WORD color)
{
	SetConsoleTextAttribute(m_hOut, color);
}

void Terminal::ResetColor()
{
	SetConsoleTextAttribute(m_hOut, COL_NORMAL);
}

void Terminal::MoveCursor(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(m_hOut, pos);
}


void Terminal::DrawBorder(int x, int y, int w, int h)
{
	SetColor(COL_DIM);

	//Top
	MoveCursor(x, y);
	std::wstring top = L"╔";
	for (int i = 0; i < w - 2; i++)
		top += L"═";
	WriteConsoleW(m_hOut, top.c_str(), (DWORD)top.size(), NULL, NULL);
	//WriteConsoleW -> takes a string and writes it to the console output. It returns the number of characters written.

	//Sides
	for (int row = 1; row < h - 1; row++)
	{
		MoveCursor(x, y + row);
		WriteConsoleW(m_hOut, L"║", 1, NULL, NULL);
		MoveCursor(x + w - 1, y + row);
		WriteConsoleW(m_hOut, L"║", 1, NULL, NULL);
	}

	//Bottom
	MoveCursor(x, y + h - 1);
	std::wstring bottom = L"╚";
	for (int i = 0; i < w - 2; i++)
		bottom += L"═";
	WriteConsoleW(m_hOut, bottom.c_str(), (DWORD)bottom.size(), NULL, NULL);

	ResetColor();

}


void Terminal::PrintAt(int x, int y, const std::wstring& text, WORD color)
{
	SetColor(color);
	MoveCursor(x, y);
	WriteConsoleW(m_hOut, text.c_str(), (DWORD)text.size(), NULL, NULL);
	ResetColor();
}

void Terminal::PrintCentered(int y, const std::wstring& text, WORD color)
{
	int x = (Width() - (int)text.size()) / 2;
	PrintAt(x, y, text, color);
}

int Terminal::ShowMenu(const std::wstring& title, const std::vector<MenuItem>& items)
{
	int selected = 0;
	const int menuWidth = 50;
	const int menuHeight = (int)items.size() + 6;
	const int menuX = (Width() - menuWidth) / 2;
	const int menuY = (Height() - menuHeight) / 2;

	auto draw = [&]()
		{
			Clear();

			PrintCentered(1, L"██████  ██    ██ ███    ███ ██████      ███████ ██████  ██████  ", COL_NORMAL);
			PrintCentered(2, L"██   ██ ██    ██ ████  ████ ██   ██     ██      ██   ██ ██   ██ ", COL_NORMAL);
			PrintCentered(3, L"██   ██ ██    ██ ██ ████ ██ ██████      █████   ██   ██ ██████  ", COL_DIM);
			PrintCentered(4, L"██   ██ ██    ██ ██  ██  ██ ██   ██     ██      ██   ██ ██   ██ ", COL_DIM);
			PrintCentered(5, L"██████   ██████  ██      ██ ██████      ███████ ██████  ██   ██ ", COL_NORMAL);
			PrintCentered(6, L"v0.1  —  Kernel Monitoring Tool", COL_DIM);

			DrawBorder(menuX, menuY, menuWidth, menuHeight);

			int titleX = menuX + (menuWidth - (int)title.size()) / 2;
			PrintAt(titleX, menuY, L"" + title + L" ", COL_NORMAL);

			for (int i = 0; i < (int)items.size(); i++)
			{
				std::wstring line = L" [" + items[i].key + L"] " + items[i].label;

				while ((int)line.size() < menuWidth - 4)
				{
					line += L" ";
				}

				if (i == selected)
				{
					SetColor(BACKGROUND_RED | FOREGROUND_RED |
						FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

					MoveCursor(menuX + 2, menuY + 2 + i);
					WriteConsoleW(m_hOut, line.c_str(), (DWORD)line.size(), NULL, NULL);
					ResetColor();
				}
				else
				{
					PrintAt(menuX + 2, menuY + 2 + i, line, COL_DIM);
				}
			}

			PrintCentered(menuY + menuHeight, L"↑ ↓ Navigate   Enter Select   Q Quit", COL_MUTED);
	};

	draw();

	while (true)
	{
		int key = _getch();

		//arrow keys
		if (key == 224 || key == 0)
		{
			key = _getch();
			if (key == 72) selected = (selected - 1 + (int)items.size()) % (int)items.size(); //up
			else if (key == 80) selected = (selected + 1) % (int)items.size(); //down

			draw();
		}
		else if (key == '\r') //enter
		{
			return selected;
		}
		else if(key == 'q' || key == 'Q')
		{
			return -1;
		}
		else
		{
			for (int i = 0; i < (int)items.size(); i++)
			{
				if(items[i].key.size() == 1 && towupper(wchar_t(key)) == towupper(items[i].key[0]))
				{
					return i;
				}
			}
		}
	}

}