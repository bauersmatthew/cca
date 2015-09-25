/*
 * File: cca.cpp
 * Edited: 24 Sep 2015
 * Author: Matthew Bauer
 */

// includes
#include <cca.h>

// main
int main(int argc, char **argv)
{
	
}

// CD: CnsMgr
CnsMgr::CnsMgr()
{
	initialized = false;
}
CnsMgr::CnsMgr(const CnsMgr&)
{
	throw cyerr("do not copy CnsMgr objects!", 8001);
}
CnsMgr::~CnsMgr()
{
	if(initialized)
		Kill();
}
void CnsMgr::Init()
{
	// get std handles
	h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if(h_stdout == INVALID_HANDLE_VALUE)
	{
		throw cyerr("failed to get stdout handle", 8002);
	}
	h_stdin = GetStdHandle(STD_INPUT_HANDLE);
	if(h_stdin == INVALID_HANDLE_VALUE)
	{
		throw cyerr("failed to get stdin handle", 8003);
	}
	
	// create mysb, set as active
	h_mysb = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);
	if(h_mysb == INVALID_HANDLE_VALUE)
	{
		throw cyerr("failed to create screen buffer", 8004);
	}
	{ // cursor info
		CONSOLE_CURSOR_INFO cci;
		cci.dwSize = 1; // anything 1-100 because not visible anyway
		cci.bVisible = FALSE;
		if(!SetConsoleCursorInfo(h_mysb, &cci))
		{
			throw cyerr("failed to set screen buffer cursor info", 8005);
		}
	}
	{ // screen buffer/window size: 50, 50
		CONSOLE_SCREEN_BUFFER_INFOEX csbi;
		csbi.cbSize = sizeof(csbi);
		if(!GetConsoleScreenBufferInfoEx(h_mysb, &csbi))
		{
			throw cyerr("failed to get screen buffer info", 8006);
		}
		COORD wndsz = {
			csbi.srWindow.Right - csbi.srWindow.Left + 1,
			csbi.srWindow.Bottom - csbi.srWindow.Top + 1}
		if(wndsz.X > WND_W | wndsz.Y > WND_H)
		{
			// resize (shrink) window first
			SMALL_RECT new_wndsz =
				{0, 0, WND_W-1, WND_H-1};
			if(!SetConsoleWindowInfo(h_mysb, FALSE, &new_wndsz))
			{
				throw cyerr("failed to set window size", 8007);
			}
		}
		// shrinking buffer is guaranteed to work now
		COORD new_bufsz = {WND_W, WND_H};
		if(!SetConsoleBufferSize(h_mysb, new_bufsz))
		{
			throw cyerr("failed to set buffer size", 8008);
		}
	}
	if(!SetConsoleMode(h_mysb, NULL)) // no output processing
	{
		throw cyerr("failed to set console output mode", 8010);
	}
	if(!SetConsoleActiveScreenBuffer(h_mysb))
	{
		throw cyerr("failed to set active screen buffer", 8009);
	}
	
	// change settings of stdin
	if(!GetConsoleMode(h_stdin, &ori_stdin_mode))
	{
		SetConsoleActiveScreenBuffer(h_stdout);
		throw cyerr("failed to get stdin console mode", 8011);
	}
	if(!SetConsoleMode(h_stdin, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT))
	{
		SetConsoleActiveScreenBuffer(h_stdout);
		throw cyerr("failed to set console input mode", 8012);
	}
	
	// try to change window title
	ori_title = new char[100];
	if(GetConsoleOriginalTitle(&ori_title, 100))
	{
		if(!SetConsoleTitle("CCA"))
		{
			title_changed = false;
		}
		else
		{
			title_changed = true;
		}
	}
	else
	{
		title_changed = false;
	}
	
	// create (our) buffer
	buf = new CHAR_INFO[WND_W * WND_H];

	// done.
	initialized = true;
}
void CnsMgr::Kill()
{
	if(!initialized)
		return;
	// don't check return values; it doesnt matter if stuff fails here
	SetConsoleActiveScreenBuffer(h_stdout);
	SetConsoleMode(h_stdin, ori_stdin_mode);
	if(title_changed)
	{
		SetConsoleTitle(ori_title);
	}
	delete[] ori_title;
	title_changed = false;
	delete[] buf;
	initialized = false;
}

