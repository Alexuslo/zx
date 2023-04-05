#include "Button.h"
#include "App.h"
#include "Scene.h"

CButton::CButton(CApp *pApp, CScene *Scene, std::string Name, std::string Caption, std::string FontName, const ResAnim *Texture) :
m_pApp(pApp),
m_Scene(Scene)
{
	setName(Name);
	
	if (m_pApp->IsUseShadow())
	{
		if (Texture)
		{
			m_Shadow = new Sprite();
			m_Shadow->setResAnim(Texture);
		}
		else
			m_Shadow = new ColorRectSprite();

		m_Shadow->setColor(ShadowColor);
		m_Shadow->setAnchor(0.5f, 0.5f);
		m_Shadow->setScale(ShadowScale);
		m_Shadow->setName(Name);
	}

	if (Scene)
	{
		if (m_Shadow)
			m_Shadow->attachTo(m_Scene->GetShadowsActor());
		
		attachTo(m_Scene->GetControlsActor());
	}

	m_TextField = new TextField();
	m_TextField->setAnchor(0.5f, 0.5f);
	m_TextField->attachTo(this);
	m_TextField->setFontSize(static_cast<int>(getHeight() / 1.6f));

	ResFont *font = m_pApp->getDefaultFont();

	TextStyle style = TextStyle(font).withColor(Color::White).alignMiddle();
	m_TextField->setStyle(style);

	m_TextField->setText(Caption);
	m_TextField->setPosition(getSize() / 2.0f);

	setResAnim(Texture);
}

CButton::~CButton()
{
	
}

void CButton::setText(std::string text)
{
	m_TextField->setText(text);

	m_TextField->setPosition(getSize() / 2.0f);
}

void CButton::UpdatePriority(int priorityDelta)
{
	int selfPriority = getPriority();
	setPriority(selfPriority + priorityDelta);
	if (m_Shadow)
		m_Shadow->setPriority(selfPriority + priorityDelta);
	if (m_TextField)
		m_TextField->setPriority(selfPriority + priorityDelta);
}

void CButton::SetSize(Vector2 size)
{
	setSize(size);
	if (m_Shadow)
		m_Shadow->setPosition(size / 2.0f);
	if (m_TextField)
		m_TextField->setPosition(size / 2.0f);
}

void CButton::UpdateShadow()
{
	if (m_Shadow)
	{
		m_Shadow->setSize(getSize());
		m_Shadow->setPosition(getPosition() + Vector2(ShadowOffset));
	}
}

void CButton::AddVisualFunc()
{
#ifdef __ANDROID__
	AddOverIn();
	AddOverOut();
	AddDown();
	AddUp();
#else
	//AddOverIn();
	//AddOverOut();
	AddClick();
	AddTouchDown();
	AddTouchUp();
#endif
}

