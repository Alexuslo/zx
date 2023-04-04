#pragma once

namespace oxygine
{
	class ColorExt : public Color
	{
	public:
		enum
		{
			Over = 0x9ACD32F1,
			ShadowColor = 0xFFFFFFAA,
			Out = 0xFFFFFFAB,
			Press = 0xFFFFFFAC,
			Click = 0xFFFFFFAD,
		};
	};
}