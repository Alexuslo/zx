#ifndef __Z80EMU_INCLUDED__
#define __Z80EMU_INCLUDED__

#include "../common.h"
#include "DefineData.h"

class CIo;
class CMemory;

union CRegisters
{
	unsigned char   Byte[14];
	unsigned short  Word[7];
};

struct CState
{
	int Status;
	
	CRegisters Registers;

	unsigned short  Alternates[4];
	int i, r, pc, iff1, iff2, im;

	void *RegisterTable[16];
	void *DDRegisterTable[16];
	void *FDRegisterTable[16];
};

struct CContext
{
	CState *CpuState;
	unsigned char *Memory;
	CIo *Io;
	int IsDone;
};

class CCpu
{
private:
	
	int EmulateCpu(int Opcode, int ElapsedCycles, int nCycles);
	bool m_bIsTapeStart = false;

public:
	CContext *Context;
	CCpu(CMemory *Memory, CIo *Io);
	~CCpu();

	void SystemCall(CContext *Context);
	void Reset();
	void Rebild();
	int Int (int DataOnBus);
	int Nmi();
	int Update(int nCycles);
	ubyte ReadPort(word Address);
	void WritePort(word Address, ubyte Byte);
	CContext *GetContext() { return Context; }

	void Exec(){ EmulateCpu(RETI_RETN, 1, 1); }
};
#endif