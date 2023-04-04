#pragma once
#include "common.h"

using namespace oxygine;
using namespace std;

class CTweenSpring
{
private:
	bool m_bIsDown = false;

	int m_n = 0;

	float m_upValue = 0.0f;
	float m_downValue = 0.0f;
	float m_maxX = 0.0f;
	float m_Scale;

public:
	typedef Actor type;

	CTweenSpring(float Scale = 1.0f, bool bIsDown = true, float upValue = 0.25, float downValue = 0.25, int n = 8 ) :
	m_bIsDown(bIsDown),
	m_upValue(upValue),
	m_downValue(downValue),
	m_n(n),
	m_Scale(Scale)
	{
		m_maxX = PI * static_cast<float>(n);
	}
	
	void init(Actor &actor) { }
	void update(Actor &actor, float p, const UpdateState& us);
	void done(type &actor) { actor.setScale(m_Scale); }
};

class CTweenElastic
{
private:
	bool m_bIsDown = false;

	int m_n = 0;

	float m_upValue = 0.0f;
	float m_downValue = 0.0f;
	float m_maxX = 0.0f;
	float m_Scale;

public:
	typedef Actor type;

	CTweenElastic(float Scale = 1.0f, bool bIsDown = true, float upValue = 0.25, float downValue = 0.25, int n = 8);

	void init(Actor &actor) {}
	void update(Actor &actor, float p, const UpdateState& us);
	void done(type &actor) { actor.setScale(m_Scale); }
};

class CTweenToggle
{
private:
	bool m_bIsDown = false;

	int m_n = 0;

	float m_UpValue = 0.0f;
	float m_DownValue = 0.0f;
	float m_StartValue = 0.0f;
	float m_FinishValue = 0.0f;
	float m_Delta = 0.0f;
	float m_MaxX = 0.0f;

public:
	typedef Actor type;

	CTweenToggle(bool bIsDown, float UpValue = 0.25, float DownValue = 0.25, int n = 8, float StartValue = 1.0, float FinishValue = ToggleFinishValue);

	void init(Actor &actor) {}
	void update(Actor &actor, float p, const UpdateState &us);
	void done(type &actor) { actor.setScale(m_FinishValue); }
};

class CTweenDelay
{
private:
	std::function<void(void)> func_;


public:
	typedef Actor type;

	CTweenDelay(std::function<void(void)> func) { func_ = std::move(func); }

	void init(Actor &actor) {}
	void update(Actor &actor, float p, const UpdateState& us);
	void done(type &actor) { }
};

class CTweenZoomIn
{
private:
	bool m_bIsHorizontalForce = false;
	bool m_bIsVerticalForce = false;

	int m_thickness = 0;

	float m_phaseTime[4];

	Vector2 m_actorSize;
	Vector2 m_deltaScale;

public:
	typedef Actor type;

	CTweenZoomIn(bool bIsHorizontalForce = true, bool bIsVerticalForce = true, int thickness = 10, float phaseTimeVal1 = 0.0f, float phaseTimeVal2 = 0.20f, float phaseTimeVal3 = 0.40f, float phaseTimeVal4 = 0.60f);

	void init(Actor &actor);
	void update(Actor &actor, float p, const UpdateState &us);
	void done(type & actor) { /*actor.setScale(0.01f);*/ }
};

class CTweenZoomOut
{
private:
	bool m_bIsHorizontalForce = false;
	bool m_bIsVerticalForce = false;

	int m_thickness = 0;

	float m_phaseTime[4];

	Vector2 m_actorSize;
	Vector2 m_deltaScaleStart;
	Vector2 m_deltaScale;

public:
	typedef Actor type;

	CTweenZoomOut(bool bIsHorizontalForce = true, bool bIsVerticalForce = true, int thickness = 10, float phaseTimeVal1 = 0.0f, float phaseTimeVal2 = 0.40f, float phaseTimeVal3 = 0.60f, float phaseTimeVal4 = 0.80f);

	void init(Actor &actor);
	void update(Actor &actor, float p, const UpdateState &us);
		void done(type & actor) { actor.setScale(1.0f); }
};

// 		char str[255];
// 		safe_sprintf(str, "frames-%s", name);
// 		const char* data = rs->getAttribute(str).as_string(0);
// 		if (data)
// 			sscanf(data, "%d-%d", &_start, &_end);

// spSprite sprite = new Sprite;
// const ResAnim* rs = resources.getResAnim("ninja");
// sprite->addTween(MyTweenAnim(rs, "run"), 1000);


