/*
 * File: el.h
 * Edited: 30 Sep 2015
 * Author: Matthew Bauer
 */
#pragma once

// element types
const uint16_t ELT_INVALID = 0;
const uint16_t ELT_WALL_V1 = 1;
const uint16_t ELT_WALL_H1 = 2;
const uint16_t ELT_WALL_F1 = 3;

// eh
class El;

// load element from id
El *LoadElementFromType(uint16_t t);

// game element
class El
{
	protected:
		char disp;
		uint16_t att;
	
	public:
		virtual El();
		
		virtual uint16_t GetType();
		
		char GetChar();
		uint16_t GetAtt();
};

class ElWall_V1 : public El
{
	ElWall_V1();
	uint16_t GetType();
};
class ElWall_H1 : public El
{
	ElWall_H1();
	uint16_t GetType()
};
class ElWall_F1 : public El
{
	ElWall_F1();
	uint16_t GetType();
}
