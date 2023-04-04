#include "Graphics.h"
#include "../Scene.h"


CGraphics::CGraphics() {}

CGraphics::CGraphics(ubyte *VideoMemory, ubyte *AttrMemory, bool bIsWindowsHost)

{
	this->VideoMemory = VideoMemory;
	this->AttrMemory = AttrMemory;
	this->m_bIsWindowsHost = bIsWindowsHost;

	for (int i = 0; i < 6144; i++)
		VideoMemory[i] = rand() % 255;
		
	for (int i = 0; i < 768; i++)
		AttrMemory[i] =/* 0x38; //*/rand() % 255;

	VBuffer = new Sprite;
	Frame = VBuffer->getAnimFrame();
	
	m_bPP = 4;
	m_Width = ScrenWidth;
	m_Height = ScreenHeight;
	m_StartX = 0;
	m_StartY = 0;

	if (m_bIsWindowsHost)
	{
		m_Width += BorderWidth * 2;
		m_Height += BorderHeight * 2;
		m_StartX = BorderWidth;
		m_StartY = BorderHeight;
	}

	Buffer = new ubyte[m_Width * m_Height * m_bPP];

	ImData.w = m_Width/*(ScreenW, ScreenH, ScreenW * bPP, TF_B8G8R8A8, Buff)*/;
	ImData.h = m_Height;
	ImData.format = TF_B8G8R8A8;
	ImData.bytespp = m_bPP;
	ImData.pitch = m_Width * m_bPP;
	ImData.data = (unsigned char*)Buffer;

	unsigned char * pixelPtr = nullptr;
	int CharSetW = ScrenWidth / 8;

	for (uint y = 0; y < ScreenHeight; y++)
	{
		for (uint x = 0; x < ScrenWidth; x += 8)
		{
			uint Line = y & 7;
			uint Row = (y & 56) >> 3;
			uint Part = (y & 192) >> 6;

			uint AddressInVARam = (y / 8) * ScrenWidth / 8 + x / 8;
			uint AddressInVRam = ((y & 192) + ((y & 56) >> 3) + ((y & 7) << 3)) * ScrenWidth / 8 + x / 8;
			assert(AddressInVRam < 6144);
			
			ubyte ByteVRam = VideoMemory[AddressInVRam];
			ubyte ByteVARam = AttrMemory[AddressInVARam];
			
			int Ink = ByteVARam & 7;
			int Paper = (ByteVARam & 56) >> 3;

			CurColor.bright = ByteVARam & 64;
			CurColor.flash = ByteVARam & 128;
			CurColor.Ink = m_bFlash ? Palette[(int)CurColor.bright][Paper] : Palette[(int)CurColor.bright][Ink];
			CurColor.Paper = m_bFlash ? Palette[(int)CurColor.bright][Ink] : Palette[(int)CurColor.bright][Paper];


			for (int c = 0; c < 8; c++)
			{
				bool BPixelIsOn = ByteVRam & 128;
				pixelPtr = ImData.getPixelPtr(x + c + m_StartX, y + m_StartY);
							   
				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.red : CurColor.Paper.red;
				pixelPtr++;
				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.green : CurColor.Paper.green;
				pixelPtr++;
				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.blue : CurColor.Paper.blue;
				pixelPtr++;
				(*pixelPtr) = 255;
				pixelPtr++;

				ByteVRam = ByteVRam << 1;
			}

		}
	}

	VBuffer->attachTo(getStage());

	texture = IVideoDriver::instance->createTexture();
	texture->init(m_Width, m_Height, TF_R8G8B8A8, true);

	texture->updateRegion(0, 0, ImData);
	texture->setLinearFilter(false);

	Diffuse df;
	df.base = texture;
	Frame.init(0, df,
		RectF(0, 0, m_Width / texture->getWidth(), m_Height / texture->getHeight()),
		RectF(Vector2(0, 0), Vector2(m_Width, m_Height)), Vector2(m_Width, m_Height));
	VBuffer->setAnimFrame(Frame);
	VBuffer->setAnchorInPixels(0.5f, 0.5f);
	VBuffer->setPosition(0, 0);
	VBuffer->setSize(m_Width * ScaleScreen, m_Height * ScaleScreen);
}

CGraphics::~CGraphics() {}

