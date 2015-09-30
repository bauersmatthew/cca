/*
 * File: cca.cpp
 * Edited: 29 Sep 2015
 * Author: Matthew Bauer
 */

// includes
#include "cca.h"
#include <iostream>
#include <thread>
#include <fstream>

// main
int main(int argc, char **argv)
{
	// setup console manager
	CnsMgr *cm = new CnsMgr();
	try
	{
		cm->Init();
	}
	catch(cyerr& ce)
	{
		std::cerr << ce.say() << std::endl;
		return 1;
	}
	
	// test
	std::ifstream fin("map");
	std::string line;
	std::getline(fin, line);
	for(int ln = 0; !fin.eof(); ln++)
	{
		cm->PutStr(line, 0, ln, ATT_FG_RED | ATT_FG_BRIGHT | ATT_UNDERSCORE);
		std::getline(fin, line);
	}
	fin.close();
	cm->Flip();
	cm->GetLine(NULL, 0, 0, 0, CnsMgr::Event(EVV_KEY_Q, EVM_KEY_DOWN, EVT_KEY), 0);
	
	// clean up
	cm->Kill();
	delete cm;
	return 0;
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
	{ // screen buffer/window size
		// set both at once
		CONSOLE_SCREEN_BUFFER_INFOEX new_csbi;
		new_csbi.cbSize = sizeof(new_csbi);
		if(!GetConsoleScreenBufferInfoEx(h_mysb, &new_csbi))
		{
			throw cyerr("failed to get screen buffer info", 8007);
		}
		if(new_csbi.dwMaximumWindowSize.X < WND_W || new_csbi.dwMaximumWindowSize.Y < WND_H)
		{
			throw cyerr("maximum window size too small", 8008);
		}
		new_csbi.dwSize = {WND_W, WND_H};
		new_csbi.dwCursorPosition = {0, 0};
		new_csbi.srWindow = {0, 0, WND_W-1, WND_H-1};
		new_csbi.wAttributes = 0;
		if(!SetConsoleScreenBufferInfoEx(h_mysb, &new_csbi))
		{
			throw cyerr("failed to set window/buffer size", 8006);
		}
		SetConsoleScreenBufferInfoEx(h_mysb, &new_csbi);
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
	if(GetConsoleOriginalTitle(ori_title, 100))
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
	
	// clear screen
	Fill(' ', 0); // black|black
	Flip();
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
	WORD ret = 0;
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
	ci->Char.UnicodeChar = 0;
	ci->Char.AsciiChar = c;
	ci->Attributes = TranslateAttributes(att);
	return true;
}
bool CnsMgr::PutStr(const std::string& str, uint8_t x, uint8_t y, uint16_t att)
{
	if(!initialized)
		return false;
	uint8_t len = str.length();
	if(x + len > WND_W)
		return false;
	for(uint8_t i = 0; i < len; i++)
	{
		Put(str[i], x+i, y, att);
	}
	return true;
}
bool CnsMgr::Fill(char c, uint16_t att)
{
	bool ret = true;
	for(uint8_t x = 0; x < WND_W; x++)
	{
		for(uint8_t y = 0; y < WND_H; y++)
		{
			ret = Put(c, x, y, att);
		}
	}
	return ret;
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
		case VK_BACK: return EVV_KEY_BACKSPACE;
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
		case VK_F1: return EVV_KEY_F1;
		case VK_F2: return EVV_KEY_F2;
		case VK_F3: return EVV_KEY_F3;
		case VK_F4: return EVV_KEY_F4;
		case VK_F5: return EVV_KEY_F5;
		case VK_F6: return EVV_KEY_F6;
		case VK_F7: return EVV_KEY_F7;
		case VK_F8: return EVV_KEY_F8;
		case VK_F9: return EVV_KEY_F9;
		case VK_F10: return EVV_KEY_F10;
		case VK_F11: return EVV_KEY_F11;
		case VK_F12: return EVV_KEY_F12;
		case VK_NUMLOCK: return EVV_KEY_NUMLOCK;
		case VK_SCROLL: return EVV_KEY_SCRLOCK;
		case VK_OEM_1: return EVV_KEY_COLONS;
		case VK_OEM_PLUS: return EVV_KEY_EQ_PLUS;
		case VK_OEM_MINUS: return EVV_KEY_MINUS_USCOR;
		case VK_OEM_PERIOD: return EVV_KEY_PERIOD_RANG;
		case VK_OEM_COMMA: return EVV_KEY_COMMA_LANG;
		case VK_OEM_2: return EVV_KEY_SLASH_QM;
		case VK_OEM_3: return EVV_KEY_TQ_TILDE;
		case VK_OEM_4: return EVV_KEY_LSB_LCB;
		case VK_OEM_5: return EVV_KEY_BS_VLINE;
		case VK_OEM_6: return EVV_KEY_RSB_RCB;
		case VK_OEM_7: return EVV_KEY_SQ_DQ;
	}
	return EVV_KEY_NOTRANS;
}