void CButton::OverIn(Event *e)
{
	if (!m_bIsEnabled) return;

	switch (m_OverInType)
	{
	case EOverType::AddColor:
		addTween(Sprite::TweenAddColor(ColorExt::Over), 200);
		break;

	case EOverType::AddSize:
		addTween(Actor::TweenScale(OverAddSize), 200);
		//setPriority(getPriority() + 1000);
		break;

	case EOverType::AddSpring:
		addTween(CTweenSpring(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		if (m_Shadow)
			m_Shadow->addTween(CTweenSpring(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		break;

	case EOverType::AddElastik:
		addTween(CTweenElastic(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		if (m_Shadow)
			m_Shadow->addTween(CTweenElastic(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		break;

	default:
		break;
	}

	m_bIsOverIn = true;
}

void CButton::OverOut(Event *e)
{
	if (!m_bIsEnabled) return;

	switch (m_OverOutType)
	{
	case EOverType::AddColor:
		if (!m_bIsPressed)
			addTween(Sprite::TweenAddColor(ColorExt::Out), 200);

		else
			addTween(Sprite::TweenAddColor(ColorExt::Press), 200);

		break;

	case EOverType::AddSize:
		addTween(Actor::TweenScale(1.0f), 200);
		//setPriority(getPriority() + 1000);
		break;

	case EOverType::AddSpring:
		addTween(CTweenSpring(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		if (m_Shadow)
			m_Shadow->addTween(CTweenSpring(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);

		break;

	case EOverType::AddElastik:
		addTween(CTweenElastic(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		if (m_Shadow)
			m_Shadow->addTween(CTweenElastic(m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
		break;

	default:
		break;
	}

	m_LastPosition.setZero();
	m_Distance.setZero();
	m_bIsOverIn = false;
}

void CButton::Click(Event *e)
{
	if (!m_bIsEnabled) return;

	if (!m_bIsSlider)
	{
		if (m_bIsToggle)
			m_bIsPressed = !m_bIsPressed;

		switch (m_ClickType)
		{
		case EClickType::None:
			break;

		case EClickType::AddColor:
		{
			Color color;

			if (!m_bIsToggle)
				addTween(Sprite::TweenColor(ColorExt::Click), 200, 1, true);
			else
				addTween(Sprite::TweenAddColor(m_bIsPressed ? ColorExt::Press : ColorExt::Out), 200, 1, false);
		}

		break;

		case EClickType::AddSize:
			if (!m_bIsToggle)
				addTween(Actor::TweenScale(ClickScale), 200, 1, true);
			else
				addTween(CTweenToggle(m_bIsPressed), 1000);
			break;

		case EClickType::AddMove:
		{
			Vector2 Position = getPosition();
			if (m_bIsToggle && !m_bIsPressed)
			{
				Position -= Vector2(ClickMove, ClickMove);
				setPriority(getPriority() + ClickPriority);
			}
			else
			{
				Position += Vector2(ClickMove, ClickMove);
				setPriority(getPriority() - ClickPriority);
			}
			addTween(Actor::TweenPosition(Position), 200, 1, !m_bIsToggle);
			break;
		}

		case EClickType::AddSpring:
			addTween(CTweenSpring(m_bIsToggle && m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
			if (m_Shadow)
				m_Shadow->addTween(CTweenSpring(m_bIsToggle && m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
			break;

		case EClickType::AddElastik:
			addTween(CTweenElastic(m_bIsToggle && m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
			if (m_Shadow)
				addTween(CTweenElastic(m_bIsToggle && m_bIsPressed ? ToggleFinishValue : 1.0f), 1000);
			break;

		default:
			break;
		}
	}
	else
	{
		//TO DO slider click
	}
}

void CButton::Move(Event *e)
{
	if (m_bIsSlider && m_bIsTouch)
	{
		Point CurrentPosition;
		SDL_GetMouseState(&CurrentPosition.x, &CurrentPosition.y);
		m_Distance = CurrentPosition - m_LastPosition;

		if (!m_bCheckSwipe || m_bIsSwipeOn)
			m_LastPosition = CurrentPosition;

		if (!m_bIsSwipeOn)
		{
			if (m_bCheckSwipe)
			{
				if (abs(m_Distance.x) >= SwipeDist || abs(m_Distance.y) >= SwipeDist)
					m_bIsSwipeOn = true;
			}
			else
				m_bIsSwipeOn = true;
		}

		if (m_bIsSwipeOn)
		{
			Vector2 Distance(m_bHorizontalScroll ? static_cast<float>(m_Distance.x) : 0.0f, m_bVerticalScroll ? static_cast<float>(m_Distance.y) : 0.0f);
			setPosition(getPosition() + Distance);
		}
	}

	if (m_Shadow)
		UpdateShadow();
}

void CButton::TouchDown(Event *e)
{
	m_bIsTouch = true;
	if (m_bIsSlider)
	{
		SDL_GetMouseState(&m_LastPosition.x, &m_LastPosition.y);
	}
}

void CButton::TouchDownOver(Event *e)
{
	m_bIsTouch = true;
	if (m_bIsSlider)
	{
		SDL_GetMouseState(&m_LastPosition.x, &m_LastPosition.y);
	}

	OverIn(e);
}

void CButton::TouchUp(Event *e)
{
	m_bIsTouch = false;
	m_bIsSwipeOn = false;
	m_LastPosition.setZero();
	m_Distance.setZero();
}

void CButton::TouchUpOver(Event *e)
{
	m_bIsTouch = false;
	m_bIsSwipeOn = false;
	m_LastPosition.setZero();
	m_Distance.setZero();

	OverOut(e);
}

void CCursorKeys::Up(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_Up = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_Up = false;
		m_UpT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_UpT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_UpT = false;
#ifdef __ANDROID__
		m_Up = false;
#endif
	}
}

void CCursorKeys::Down(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_Down = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_Down = false;
		m_DownT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_DownT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_DownT = false;
#ifdef __ANDROID__
		m_Down = false;
#endif
	}
}

void CCursorKeys::Left(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_Left = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_Left = false;
		m_LeftT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_LeftT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_LeftT = false;
#ifdef __ANDROID__
		m_Left = false;
#endif
	}
}

void CCursorKeys::Right(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_Right = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_Right = false;
		m_RightT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_RightT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_RightT = false;
#ifdef __ANDROID__
		m_Right = false;
#endif
	}
}

void CCursorKeys::UpRight(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_UpRight = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_UpRight = false;
		m_UpRightT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_UpRightT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_UpRightT = false;
#ifdef __ANDROID__
		m_UpRight = false;
#endif
	}
}

void CCursorKeys::UpLeft(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_UpLeft = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_UpLeft = false;
		m_UpLeftT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_UpLeftT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_UpLeftT = false;
#ifdef __ANDROID__
		m_UpLeft = false;
#endif
	}
}

void CCursorKeys::DownRight(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_DownRight = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_DownRight = false;
		m_DownRightT = false;
	}
	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_DownRightT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_DownRightT = false;
#ifdef __ANDROID__
		m_DownRight = false;
#endif
	}
}

void CCursorKeys::DownLeft(Event *e)
{
	if (e->type == TouchEvent::OVER)
		m_DownLeft = true;

	else if (e->type == TouchEvent::OUTX)
	{
		m_DownLeft = false;
		m_DownLeftT = false;
	}

	else if (e->type == TouchEvent::TOUCH_DOWN)
		m_DownLeftT = true;

	else if (e->type == TouchEvent::TOUCH_UP)
	{
		m_DownLeftT = false;
#ifdef __ANDROID__
		m_DownLeft = false;
#endif
	}
}

CCursorKeys::CCursorKeys(CApp *pApp, CScene *scene, const char *id, int corner/* = 0*/, float sizeOfSmallScreen/* = 0.3f*/, char *fontName /*= "default"*/, float separator/* = 5*/) :
m_pApp(pApp),
m_Scene(scene)
{
	setName(id);
	float blockSize = static_cast<float>(std::fminf(static_cast<float>(core::getDisplaySize().x), static_cast<float>(core::getDisplaySize().y))) * sizeOfSmallScreen;
	float buttonSize = std::floorf((blockSize - separator * 4.0f)/ 3.0f);
	float buttonOffset = buttonSize + separator;

	m_KeyActor = new Actor();
	m_KeyActor->setSize(blockSize, blockSize);
	m_KeyActor->setAnchor(0.5f, 0.5f);

	m_ShadowActor = new Actor();
	m_ShadowActor->setSize(blockSize, blockSize);
	m_ShadowActor->setAnchor(0.5f, 0.5f);

	spSprite substrate = new ColorRectSprite();
	substrate->setSize(blockSize, blockSize);
	substrate->setColor(Color(64, 64, 32, 100));
	substrate->setAnchor(0.5f, 0.5f);
	substrate->attachTo(m_KeyActor);
	substrate->setPosition(m_KeyActor->getSize() / 2.0f);


	struct InfoButton
	{
		const char *m_Caption;
		int m_mulX = 0;
		int m_mulY = 0;
		int m_priorityFactor = 0;
		float m_scaleX = 1.0f;
		float m_scaleY = 1.0f;
		//void (CCursorKeys::*m_func)(Event *e) = nullptr;
		using CallBackFunc = void(CCursorKeys::*)(Event * e);
		CallBackFunc m_func = nullptr;

		InfoButton(const char *caption, int mulX, int mulY, int priorityFactor, float scaleX, float scaleY, CallBackFunc func) :
			m_Caption(caption),
			m_mulX(mulX),
			m_mulY(mulY),
			m_priorityFactor(priorityFactor),
			m_scaleX(scaleX),
			m_scaleY(scaleY),
			m_func(func)
		{

		}
	};

	std::array<InfoButton, 8> infoButtons = 
	{ 
		InfoButton("up",		0, -1, 1,  1.2f, 1.0f, &CCursorKeys::Up),
		InfoButton( "",			1, -1, 0,  1.0f, 1.0f, &CCursorKeys::UpRight),
		InfoButton( "right",	1,  0, 1,  1.0f, 1.2f, &CCursorKeys::Right),
		InfoButton( "",			1,  1, 0,  1.0f, 1.0f, &CCursorKeys::DownRight),
		InfoButton( "down",		0,  1, 1,  1.2f, 1.0f, &CCursorKeys::Down),
		InfoButton( "",		   -1,  1, 0,  1.0f, 1.0f, &CCursorKeys::DownLeft),
		InfoButton( "left",    -1,  0, 1,  1.0f, 1.2f, &CCursorKeys::Left),
		InfoButton( "",	       -1, -1, 0,  1.0f, 1.0f, &CCursorKeys::UpLeft)
	};

	int idSuffix = 1;
	for (auto i : infoButtons)
	{
		std::string Name = std::string(id) + "_" + std::to_string(idSuffix);

		spCButton b = new CButton(pApp, nullptr, Name, i.m_Caption, "Default", pApp->getTexture("button2"));

		b->SetIndex(idSuffix);
		b->SetSize(buttonSize * i.m_scaleX, buttonSize * i.m_scaleY);
		b->attachTo(m_KeyActor);
		b->setAnchor(0.5f, 0.5f);
		b->setPosition(buttonOffset * i.m_mulX + blockSize / 2.0f, buttonOffset * i.m_mulY + blockSize / 2.0f);
		if (pApp->IsUseShadow())
		{
			b->GetShadov()->attachTo(m_ShadowActor);
			b->UpdateShadow();
		}
		b->UpdatePriority(i.m_priorityFactor);

		b->AddVisualFunc();
		
		m_CursorKeys.push_back(b);
	}

	m_CursorKeys[0]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::Up));
	m_CursorKeys[0]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::Up));
	m_CursorKeys[0]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::Up));
	m_CursorKeys[0]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::Up));

	m_CursorKeys[1]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::UpRight));
	m_CursorKeys[1]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::UpRight));
	m_CursorKeys[1]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::UpRight));
	m_CursorKeys[1]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::UpRight));

	m_CursorKeys[2]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::Right));
	m_CursorKeys[2]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::Right));
	m_CursorKeys[2]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::Right));
	m_CursorKeys[2]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::Right));

	m_CursorKeys[3]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::DownRight));
	m_CursorKeys[3]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::DownRight));
	m_CursorKeys[3]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::DownRight));
	m_CursorKeys[3]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::DownRight));

	m_CursorKeys[4]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::Down));
	m_CursorKeys[4]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::Down));
	m_CursorKeys[4]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::Down));
	m_CursorKeys[4]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::Down));

	m_CursorKeys[5]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::DownLeft));
	m_CursorKeys[5]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::DownLeft));
	m_CursorKeys[5]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::DownLeft));
	m_CursorKeys[5]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::DownLeft));

	m_CursorKeys[6]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::Left));
	m_CursorKeys[6]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::Left));
	m_CursorKeys[6]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::Left));
	m_CursorKeys[6]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::Left));

	m_CursorKeys[7]->addEventListener(TouchEvent::OVER, CLOSURE(this, &CCursorKeys::UpLeft));
	m_CursorKeys[7]->addEventListener(TouchEvent::OUTX, CLOSURE(this, &CCursorKeys::UpLeft));
	m_CursorKeys[7]->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CCursorKeys::UpLeft));
	m_CursorKeys[7]->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CCursorKeys::UpLeft));

	Vector2 size;
	Vector2 position;

	if (scene)
	{
		m_KeyActor->attachTo(scene->GetControlsActor());
		m_ShadowActor->attachTo(scene->GetShadowsActor());
		size = scene->getSelfSize();
	}
	else
	{
		m_KeyActor->attachTo(pApp->GetControlsActor());
		m_ShadowActor->attachTo(scene->GetShadowsActor());
		size = getStage()->getSize();
	}
	
	if (corner == 0)
		position = Vector2(blockSize / 2.0f, blockSize / 2.0f);

	else if (corner == 1)
		position = Vector2(size.x - blockSize / 2.0f, blockSize / 2.0f);

	else if (corner == 3)
		position = Vector2(size.x - blockSize / 2.0f, size.y - blockSize / 2.0f);

	else if (corner == 4)
		position = Vector2(blockSize / 2.0f, size.y - blockSize / 2.0f);
	else
		position = size / 2.0f;


	m_KeyActor->setPosition(position);
	m_ShadowActor->setPosition(position);
}

