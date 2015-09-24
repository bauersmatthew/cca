/*
 * File: cca.h
 * Edited: 23 Sep 2015
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
class OutMgr
{
	private:
		bool initialized;
		
		HANDLE h_stdout;
		HANDLE h_stdin;
		HANDLE h_mysb;
	
	public:
		OutMgr();
		OutMgr(const OutMgr&); // throws
		~OutMgr();
		
		void Init();
		void Kill();
		
		void Flip();
}
