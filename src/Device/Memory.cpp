#include "Memory.h"

CMemory::CMemory() { }
CMemory::~CMemory() { }

CMemory48::CMemory48()
{
	file::buffer b;
	int fileSize = file::read("f/48.rom", b);
	
	file::makeDirectory("f");

	Memory = new unsigned char[RamSize48];
	
	for (int i = 0; i < RamSize48; i++)
		Memory[i] = i < b.size() ? b[i] : 0;

	srand(time(0));
}

CMemory48::~CMemory48() { }

ubyte CMemory48::ReadByte(word Address)
{
	assert(Address > RomSize48 &&Address < RamSize48);
	return Memory[Address];
}

void CMemory48::WriteByte(word Address, ubyte Byte)
{
	assert(Address > 16383);
	assert(Address > RomSize48 &&Address < RamSize48);
	Memory[Address] = Byte;
}

CMemory128::CMemory128() { }
CMemory128::~CMemory128() { }

CMemoryCustom::CMemoryCustom() { }
CMemoryCustom::~CMemoryCustom() { }

