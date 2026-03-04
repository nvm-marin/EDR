#pragma once
#include <Windows.h>
#include <string>
#include <vector>

// ---- Console Colors ----
#define COL_BG 0 
#define COL_NORMAL FOREGROUND_RED | FOREGROUND_INTENSITY
#define COL_DIM FOREGROUND_RED
#define COL_SELETED FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define COL_ACCENT  FOREGROUND_RED | FOREGROUND_INTENSITY
#define COL_TITLE   FOREGROUND_RED | FOREGROUND_INTENSITY
#define COL_EVENT_PROC  FOREGROUND_RED | FOREGROUND_INTENSITY
#define COL_EVENT_TITLE FOREGROUND_RED | FOREGROUND_GREEN
#define COL_MUTED FOREGROUND_RED | FOREGROUND_BLUE

struct MenuItem
{
	std::wstring key;
	std::wstring label;
};


class Terminal
{
public:
	Terminal();
	//Setup
	void Init();
	void Clear();
	void HideCursor();
	void ShowCursor();


	//Drawing
	void SetColor(WORD color);
	void ResetColor();
	void MoveCursor(int x, int y);
	void DrawBorder(int x, int y, int w, int h);
	void PrintAt(int x, int y, const std::wstring& text, WORD color);
	void PrintCentered(int y, const std::wstring& text, WORD color);

	//Menu 
	int  ShowMenu(const std::wstring& title, const std::vector<MenuItem>& items);

	//Helpers
	int Width() const { return m_Width; }
	int Height() const { return m_Height; }

private:
	HANDLE m_hOut;
	HANDLE m_hIn;
	int m_Width;
	int m_Height;

	void GetConsoleSize();
};