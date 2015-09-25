/*
 * File: cca.h
 * Edited: 24 Sep 2015
 * Author: Matthew Bauer
 */
#pragma once

#include <string>
#include <stdint.h>
#include <windows.h>

#define RET_IF(x, r) if(x) return r;
#define RET_IF_V(x) if(x) return;

struct cyerr
{
	std::string msg;
	int val;
	cyerr(const std::string m, int v)
	{
		msg = m;
		val = v;
	}
}

// output manger
const uint8_t WND_W = 50; // window/buffer is 50 chars wide
const uint8_t WND_H = 50;

const uint16_t ATT_FG_BLACK = 0;
const uint16_t ATT_FG_RED = 1<<0;
const uint16_t ATT_FG_GREEN = 1<<1;
const uint16_t ATT_FG_BLUE = 1<<2;
const uint16_t ATT_FG_YELLOW = ATT_FG_RED | ATT_FG_GREEN;
const uint16_t ATT_FG_MAGENTA = ATT_FG_RED | ATT_FG_BLUE;
const uint16_t ATT_FG_CYAN = ATT_FG_GREEN | ATT_FG_BLUE;
const uint16_t ATT_FG_WHITE = ATT_FG_RED | ATT_FG_GREEN | ATT_FG_BLUE;
const uint16_t ATT_BG_BLACK = 0;
const uint16_t ATT_BG_RED = 1<<3;
const uint16_t ATT_BG_GREEN = 1<<4;
const uint16_t ATT_BG_BLUE = 1<<5;
const uint16_t ATT_BG_YELLOW = ATT_BG_RED | ATT_BG_GREEN;
const uint16_t ATT_BG_MAGENTA = ATT_BG_RED | ATT_BG_BLUE;
const uint16_t ATT_BG_CYAN = ATT_BG_GREEN | ATT_BG_BLUE;
const uint16_t ATT_BG_WHITE = ATT_BG_RED | ATT_BG_GREEN | ATT_BG_BLUE;
const uint16_t ATT_FG_BRIGHT = 1<<6;
const uint16_t ATT_BG_BRIGHT = 1<<7;
const uint16_t ATT_UNDERSCORE = 1<<8;

