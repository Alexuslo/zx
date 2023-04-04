#pragma once
#include"../common.h"

class CMemory
{
protected:
	unsigned char *Memory;

public:
	CMemory();
	~CMemory();

	virtual ubyte *GetMemoryPtr() { return nullptr; }
	virtual ubyte *GetVideoMemoryPtr() { return nullptr; }
	virtual ubyte *GetAttrMemoryPtr() { return nullptr; }
};

class CMemory48 : public CMemory
{
protected:
	ubyte BuffByte;

	bool bIsProtectWriteRom = false;

public:
	CMemory48();
	~CMemory48();

	ubyte *GetMemoryPtr() { return Memory; }
	ubyte *GetVideoMemoryPtr() { return &Memory[16384]; }
	ubyte *GetAttrMemoryPtr() { return &Memory[16384 + 6144]; }

	ubyte ReadByte(word Address);
	void WriteByte(word Address, ubyte Byte);
};

class CMemory128 : public CMemory
{
protected:


public:
	CMemory128();
	~CMemory128();
};

class CMemoryCustom : public CMemory
{
protected:


public:
	CMemoryCustom();
	~CMemoryCustom();
};