WORD CnsMgr::TranslateAttributes(uint16_t att)
{
	DWORD ret = 0;;
	if(att & ATT_FG_RED)
		ret |= FOREGROUND_RED;
	if(att & ATT_FG_GREEN)
		ret |= FOREGROUND_GREEN;
	if(att & ATT_FG_BLUE)
		ret |= FOREGROUND_BLUE;
	if(att & ATT_FG_BRIGHT)
		ret |= FOREGROUND_INTENSITY;
	if(att & ATT_BG_RED)
		ret |= BACKGROUND_RED;
	if(att & ATT_BG_GREEN)
		ret |= BACKGROUND_GREEN;
	if(att & ATT_BG_BLUE)
		ret |= BACKGROUND_BLUE;
	if(att & ATT_BG_BRIGHT)
		ret |= BACKGROUND_INTENSITY;
	if(att & ATT_UNDERSCORE)
		ret |= COMMON_LVB_UNDERSCORE;
	return ret;
}

bool CnsMgr::Put(char c, uint8_t x, uint8_t y, uint16_t att)
{
	if(!initialized)
		return false;
	if(x >= WND_W || y >= WND_H)
		return false;
	CHAR_INFO *ci = buf + y*WND_W + x;
	ci->Char.AsciiChar = c;
	ci->Attributes = TranslateAttributes(att);
}
bool CnsMgr::Flip()
{
	if(!initialized)
		return false;
	COORD tmp_bufsz = {WND_W, WND_H};
	COORD tmp_bufcrd = {0, 0};
	SMALL_RECT tmp_writereg = {0, 0, WND_W-1, WND_H-1};
	return WriteConsoleOutput(h_mysb, buf, tmp_bufsz, tmp_bufcrd, &tmp_writereg);
}

uint16_t CnsMgr::TranslateWindowsVK(WORD vk)
{
	if(vk >= 0x41 && vk <= 0x5a)
	{
		return vk + ('a' - 'A');
	}
	if(vk >= 0x30 && vk <= 0x39)
	{
		return vk;
	}
	switch(vk)
	{
		case VK_TAB: return EVV_KEY_TAB;
		case VK_RETURN: return EVV_KEY_ENTER;
		case VK_SHIFT: return EVV_KEY_SHIFT;
		case VK_CONTROL: return EVV_KEY_CONTROL;
		case VK_MENU: return EVV_KEY_ALT;
		case VK_CAPITAL: return EVV_KEY_CAPSLOCK;
		case VK_ESCAPE: return EVV_KEY_ESCAPE;
		case VK_SPACE: return EVV_KEY_SPACE;
		case VK_NEXT: return EVV_KEY_PGDOWN;
		case VK_PRIOR: return EVV_KEY_PGUP;
		case VK_HOME: return EVV_KEY_HOME;
		case VK_END: return EVV_KEY_END;
		case VK_LEFT: return EVV_KEY_LEFT;
		case VK_RIGHT: return EVV_KEY_RIGHT;
		case VK_UP: return EVV_KEY_UP;
		case VK_DOWN: return EVV_KEY_DOWN;
		case VK_PRINT: return EVV_KEY_PRINTSCRN;
		case VK_INSERT: return EVV_KEY_INSERT;
		case VK_DELETE: return EVV_KEY_DELETE;
		case VK_NUMPAD0: return EVV_KEY_NUMPAD0;
		case VK_NUMPAD1: return EVV_KEY_NUMPAD1;
		case VK_NUMPAD2: return EVV_KEY_NUMPAD2;
		case VK_NUMPAD3: return EVV_KEY_NUMPAD3;
		case VK_NUMPAD4: return EVV_KEY_NUMPAD4;
		case VK_NUMPAD5: return EVV_KEY_NUMPAD5;
		case VK_NUMPAD6: return EVV_KEY_NUMPAD6;
		case VK_NUMPAD7: return EVV_KEY_NUMPAD7;
		case VK_NUMPAD8: return EVV_KEY_NUMPAD8;
		case VK_NUMPAD9: return EVV_KEY_NUMPAD9;
	}
}

bool CnsMgr::GetNextEvent(Event& ev)
{
	
}
