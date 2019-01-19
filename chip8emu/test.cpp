#include "pch.h"
#include "chip8emu.h"

int main()
{
	srand(time(NULL));
	chip8emu ch8;
	ch8.initialize();
}