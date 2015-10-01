/*
 * File: el.cpp
 * Edited: 30 Sep 2015
 * Author: Matthew Bauer
 */

#include "el.h"
#include "cca.h"

// FD
El *LoadElementFromType(uint16_t t)
{
	switch(t)
	{
		case ELT_WALL_V1:
			return new ElWall_V1;
		case ELT_WALL_H1:
			return new ElWall_H1;
		case ELT_WALL_F1:
			return new El
	}
}

// CD: El
El::El()
{
	disp = 0;
	att = 0;
}
uint16_t El::GetType()
{
	return ELT_INVALID;
}
char El::GetChar()
{
	return disp;
}
char El::GetAtt()
{
	return att;
}

// CD: ElWall_V1
ElWall_V1::ElWall_V1()
{
	disp = '|';
	att = ATT_FG_WHITE;
}
uint16_t ElWall_V1::GetType()
{
	return ELT_WALL_V1;
}

 // CD: ElWall_H1
ElWall_H1::ElWall_H1()
{
 	disp = '-';
	att = ATT_FG_WHITE;
}
uint16_t ElWall_H1::GetType()
{
	return ELT_WALL_H1;
}

// CD: ElWall_F1
ElWall_F1::ElWall_F1()
{
	disp = '+';
	att = ATT_FG_WHITE;
}
uint16_t ElWall_F1::GetType()
{
	return ELT_WALL_F1;
}
