#pragma once

#include "common.h"
#include "sound/oxygine-sound.h"

using namespace oxygine;

class CSoundBank
{
	bool bIsInit = false; 

	Resources resources;
	SoundPlayer splayer;

	enum class EError { None, LoadOrParsingError, RootError };

	class Wave
	{
		SDL_AudioSpec spec;
		Uint8 *sound = nullptr;         /* Pointer to wave data */
		Uint32 soundlen = 0;			/* Length of wave data */
		int soundpos = 0;               /* Current play position */

		std::string name;

		bool bIsLoop = false;
		bool bIsPlaying = false;
	};

	std::vector<Wave *> soundBank;

	EError error = EError::None;
	XmlDocument xmlFile;
	XmlParseResult result;
	std::vector<std::string> soundsName;
	
public:

	CSoundBank()
	{
		if (bIsInit)
			return;

		SoundSystem::create()->init(16);

		//initialize SoundPlayer
		SoundPlayer::initialize();
		splayer.setResources(&resources);
		splayer.setVolume(0.5f);

		//load xml file with resources definition
		resources.loadXML("sounds/res.xml");

#ifdef EMSCRIPTEN
		emscStartSoundsPreloading(resources);
#endif
	}
		
	~CSoundBank()
	{
		splayer.stop();
		resources.free();

		SoundPlayer::free();
		SoundSystem::free();
	}

	void update()
	{
		SoundSystem::get()->update();
		splayer.update();
	}


	int init(std::string xmlFileName, std::string rootName = "sound")
	{
		result = xmlFile.load_file(xmlFileName.c_str());

		if (result.status != status_ok)
			error = EError::LoadOrParsingError;

		XmlNode root =xmlFile.first_child();
		
		if (!root)
		{
			error = EError::RootError;
			return -1;
		}
		else
		{
			std::string name = root.name();

			while (name != rootName && root.next_sibling())
			{
				root = root.next_sibling();
				name = root.name();
			}

			if (name != rootName)
				error = EError::RootError;
		}

		if (error != EError::None)
		{
			return -1;
		}
		else
		{
			for (XmlAttribute &i : root.attributes())
				if (i.name() == "sound")
					soundsName.push_back(i.value());
		}
	}


};

