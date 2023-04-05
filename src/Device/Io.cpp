#include "Io.h"
#include "../Func.h"
#include "../Button.h"

namespace bit
{
	void SetBit(ubyte &Data, bool State, ubyte NumBit)
	{
		ubyte MaxBits = (sizeof Data) * 8;

		if (NumBit > MaxBits)
			NumBit %= MaxBits;

		if (State)
			Data |= (0x1 << NumBit);
		else
			Data &= ~(0x1 << NumBit);
	};

	bool TestBit(ubyte &Data, ubyte NumBit)
	{
		ubyte MaxBits = (sizeof Data) * 8;

		if (NumBit > MaxBits)
			NumBit %= MaxBits;

		return (bool)(Data & (0x1 << NumBit));
	};
};

bool CIo::m_bIsKeyInit = false;

CIo::CIo(CDevice *Device) :
	m_pDevice(Device)
{
	key::init();
	m_bIsKeyInit = true;
	m_pFireKey = m_pDevice->GetFireButton();
	m_pCursorKeys = m_pDevice->GetCursorKeys();
	m_joystick = m_pDevice->GetJoustick();
}

CIo::~CIo()
{

}

ubyte CIo::ReadPort(word Address)
{
	ubyte HAddress = (Address & 0xFF00) >> 8;
	ubyte LAddress = Address & 0x00FF;
	ubyte RetValue = 0xFF;

	RetValue &= ~(m_Port254.Cin.TapeBit << 6);

	if ((LAddress & 0x01) == 0x00)
	{
		ubyte RetVal = 0xFF;
		RetVal = ~(m_Port254.Cin.TapeBit << 6);

		if (HAddress == 0x00)
		{
			RetVal &= m_Keyboard[8];
			return RetVal;
		}

		for (int i = 0; i < 8; i++)
		{
			if ((HAddress | m_KeysPort[i]) == m_KeysPort[i])
				RetVal &= m_Keyboard[i];
		}
		return RetVal;
	}
	else
	{
		return 0xFF;
	}
}

void CIo::WritePort(word Address, ubyte Byte)
{
	ubyte HAddress = (Address & 0xFF00) >> 8;
	ubyte LAddress = Address & 0x00FF;
	ubyte RetValue = 0xFF;

	LAddress &= 0x01;

	if (LAddress == 0x00)
		m_Port254.Cout.DataOut = Byte;
}

void CIo::Update()
{
	m_Keyboard[8] = 0xFF;

	for (int i = 0; i < 8; i++)
	{
		m_Keyboard[i] = 0xFF;

		for (int j = 0; j < 5; j++)
		{
			m_Keyboard[i] &= (~(((ubyte)key::isPressed(m_Keys[j + i * 5])) << j));
		}

		m_Keyboard[8] &= m_Keyboard[i];
	}

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	Vector2 dir;
	m_joystick->GetDirection(dir);

	std::tuple<bool, bool, bool, bool> directionInfo = std::tuple<bool, bool, bool, bool>(false, false, false, false);

	if (m_joystick->GetStateDirection(directionInfo))
		std::tie(up, right, down, left) = directionInfo;

	DebugActor::addDebugString("   up - %d\nright - %d\n down - %d\n left - %d", static_cast<int>(up), static_cast<int>(right), static_cast<int>(down), static_cast<int>(left));

	if (up)
	{
		bit::SetBit(m_Keyboard[2], false, BIT_0);//Q
		//DebugActor::addDebugString("Up");
	}

	if (down)
	{
		bit::SetBit(m_Keyboard[4], false, BIT_0);//A
		//DebugActor::addDebugString("Down");
	}

	if (right)
	{
		bit::SetBit(m_Keyboard[3], false, BIT_0);//P
		//DebugActor::addDebugString("Right");
	}

	if (left)
	{
		bit::SetBit(m_Keyboard[3], false, BIT_1);//O
		//DebugActor::addDebugString("Left");
	}


// 	if (m_pCursorKeys->IsUp() || m_pCursorKeys->IsUpLeft() || m_pCursorKeys->IsUpRight())
// 	{
// 		bit::SetBit(m_Keyboard[2], false, BIT_0);//Q
// 		DebugActor::addDebugString("Up");
// 	}
// 
// 	if (m_pCursorKeys->IsDown() || m_pCursorKeys->IsDownLeft() || m_pCursorKeys->IsDownRight())
// 	{
// 		bit::SetBit(m_Keyboard[4], false, BIT_0);//A
// 		DebugActor::addDebugString("Down");
// 	}
// 
// 	if (m_pCursorKeys->IsRight() || m_pCursorKeys->IsUpRight() || m_pCursorKeys->IsDownRight())
// 	{
// 		bit::SetBit(m_Keyboard[3], false, BIT_0);//P
// 		DebugActor::addDebugString("Right");
// 	}
// 
// 	if (m_pCursorKeys->IsLeft() || m_pCursorKeys->IsUpLeft() || m_pCursorKeys->IsDownLeft())
// 	{
// 		bit::SetBit(m_Keyboard[3], false, BIT_1);//O
// 		DebugActor::addDebugString("Left");
// 	}

	if (m_pFireKey->IsOverTouch())
	{
		bit::SetBit(m_Keyboard[7], false, BIT_0);//M
		DebugActor::addDebugString("Fire");
	}

	if (m_pDevice->IsNeedPressEnterOrNull())
	{
		bit::SetBit(m_Keyboard[1], false, BIT_0);//0
		bit::SetBit(m_Keyboard[5], false, BIT_0);//Enter
		DebugActor::addDebugString("Enter & null");
	}

/*
	//delete
	if (key::isPressed(SDL_SCANCODE_BACKSPACE))
	{
		bit::SetBit(m_Keyboard[6], false, BIT_0);//caps
		bit::SetBit(m_Keyboard[1], false, BIT_0);//null
	}

	//esc
	if (key::isPressed(SDL_SCANCODE_ESCAPE))
	{
		m_pDevice->ResetRequest();
		//bit::SetBit(m_Keyboard[6], false, BIT_0);//caps
		//bit::SetBit(m_Keyboard[7], false, BIT_0);//space
	}

	//pause/break
	if (key::isPressed(SDL_SCANCODE_PAUSE))
	{
		bit::SetBit(m_Keyboard[6], false, BIT_0);//caps
		bit::SetBit(m_Keyboard[7], false, BIT_0);//space
	}

	//.
	if (key::isPressed(SDL_SCANCODE_PERIOD))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[7], false, BIT_2);//.
	}

	//,
	if (key::isPressed(SDL_SCANCODE_COMMA))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[7], false, BIT_3);//,
	}

	if (key::isPressed(SDL_SCANCODE_APOSTROPHE))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[1], false, BIT_3);//'
	}

	//;
	if (key::isPressed(SDL_SCANCODE_SEMICOLON))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[3], false, BIT_1);//,
	}

	//:
	if (key::isPressed(SDLK_COLON) && (key::isPressed(SDL_SCANCODE_RSHIFT) || key::isPressed(SDL_SCANCODE_LSHIFT)))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[6], false, BIT_1);//:
	}

	//"
	if (key::isPressed(SDLK_QUOTEDBL) && (key::isPressed(SDL_SCANCODE_RSHIFT) || key::isPressed(SDL_SCANCODE_LSHIFT)))
	{
		bit::SetBit(m_Keyboard[7], false, BIT_1);//sshift
		bit::SetBit(m_Keyboard[3], false, BIT_0);//"
	}*/
}