const uint16_t EVV_KEY_A = 'a';
const uint16_t EVV_KEY_B = 'b';
const uint16_t EVV_KEY_C = 'c';
const uint16_t EVV_KEY_D = 'd';
const uint16_t EVV_KEY_E = 'e';
const uint16_t EVV_KEY_F = 'f';
const uint16_t EVV_KEY_G = 'g';
const uint16_t EVV_KEY_H = 'h';
const uint16_t EVV_KEY_I = 'i';
const uint16_t EVV_KEY_J = 'j';
const uint16_t EVV_KEY_K = 'k';
const uint16_t EVV_KEY_L = 'l';
const uint16_t EVV_KEY_M = 'm';
const uint16_t EVV_KEY_N = 'n';
const uint16_t EVV_KEY_O = 'o';
const uint16_t EVV_KEY_P = 'p';
const uint16_t EVV_KEY_Q = 'q';
const uint16_t EVV_KEY_R = 'r';
const uint16_t EVV_KEY_S = 's';
const uint16_t EVV_KEY_T = 't';
const uint16_t EVV_KEY_U = 'u';
const uint16_t EVV_KEY_V = 'v';
const uint16_t EVV_KEY_W = 'w';
const uint16_t EVV_KEY_X = 'x';
const uint16_t EVV_KEY_Y = 'y';
const uint16_t EVV_KEY_Z = 'z';
const uint16_t EVV_KEY_TAB = '\t';
const uint16_t EVV_KEY_ENTER = '\n';
const uint16_t EVV_KEY_SHIFT = 1000;
const uint16_t EVV_KEY_CONTROL = 1001;
const uint16_t EVV_KEY_ALT = 1002;
const uint16_t EVV_KEY_CAPSLOCK = 1003;
const uint16_t EVV_KEY_ESCAPE = 1004;
const uint16_t EVV_KEY_SPACE = ' ';
const uint16_t EVV_KEY_PGDOWN = 1005;
const uint16_t EVV_KEY_PGUP = 1006;
const uint16_t EVV_KEY_HOME = 1007;
const uint16_t EVV_KEY_END = 1008;
const uint16_t EVV_KEY_LEFT = 1009;
const uint16_t EVV_KEY_RIGHT = 1010;
const uint16_t EVV_KEY_UP = 1011;
const uint16_t EVV_KEY_DOWN = 1012;
const uint16_t EVV_KEY_PRINTSCRN = 1013;
const uint16_t EVV_KEY_INSERT = 1014;
const uint16_t EVV_KEY_DELETE = 1015;
const uint16_t EVV_KEY_0 = '0';
const uint16_t EVV_KEY_1 = '1';
const uint16_t EVV_KEY_2 = '2';
const uint16_t EVV_KEY_3 = '3';
const uint16_t EVV_KEY_4 = '4';
const uint16_t EVV_KEY_5 = '5';
const uint16_t EVV_KEY_6 = '6';
const uint16_t EVV_KEY_7 = '7';
const uint16_t EVV_KEY_8 = '8';
const uint16_t EVV_KEY_9 = '9';
const uint16_t EVV_KEY_NUMPAD0 = '0' + 2000;
const uint16_t EVV_KEY_NUMPAD1 = '1' + 2000;
const uint16_t EVV_KEY_NUMPAD2 = '2' + 2000;
const uint16_t EVV_KEY_NUMPAD3 = '3' + 2000;
const uint16_t EVV_KEY_NUMPAD4 = '4' + 2000;
const uint16_t EVV_KEY_NUMPAD5 = '5' + 2000;
const uint16_t EVV_KEY_NUMPAD6 = '6' + 2000;
const uint16_t EVV_KEY_NUMPAD7 = '7' + 2000;
const uint16_t EVV_KEY_NUMPAD8 = '8' + 2000;
const uint16_t EVV_KEY_NUMPAD9 = '9' + 2000;
const uint16_t EVV_KEY_F1 = 1018;
const uint16_t EVV_KEY_F2 = 1019;
const uint16_t EVV_KEY_F3 = 1020;
const uint16_t EVV_KEY_F4 = 1021;
const uint16_t EVV_KEY_F5 = 1022;
const uint16_t EVV_KEY_F6 = 1023;
const uint16_t EVV_KEY_F7 = 1024;
const uint16_t EVV_KEY_F8 = 1025;
const uint16_t EVV_KEY_F9 = 1026;
const uint16_t EVV_KEY_F10 = 1027;
const uint16_t EVV_KEY_F11 = 1028;
const uint16_t EVV_KEY_F12 = 1029;
const uint16_t EVV_KEY_NUMLOCK = 1030;
const uint16_t EVV_KEY_SCRLOCK = 1031;
const uint16_t EVV_KEY_COLONS = ';'; // the ;/: key
const uint16_t EVV_KEY_EQ_PLUS = '='; // the =/+ key
const uint16_t EVV_KEY_MINS_USCR = '-'; // the -/_ key
const uint16_t EVV_KEY_COMMA_LANG = ','; // the ,/< key
const uint16_t EVV_KEY_PERIOD_RANG = '.'; // the ./> key
const uint16_t EVV_KEY_SLASH_QM = '/'; // the //? key
const uint16_t EVV_KEY_TQ_TILDE = '~'; // `/~ key
const uint16_t EVV_KEY_LSB_LCB = '['; // the [/{ key
const uint16_t EVV_KEY_RSB_RCB = ']'; // the ]/} key
const uint16_t EVV_KEY_BS_VLINE = '\\'; // the \/| key
const uint16_t EVV_KEY_SQ_DQ = '\''; // the '/" key

class CnsMgr
{
	public:
		class Event
		{
			public:
				uint16_t val;
				uint16_t meta;
				uint8_t type;
		}
	
	private:
		bool initialized;
		
		HANDLE h_stdout;
		HANDLE h_stdin; DWORD ori_stdin_mode;
		HANDLE h_mysb;
		
		char *ori_title;
		bool title_changed;
		
		CHAR_INFO *buf;
		
		WORD TranslateAttributes(uint16_t att);
		uint16_t TranslateWindowsVK(WORD vk);
		
		std::vector<Event> ev_q;
	
	public:
		CnsMgr();
		CnsMgr(const OutMgr&); // throws
		~CnsMgr();
		
		void Init();
		void Kill();
		
		bool Put(char c, uint8_t x, uint8_t y, uint16_t att);
		bool Flip();
		
		bool GetNextEvent(Event& ev); // returns false if empty
}
