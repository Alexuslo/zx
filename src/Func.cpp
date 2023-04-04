#include "Func.h"
#include "App.h"
#include "Device/Memory.h"
#include "Device/Io.h"
#include "Device/Graphics.h"
#include "Cpu/Cpu.h"
#include "Device/SnapShot.h"

CDevice::CDevice(spCScene parentScene, spActor cursorKeys, spController joystick, spCButton fireButton)
{
	m_ParentScene = parentScene;
	m_joystick = joystick;
	m_cursorKeys = cursorKeys;
	m_fireKey = fireButton;

	m_memory = new CMemory48();
	m_io = new CIo(this);
	m_graphics = new CGraphics(m_memory->GetVideoMemoryPtr(), m_memory->GetAttrMemoryPtr(), /*bIsWinDetected*/false);
	m_graphics->AttachBufferToScene(parentScene, static_cast<int>(fireButton->getWidth() / 2.0f));

	m_cpu = new CCpu(m_memory, m_io);
	m_cpu->Reset();

	m_snapShot = new CSnapShot(m_cpu->GetContext());
	//m_snapShot->LoadSnapShot("snap.esf");
	//m_snapShot->PopSnapShot();
	m_lastTime = getTimeMS();
}

void CDevice::Update()
{
	uint32_t newTime = getTimeMS();
	uint32_t deltaTime = newTime - m_lastTime;
	m_lastTime = newTime;

	m_timerInt += deltaTime;
	if (m_timerInt > FrameTime)
	{
		m_timerInt = 0;
		m_cpu->Int(255);
	}

	m_timerFlash += deltaTime;
	if (m_timerFlash > FlashTime)
	{
		m_timerFlash = 0;
		m_graphics->Flash();
	}

	int tactsPerFrame = CpuTacts / 60;
	int nCurrentTacts = 0;
	int elapsedTime = newTime + FrameTime;


	while (!m_bIspaused && ((nCurrentTacts < tactsPerFrame) || (m_fullAcceleration && elapsedTime > getTimeMS())))
		nCurrentTacts = m_cpu->Update(tactsPerFrame);
	
	m_io->Update();
	m_graphics->UpdateScreen();
}

void CDevice::PopSnapShot()
{
	m_snapShot->PopSnapShot();
}

void CDevice::PushSnapShot()
{
	m_snapShot->PushSnapShot();
}

void CDevice::SaveSnapShot(std::string name)
{
	m_snapShot->SaveSnapShot(name);
}

void CDevice::LoadSnapShot(std::string name)
{
	m_snapShot->LoadSnapShot(name);
}