void CSettingsButton::update(const UpdateState &us)
{
	Button::update(us);
	if (m_bIsAccelerating)
	{
		m_angleDelta += Acceleration;
		if (m_angleDelta > MaxRotation)
		{
			m_angleDelta = MaxRotation;
			m_bIsAccelerating = false;
		}
	}
	
	m_angle += m_angleDelta;
	setRotationDegrees(m_angle);
}


CJoystick::CJoystick(CApp *pApp, CScene *scene) : 
	m_pApp(pApp),
	m_scene(scene),
	m_bIsPressed(false), 
	m_dir(0, 0)
{
	Vector2 stageSize = Vector2(getStage()->getWidth() / 2.0f, getStage()->getHeight());

	for (float i : {0.0f, PI / 2.0f, PI, PI * 3.0f / 2.0f})
	{
		CDirectionData directionData(this);
		
		directionData.Create(i, m_AnglePress, m_AngleInterpolate);
		
		m_directionData.push_back(directionData);
	}

	setAnchor(0.5f, 0.5f);
	setPosition(stageSize / 2.0f);
	setSize(stageSize);
	setAlpha(100);

	//setResAnim(m_pApp->getTexture("joystick"));
	//setAlpha(128);

	//handle touch events
	addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CJoystick::onEvent));
	addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CJoystick::onEvent));
	addEventListener(TouchEvent::MOVE, CLOSURE(this, &CJoystick::onEvent));

	m_finger = new Sprite;
	m_finger->setResAnim(m_pApp->getTexture("finger"));
	m_finger->attachTo(this);
	m_finger->setVisible(false);
	m_finger->setAnchor(Vector2(0.5f, 0.5f));
	m_finger->setTouchEnabled(false);

	m_Center = new Sprite;
	m_Center->setResAnim(m_pApp->getTexture("finger"));
	m_Center->attachTo(this);
	m_Center->setVisible(false);
	m_Center->setAnchor(Vector2(0.5f, 0.5f));
	m_Center->setTouchEnabled(false);
}

