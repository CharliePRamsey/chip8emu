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
	unsigned char thing1;
	int x;
	int gameBytes = 0;
	int memLoc = 512;
	//open game with fopen, binary mode
	FILE* gameFile;
	fopen_s(&gameFile, game, "rb");
	if (gameFile == NULL)
		perror("Error Opening Game File");
	else
	{
		while (!feof(gameFile) && memLoc < 3794)
		{
			x = fread_s(&thing1, 1, 1, 1, gameFile);
			mem[memLoc] = thing1;
			gameBytes++;
			memLoc++;
		}
	}

	std::cout << "Game File Read. Number of Bytes: " << gameBytes << std::endl;
	//load game into memory starting at byte 0x200
}

void chip8emu::emulateCycle()
{
	//fetch opcode
	opcode = mem[pc] << 8 | mem[pc + 1];
	//decode & execute opcode
	int x;
	int y;
	switch (opcode & 0xF000)
	{
		//1NNN: Jump to address NNN.
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		//2NNN: Call subroutine at NNN. Must push current program counter to stack for return.
		case 0x2000:
			stack[sp] = pc + 2;
			sp++;
			pc = opcode & 0x0FFF;
			break;
		//3XNN: Skip next instruction if V[X] == NN.
		case 0x3000:
			x = (opcode >> 8) & 0x0F;
			if (V[x] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		//4XNN: Skip next instruction if V[X] != NN.
		case 0x4000:
			x = (opcode >> 8) & 0x0F;
			if (V[x] != opcode & 0x00FF)
				pc += 4;
			else
				pc += 2;
			break;
		//5XY0: Skip next instruction if V[X] == V[Y].
		case 0x5000:
			x = (opcode >> 8) & 0x0F;
			y = (opcode >> 4) & 0x0F;
			if (V[x] == V[y])
				pc += 4;
			else
				pc += 2;
			break;
		//6XNN: Set V[X] to NN.
		case 0x6000:
			x = (opcode >> 8) & 0x0F;
			V[x] = opcode & 0x00FF;
			pc += 2;
			break;
		//7XNN: Add NN to V[X]. Carry flag is not changed.
		case 0x7000:
			x = (opcode >> 8) & 0x0F;
			V[x] = V[x] + (opcode & 0x00FF);
			pc += 2;
			break;
		//Does a variety of things depending on the least significant 4 bits.
		case 0x8000:
			x = (opcode >> 8) & 0x0F;
			y = (opcode >> 4) & 0x0F;
			switch (opcode & 0x000F)
			{
				//8XY0: Set V[x] = V[Y]. 
				case 0x0000:
					V[x] = V[y];
					pc += 2;
					break;
				//ENDING HERE FOR THE NIGHT. NEXT STEP IS OPCODE 8XY1.
			}
			break;
	}

	//decrement timers
	delayTimer--;
	if (soundTimer == 1)
		std::cout << "DING" << std::endl;
	else
		soundTimer--;
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
