#include "SnapShot.h"


CSnapShot::CSnapShot(CContext* Context):
m_pContext(Context)
{
	m_BufferSize = TotalRam48 + CpuStateSize;
	m_pSnapShot = new ubyte [m_BufferSize];
}

CSnapShot::~CSnapShot()
{

}

bool CSnapShot::PushSnapShot()
{
	word PartStateSize = CpuStateSize;

	memcpy(m_pSnapShot, m_pContext->Memory + StartAddressRam48, TotalRam48);
	memcpy(m_pSnapShot + TotalRam48, m_pContext->CpuState, PartStateSize);
	
	m_bSnapShotIsReady = true;

	return true;
}

bool CSnapShot::PopSnapShot()
{
	if (!m_bSnapShotIsReady)
		return false;

	word PartStateSize = CpuStateSize;

	memcpy(m_pContext->Memory + StartAddressRam48, m_pSnapShot, TotalRam48);
	memcpy(m_pContext->CpuState, m_pSnapShot + TotalRam48, PartStateSize);

	return true;
}

bool CSnapShot::SaveSnapShot(std::string name /*= "None"*/)
{
	file::buffer b;
 	for (int i = 0; i < TotalRam48 + CpuStateSize; i++)
 		b.push_back(m_pSnapShot[i]);
	ox::file::write(name, b);

// 	std::ofstream File(name, std::ofstream::binary);
// 
// 	File.write((char *)m_pSnapShot, m_BufferSize);
// 	File.close();

	return true;
}

bool CSnapShot::LoadSnapShot(std::string name /*= "None"*/)
{
	if (name != "None")
	{
		
		for (int i = 0; i < m_BufferSize; i++)
			m_pSnapShot[i] = 0;

		file::buffer b;
		int fileSize = file::read(name, b);

		if (fileSize)
		{
			for (int i = 0; i < static_cast<int>(b.getSize()); i++)
				m_pSnapShot[i] = b[i];

			m_bSnapShotIsReady = true;
			return true;
		}
	}
	m_bSnapShotIsReady = false;
	return m_bSnapShotIsReady;
}

bool CSnapShot::ClearSnapShot() 
{ 
	m_bSnapShotIsReady = false; 
	return true; 
};