bool CnsMgr::GetNextEvent(Event& ev)
{
	if(!initialized)
		return false;
	
	ev.val = 0;
	ev.meta = 0;
	ev.type = 0;

	INPUT_RECORD irec;
	DWORD num_read;
	if(!ev_q.empty())
	{
		ev = ev_q.at(0);
		ev_q.erase(ev_q.begin());
		return true;
	}
	
	if(!GetNumberOfConsoleInputEvents(h_stdin, &num_read))
	{
		return false;
	}
	if(num_read == 0)
	{
		ev.type = EVT_NONE;
		return true;
	}

	if(!ReadConsoleInput(h_stdin, &irec, 1, &num_read))
	{
		return false;
	}
	if(irec.EventType == KEY_EVENT)
	{
		ev.type = EVT_KEY;
		ev.val = TranslateWindowsVK(irec.Event.KeyEvent.wVirtualKeyCode);
		ev.meta = EVM_KEY_NONE;
		
		DWORD cks = irec.Event.KeyEvent.dwControlKeyState;
		if(cks & CAPSLOCK_ON)
			ev.meta |= EVM_KEY_CAPSLOCK;
		if(cks & LEFT_ALT_PRESSED || cks & RIGHT_ALT_PRESSED)
			ev.meta |= EVM_KEY_ALT;
		if(cks & LEFT_CTRL_PRESSED | cks & RIGHT_CTRL_PRESSED)
			ev.meta |= EVM_KEY_CONTROL;
		if(cks & NUMLOCK_ON)
			ev.meta |= EVM_KEY_NUMLOCK;
		if(cks & SCROLLLOCK_ON)
			ev.meta |= EVM_KEY_SCRLOCK;
		if(cks & SHIFT_PRESSED)
			ev.meta |= EVM_KEY_SHIFT;
		if(irec.Event.KeyEvent.bKeyDown)
		{
			if(irec.Event.KeyEvent.wRepeatCount > 1)
			{
				ev.meta |= EVM_KEY_HOLD;
				for(int i = 1; i < irec.Event.KeyEvent.wRepeatCount; i++)
				{
					ev_q.push_back(ev);
				}
			}
			else
			{
				ev.meta |= EVM_KEY_DOWN;
			}
		}
		else
		{
			ev.meta |= EVM_KEY_UP;
		}
		return true;
	}
	if(irec.EventType = MOUSE_EVENT)
	{
		ev.type = EVT_MOUSE;
		ev.val = 0;
		ev.val |= (EVV_MOUSE_XMASK & irec.Event.MouseEvent.dwMousePosition.X);
		ev.val |= (EVV_MOUSE_YMASK & irec.Event.MouseEvent.dwMousePosition.Y);
		
		ev.meta = 0;
		DWORD winbs = irec.Event.MouseEvent.dwButtonState;
		if(winbs & FROM_LEFT_1ST_BUTTON_PRESSED)
			ev.meta |= EVM_MOUSE_LEFT;
		if(winbs & RIGHTMOST_BUTTON_PRESSED)
			ev.meta |= EVM_MOUSE_RIGHT;
		
		DWORD winck = irec.Event.MouseEvent.dwControlKeyState;
		if(winck & LEFT_ALT_PRESSED || winck & RIGHT_ALT_PRESSED)
			ev.meta |= EVM_MOUSE_ALT;
		if(winck & LEFT_CTRL_PRESSED || winck & RIGHT_CTRL_PRESSED)
			ev.meta |= EVM_MOUSE_CONTROL;
		if(winck & SHIFT_PRESSED)
			ev.meta |= EVM_MOUSE_SHIFT;
		
		DWORD winef = irec.Event.MouseEvent.dwEventFlags;
		if(winef & DOUBLE_CLICK)
			ev.meta |= EVM_MOUSE_DOUBLE;
		if(winef & MOUSE_MOVED)
			ev.meta |= EVM_MOUSE_MOVED;
		
		return true;
	}
	return true;
}

