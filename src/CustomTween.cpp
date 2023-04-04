#include "CustomTween.h"



void CTweenSpring::update(Actor &actor, float p, const UpdateState &us)
{
	float w = m_maxX * p;
	if (m_bIsDown)
		w += PI;

	float value = static_cast<float>(sinf(w) * powf(E, -(m_n * p)));

	if (value < 0.0f)
		value *= m_downValue;

	else if (value > 0.0f)
		value *= m_upValue;

	actor.setScale(m_Scale + value);
}

CTweenElastic::CTweenElastic(float Scale /*= 1.0f*/, bool bIsDown /*= true*/, float upValue /*= 0.25*/, float downValue /*= 0.25*/, int n /*= 8*/) :
	m_bIsDown(bIsDown),
	m_upValue(upValue),
	m_downValue(downValue),
	m_n(n),
	m_Scale(Scale)
{
	m_maxX = DPI * static_cast<float>(n);
}

void CTweenElastic::update(Actor &actor, float p, const UpdateState &us)
{
	float w = m_maxX * p;
	if (m_bIsDown)
		w += PI;

	float value = static_cast<float>(sinf(w) * powf(E, -(m_n * p)));

	if (value < 0.0f)
		value *= m_downValue;

	else if (value > 0.0f)
		value *= m_upValue;

	actor.setScale(m_Scale + value, m_Scale - value);
}

CTweenToggle::CTweenToggle(bool bIsDown, float UpValue /*= 0.25*/, float DownValue /*= 0.25*/, int n /*= 8*/, float StartValue /*= 1.0*/, float FinishValue /*= ToggleFinishValue*/) :
	m_bIsDown(bIsDown),
	m_UpValue(UpValue),
	m_DownValue(DownValue),
	m_n(n)
{
	if (m_bIsDown)
	{
		m_StartValue = StartValue;
		m_FinishValue = FinishValue;
	}
	else
	{
		m_StartValue = FinishValue;
		m_FinishValue = StartValue;
	}

	m_MaxX = PI * static_cast<float>(n);
	m_Delta = m_StartValue - m_FinishValue;
}

void CTweenToggle::update(Actor &actor, float p, const UpdateState &us)
{
	float w = m_MaxX * p;
	if (m_bIsDown)
		w += PI;

	float value = static_cast<float>(sinf(w) * powf(E, -(m_n * p)));

	if (value < 0.0f)
		value *= m_DownValue;

	else if (value > 0.0f)
		value *= m_UpValue;

	float g = m_FinishValue + m_Delta / powf(3, (p * 8));

	actor.setScale((1.0f + value) * g);
}

void CTweenDelay::update(Actor &actor, float p, const UpdateState &us)
{
	if (p == 1.0f)
		func_();
}

CTweenZoomIn::CTweenZoomIn(bool bIsHorizontalForce /*= true*/, bool bIsVerticalForce /*= true*/, int thickness /*= 10*/, float phaseTimeVal1 /*= 0.0f*/, float phaseTimeVal2 /*= 0.20f*/, float phaseTimeVal3 /*= 0.40f*/, float phaseTimeVal4 /*= 0.60f*/) :
	m_bIsHorizontalForce(bIsHorizontalForce),
	m_bIsVerticalForce(bIsVerticalForce),
	m_thickness(thickness)
{

	m_phaseTime[0] = phaseTimeVal1;
	m_phaseTime[1] = phaseTimeVal2;
	m_phaseTime[2] = phaseTimeVal3;
	m_phaseTime[3] = phaseTimeVal4;
}

void CTweenZoomIn::init(Actor &actor)
{

	m_actorSize = actor.getSize();
	m_deltaScale = Vector2(1.0f - m_thickness / m_actorSize.x, 1.0f - m_thickness / m_actorSize.y);
}

