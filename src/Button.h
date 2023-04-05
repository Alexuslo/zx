#pragma once
#include "common.h"
#include "CustomTween.h"

class CButton : public Button
{
	bool m_bIsEnabled = true;
	bool m_bIsToggle = false;
	bool m_bIsSlider = false;
	bool m_bIsPressed = false;
	bool m_bIsTouch = false;
	bool m_bIsSwipeOn = false;
	bool m_bHorizontalScroll = false;
	bool m_bVerticalScroll = false;
	bool m_bCheckSwipe = false;
	bool m_bIsOverIn = false;
	
	int m_Index = 0;

	CApp *m_pApp = nullptr;
	CScene *m_Scene = nullptr;

	spSprite m_Shadow;
	
	spTextField m_TextField;

	EOverType m_OverInType = EOverType::None;
	EOverType m_OverOutType = EOverType::None;
	EClickType m_ClickType = EClickType::None;

	Point m_LastPosition;
	Point m_Distance;
	
public:
	CButton(CApp *pApp, CScene *Scene, std::string Name, std::string Caption, std::string FontName, const ResAnim *Texture = nullptr);
	~CButton();

	//void update(const UpdateState &us) {}

	void setText(std::string text);

	spSprite GetShadov() const { return m_Shadow; }

	bool IsEnabled() { return m_bIsEnabled; }
	void SetEnabled() { m_bIsEnabled = true; }
	void SetDisabled() { m_bIsEnabled = false; }

	void UpdatePriority(int priorityDelta);

	void SetSize(float xy) { SetSize(Vector2(xy, xy)); }
	void SetSize(float x, float y) { SetSize(Vector2(x, y)); }
	void SetSize(Vector2 size);

	EOverType GetOverInType() const { return m_OverInType; }
	CButton *SetOverInType(EOverType val) { m_OverInType = val; return this; }
	EOverType GetOverOutType() const { return m_OverOutType; }
	CButton *SetOverOutType(EOverType val) { m_OverOutType = val; return this; }
	EClickType GetClickType() const { return m_ClickType; }
	CButton *SetClickType(EClickType val) { m_ClickType = val; return this; }

	void SetToggle(bool bIsToggle) { m_bIsToggle = bIsToggle; }
	bool IsToggle() { return m_bIsToggle; }

	void SetSlider(bool bIsSlider) { m_bIsSlider = bIsSlider; }
	bool IsSlider() { return m_bIsSlider; }

	void SetPressed(bool bIsPressed) { m_bIsPressed = bIsPressed; }
	bool IsPressed() { return m_bIsPressed; }

	bool IsHorizontalScroll() const { return m_bHorizontalScroll; }
	void SetHorizontalScroll(bool val) { m_bHorizontalScroll = val; }
	bool IsVerticalScroll() const { return m_bVerticalScroll; }
	void SetVerticalScroll(bool val) { m_bVerticalScroll = val; }
	bool IsCheckSwipe() const { return m_bCheckSwipe; }
	void SetCheckSwipe(bool val) { m_bCheckSwipe = val; }
		
	CButton *AddOverIn(EOverType OverType = EOverType::AddColor) { m_OverInType = OverType; addEventListener(TouchEvent::OVER, CLOSURE(this, &CButton::OverIn)); return this; }
	CButton *AddOverOut(EOverType OverType = EOverType::AddColor) { m_OverOutType = OverType; addEventListener(TouchEvent::OUTX, CLOSURE(this, &CButton::OverOut)); return this; }
	CButton *AddClick(EClickType ClickType = EClickType::AddColor) { m_ClickType = ClickType; addEventListener(TouchEvent::CLICK, CLOSURE(this, &CButton::Click)); return this; }
	CButton *AddMove() { addEventListener(TouchEvent::MOVE, CLOSURE(this, &CButton::Move)); return this; }

	CButton *AddTouchDown() { addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CButton::TouchDownOver)); return this; }
	CButton *AddTouchUp() { addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CButton::TouchUpOver)); return this; }

	CButton *AddDown() { addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &CButton::TouchDown)); return this; }
	CButton *AddUp() { addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &CButton::TouchUp)); return this; }
	CButton *AddWheelDir() { addEventListener(TouchEvent::WHEEL_DIR, CLOSURE(this, &CButton::WheelDir)); return this; }

	void UpdateShadow();

	void AddVisualFunc();

	void OverIn(Event *e);

	void OverOut(Event *e);

	void Click(Event *e);

	void Move(Event *e);

	void TouchDown(Event *e);

	void TouchDownOver(Event *e);

	void TouchUp(Event *e);

	void TouchUpOver(Event *e);

	void WheelDir(Event *e) { }

	int GetIndex() const { return m_Index; }
	void SetIndex(int val) { m_Index = val; }
	bool IsOverIn() const { return m_bIsOverIn; }
	bool IsTouch() const { return m_bIsTouch; }
	bool IsOverTouch() const { return m_bIsOverIn || m_bIsTouch; }
};

