#include "pch.h"
#include "chip8emu.h"
#include <iostream>

int main()
{
	unsigned char buff[4];
	int ent = 0;
	FILE* myFile;
	fopen_s(&myFile, "myFile.txt", "rb");
	std::cout << "TESTING START" << std::endl;
	while (!feof(myFile))
	{
		ent = fread_s(buff, sizeof(buff), 1, 3, myFile);
		std::cout << ent;
	}
	std::cout << "TESTING END" << std::endl;
	return 0;
}