void CTweenZoomIn::update(Actor &actor, float p, const UpdateState &us)
{
	if (m_bIsHorizontalForce)
	{
		if (p > m_phaseTime[0] && p <= m_phaseTime[1])
		{
			float scaleFactor = p / (m_phaseTime[1] - m_phaseTime[0]);
			actor.setScaleX(1.0f - m_deltaScale.x * scaleFactor);
		}
	}
	else
	{
		if (p > m_phaseTime[2] && p <= m_phaseTime[3])
		{
			float scaleFactor = (p - m_phaseTime[2]) / (m_phaseTime[3] - m_phaseTime[2]);
			actor.setScaleX(1.0f - m_deltaScale.x * scaleFactor);
		}
	}

	if (m_bIsVerticalForce)
	{
		if (p > m_phaseTime[0] && p <= m_phaseTime[1])
		{
			float scaleFactor = p / (m_phaseTime[1] - m_phaseTime[0]);
			actor.setScaleY(1.0f - m_deltaScale.y * scaleFactor);
		}
	}
	else
	{
		if (p > m_phaseTime[2] && p <= m_phaseTime[3])
		{
			float scaleFactor = (p - m_phaseTime[2]) / (m_phaseTime[3] - m_phaseTime[2]);
			actor.setScaleY(1.0f - m_deltaScale.y * scaleFactor);
		}
	}
}

CTweenZoomOut::CTweenZoomOut(bool bIsHorizontalForce /*= true*/, bool bIsVerticalForce /*= true*/, int thickness /*= 10*/, float phaseTimeVal1 /*= 0.0f*/, float phaseTimeVal2 /*= 0.40f*/, float phaseTimeVal3 /*= 0.60f*/, float phaseTimeVal4 /*= 0.80f*/) :
	m_bIsHorizontalForce(bIsHorizontalForce),
	m_bIsVerticalForce(bIsVerticalForce),
	m_thickness(thickness)
{
	m_phaseTime[0] = phaseTimeVal1;
	m_phaseTime[1] = phaseTimeVal2;
	m_phaseTime[2] = phaseTimeVal3;
	m_phaseTime[3] = phaseTimeVal4;
}

void CTweenZoomOut::init(Actor &actor)
{
	actor.setScale(1.0f);
	m_actorSize = actor.getSize();
	m_deltaScaleStart = Vector2(m_thickness / m_actorSize.x, m_thickness / m_actorSize.y);
	m_deltaScale = Vector2(1.0f - m_deltaScaleStart.x, 1.0f - m_deltaScaleStart.y);
	actor.setScale(m_deltaScaleStart);
}

void CTweenZoomOut::update(Actor &actor, float p, const UpdateState &us)
{
	if (m_bIsHorizontalForce)
	{
		if (p > m_phaseTime[1] && p <= m_phaseTime[2])
		{
			float Scale = ((p - m_phaseTime[1]) / (m_phaseTime[2] - m_phaseTime[1])) * m_deltaScale.x + m_deltaScaleStart.x;
			actor.setScaleX(Scale);
		}
	}
	else
	{
		if (p > m_phaseTime[3] && p <= 1.0f)
		{
			float Scale = ((p - m_phaseTime[3]) / (1.0f - m_phaseTime[3])) * m_deltaScale.x + m_deltaScaleStart.x;
			actor.setScaleX(Scale);
		}
	}

	if (m_bIsVerticalForce)
	{
		if (p > m_phaseTime[0] && p <= m_phaseTime[1])
		{
			float Scale = ((p - m_phaseTime[1]) / (m_phaseTime[2] - m_phaseTime[1])) * m_deltaScale.x + m_deltaScaleStart.y;
			actor.setScaleY(Scale);
		}
	}
	else
	{
		if (p > m_phaseTime[3] && p <= 1.0f)
		{
			float Scale = ((p - m_phaseTime[3]) / (1.0f - m_phaseTime[3])) * m_deltaScale.x + m_deltaScaleStart.x;
			actor.setScaleY(Scale);
		}
	}
}