char CnsMgr::KeyEventToChar(const CnsMgr::Event& kev, char bad)
{
	char base = bad;
	if(kev.val >= EVV_KEY_A && kev.val <= EVV_KEY_Z)
		base = (char)kev.val;
	else if(kev.val >= EVV_KEY_0 && kev.val <= EVV_KEY_9)
		base = (char)kev.val;
	else if(kev.val >= EVV_KEY_NUMPAD0 && kev.val <= EVV_KEY_NUMPAD9)
		base = (char)(kev.val-2000);
	else if(
		kev.val == EVV_KEY_TAB ||
		kev.val == EVV_KEY_ENTER ||
		kev.val == EVV_KEY_COLONS ||
		kev.val == EVV_KEY_EQ_PLUS ||
		kev.val == EVV_KEY_MINUS_USCOR ||
		kev.val == EVV_KEY_COMMA_LANG ||
		kev.val == EVV_KEY_PERIOD_RANG ||
		kev.val == EVV_KEY_SLASH_QM || 
		kev.val == EVV_KEY_TQ_TILDE ||
		kev.val == EVV_KEY_LSB_LCB || 
		kev.val == EVV_KEY_RSB_RCB ||
		kev.val == EVV_KEY_BS_VLINE || 
		kev.val == EVV_KEY_SQ_DQ ||
		kev.val == EVV_KEY_SPACE
		)
		base = (char)kev.val;
	else
		return bad;
	
	if(kev.meta & EVM_KEY_SHIFT)
	{
		if(base >= 'a' && base <= 'z')
			base -= (char)('a'-'A');
		else switch(base)
		{
			case '0': return ')';
			case '1': return '!';
			case '2': return '@';
			case '3': return '#';
			case '4': return '$';
			case '5': return '%';
			case '6': return '^';
			case '7': return '&';
			case '8': return '*';
			case '9': return '(';
			case ';': return ':';
			case '=': return '+';
			case '-': return '_';
			case ',': return '<';
			case '.': return '>';
			case '/': return '?';
			case '`': return '~';
			case '[': return '{';
			case ']': return '}';
			case '\\': return '|';
			case '\'': return '\"';
		}
	}
	if(kev.meta & EVM_KEY_CAPSLOCK)
	{
		if(base >= 'a' && base <= 'z')
			base -= ('a' - 'A');
		else if(base >= 'A' && base <= 'Z')
			base += ('a' - 'A');
	}
	return base; // a-z, A-Z, \t, \n, space
}

#define CHAR_BLOCK ((char)219)
const CnsMgr::Event CnsMgr::KEY_EVENT_ENTER = {EVV_KEY_ENTER, EVM_KEY_DOWN, EVT_KEY};
bool CnsMgr::GetLine(std::string *str, uint8_t sx, uint8_t sy, uint16_t att, const Event& end, int8_t max, char show)
{
	if(!initialized)
		return false;
	if(sx > WND_W || sy >= WND_H) // > for sx, so that u can use it to get enter when at end of line
		return false;
	
	if(max == -1)
		max = WND_H - sx;
	Event my_end = end;
	if(my_end.type != EVT_KEY)
		my_end = KEY_EVENT_ENTER;
		
	std::string print = std::string(max, ' ');
	std::string read = "";	
	
	uint8_t xpos = sx;
	
	if(xpos < sx + print.size())
		Put(CHAR_BLOCK, xpos, sy, att);
	Flip();

	Event ev;
	while(true)
	{
		ZeroMemory(&ev, sizeof(ev));
		if(!GetNextEvent(ev))
			return false;
		
		if(ev.type != EVT_KEY)
			continue;
		if(ev.val == my_end.val && ev.meta == my_end.meta)
			break;
		if(!(ev.meta & EVM_KEY_DOWN || ev.meta & EVM_KEY_HOLD))
			continue;
		
		if(ev.val == EVV_KEY_BACKSPACE)
		{
			if(!read.empty())
			{
				read = read.substr(0, read.size()-1);
				print[xpos-sx] = ' ';
				xpos--;
				PutStr(print, sx, sy, att);
				if(xpos < sx + print.size())
					Put(CHAR_BLOCK, xpos, sy, att);
				Flip();
				continue;
			}
			continue;
		}
		
		if(xpos >= sx + print.size())
			continue;

		char ch = KeyEventToChar(ev);
		if(ch == 0 || ch == '\n' || ch == '\t')
			continue;
		read += ch;
		if(show == 0)
			print[xpos-sx] = ch;
		else
			print[xpos-sx] = show;
		xpos++;
		PutStr(print, sx, sy, att);
		if(xpos < sx + print.size())
			Put(CHAR_BLOCK, xpos, sy, att);
		Flip();
	}
	
	if(str != NULL)
		*str = read;
	return true;
}
