#include "Cpu.h"
#include "../Device/Io.h"
#include "../Device/Memory.h"

#define Z80_CPU_SPEED           4000000   /* In Hz. */
#define CYCLES_PER_STEP         (Z80_CPU_SPEED / 50)
#define MAXIMUM_STRING_LENGTH   100

#define INDIRECT_HL     0x06

static const int XOR_CONDITION_TABLE[8] = { Z80_Z_FLAG, 0, Z80_C_FLAG, 0, Z80_P_FLAG, 0, Z80_S_FLAG, 0 };
static const int AND_CONDITION_TABLE[8] = { Z80_Z_FLAG, Z80_Z_FLAG, Z80_C_FLAG, Z80_C_FLAG, Z80_P_FLAG, Z80_P_FLAG, Z80_S_FLAG, Z80_S_FLAG };
static const int RST_TABLE[8] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
static const int OVERFLOW_TABLE[4] = { 0, Z80_V_FLAG, Z80_V_FLAG, 0 };

CCpu::CCpu(CMemory *Memory, CIo *Io)
{
	Context = new CContext;
	Context->CpuState = new CState;
	Context->Memory = Memory->GetMemoryPtr();
	Context->Io = Io;
}

CCpu::~CCpu() { }

void CCpu::Reset()
{
	CState *State = Context->CpuState;
	State->Status = 0;
	AF = 0xffff;
	SP = 0xffff;
	State->i = State->pc = State->iff1 = State->iff2 = 0;
	State->im = Z80_INTERRUPT_MODE_0;

	State->RegisterTable[0]  = &State->Registers.Byte[Z80_B ];
	State->RegisterTable[1]  = &State->Registers.Byte[Z80_C ];
	State->RegisterTable[2]  = &State->Registers.Byte[Z80_D ];
	State->RegisterTable[3]  = &State->Registers.Byte[Z80_E ];
	State->RegisterTable[4]  = &State->Registers.Byte[Z80_H ];
	State->RegisterTable[5]  = &State->Registers.Byte[Z80_L ];   
	State->RegisterTable[6]  = &State->Registers.Word[Z80_HL];
	State->RegisterTable[7]  = &State->Registers.Byte[Z80_A ];
	State->RegisterTable[8]  = &State->Registers.Word[Z80_BC];
	State->RegisterTable[9]  = &State->Registers.Word[Z80_DE];
	State->RegisterTable[10] = &State->Registers.Word[Z80_HL];
	State->RegisterTable[11] = &State->Registers.Word[Z80_SP];
	State->RegisterTable[12] = &State->Registers.Word[Z80_BC];
	State->RegisterTable[13] = &State->Registers.Word[Z80_DE];
	State->RegisterTable[14] = &State->Registers.Word[Z80_HL];
	State->RegisterTable[15] = &State->Registers.Word[Z80_AF];

	for (int i = 0; i < 16; i++)
		State->DDRegisterTable[i] = State->FDRegisterTable[i] = State->RegisterTable[i];

	State->DDRegisterTable[4] = &State->Registers.Byte[Z80_IXH];
	State->DDRegisterTable[5] = &State->Registers.Byte[Z80_IXL];
	State->DDRegisterTable[6] = &State->Registers.Word[Z80_IX];
	State->DDRegisterTable[10] = &State->Registers.Word[Z80_IX];
	State->DDRegisterTable[14] = &State->Registers.Word[Z80_IX];

	State->FDRegisterTable[4] = &State->Registers.Byte[Z80_IYH];
	State->FDRegisterTable[5] = &State->Registers.Byte[Z80_IYL];
	State->FDRegisterTable[6] = &State->Registers.Word[Z80_IY];
	State->FDRegisterTable[10] = &State->Registers.Word[Z80_IY];
	State->FDRegisterTable[14] = &State->Registers.Word[Z80_IY];
}

void CCpu::Rebild()
{
	CState* State = Context->CpuState;
	State->RegisterTable[0] = &State->Registers.Byte[Z80_B];
	State->RegisterTable[1] = &State->Registers.Byte[Z80_C];
	State->RegisterTable[2] = &State->Registers.Byte[Z80_D];
	State->RegisterTable[3] = &State->Registers.Byte[Z80_E];
	State->RegisterTable[4] = &State->Registers.Byte[Z80_H];
	State->RegisterTable[5] = &State->Registers.Byte[Z80_L];
	State->RegisterTable[6] = &State->Registers.Word[Z80_HL];
	State->RegisterTable[7] = &State->Registers.Byte[Z80_A];
	State->RegisterTable[8] = &State->Registers.Word[Z80_BC];
	State->RegisterTable[9] = &State->Registers.Word[Z80_DE];
	State->RegisterTable[10] = &State->Registers.Word[Z80_HL];
	State->RegisterTable[11] = &State->Registers.Word[Z80_SP];
	State->RegisterTable[12] = &State->Registers.Word[Z80_BC];
	State->RegisterTable[13] = &State->Registers.Word[Z80_DE];
	State->RegisterTable[14] = &State->Registers.Word[Z80_HL];
	State->RegisterTable[15] = &State->Registers.Word[Z80_AF];

	for (int i = 0; i < 16; i++)
		State->DDRegisterTable[i] = State->FDRegisterTable[i] = State->RegisterTable[i];

	State->DDRegisterTable[4] = &State->Registers.Byte[Z80_IXH];
	State->DDRegisterTable[5] = &State->Registers.Byte[Z80_IXL];
	State->DDRegisterTable[6] = &State->Registers.Word[Z80_IX];
	State->DDRegisterTable[10] = &State->Registers.Word[Z80_IX];
	State->DDRegisterTable[14] = &State->Registers.Word[Z80_IX];

	State->FDRegisterTable[4] = &State->Registers.Byte[Z80_IYH];
	State->FDRegisterTable[5] = &State->Registers.Byte[Z80_IYL];
	State->FDRegisterTable[6] = &State->Registers.Word[Z80_IY];
	State->FDRegisterTable[10] = &State->Registers.Word[Z80_IY];
	State->FDRegisterTable[14] = &State->Registers.Word[Z80_IY];
}

