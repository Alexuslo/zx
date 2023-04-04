#pragma once

#include "ox/oxygine.hpp"
#include "ox/Stage.hpp"
#include "ox/DebugActor.hpp"
#include "oxygine-framework.h"
#include "flow/oxygine-flow.h"

#include <stdlib.h>
#include <time.h>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <functional>
#include <memory>
#include <vector>
#include <stack>
#include "math.h"
#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "Addons/Addons.h"

#define PI 3.1417f
#define PI_2 1.5709‬f
#define DPI 6.2834f 
#define E  2.7183f

#define ToggleFinishValue 0.9f
#define OverAddSize 1.1f
#define ClickScale 0.8f
#define ClickMove 5.0f

#define ClickPriority 0
#define SwipeDist 20

#define CursorKeyPriority 3000
#define ButtonPriority 2000
#define ShadowPriority 1000
#define TextFieldPriority (ButtonPriority + 1)
#define ShadowScale 1.03f
#define ShadowOffset {5.0f, 5.0f}
#define ShadowColor Color(0, 0, 0, 150)

#define UpdateTime 20

#define MaxRotation 2.0f
#define Acceleration 0.005f  

#define CpuTacts 3500000   /* In Hz. */
#define CpuTactsPerFrame (CpuTacts / 60.0)
#define CyclesPerStep 30	
#define MaxStringLenght 100

#define ScrenWidth 256
#define ScreenHeight 192
#define BorderWidth 24
#define BorderHeight 16
#define ScaleScreen 0.01
#define BlinkTimerMS 50000

#define FrameTime 1000 / 60
#define FlashTime 500

#define RamSize48 65536
#define RomSize48 16384
#define StartAddressRam48 16384
#define TotalRam48 49152
#define CpuStateSize 50

#define JoustickFrames 10

class CScene;
class CBase;
class CTweenSpring;
class CButton;
class CApp;
class CSoundBank;
class CCursorKeys;

using namespace pugi;
using namespace oxygine;
using namespace oxygine::flow;

typedef pugi::xml_document XmlDocument;
typedef pugi::xml_parse_result XmlParseResult;
typedef pugi::xml_node XmlNode;
typedef pugi::xml_attribute XmlAttribute;

typedef intrusive_ptr<CScene> spCScene;
typedef std::list<spCScene> Scenes;
typedef intrusive_ptr<CButton> spCButton;
typedef intrusive_ptr<ClipRectActor> spClipRectActor;
typedef intrusive_ptr<CCursorKeys> spCCursorKeys;

typedef unsigned int uint;
typedef unsigned short word;
typedef unsigned char ubyte;

enum class EDirection { Left, Up, Right, Down };
enum class ETransitionType { None, Move, Fade, Shutters, Quads, FadeFromPoint };
enum class EButtonState {Unpressed, Pressed, Disabled};
enum class EOverType { None, AddColor, AddSize, AddSpring, AddElastik };
enum class EClickType { None, AddColor, AddSize, AddMove, AddSpring, AddElastik };

struct CRetValue
{
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	CRetValue() { }
	void ResetAll()
	{
		up = down = left = right = false;
	}
};

#define KEY_1		SDL_SCANCODE_1			
#define KEY_2		SDL_SCANCODE_2			
#define KEY_3		SDL_SCANCODE_3			
#define KEY_4		SDL_SCANCODE_4			
#define KEY_5		SDL_SCANCODE_5			
#define KEY_0		SDL_SCANCODE_0			
#define KEY_9		SDL_SCANCODE_9			
#define KEY_8		SDL_SCANCODE_8			
#define KEY_7		SDL_SCANCODE_7			
#define KEY_6		SDL_SCANCODE_6			
#define KEY_Q		SDL_SCANCODE_Q			
#define KEY_W		SDL_SCANCODE_W			
#define KEY_E		SDL_SCANCODE_E			
#define KEY_R		SDL_SCANCODE_R			
#define KEY_T		SDL_SCANCODE_T			
#define KEY_P		SDL_SCANCODE_P			
#define KEY_O		SDL_SCANCODE_O			
#define KEY_I		SDL_SCANCODE_I			
#define KEY_U		SDL_SCANCODE_U			
#define KEY_Y		SDL_SCANCODE_Y			
#define KEY_A		SDL_SCANCODE_A			
#define KEY_S		SDL_SCANCODE_S			
#define KEY_D		SDL_SCANCODE_D			
#define KEY_F		SDL_SCANCODE_F			
#define KEY_G		SDL_SCANCODE_G			
#define KEY_ENTER	SDL_SCANCODE_RETURN	
#define KEY_L		SDL_SCANCODE_L			
#define KEY_K		SDL_SCANCODE_K			
#define KEY_J		SDL_SCANCODE_J			
#define KEY_H		SDL_SCANCODE_H			
#define KEY_CAPS	SDL_SCANCODE_LSHIFT	
#define KEY_Z		SDL_SCANCODE_Z			
#define KEY_X		SDL_SCANCODE_X			
#define KEY_C		SDL_SCANCODE_C			
#define KEY_V		SDL_SCANCODE_V			
#define KEY_SPACE	SDL_SCANCODE_SPACE	
#define KEY_SYMBOL	SDL_SCANCODE_RSHIFT	
#define KEY_M		SDL_SCANCODE_M			
#define KEY_N		SDL_SCANCODE_N			
#define KEY_B		SDL_SCANCODE_B		

enum EBitsNum { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7, BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15 };

#define Hi 255
#define Lo 200
#define Nl 0