class CCursorKeys : public Actor
{
	bool m_Up = false;
	bool m_Down = false;
	bool m_Left = false;
	bool m_Right = false;
	bool m_UpLeft = false;
	bool m_UpRight = false;
	bool m_DownLeft = false;
	bool m_DownRight = false;

	bool m_UpT = false;
	bool m_DownT = false;
	bool m_LeftT = false;
	bool m_RightT = false;
	bool m_UpLeftT = false;
	bool m_UpRightT = false;
	bool m_DownLeftT = false;
	bool m_DownRightT = false;

	CApp *m_pApp = nullptr;
	CScene *m_Scene = nullptr;

	spActor m_KeyActor;
	spActor m_ShadowActor;

	std::vector<spCButton> m_CursorKeys;

	void Up(Event *e);
	void Down(Event *e);
	void Left(Event *e);
	void Right(Event *e);
	void UpRight(Event *e);
	void UpLeft(Event *e);
	void DownRight(Event *e);
	void DownLeft(Event *e);
	
public:
	CCursorKeys(CApp *pApp, CScene *Scene, const char *Id, int corner = 4, float SizeOfSmallScreen = 0.3f, char *FontName = "default", float Separator = 5);
	~CCursorKeys() {}

	bool IsUp() { return m_Up || m_UpT; }
	bool IsDown() { return m_Down || m_DownT; }
	bool IsLeft() { return m_Left || m_LeftT; }
	bool IsRight() { return m_Right || m_RightT; }
	bool IsUpRight() { return m_UpRight || m_UpRightT; }
	bool IsUpLeft() { return m_UpLeft || m_UpLeftT; }
	bool IsDownRight() { return m_DownRight || m_DownRightT; }
	bool IsDownLeft() { return m_DownLeft || m_DownLeftT; }
};

class CSettingsButton : public CButton
{
	bool m_bIsAccelerating = true;
	
	float m_angle = 0.0f;
	float m_angleDelta = 0.0f;

public:
	CSettingsButton(CApp *pApp, CScene *Scene, std::string Name, std::string Caption, std::string FontName = "default", const ResAnim *Texture = nullptr) :
		CButton(pApp, Scene, Name, Caption, FontName, Texture)
	{ }

	~CSettingsButton() { }

	void update(const UpdateState &us);
};


DECLARE_SMART(CJoystick, spController);
class CJoystick : public Sprite
{
	class CDirectionData
	{
		float m_LeftValueInterpolate = 0.0f;
		float m_LeftValuePress = 0.0f;
		float m_LeftCenter = 0.0f;
		float m_RightCenter = 0.0f;
		float m_RightValuePress = 0.0f;
		float m_RightValueInterpolate = 0.0f;

		float m_AngleInterpolate = 0.0f;
		float m_AnglePress = 0.0f;

		float m_InterpolatedFactor = 0.0f;

		int m_nPressFrames = 0;
		int m_nNoPressFrames = 0;
		int m_nFrame = 0;

		bool m_bisInterpolated = false;

		bool m_bIsPressed = false;
		bool m_bIsInterpolated = false;

		CJoystick *m_pJoystick = nullptr;

	public:
		CDirectionData(CJoystick *pJoystick) :
		m_pJoystick(pJoystick) 
		{

		}

		~CDirectionData() 
		{

		}

		bool GetState() const { return m_bIsPressed; }

		void Create(float axis, float anglePress, float angleInterpolate);
		
		void Update(float angle);

		bool IsPressed() { return m_bIsPressed; }
		bool IsInterpolated() { return m_bIsInterpolated; }
		bool IsInterpolated() const { return m_bIsInterpolated; }
		void SetInterpolated(bool val) { m_bIsInterpolated = val; }
	};

	float m_AnglePress = 1.2f;
	float m_AngleInterpolate = PI / 3.0f;
	
	CApp *m_pApp = nullptr;
	CScene *m_scene = nullptr;
	
	std::vector<CDirectionData> m_directionData;

	bool m_bIsPressed = false;
	bool m_bIsInterpolated = false;
	
	Vector2 m_dir;
	Vector2 m_pos;
	
	spSprite m_finger;
	spSprite m_Center;
	
	void onEvent(Event *e);

public:
	CJoystick(CApp *pApp, CScene *scene);

	bool GetDirection(Vector2 &dir) const;
	bool GetStateDirection(std::tuple<bool, bool, bool, bool> &stateDirection);
	void Update();
	bool IsInterpolated() const { return m_bIsInterpolated; }
	void SetInterpolated(bool val) 
	{ 
		m_bIsInterpolated = val; 
		
		for (CDirectionData &i : m_directionData)
			i.SetInterpolated(m_bIsInterpolated);
	}
};