int CCpu::Int(int DataOnBus)
{
	//return 0;
	CState *State = Context->CpuState;
	State->Status = 0;
	if (State->iff1) {

		State->iff1 = State->iff2 = 0;
		State->r = (State->r & 0x80) | ((State->r + 1) & 0x7f);
		switch (State->im)
		{
		case Z80_INTERRUPT_MODE_0:
		{
			//Assuming the opcode in data_on_bus is an RST instruction, accepting the interrupt should take 2 + 11 = 13 cycles.
			return EmulateCpu(DataOnBus, 2, 4);
		}

		case Z80_INTERRUPT_MODE_1:
		{
			int	ElapsedCycles;
			ElapsedCycles = 0;
			SP -= 2;
			Z80_WRITE_WORD_INTERRUPT(SP, State->pc);
			State->pc = 0x0038;

			return ElapsedCycles + 13;
		}

		case Z80_INTERRUPT_MODE_2:

		default:
		{
			int	ElapsedCycles;
			int vector;
			ElapsedCycles = 0;
			SP -= 2;
			Z80_WRITE_WORD_INTERRUPT(SP, State->pc);
			vector = State->i << 8 | DataOnBus;
#ifdef Z80_MASK_IM2_VECTOR_ADDRESS
			vector &= 0xfffe;
#endif
			Z80_READ_WORD_INTERRUPT(vector, State->pc);

			return ElapsedCycles + 19;
		}
		}
	}
	else

		return 0;
}

int CCpu::Nmi()
{
	CState *State = Context->CpuState;
	int	ElapsedCycles;
	State->Status = 0;
	State->iff2 = State->iff1;
	State->iff1 = 0;
	State->r = (State->r & 0x80) | ((State->r + 1) & 0x7f);
	ElapsedCycles = 0;
	SP -= 2;
	Z80_WRITE_WORD_INTERRUPT(SP, State->pc);
	State->pc = 0x0066;

	return ElapsedCycles + 11;
}

ubyte CCpu::ReadPort(word Address) { return Context->Io->ReadPort(Address); }

void CCpu::WritePort(word Address, ubyte Byte) { Context->Io->WritePort(Address, Byte); }

void CCpu::SystemCall(CContext *Context)
{
	
	if (Context->CpuState->Registers.Byte[Z80_C] == 2)

		printf("%c", Context->CpuState->Registers.Byte[Z80_E]);

	else if (Context->CpuState->Registers.Byte[Z80_C] == 9) {

		int     i, c;

		for (i = Context->CpuState->Registers.Word[Z80_DE], c = 0;
			Context->Memory[i] != '$';
			i++) {

			printf("%c", Context->Memory[i & 0xffff]);
			if (c++ > MAXIMUM_STRING_LENGTH) {

				fprintf(stderr,
					"String to print is too long!\n");
				exit(EXIT_FAILURE);

			}

		}

	}
}

int CCpu::Update(int nCycles)
{
	int Total = 0;
	do
	{
		int ElapsedCycles = 0;
		int PC = Context->CpuState->pc;
		int Opcode;

		Context->CpuState->Status = 0;

		Z80_FETCH_BYTE(PC, Opcode);
		Context->CpuState->pc = PC + 1;

		Total += EmulateCpu(Opcode, ElapsedCycles, nCycles);
	} while (!Context->IsDone);
	return Total;
}

