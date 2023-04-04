#pragma once
#include "../common.h"

class CGraphics
{
protected:
	spNativeTexture texture;
	oxygine::AnimationFrame Frame;
	ImageData ImData;
	Diffuse df;

	struct PixelColor
	{
		ubyte red = 0;
		ubyte green = 0;
		ubyte blue = 0;

		PixelColor()
		{
			red = 0;
			green = 0;
			blue = 0;
		}

		PixelColor(ubyte r, ubyte g, ubyte b)
		{
			red = r;
			green = g;
			blue = b;
		}
	};

	struct CharSetColor
	{
		PixelColor Ink;
		PixelColor Paper;
		bool bright = 0;
		bool flash = 0;
	};

	CharSetColor CurColor;

	PixelColor Palette[2][8] =
	{
		{
			{Nl, Nl, Nl},
			{Lo, Nl, Nl},
			{Nl, Nl, Lo},
			{Lo, Nl, Lo},
			{Nl, Lo, Nl},
			{Lo, Lo, Nl},
			{Nl, Lo, Lo},
			{Lo, Lo, Lo}
		},

		{
			{(ubyte)Nl, (ubyte)Nl, (ubyte)Nl},
			{(ubyte)Hi, (ubyte)Nl, (ubyte)Nl},
			{(ubyte)Nl, (ubyte)Nl, (ubyte)Hi},
			{(ubyte)Hi, (ubyte)Nl, (ubyte)Hi},
			{(ubyte)Nl, (ubyte)Hi, (ubyte)Nl},
			{(ubyte)Hi, (ubyte)Hi, (ubyte)Nl},
			{(ubyte)Nl, (ubyte)Hi, (ubyte)Hi},
			{(ubyte)Hi, (ubyte)Hi, (ubyte)Hi}
		}
	};

	//b r g
	PixelColor BorderPalette[8] = {
		{(ubyte)Nl, (ubyte)Nl, (ubyte)Nl},//
		{(ubyte)Nl, (ubyte)Nl, (ubyte)Lo},// 
		{(ubyte)Nl, (ubyte)Lo, (ubyte)Nl},// 
		{(ubyte)Nl, (ubyte)Lo, (ubyte)Lo},// 
		{(ubyte)Lo, (ubyte)Nl, (ubyte)Nl},//
		{(ubyte)Lo, (ubyte)Nl, (ubyte)Lo},//
		{(ubyte)Lo, (ubyte)Lo, (ubyte)Nl},// 	
		{(ubyte)Lo, (ubyte)Lo, (ubyte)Lo},// 
	};

	oxygine::spSprite VBuffer;

	ubyte *Buffer;

	ubyte *VideoMemory;
	ubyte *AttrMemory;

	int m_bPP = 4;
	int m_Width = 0;
	int m_Height = 0;
	int m_StartX = 0;
	int m_StartY = 0;
	int m_LastX = 0;
	int m_LastY = 0;

	int m_BlinkTimer = 0;

	bool m_bFlash = false;
	bool m_bIsWindowsHost = false;
	bool m_bIsComplete = false;
	bool m_bIsNeedBlinkOk = false;

public:
	CGraphics();
	CGraphics(ubyte *VideoMemory, ubyte *AttrMemory, bool bIsWindowsHost);
	~CGraphics();

	void UpdateScreen();
	void UpdateBorder(ubyte BorderColor, int nTacts, uint dt);
	void Flash() { m_bFlash = !m_bFlash; }
	void BlinkBorder(bool bIsOk);
	void AttachBufferToScene(spCScene scene, int offset = 0);
};

