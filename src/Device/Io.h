#pragma once
#include "../common.h"

class CDevice;
class CJoystick;

struct CPort254
{
	union
	{
		ubyte DataOut = 0;
		struct
		{
			ubyte BorderColor : 3, TapeBit : 1, SoundBit : 1, NotUsed1 : 3;
		};
	}Cout;

	union
	{
		ubyte DataIn = 0;
		struct
		{
			ubyte Keyboard : 5, NotUsed1 : 1, TapeBit : 1, NotUsed2 : 1;
		};
	}Cin;
};

class CIo
{
private:
	CDevice *m_pDevice = nullptr;

	CButton *m_pFireKey = nullptr;
	CCursorKeys *m_pCursorKeys = nullptr;
	CJoystick *m_joystick = nullptr;

	static bool m_bIsKeyInit;
	CPort254 m_Port254;

	std::array<ubyte, 9> m_Keyboard;

	std::array<ubyte, 8> m_KeysMasks = { 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80 };
	std::array<ubyte, 8> m_KeysPort = { 0xF7, 0xEF, 0xFB, 0xDF, 0xFD, 0xBF, 0xFE, 0x7F };
	std::array<ubyte, 40> m_Keys = { KEY_1,		KEY_2,	KEY_3,	KEY_4,	KEY_5,	KEY_0,		KEY_9,		KEY_8,	KEY_7,	KEY_6,
									KEY_Q,		KEY_W,	KEY_E,	KEY_R,	KEY_T,	KEY_P,		KEY_O,		KEY_I,	KEY_U,	KEY_Y,
									KEY_A,		KEY_S,	KEY_D,	KEY_F,	KEY_G,	KEY_ENTER,	KEY_L,		KEY_K,	KEY_J,	KEY_H,
									KEY_CAPS,	KEY_Z,	KEY_X,	KEY_C,	KEY_V,	KEY_SPACE,	KEY_SYMBOL, KEY_M,	KEY_N,	KEY_B };

public:

	CIo(CDevice *Spectrum);
	~CIo();

	ubyte ReadPort(word Addres);

	ubyte GetBorderColor() { return m_Port254.Cout.BorderColor; };
	void SetBorderColor(ubyte Byte) { m_Port254.Cout.BorderColor = Byte; }

	void WritePort(word Address, ubyte Byte);

	void SetTapeBit(bool TapeBit) { m_Port254.Cin.TapeBit = TapeBit; }

	void Update();
};