int CCpu::EmulateCpu(int Opcode, int ElapsedCycles, int nCycles)
{
	CState *State = Context->CpuState;
		
		int pc = State->pc;
		int r = State->r & 0x7f;
		goto start_emulation;

		for (; ; )
		{

			void    **Registers;
			int     Instruction;

			Z80_FETCH_BYTE(pc, Opcode);
			pc++;

		start_emulation:

			Registers = State->RegisterTable;

		emulate_next_opcode:

			Instruction = INSTRUCTION_TABLE[Opcode];

		emulate_next_instruction:

			ElapsedCycles += 4;
			r++;
			switch (Instruction)
			{
				//8-bit load group.
#if 1
			case LD_R_R:
			{
				R(Y(Opcode)) = R(Z(Opcode));
				break;
			}

			case LD_R_N:
			{
				READ_N(R(Y(Opcode)));
				break;
			}

			case LD_R_INDIRECT_HL:
			{
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, R(Y(Opcode)));
				}
				else
				{
					int d;
					READ_D(d);
					d += HL_IX_IY;
					READ_BYTE(d, S(Y(Opcode)));
					ElapsedCycles += 5;
				}
				break;
			}

			case LD_INDIRECT_HL_R:
			{
				if (Registers == State->RegisterTable)
				{
					WRITE_BYTE(HL, R(Z(Opcode)));
				}
				else
				{
					int     d;
					READ_D(d);
					d += HL_IX_IY;
					WRITE_BYTE(d, S(Z(Opcode)));
					ElapsedCycles += 5;
				}
				break;
			}

			case LD_INDIRECT_HL_N:
			{
				int     n;
				if (Registers == State->RegisterTable)
				{
					READ_N(n);
					WRITE_BYTE(HL, n);
				}
				else {
					int     d;
					READ_D(d);
					d += HL_IX_IY;
					READ_N(n);
					WRITE_BYTE(d, n);
					ElapsedCycles += 2;
				}
				break;
			}

			case LD_A_INDIRECT_BC:
			{
				READ_BYTE(BC, A);
				break;
			}

			case LD_A_INDIRECT_DE:
			{
				READ_BYTE(DE, A);
				break;
			}

			case LD_A_INDIRECT_NN:
			{
				int     nn;
				READ_NN(nn);
				READ_BYTE(nn, A);
				break;
			}

			case LD_INDIRECT_BC_A:
			{
				WRITE_BYTE(BC, A);
				break;
			}

			case LD_INDIRECT_DE_A:
			{
				WRITE_BYTE(DE, A);
				break;
			}

			case LD_INDIRECT_NN_A:
			{
				int     nn;
				READ_NN(nn);
				WRITE_BYTE(nn, A);
				break;
			}

			case LD_A_I_LD_A_R:
			{
				int a;
				int f;
				a = Opcode == OPCODE_LD_A_I ? State->i : (State->r & 0x80) | (r & 0x7f);
				f = SZYX_FLAGS_TABLE[a];

				//Note: On a real processor, if an interrupt occurs during the execution of either 
				//"LD A, I" or "LD A, R", the parity flag is reset. That can never happen here.

				f |= State->iff2 << Z80_P_FLAG_SHIFT;
				f |= F & Z80_C_FLAG;
				AF = (a << 8) | f;
				ElapsedCycles++;
				break;
			}

			case LD_I_A_LD_R_A:
			{
				if (Opcode == OPCODE_LD_I_A)
				{
					State->i = A;
				}
				else
				{
					State->r = A;
					r = A & 0x7f;
				}
				ElapsedCycles++;
				break;
			}
#endif		
			//16-bit load group.
#if 2
			case LD_RR_NN:
			{
				READ_NN(RR(P(Opcode)));
				break;
			}

			case LD_HL_INDIRECT_NN:
			{
				int nn;
				READ_NN(nn);
				READ_WORD(nn, HL_IX_IY);
				break;
			}

			case LD_RR_INDIRECT_NN:
			{
				int nn;
				READ_NN(nn);
				READ_WORD(nn, RR(P(Opcode)));
				break;
			}

			case LD_INDIRECT_NN_HL:
			{
				int nn;
				READ_NN(nn);
				WRITE_WORD(nn, HL_IX_IY);
				break;
			}

			case LD_INDIRECT_NN_RR:
			{
				int nn;
				READ_NN(nn);
				WRITE_WORD(nn, RR(P(Opcode)));
				break;
			}

			case LD_SP_HL:
			{
				SP = HL_IX_IY;
				ElapsedCycles += 2;
				break;
			}

			case PUSH_SS:
			{
				PUSH(SS(P(Opcode)));
				ElapsedCycles++;
				break;
			}

			case POP_SS:
			{
				POP(SS(P(Opcode)));
				break;
			}
#endif	
			//Exchange, block transfer and search group.
#if 3
			case EX_DE_HL:
			{
				EXCHANGE(DE, HL);
				break;
			}

			case EX_AF_AF_PRIME:
			{
				EXCHANGE(AF, State->Alternates[Z80_AF]);
				break;
			}

			case EXX:
			{
				EXCHANGE(BC, State->Alternates[Z80_BC]);
				EXCHANGE(DE, State->Alternates[Z80_DE]);
				EXCHANGE(HL, State->Alternates[Z80_HL]);
				break;
			}

			case EX_INDIRECT_SP_HL:
			{
				int t;
				READ_WORD(SP, t);
				WRITE_WORD(SP, HL_IX_IY);
				HL_IX_IY = t;
				ElapsedCycles += 3;
				break;
			}

			case LDI_LDD:
			{
				int     n, f, d;
				READ_BYTE(HL, n);
				WRITE_BYTE(DE, n);
				f = F & SZC_FLAGS;
				f |= --BC ? Z80_P_FLAG : 0;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				n += A;
				f |= n & Z80_X_FLAG;
				f |= (n << (Z80_Y_FLAG_SHIFT - 1))
					& Z80_Y_FLAG;
#endif
				F = f;
				d = Opcode == OPCODE_LDI ? +1 : -1;
				DE += d;
				HL += d;
				ElapsedCycles += 2;
				break;
			}

			case LDIR_LDDR:
			{
				int     d, f, bc, de, hl, n;
#ifdef Z80_HANDLE_SELF_MODIFYING_CODE
				int     p, q;
				p = (pc - 2) & 0xffff;
				q = (pc - 1) & 0xffff;
#endif                          
				d = Opcode == OPCODE_LDIR ? +1 : -1;
				f = F & SZC_FLAGS;
				bc = BC;
				de = DE;
				hl = HL;
				r -= 2;
				ElapsedCycles -= 8;
				for (; ; )
				{
					r += 2;
					Z80_READ_BYTE(hl, n);
					Z80_WRITE_BYTE(de, n);
					hl += d;
					de += d;
					if (--bc)
					{
						ElapsedCycles += 21;
					}
					else
					{
						ElapsedCycles += 16;
						break;
					}
#ifdef Z80_HANDLE_SELF_MODIFYING_CODE
					if (((de - d) & 0xffff) == p || ((de - d) & 0xffff) == q)
					{
						f |= Z80_P_FLAG;
						pc -= 2;
						break;
					}
#endif                                  
					if (ElapsedCycles < nCycles)
					{
						continue;
					}
					else
					{
						f |= Z80_P_FLAG;
						pc -= 2;
						break;
					}
				}
				HL = hl;
				DE = de;
				BC = bc;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				f |= n & Z80_X_FLAG;
				f |= (n << (Z80_Y_FLAG_SHIFT - 1)) & Z80_Y_FLAG;
				n += A;
#endif
				F = f;
				break;
			}

			case CPI_CPD:
			{
				int a;
				int n;
				int z;
				int f;
				a = A;
				READ_BYTE(HL, n);
				z = a - n;
				HL += Opcode == OPCODE_CPI ? +1 : -1;
				f = (a ^ n ^ z) & Z80_H_FLAG;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				n = z - (f >> Z80_H_FLAG_SHIFT);
				f |= (n << (Z80_Y_FLAG_SHIFT - 1)) & Z80_Y_FLAG;
				f |= n & Z80_X_FLAG;
#endif
				f |= SZYX_FLAGS_TABLE[z & 0xff] & SZ_FLAGS;
				f |= --BC ? Z80_P_FLAG : 0;
				F = f | Z80_N_FLAG | (F & Z80_C_FLAG);
				ElapsedCycles += 5;
				break;
			}

			case CPIR_CPDR:
			{
				int d;
				int a;
				int bc;
				int hl;
				int n;
				int z;
				int f;
				d = Opcode == OPCODE_CPIR ? +1 : -1;
				a = A;
				bc = BC;
				hl = HL;
				r -= 2;
				ElapsedCycles -= 8;
				for (; ; )
				{
					r += 2;
					Z80_READ_BYTE(hl, n);
					z = a - n;
					hl += d;
					if (--bc && z)
					{
						ElapsedCycles += 21;
					}
					else
					{
						ElapsedCycles += 16;
						break;
					}
					if (ElapsedCycles < nCycles)
					{
						continue;
					}
					else
					{
						pc -= 2;
						break;
					}
				}
				HL = hl;
				BC = bc;
				f = (a ^ n ^ z) & Z80_H_FLAG;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				n = z - (f >> Z80_H_FLAG_SHIFT);
				f |= (n << (Z80_Y_FLAG_SHIFT - 1)) & Z80_Y_FLAG;
				f |= n & Z80_X_FLAG;
#endif
				f |= SZYX_FLAGS_TABLE[z & 0xff] & SZ_FLAGS;
				f |= bc ? Z80_P_FLAG : 0;
				F = f | Z80_N_FLAG | (F & Z80_C_FLAG);
				break;
			}
#endif		
			//8-bit arithmetic and logical group.
#if 4
			case ADD_R:
			{
				ADD(R(Z(Opcode)));
				break;
			}

			case ADD_N:
			{
				int     n;
				READ_N(n);
				ADD(n);
				break;
			}

			case ADD_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				ADD(x);
				break;
			}

			case ADC_R:
			{
				ADC(R(Z(Opcode)));
				break;
			}

			case ADC_N:
			{
				int n;
				READ_N(n);
				ADC(n);
				break;
			}

			case ADC_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				ADC(x);
				break;
			}

			case SUB_R:
			{
				SUB(R(Z(Opcode)));
				break;
			}

			case SUB_N:
			{
				int n;
				READ_N(n);
				SUB(n);
				break;
			}

			case SUB_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				SUB(x);
				break;
			}

			case SBC_R:
			{
				SBC(R(Z(Opcode)));
				break;
			}

			case SBC_N:
			{
				int n;
				READ_N(n);
				SBC(n);
				break;
			}

			case SBC_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				SBC(x);
				break;
			}

			case AND_R:
			{
				AND(R(Z(Opcode)));
				break;
			}

			case AND_N:
			{
				int n;
				READ_N(n);
				AND(n);
				break;
			}

			case AND_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				AND(x);
				break;
			}

			case OR_R:
			{
				OR(R(Z(Opcode)));
				break;
			}

			case OR_N:
			{
				int n;
				READ_N(n);
				OR(n);
				break;
			}

			case OR_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				OR(x);
				break;
			}

			case XOR_R:
			{
				XOR(R(Z(Opcode)));
				break;
			}

			case XOR_N:
			{
				int n;
				READ_N(n);
				XOR(n);
				break;
			}

			case XOR_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				XOR(x);
				break;
			}

			case CP_R:
			{
				CP(R(Z(Opcode)));
				break;
			}

			case CP_N:
			{
				int n;
				READ_N(n);
				CP(n);
				break;
			}

			case CP_INDIRECT_HL:
			{
				int x;
				READ_INDIRECT_HL(x);
				CP(x);
				break;
			}

			case INC_R:
			{
				INC(R(Y(Opcode)));
				break;
			}

			case INC_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					INC(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					READ_D(d);
					d += HL_IX_IY;
					READ_BYTE(d, x);
					INC(x);
					WRITE_BYTE(d, x);
					ElapsedCycles += 6;
				}
				break;
			}

			case DEC_R:
			{
				DEC(R(Y(Opcode)));
				break;
			}

			case DEC_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					DEC(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					READ_D(d);
					d += HL_IX_IY;
					READ_BYTE(d, x);
					DEC(x);
					WRITE_BYTE(d, x);
					ElapsedCycles += 6;
				}
				break;

			}
