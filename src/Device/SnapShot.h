#include "../common.h"
#include <array>
#include "../Cpu/Cpu.h"


class CSnapShot
{
private:
	ubyte *m_pSnapShot = nullptr;
	CContext* m_pContext = nullptr;

	bool m_bSnapShotIsReady = false;

	int m_BufferSize = 0;

public:
	CSnapShot(CContext* Context);
	~CSnapShot();

	bool PushSnapShot();
	bool PopSnapShot();
	bool ClearSnapShot();

	bool SaveSnapShot(std::string Name = "None");
	bool LoadSnapShot(std::string Name = "None");
};