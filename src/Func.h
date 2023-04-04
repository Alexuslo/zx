#pragma once
#include "common.h"
#include "Scene.h"
#include "Button.h"

using namespace oxygine;
class CApp;

class CBase 
{
private:

public:
    CBase() {};

    ~CBase() {};

    virtual void Init() {};

    virtual void Run() {};

	virtual void Update() {};

    virtual void Update(float dtf) {};

    virtual void Update(int dti) {};
};

class CMemory;
class CMemory48;
class CMemory128;
class CMemoryCustom;
class CIo;
class CGraphics;
class CCpu;
class CSnapShot;

class CDevice : public CBase
{
	static bool m_bIsWinDetected;

	bool m_CpuStart = false;
	bool m_Border = false;
	bool m_bStartTape = false;
	bool m_LastStateTapeBit = false;
	bool m_fullAcceleration = false;
	bool m_bIspaused = true;
	bool m_bIsNeedPressEnterOrNull = false;

	int m_state = -1;
	int m_laststate = m_state;
	int m_duration = 0;

	uint32_t m_lastTime = 0;
	uint32_t m_timerInt = 0;
	uint32_t m_timerFlash = 0;

	double m_total = 0.0;

	spCScene m_ParentScene = nullptr;
	spActor m_cursorKeys = nullptr;
	spCButton m_fireKey = nullptr;
	spController m_joystick = nullptr;

	CMemory *m_memory = nullptr;
	CIo *m_io = nullptr;
	CGraphics *m_graphics = nullptr;
	CCpu *m_cpu = nullptr;
	CSnapShot *m_snapShot = nullptr;

	// 	CTimeMaster *TimeMaster;
	// 	CTape *Tape;
	// 	CSettings *Settings;

public:
	CDevice(spCScene parentScene, spActor cursorKeys,spController joystick, spCButton fireButton);
	~CDevice() { }

	void Render() { }
	void Update();
	void WriteByte() { }
	void ResetRequest() { }
	void Init() { }
	void Run() { m_bIspaused = false; }
	void Stop() { m_bIspaused = true; }
	void Update(int dti) { }
	void Update(float dtf) { }
	bool IsFullAcceleration() const { return m_fullAcceleration; }
	void SetFullAcceleration(bool val) { m_fullAcceleration = val; }
	void PopSnapShot();
	void PushSnapShot();
	void SaveSnapShot(std::string name);
	void LoadSnapShot(std::string name);

	void SetNeedPressEnterOrNull() { m_bIsNeedPressEnterOrNull = true; }
	bool IsNeedPressEnterOrNull() 
	{
		if (m_bIsNeedPressEnterOrNull)
		{
			m_bIsNeedPressEnterOrNull = false;
			return true;
		}
		else
			return false;
	}
	CCursorKeys *GetCursorKeys() const { return static_cast<CCursorKeys *>(m_cursorKeys.get()); }
	CButton *GetFireButton() const { return m_fireKey.get(); }
	CJoystick *GetJoustick() const { return m_joystick.get(); }
};

