#pragma once
#include <string>
#include <iostream>
#include <conio.h>
#include <time.h>

class chip8emu
{
private:
	/*Chip-8 has a memory of 4096 bytes. The first 512 bytes are used for the Chip-8 interpreter, 
	the last 256 bytes are used for display refresh, and the 96 below that are for the call stack, 
	internal use, and other variables.*/
	unsigned char mem[4096];

	/*Chip-8 display has a resolution of 64x32. Each pixel is either on or off; monochrome. Graphics
	are drawn using sprites: 8 pixels wide and 1-15 pixels tall. Sprites are XOR'd with corresponding 
	screen-space pixels. If a screen pixel is set from 1 to 0 as a result of a sprite pixel, the carry 
	flag (VF) is set to 1 to represent collision.*/
	unsigned char gfx[2048];

	//Chip-8 has 16 8-bit registers from V0-VF. VF is also used as a flag in some circumstances.
	unsigned char V[16];
	//The address register is called I
	unsigned short i;
	//pc is the Program Counter
	unsigned short pc;
	//The stack and stack pointer are used to store return addresses for subroutine calls
	unsigned short stack[16];
	unsigned short sp;
	//Opcode tells the Chip-8 which instruction to execute, as well as other necessary information
	unsigned short opcode;
	//Timers count down at 60 Hz until they reach 0. When the sound timer is 0, the system plays its buzzer noise.
	unsigned char delayTimer;
	unsigned char soundTimer;

	unsigned char keys[16];

	unsigned char ch8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
public:
	void initialize();
	void loadGame(const char* game);
	void emulateCycle();
	void setKeys();
	bool drawFlag;
};