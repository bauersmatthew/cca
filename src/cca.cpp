/*
 * File: cca.cpp
 * Edited: 23 Sep 2015
 * Author: Matthew Bauer
 */

// includes
#include <cca.h>

// main
int main(int argc, char **argv)
{
	
}

// CD: OutMgr
OutMgr::OutMgr()
{
	initialized = false;
}
OutMgr::OutMgr(const OutMgr&)
{
	throw cyerr("do not copy OutMgr objects!", 8001);
}
OutMgr::~OutMgr()
{
	if(initialized)
		Kill();
}
void OutMgr::Init()
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
	{ // screen buffer info (ex)
		CONSOLE_SCREEN_BUFFER_INFOEX csbi;
		csbi.cbSize = sizeof(csbi);
		csbi.dwSize = {50, 50};
		csbi.dwCursorPosition = {0, 0};
		csbi.srWindow = {0, 0, 49, 49};
		if(!SetConsoleScreenBufferInfoEx(h_mysb, &csbi))
		{
			throw cyerr("failed to set screen buffer/window sizes", 8006);
		}
	}
	{
		
	}
}