#endif			
			//General-purpose arithmetic and CPU control group.
#if 5
			case DAA:
			{
				int a;
				int c;
				int d;

				//The following algorithm is from comp.sys.sinclair's FAQ.

				a = A;
				if (a > 0x99 || (F & Z80_C_FLAG))
				{
					c = Z80_C_FLAG;
					d = 0x60;
				}
				else
				{
					c = d = 0;
				}
				if ((a & 0x0f) > 0x09 || (F & Z80_H_FLAG))
				{
					d += 0x06;
				}
				A += F & Z80_N_FLAG ? -d : +d;
				F = SZYXP_FLAGS_TABLE[A] | ((A ^ a) & Z80_H_FLAG) | (F & Z80_N_FLAG) | c;
				break;
			}

			case CPL:
			{
				A = ~A;
				F = (F & (SZPV_FLAGS | Z80_C_FLAG))
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (A & YX_FLAGS)
#endif
					| Z80_H_FLAG | Z80_N_FLAG;
				break;
			}

			case NEG:
			{
				int a;
				int f;
				int z;
				int c;
				a = A;
				z = -a;
				c = a ^ z;
				f = Z80_N_FLAG | (c & Z80_H_FLAG);
				f |= SZYX_FLAGS_TABLE[z &= 0xff];
				c &= 0x0180;
				f |= OVERFLOW_TABLE[c >> 7];
				f |= c >> (8 - Z80_C_FLAG_SHIFT);
				A = z;
				F = f;
				break;
			}

			case CCF:
			{
				int c;
				c = F & Z80_C_FLAG;
				F = (F & SZPV_FLAGS) | (c << Z80_H_FLAG_SHIFT)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (A & YX_FLAGS)
#endif
					| (c ^ Z80_C_FLAG);
				break;
			}

			case SCF:
			{
				F = (F & SZPV_FLAGS)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (A & YX_FLAGS)
#endif
					| Z80_C_FLAG;
				break;
			}

			case NOP:
			{
				break;
			}

			case HALT:
			{
#ifdef Z80_CATCH_HALT
				State->Status = Z80_STATUS_FLAG_HALT;
#else
				// If an HALT instruction is executed, the Z80 keeps executing NOPs until an interrupt is
				// generated. Basically nothing happens for the remaining number of cycles.
				if (ElapsedCycles < nCycles)
					ElapsedCycles = nCycles;
#endif
				goto stop_emulation;
			}

			case DI:
			{
				State->iff1 = State->iff2 = 0;
#ifdef Z80_CATCH_DI
				State->Status = Z80_STATUS_FLAG_DI;
				goto stop_emulation;
#else
				/*
				No interrupt can be accepted right after a DI or EI instruction on an actual Z80
				processor. By adding 4 cycles to number_cycles, at least one more
				instruction will be executed. However, this will fail if the next instruction has
				multiple 0xdd or 0xfd prefixes and z80_PREFIX_FAILSAFE is defined, but that
				is an unlikely pathological case.*/
				nCycles += 4;
				break;
#endif                  
			}

			case EI:
			{
				State->iff1 = State->iff2 = 1;
#ifdef Z80_CATCH_EI
				State->Status = Z80_STATUS_FLAG_EI;
				goto stop_emulation;
#else
				/* See comment for DI. */
				nCycles += 4;
				break;
#endif
			}

			case IM_N:
			{
				/*
				"IM 0/1" (0xed prefixed opcodes 0x4e and 0x6e) is treated like a "IM 0". */
				if ((Y(Opcode) & 0x03) <= 0x01)
				{
					State->im = Z80_INTERRUPT_MODE_0;
				}
				else if (!(Y(Opcode) & 1))
				{
					State->im = Z80_INTERRUPT_MODE_1;
				}
				else
				{
					State->im = Z80_INTERRUPT_MODE_2;
				}

				break;
			}
