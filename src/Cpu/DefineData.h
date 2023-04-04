#ifndef __DEFIN_DATA__
#define __DEFIN_DATA__

#include "z80config.h"
#include "z80user.h"
#include "instructions.h"
#include "macros.h"
#include "tables.h"

enum { Z80_STATUS_HALT = 1, Z80_STATUS_DI, Z80_STATUS_EI, Z80_STATUS_RETI, Z80_STATUS_RETN, Z80_STATUS_ED_UNDEFINED, Z80_STATUS_PREFIX };

#ifdef Z80_BIG_ENDIAN

#define Z80_B            0
#define Z80_C            1
#define Z80_D            2
#define Z80_E            3
#define Z80_H            4
#define Z80_L            5
#define Z80_A            6
#define Z80_F            7
#define Z80_IXH          8
#define Z80_IXL          9
#define Z80_IYH          10
#define Z80_IYL          11

#else

#define Z80_B            1
#define Z80_C            0
#define Z80_D            3
#define Z80_E            2
#define Z80_H            5
#define Z80_L            4
#define Z80_A            7
#define Z80_F            6
#define Z80_IXH          9
#define Z80_IXL          8
#define Z80_IYH          11
#define Z80_IYL          10

#endif

#define Z80_BC                  0
#define Z80_DE                  1
#define Z80_HL                  2
#define Z80_AF                  3
#define Z80_IX                  4
#define Z80_IY                  5 
#define Z80_SP                  6

/* Z80's flags. */

#define Z80_S_FLAG_SHIFT        7       
#define Z80_Z_FLAG_SHIFT        6
#define Z80_Y_FLAG_SHIFT        5
#define Z80_H_FLAG_SHIFT        4
#define Z80_X_FLAG_SHIFT        3
#define Z80_PV_FLAG_SHIFT       2
#define Z80_N_FLAG_SHIFT        1
#define Z80_C_FLAG_SHIFT        0

#define Z80_S_FLAG              (1 << Z80_S_FLAG_SHIFT)
#define Z80_Z_FLAG              (1 << Z80_Z_FLAG_SHIFT)
#define Z80_Y_FLAG              (1 << Z80_Y_FLAG_SHIFT)
#define Z80_H_FLAG              (1 << Z80_H_FLAG_SHIFT)
#define Z80_X_FLAG              (1 << Z80_X_FLAG_SHIFT)
#define Z80_PV_FLAG             (1 << Z80_PV_FLAG_SHIFT)
#define Z80_N_FLAG              (1 << Z80_N_FLAG_SHIFT)
#define Z80_C_FLAG              (1 << Z80_C_FLAG_SHIFT)

#define Z80_P_FLAG_SHIFT        Z80_PV_FLAG_SHIFT
#define Z80_V_FLAG_SHIFT        Z80_PV_FLAG_SHIFT
#define Z80_P_FLAG              Z80_PV_FLAG
#define Z80_V_FLAG              Z80_PV_FLAG

/* Z80's three interrupt modes. */

enum { Z80_INTERRUPT_MODE_0, Z80_INTERRUPT_MODE_1, Z80_INTERRUPT_MODE_2 };


#endif
