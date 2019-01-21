// chip8emu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "chip8emu.h"
#include <iostream>
//probably more includes


void chip8emu::initialize()
{
	//Clear registers, stack, and keys
	for (int rgr = 0; rgr < 16; rgr++)
	{
		V[rgr] = 0;
		stack[rgr] = 0;
		keys[rgr] = 0;
	}
	//Clear memory
	for (int memLoc = 0; memLoc < 4096; memLoc++)
		mem[memLoc] = 0;
	//Clear display
	for (int gLoc = 0; gLoc < 2048; gLoc++)
		gfx[gLoc] = 0;

	i = 0;
	sp = 0;
	//Program counter starts at 0x200 because the first 512 bytes are reserved for the interpreter.
	pc = 0x200;
	opcode = 0;

	soundTimer = 0;
	delayTimer = 0;

	waitForKey = false;

	//Load the fontset into memory.
	for (int j = 0; j < 80; j++)
	{
		mem[j] = ch8_fontset[j];
	}
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

void chip8emu::keyEvent(int key, int type)
{
	for (int j = 0; j < 16; j++)
	{
		if (keymappings[j] == key)
		{
			std::cout << j << " was pressed." << std::endl;
			key = j;
			break;
		}
	}
	if (type == 1)
	{
		keys[key] = 1;
		if (waitForKey)
		{
			V[keySlot] = key;
			waitForKey = false;
		}
	}
	else if (type == 0)
		keys[key] = 0;
}

void chip8emu::emulateCycle()
{
	//fetch opcode
	opcode = mem[pc] << 8 | mem[pc + 1];
	//decode & execute opcode
	int x = (opcode >> 8) & 0x0F;
	int y = (opcode >> 4) & 0x0F;
	switch (opcode & 0xF000)
	{
		case 0x0000:
			switch (opcode & 0x000F)
			{
				//00E0: Clear the display.
				case 0x0000:
					for (int gLoc = 0; gLoc < 2048; gLoc++)
						gfx[gLoc] = 0;
					drawFlag = true;
					pc += 2;
					break;
				//00EE: Return from a subroutine.
				case 0x000E:
					pc = stack[sp];
					sp--;
					break;
			}
			break;
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
			if (V[x] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		//4XNN: Skip next instruction if V[X] != NN.
		case 0x4000:
			if (V[x] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		//5XY0: Skip next instruction if V[X] == V[Y].
		case 0x5000:
			if (V[x] == V[y])
				pc += 4;
			else
				pc += 2;
			break;
		//6XNN: Set V[X] to NN.
		case 0x6000:
			V[x] = opcode & 0x00FF;
			pc += 2;
			break;
		//7XNN: Add NN to V[X]. Carry flag is not changed.
		case 0x7000:
			V[x] = V[x] + (opcode & 0x00FF);
			pc += 2;
			break;
			//Does a variety of things depending on the least significant 4 bits.
		case 0x8000:
			switch (opcode & 0x000F)
			{
				//8XY0: Set V[X] = V[Y]. 
			case 0x0000:
				V[x] = V[y];
				pc += 2;
				break;
				//8XY1: Sets V[X] = V[X] | V[Y]. Bitwise OR.
			case 0x0001:
				V[x] = V[x] | V[y];
				pc += 2;
				break;
				//8XY2: Sets V[X] = V[X] & V[Y]. Bitwise AND.
			case 0x0002:
				V[x] = V[x] & V[y];
				pc += 2;
				break;
				//8XY3: Sets V[X] = V[X] ^ V[Y]. Bitwise XOR.
			case 0x0003:
				V[x] = V[x] ^ V[y];
				pc += 2;
				break;
				//8XY4: Sets V[X] = V[X] + V[Y]. Sets V[F] (Carry Flag) to 1 if there is a carry, 0 otherwise.
			case 0x0004:
				if (V[y] > (255 - V[x]))
					V[15] = 1;
				else
					V[15] = 0;
				V[x] = V[x] + V[y];

				pc += 2;
				break;
				//8XY5: Sets V[X] = V[X] - V[Y]. Sets V[F] (Carry Flag) to 0 if there is a borrow, 1 otherwise.
			case 0x0005:
				if (V[x] < V[y])
					V[15] = 0;
				else
					V[15] = 1;
				V[x] = V[x] - V[y];
				pc += 2;
				break;
				//8XY6: Stores least significant bit of V[X] in V[F], then shifts V[X] right 1.
			case 0x0006:
				V[15] = V[x] & 1;
				V[x] = V[x] >> 1;
				pc += 2;
				break;
				//8XY5: Sets V[X] = V[Y] - V[X]. Sets V[F] (Carry Flag) to 0 if there is a borrow, 1 otherwise.
			case 0x0007:
				if (V[y] < V[x])
					V[15] = 0;
				else
					V[15] = 1;
				V[x] = V[y] - V[x];
				pc += 2;
				break;
				//8XYE: Stores most siginificant bit of V[X] in V[F], then shifts V[X] left 1.
			case 0x000E:
				V[15] = V[x] >> 7;
				V[x] = V[x] << 1;
				pc += 2;
				break;
			}
			break;
		//9XY0: Skip next instruction if V[X] != V[Y].
		case 0x9000:
			if (V[x] != V[y])
				pc += 4;
			else
				pc += 2;
			break;
		//ANNN: Set I = NNN.
		case 0xA000:
			i = opcode & 0x0FFF;
			pc += 2;
			break;
		//BNNN: Jump to address NNN + V[0].
		case 0xB000:
			pc = ((opcode & 0x0FFF) + V[0]);
			break;
		//CXNN: Sets V[X] equal to the result of a bitwise AND operation on NN and a random number (usually 0-255).
		case 0xC000:
			V[x] = (opcode & 0x00FF) & (rand() % 255);
			break;
		//DXYN: Draws a sprite at (V[X], V[Y]) with height of N. The sprite data is located at memory location I. If the sprite overlaps with a display pixel already set to 1, the sprite is unset and V[F] is set to 1. Otherwise, V[F] is set to 0.
		case 0xD000:
		{
			unsigned char height = opcode & 0x000F;
			unsigned char sprite;
			V[15] = 0;
			for (int row = 0; row < height; row++)
			{
				sprite = mem[i + row];
				for (int pixel = 0; pixel < 8; pixel++)
				{
					if ((sprite & (0x80 >> pixel)) != 0)
					{
						if (gfx[((V[y] + row) * 64) + (V[x] + pixel)] == 1)
							V[15] = 1;
						gfx[((V[y] + row) * 64) + (V[x] + pixel)] ^= 1;
					}
				}
			}
			drawFlag = true;
			pc += 2;
		}
		break;
		case 0xE000:
			switch (opcode & 0x00FF)
			{
				//EX9E: Skip next instruction if the key stored in V[X] is pressed.
				case 0x009E:
					if (keys[x] != 0)
						pc += 4;
					else
						pc += 2;
					break;
				//EXA1: Skip next instruction if the key stored in V[X] is not pressed.
				case 0x00A1:
					if (keys[x] == 0)
						pc += 4;
					else
						pc += 2;
					break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF)
			{
				//FX07: Set V[X] = delayTimer.
				case 0x0007:
					V[x] = delayTimer;
					pc += 2;
					break;
				//FX0A: Wait for a key press, blocking all operation until a key press event. Store that key in V[X]. This is done via a public variable, waitForKey, that disallows calling emulateCycle until a key press event has been detected. X is recorded as keySlot.
				case 0x000A:
				{
					keySlot = (opcode >> 8) & 0x0F;
					waitForKey = true;
				}
				break;
				//FX15: Set delayTimer = V[X].
				case 0x0015:
					delayTimer = V[x];
					pc += 2;
					break;
				//FX18: Set soundTimer = V[X].
				case 0x0018:
					soundTimer = V[x];
					pc += 2;
					break;
				//FX1E: Set I = I + V[X].
				case 0x001E:
					i = i + V[x];
					pc += 2;
					break;
				//FX29: Set I to the location of the sprite corresponding to the character stored in V[X].
				case 0x0029:
					i = V[x] * 5;
					pc += 2;
					break;
				//FX33: Stores the BCD of V[X]. The hundreds digit goes into memory location I, tens in I+1, ones in I+2.
				case 0x0033:
				{
					unsigned char ones = V[x];
					unsigned char hundreds = ones / 100;
					unsigned char tens;
					ones -= hundreds * 100;
					tens = ones / 10;
					ones -= tens * 10;

					mem[i] = hundreds;
					mem[i + 1] = tens;
					mem[i + 2] = ones;

					pc += 2;
				}
				break;
				//FX55: Stores V[0]-V[X] into memory starting at memory location I.
				case 0x0055:
					for (int j = 0; j < x; j++)
						mem[i+j] = V[j];
					pc += 2;
					break;
				//FX65: Sets V[0]-V[X] from memory starting at memory lcoation I.
				case 0x0065:
					for (int j = 0; j < x; j++)
						V[j] = mem[i + j];
					pc += 2;
					break;
			}
			break;
	}

	//Decrement timers.
	delayTimer--;
	//If soundTimer reaches 0, play the system noise.
	if (soundTimer == 1)
		std::cout << "DING" << std::endl;
	else
		soundTimer--;
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