#endif
			//16-bit arithmetic group.
#if 6
			case ADD_HL_RR:
			{
				int x;
				int y;
				int z;
				int f;
				int c;
				x = HL_IX_IY;
				y = RR(P(Opcode));
				z = x + y;
				c = x ^ y ^ z;
				f = F & SZPV_FLAGS;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				f |= (z >> 8) & YX_FLAGS;
				f |= (c >> 8) & Z80_H_FLAG;
#endif
				f |= c >> (16 - Z80_C_FLAG_SHIFT);
				HL_IX_IY = z;
				F = f;
				ElapsedCycles += 7;
				break;
			}

			case ADC_HL_RR:
			{
				int x;
				int y;
				int z;
				int f;
				int c;
				x = HL;
				y = RR(P(Opcode));
				z = x + y + (F & Z80_C_FLAG);
				c = x ^ y ^ z;
				f = z & 0xffff ? (z >> 8) & SYX_FLAGS : Z80_Z_FLAG;
#ifndef Z80_DMENTED_FLAGS_ONLY
				f |= (c >> 8) & Z80_H_FLAG;
#endif
				f |= OVERFLOW_TABLE[c >> 15];
				f |= z >> (16 - Z80_C_FLAG_SHIFT);
				HL = z;
				F = f;
				ElapsedCycles += 7;
				break;
			}

			case SBC_HL_RR:
			{
				int x, y, z, f, c;
				x = HL;
				y = RR(P(Opcode));
				z = x - y - (F & Z80_C_FLAG);
				c = x ^ y ^ z;
				f = Z80_N_FLAG;
				f |= z & 0xffff ? (z >> 8) & SYX_FLAGS : Z80_Z_FLAG;
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
				f |= (c >> 8) & Z80_H_FLAG;
#endif
				c &= 0x018000;
				f |= OVERFLOW_TABLE[c >> 15];
				f |= c >> (16 - Z80_C_FLAG_SHIFT);
				HL = z;
				F = f;
				ElapsedCycles += 7;
				break;
			}

			case INC_RR:
			{
				int x;
				x = RR(P(Opcode));
				x++;
				RR(P(Opcode)) = x;
				ElapsedCycles += 2;
				break;
			}

			case DEC_RR:
			{
				int x;
				x = RR(P(Opcode));
				x--;
				RR(P(Opcode)) = x;
				ElapsedCycles += 2;
				break;
			}