void CGraphics::UpdateBorder(ubyte BorderColor, int nTacts, uint dt)
{
	if (!m_bIsWindowsHost || m_bIsComplete)
		return;

	if (m_BlinkTimer > 0)
	{
		m_BlinkTimer -= dt;
		BorderColor = m_bIsNeedBlinkOk ? 4 : 2;

		if (m_BlinkTimer <= 0)
			m_BlinkTimer = 0;
	}

	unsigned char * pixelPtr = nullptr;
	
	while (nTacts > 0)
	{
		m_LastX++;
		if (m_LastX == m_Width)
		{
			m_LastX = 0;
			m_LastY++;
			if (m_LastY == m_Height)
			{
				m_LastX = 0;
				m_LastY = 0;
				m_bIsComplete = true;
			}
		}

		nTacts--;

		if (m_LastY > BorderHeight && m_LastY < ScreenHeight - BorderHeight)
		{
			if (m_LastX > BorderWidth && m_LastX < ScrenWidth - BorderWidth)
			{
				continue;
			}
		}
			
		pixelPtr = ImData.getPixelPtr(m_LastX, m_LastY);
		
		(*pixelPtr) = BorderPalette[BorderColor].blue;
		pixelPtr++;
		(*pixelPtr) = BorderPalette[BorderColor].red;
		pixelPtr++;
		(*pixelPtr) = BorderPalette[BorderColor].green;
		pixelPtr++;
		(*pixelPtr) = 255;
		pixelPtr++;

		
	}
}

void CGraphics::UpdateScreen()
{
	static bool BorderLastState = false;
#if 0
	for(int i = 0; i < 6144; i++)
		VideoMemory[i] = rand() % 255;

	for (int i = 0; i < 768; i++)
		AttrMemory[i] = rand() % 255;
#endif
	unsigned char * pixelPtr = nullptr;
	int CharSetW = ScrenWidth / 8;

	for (uint y = 0; y < ScreenHeight; y++)
	{
		for (uint x = 0; x < ScrenWidth; x += 8)
		{
			uint AddressInVARam = (y / 8) * ScrenWidth / 8 + x / 8;
			uint AddressInVRam = ((y & 192) + ((y & 56) >> 3) + ((y & 7) << 3)) * ScrenWidth / 8 + x / 8;
			assert(AddressInVRam < 6144);
			ubyte ByteVARam = AttrMemory[AddressInVARam];
			ubyte ByteVRam = VideoMemory[AddressInVRam];

			int Ink = ByteVARam & 7;
			int Paper = (ByteVARam & 56) >> 3;

			CurColor.bright = ByteVARam & 64;
			CurColor.flash = ByteVARam & 128;
			CurColor.Ink = CurColor.flash && m_bFlash ? Palette[CurColor.bright][Paper] : Palette[CurColor.bright][Ink];
			CurColor.Paper = CurColor.flash && m_bFlash ? Palette[CurColor.bright][Ink] : Palette[CurColor.bright][Paper];


			for (int c = 0; c < 8; c++)
			{
				bool BPixelIsOn = ByteVRam & 128;
				pixelPtr = ImData.getPixelPtr(x + c + m_StartX, y + m_StartY);

				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.red : CurColor.Paper.red;
				pixelPtr++;
				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.green : CurColor.Paper.green;
				pixelPtr++;
				(*pixelPtr) = BPixelIsOn ? CurColor.Ink.blue : CurColor.Paper.blue;
				pixelPtr++;
				(*pixelPtr) = 255;
				pixelPtr++;

				ByteVRam = ByteVRam << 1;
			}

		}
	}

	m_bIsComplete = false;
	VBuffer->getAnimFrame().getDiffuse().base->updateRegion(0, 0, ImData);
}

void CGraphics::BlinkBorder(bool bIsOk)
{
	m_bIsNeedBlinkOk = bIsOk;
	m_BlinkTimer = BlinkTimerMS;
}

void CGraphics::AttachBufferToScene(spCScene scene, int offset)
{
	Vector2 sceneSize = scene->getSelf()->getSize();
	VBuffer->setScale(sceneSize.y / VBuffer->getHeight());
	VBuffer->setAnchor(0.5f, 0.5f);
	VBuffer->setPosition(sceneSize / 2.0f);
	VBuffer->attachTo(scene->getSelf());

	if (offset != 0)
		VBuffer->setPosition(Vector2(VBuffer->getPosition().x + offset, VBuffer->getPosition().y));
}