void CJoystick::onEvent(Event *e)
{
	TouchEvent *te = safeCast<TouchEvent *>(e);

	//if player touched down
	if (te->type == TouchEvent::TOUCH_DOWN)
	{
		m_Center->setVisible(true);
		m_finger->setVisible(true);
		//setColor(Color::Red);
		m_bIsPressed = true;
		m_pos = te->localPosition;

		m_Center->setPosition(m_pos);
	}

	//if player touched up
	if (te->type == TouchEvent::TOUCH_UP)
	{
		m_Center->setVisible(false);
		m_finger->setVisible(false);
		setColor(Color::White);
		m_bIsPressed = false;
	}

	if (te->type == TouchEvent::MOVE)
	{
	}

	Vector2 center = m_Center->getPosition();
	m_dir = te->localPosition - center;

// 	if (m_dir.length() > 100)
// 		m_dir.normalizeTo(100);

	m_finger->setPosition(center + m_dir);

	if (!m_bIsPressed)
	{
		m_dir = Vector2(0, 0);
	}
}

bool CJoystick::GetDirection(Vector2 &dir) const
{
	dir = m_dir;
	return m_bIsPressed;
}

bool CJoystick::GetStateDirection(std::tuple<bool, bool, bool, bool> &stateDirection)
{
	float lenght = m_dir.length();
	float angle = m_dir.x > 0.0 ? (acosf(-m_dir.y / lenght)) : 2 * PI - (acosf(-m_dir.y / lenght));
	
	DebugActor::addDebugString(" ANGLE - %f", angle);

	for (auto &i : m_directionData /*/= 0; i < static_cast<int>(m_directionData.size()); i++*/)
		i.Update(angle);

//	stateDirection = std::tuple<bool, bool, bool, bool>(m_directionData[0].GetState(), m_directionData[1].GetState(), m_directionData[2].GetState(), m_directionData[3].GetState());

	std::get<0>(stateDirection) = m_directionData[0].GetState();
	std::get<1>(stateDirection) = m_directionData[1].GetState();
	std::get<2>(stateDirection) = m_directionData[2].GetState();
	std::get<3>(stateDirection) = m_directionData[3].GetState();

	return m_bIsPressed;
}