#endif
			//Rotate and shift group.
#if 7
			case RLCA:
			{
				A = (A << 1) | (A >> 7);
				F = (F & SZPV_FLAGS) | (A & (YX_FLAGS | Z80_C_FLAG));
				break;
			}

			case RLA:
			{
				int a, f;
				a = A << 1;
				f = (F & SZPV_FLAGS)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (a & YX_FLAGS)
#endif
					| (A >> 7);
				A = a | (F & Z80_C_FLAG);
				F = f;
				break;
			}

			case RRCA:
			{
				int c;
				c = A & 0x01;
				A = (A >> 1) | (A << 7);
				F = (F & SZPV_FLAGS)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (A & YX_FLAGS)
#endif
					| c;
				break;
			}

			case RRA:
			{
				int     c;
				c = A & 0x01;
				A = (A >> 1) | ((F & Z80_C_FLAG) << 7);
				F = (F & SZPV_FLAGS)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (A & YX_FLAGS)
#endif
					| c;
				break;
			}

			case RLC_R:
			{
				RLC(R(Z(Opcode)));
				break;
			}

			case RLC_INDIRECT_HL:
			{
				int     x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					RLC(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					RLC(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case RL_R:
			{
				RL(R(Z(Opcode)));
				break;
			}

			case RL_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					RL(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					RL(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case RRC_R:
			{
				RRC(R(Z(Opcode)));
				break;
			}

			case RRC_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					RRC(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					RRC(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case RR_R:
			{
				RR_INSTRUCTION(R(Z(Opcode)));
				break;
			}

			case RR_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					RR_INSTRUCTION(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					RR_INSTRUCTION(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case SLA_R:
			{
				SLA(R(Z(Opcode)));
				break;
			}

			case SLA_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					SLA(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					SLA(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case SLL_R:
			{
				SLL(R(Z(Opcode)));
				break;
			}

			case SLL_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					SLL(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					SLL(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case SRA_R:
			{
				SRA(R(Z(Opcode)));
				break;
			}

			case SRA_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					SRA(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					SRA(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;

			}

			case SRL_R:
			{
				SRL(R(Z(Opcode)));
				break;
			}

			case SRL_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					SRL(x);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					SRL(x);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case RLD_RRD:
			{
				int x, y;
				READ_BYTE(HL, x);
				y = (A & 0xf0) << 8;
				y |= Opcode == OPCODE_RLD ? (x << 4) | (A & 0x0f) : ((x & 0x0f) << 8) | ((A & 0x0f) << 4) | (x >> 4);
				WRITE_BYTE(HL, y);
				y >>= 8;
				A = y;
				F = SZYXP_FLAGS_TABLE[y] | (F & Z80_C_FLAG);
				ElapsedCycles += 4;
				break;
			}
#endif
			//Bit set, reset, and test group.
#if 8
			case BIT_B_R:
			{
				int x;
				x = R(Z(Opcode)) & (1 << Y(Opcode));
				F = (x ? 0 : Z80_Z_FLAG | Z80_P_FLAG)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (x & Z80_S_FLAG) | (R(Z(Opcode)) & YX_FLAGS)
#endif
					| Z80_H_FLAG | (F & Z80_C_FLAG);
				break;
			}

			case BIT_B_INDIRECT_HL:
			{
				int d, x;
				if (Registers == State->RegisterTable)
				{
					d = HL;
					ElapsedCycles++;
				}
				else
				{
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					pc += 2;
					ElapsedCycles += 5;
				}
				READ_BYTE(d, x);
				x &= 1 << Y(Opcode);
				F = (x ? 0 : Z80_Z_FLAG | Z80_P_FLAG)
#ifndef Z80_DOCUMENTED_FLAGS_ONLY
					| (x & Z80_S_FLAG) | (d & YX_FLAGS)
#endif
					| Z80_H_FLAG | (F & Z80_C_FLAG);
				break;
			}

			case SET_B_R:
			{
				R(Z(Opcode)) |= 1 << Y(Opcode);
				break;
			}

			case SET_B_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					x |= 1 << Y(Opcode);
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					x |= 1 << Y(Opcode);
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}

			case RES_B_R:
			{
				R(Z(Opcode)) &= ~(1 << Y(Opcode));
				break;
			}

			case RES_B_INDIRECT_HL:
			{
				int x;
				if (Registers == State->RegisterTable)
				{
					READ_BYTE(HL, x);
					x &= ~(1 << Y(Opcode));
					WRITE_BYTE(HL, x);
					ElapsedCycles++;
				}
				else
				{
					int d;
					Z80_FETCH_BYTE(pc, d);
					d = ((signed char)d) + HL_IX_IY;
					READ_BYTE(d, x);
					x &= ~(1 << Y(Opcode));
					WRITE_BYTE(d, x);
					if (Z(Opcode) != INDIRECT_HL)			R(Z(Opcode)) = x;
					pc += 2;
					ElapsedCycles += 5;
				}
				break;
			}
#endif
			//Jump group.
#if 9
			case JP_NN:
			{
				int nn;
				Z80_FETCH_WORD(pc, nn);
				pc = nn;
				ElapsedCycles += 6;
				break;
			}

			case JP_CC_NN:
			{
				int nn;
				if (CC(Y(Opcode)))
				{
					Z80_FETCH_WORD(pc, nn);
					pc = nn;
				}
				else
				{
#ifdef Z80_FALSE_CONDITION_FETCH
					Z80_FETCH_WORD(pc, nn);
#endif          
					pc += 2;
				}
				ElapsedCycles += 6;
				break;
			}

			case JR_E:
			{
				int e;
				Z80_FETCH_BYTE(pc, e);
				pc += ((signed char)e) + 1;
				ElapsedCycles += 8;
				break;
			}

			case JR_DD_E:
			{
				int e;
				if (DD(Q(Opcode)))
				{
					Z80_FETCH_BYTE(pc, e);
					pc += ((signed char)e) + 1;
					ElapsedCycles += 8;
				}
				else
				{
#ifdef Z80_FALSE_CONDITION_FETCH
					Z80_FETCH_BYTE(pc, e);
#endif
					pc++;
					ElapsedCycles += 3;
				}
				break;
			}

			case JP_HL:
			{
				pc = HL_IX_IY;
				break;
			}

			case DJNZ_E:
			{
				int e;
				if (--B)
				{
					Z80_FETCH_BYTE(pc, e);
					pc += ((signed char)e) + 1;
					ElapsedCycles += 9;
				}
				else
				{
#ifdef Z80_FALSE_CONDITION_FETCH
					Z80_FETCH_BYTE(pc, e);
#endif
					pc++;
					ElapsedCycles += 4;
				}
				break;
			}
#endif
			//Call and return group.
#if 10
			case CALL_NN:
			{
				int nn;
				READ_NN(nn);
				PUSH(pc);
				pc = nn;
				ElapsedCycles++;
				break;
			}

			case CALL_CC_NN:
			{
				int nn;
				if (CC(Y(Opcode)))
				{
					READ_NN(nn);
					PUSH(pc);
					pc = nn;
					ElapsedCycles++;
				}
				else
				{
#ifdef Z80_FALSE_CONDITION_FETCH
					Z80_FETCH_WORD(pc, nn);
#endif
					pc += 2;
					ElapsedCycles += 6;
				}
				break;
			}

			case RET:
			{
				POP(pc);
				break;
			}

			case RET_CC:
			{
				if (CC(Y(Opcode)))			POP(pc);
				ElapsedCycles++;
				break;
			}

			case RETI_RETN:
			{
				State->iff1 = State->iff2;
				POP(pc);
#if defined(Z80_CATCH_RETI) && defined(Z80_CATCH_RETN)
				State->Status = Opcode == OPCODE_RETI ? Z80_STATUS_FLAG_RETI : Z80_STATUS_FLAG_RETN;
				goto stop_emulation;
#elif defined(Z80_CATCH_RETI)
				State->Status = Z80_STATUS_FLAG_RETI;
				goto stop_emulation;
#elif defined(Z80_CATCH_RETN)
				State->Status = Z80_STATUS_FLAG_RETN;
				goto stop_emulation;
#else
				break;
#endif
			}

			case RST_P:
			{
				PUSH(pc);
				pc = RST_TABLE[Y(Opcode)];
				ElapsedCycles++;
				break;
			}
#endif
			//Input and output group. 

			case IN_A_N:
			{
				int n;
				int Address = (A) << 8;
				READ_N(n);
				Address += n;
				Z80_INPUT_BYTE(Address, A);
				ElapsedCycles += 4;
				break;
			}

			case IN_R_C:
			{
				int x = 255;
				Z80_INPUT_BYTE(BC, x);
				if (Y(Opcode) != INDIRECT_HL)			R(Y(Opcode)) = x;
				F = SZYXP_FLAGS_TABLE[x] | (F & Z80_C_FLAG);
				ElapsedCycles += 4;
				break;
			}
			/* 
			Some of the undocumented flags for "INI", "IND",
			  "INIR", "INDR",  "OUTI", "OUTD", "OTIR", and
			  "OTDR" are really really strange. The emulator
			  implements the specifications described in "The
			  Undocumented Z80 Documented Version 0.91".
			 */

			case INI_IND:
			{
				int x, f;
				Z80_INPUT_BYTE(C, x);
				WRITE_BYTE(HL, x);
				f = SZYX_FLAGS_TABLE[--B & 0xff] | (x >> (7 - Z80_N_FLAG_SHIFT));
				if (Opcode == OPCODE_INI)
				{
					HL++;
					x += (C + 1) & 0xff;
				}
				else
				{
					HL--;
					x += (C - 1) & 0xff;
				}
				f |= x & 0x0100 ? HC_FLAGS : 0;
				f |= SZYXP_FLAGS_TABLE[(x & 0x07) ^ B] & Z80_P_FLAG;
				F = f;
				ElapsedCycles += 5;
				break;
			}

			case INIR_INDR:
			{
				int d, b, hl, x, f;
#ifdef Z80_HANDLE_SELF_MODIFYING_CODE
				int     p, q;
				p = (pc - 2) & 0xffff;
				q = (pc - 1) & 0xffff;
#endif                          
				d = Opcode == OPCODE_INIR ? +1 : -1;
				b = B;
				hl = HL;
				r -= 2;
				ElapsedCycles -= 8;
				for (; ; )
				{
					r += 2;
					Z80_INPUT_BYTE(C, x);
					Z80_WRITE_BYTE(hl, x);
					hl += d;
					if (--b)
					{
						ElapsedCycles += 21;
					}
					else
					{
						f = Z80_Z_FLAG;
						ElapsedCycles += 16;
						break;
					}
#ifdef Z80_HANDLE_SELF_MODIFYING_CODE
					if (((hl - d) & 0xffff) == p || ((hl - d) & 0xffff) == q)
					{
						f = SZYX_FLAGS_TABLE[b];
						pc -= 2;
						break;
					}
#endif                                  
					if (ElapsedCycles < nCycles)
					{
						continue;
					}
					else
					{
						f = SZYX_FLAGS_TABLE[b];
						pc -= 2;
						break;
					}
				}
				HL = hl;
				B = b;
				f |= x >> (7 - Z80_N_FLAG_SHIFT);
				x += (C + d) & 0xff;
				f |= x & 0x0100 ? HC_FLAGS : 0;
				f |= SZYXP_FLAGS_TABLE[(x & 0x07) ^ b] & Z80_P_FLAG;
				F = f;
				break;
			}

			case OUT_N_A: {
							   				 			  			  
				int     n;
				int Address = (A) << 8;
				READ_N(n);
				Address += n;
				Z80_OUTPUT_BYTE(Address, A);

				ElapsedCycles += 4;

				break;

			}

			case OUT_C_R: {

				int     x;

				x = Y(Opcode) != INDIRECT_HL
					? R(Y(Opcode))
					: 0;
				Z80_OUTPUT_BYTE(C, x);

				ElapsedCycles += 4;

				break;

			}

			case OUTI_OUTD: {

				int     x, f;

				READ_BYTE(HL, x);
				Z80_OUTPUT_BYTE(C, x);

				HL += Opcode == OPCODE_OUTI ? +1 : -1;

				f = SZYX_FLAGS_TABLE[--B & 0xff]
					| (x >> (7 - Z80_N_FLAG_SHIFT));
				x += HL & 0xff;
				f |= x & 0x0100 ? HC_FLAGS : 0;
				f |= SZYXP_FLAGS_TABLE[(x & 0x07) ^ B]
					& Z80_P_FLAG;
				F = f;

				break;

			}

			case OTIR_OTDR: {

				int     d, b, hl, x, f;

				d = Opcode == OPCODE_OTIR ? +1 : -1;

				b = B;
				hl = HL;

				r -= 2;
				ElapsedCycles -= 8;
				for (; ; ) {

					r += 2;

					Z80_READ_BYTE(hl, x);
					Z80_OUTPUT_BYTE(C, x);

					hl += d;
					if (--b)

						ElapsedCycles += 21;

					else {

						f = Z80_Z_FLAG;
						ElapsedCycles += 16;
						break;

					}

					if (ElapsedCycles < nCycles)

						continue;

					else {

						f = SZYX_FLAGS_TABLE[b];
						pc -= 2;
						break;

					}

				}

				HL = hl;
				B = b;

				f |= x >> (7 - Z80_N_FLAG_SHIFT);
				x += hl & 0xff;
				f |= x & 0x0100 ? HC_FLAGS : 0;
				f |= SZYXP_FLAGS_TABLE[(x & 0x07) ^ b]
					& Z80_P_FLAG;
				F = f;

				break;

			}

							/* Prefix group. */

			case CB_PREFIX: {

				/* Special handling if the 0xcb prefix is
				 * prefixed by a 0xdd or 0xfd prefix.
				 */

				if (Registers != State->RegisterTable) {

					r--;

					/* Indexed memory access routine will
					 * correctly update pc.
					 */

					Z80_FETCH_BYTE(pc + 1, Opcode);

				}
				else {

					Z80_FETCH_BYTE(pc, Opcode);
					pc++;

				}
				Instruction = CB_INSTRUCTION_TABLE[Opcode];

				goto emulate_next_instruction;

			}

			case DD_PREFIX: {

				Registers = State->DDRegisterTable;

#ifdef Z80_PREFIX_FAILSAFE

				/* Ensure that at least number_cycles cycles
				 * are executed.
				 */

				if (ElapsedCycles < nCycles) {

					Z80_FETCH_BYTE(pc, Opcode);
					pc++;
					goto emulate_next_opcode;

				}
				else {

					State->Status = Z80_STATUS_PREFIX;
					pc--;
					ElapsedCycles -= 4;
					goto stop_emulation;

				}

#else

				Z80_FETCH_BYTE(pc, Opcode);
				pc++;
				goto emulate_next_opcode;

#endif                          

			}

			case FD_PREFIX: {

				Registers = State->FDRegisterTable;

#ifdef Z80_PREFIX_FAILSAFE

				if (ElapsedCycles < nCycles) {

					Z80_FETCH_BYTE(pc, Opcode);
					pc++;
					goto emulate_next_opcode;

				}
				else {

					State->Status = Z80_STATUS_PREFIX;
					pc--;
					ElapsedCycles -= 4;
					goto stop_emulation;

				}

#else

				Z80_FETCH_BYTE(pc, Opcode);
				pc++;
				goto emulate_next_opcode;

#endif                          

			}

			case ED_PREFIX: {

				Registers = State->RegisterTable;
				Z80_FETCH_BYTE(pc, Opcode);
				pc++;
				Instruction = ED_INSTRUCTION_TABLE[Opcode];

				goto emulate_next_instruction;

			}

							/* Special/pseudo instruction group. */

			case ED_UNDEFINED: {

#ifdef Z80_CATCH_ED_UNDEFINED

				State->Status = Z80_STATUS_FLAG_ED_UNDEFINED;
				pc -= 2;
				goto stop_emulation;

#else

				break;

#endif

			}

			}
			if (ElapsedCycles >= nCycles)
				goto stop_emulation;
		}
	stop_emulation:

		State->r = (State->r & 0x80) | (r & 0x7f);
		State->pc = pc & 0xffff;
		return ElapsedCycles;
	
}