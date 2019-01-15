// chip8emu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "chip8emu.h"
//probably more includes


void chip8emu::initialize()
{
	for (int rgr = 0; rgr < 16; rgr++) //Clear registers, stack, and keys
	{
		V[rgr] = 0;
		stack[rgr] = 0;
		keys[rgr] = 0;
	}
	for (int memLoc = 0; memLoc < 4096; memLoc++) //Clear memory
		mem[memLoc] = 0;
	for (int gLoc = 0; gLoc < 2048; gLoc++) //Clear display
		gfx[gLoc] = 0;

	i = 0;
	sp = 0;
	pc = 0x200; //Program counter starts at 0x200 because the first 512 bytes are reserved for the interpreter
	opcode = 0;

	soundTimer = 0;
	delayTimer = 0;
}

void chip8emu::loadGame(const char* game)
{
	int memLoc = 0;
	int buffer = 0;
	//open game with fopen, binary mode
	FILE* gameFile;
	fopen_s(&gameFile, game, "rb");
	if (gameFile == NULL)
		perror("Error Opening Game File");
	else
	{
		while (!feof(gameFile))
		{
			buffer = fgetc(gameFile);
			if (buffer >= 0)
				mem[memLoc] = buffer;
			memLoc++;
		}
	}
	//load game into memory starting at byte 0x200
}

void chip8emu::emulateCycle()
{
	//fetch opcode
	//decode opcode
	//execute opcode

	//decrement timers
	//increment pc
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