void CJoystick::Update()
{

}

void CJoystick::CDirectionData::Create(float axis, float anglePress, float angleInterpolate)
{
	m_LeftCenter = m_RightCenter = axis;
	m_AnglePress = anglePress;
	m_AngleInterpolate = angleInterpolate;

	m_LeftCenter = m_RightCenter = axis;
	m_LeftValuePress = axis - anglePress;
	m_RightValuePress = axis + anglePress;

	if (m_LeftValuePress < 0.0f)
	{
		m_LeftValuePress += 2 * PI;
		m_LeftCenter += 2 * PI;
	}

	m_LeftValueInterpolate = m_LeftValuePress - angleInterpolate;
	m_RightValueInterpolate = m_RightValuePress + angleInterpolate;
}

void CJoystick::CDirectionData::Update(float angle)
{
	m_bIsPressed = false;

	if (m_bIsInterpolated)
	{
		m_nFrame++;
		int MaxFrames = static_cast<int>(JoustickFrames);

		if (m_nFrame >= MaxFrames)
			m_nFrame %= MaxFrames;

		m_InterpolatedFactor = 0.0f;
	}

	if ((angle >= m_LeftValuePress && angle <= m_LeftCenter) || (angle >= m_RightCenter && angle <= m_RightValuePress))
	{
		m_bIsPressed = true;
	}
	else if (m_bIsInterpolated && angle >= m_LeftValueInterpolate && angle < m_LeftValuePress)
	{
		m_InterpolatedFactor = (m_LeftValuePress - angle) / m_AngleInterpolate;

		m_nPressFrames = static_cast<int>(std::floorf(m_InterpolatedFactor * static_cast<float>(JoustickFrames)));
		m_nNoPressFrames = static_cast<int>(JoustickFrames) - m_nPressFrames;

		m_bIsPressed = m_nFrame < m_nNoPressFrames ? true : false;
	}
	else if (m_bIsInterpolated && angle > m_RightValuePress && angle <= m_RightValueInterpolate)
	{
		m_InterpolatedFactor = (m_RightValueInterpolate - angle) / m_AngleInterpolate;

		m_nPressFrames = static_cast<int>(std::floorf(m_InterpolatedFactor * static_cast<float>(JoustickFrames)));
		m_nNoPressFrames = static_cast<int>(JoustickFrames) - m_nPressFrames;

		m_bIsPressed = m_nFrame < m_nPressFrames ? true : false;
	}
}
