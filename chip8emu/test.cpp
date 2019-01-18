#include "pch.h"
#include "chip8emu.h"
#include <iostream>

int main()
{
	short opcode = 0x0540;
	opcode = (opcode >> 8) & 0x0F;
	std::cout << "hello" << std::endl;
	std::cout << opcode << std::endl;
	std::cout << "bye" << std::endl;
}