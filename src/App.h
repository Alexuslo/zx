#pragma once
#include "common.h"
#include "sound/oxygine-sound.h"


class CApp : public ref_counter
{
private:

	static CApp *m_pInstance;

	bool m_UseShadow = false;

	int m_IdSuffix = 0;
	
	Resources m_resGfx;
	Resources m_resSfx;
	SoundPlayer m_splayer;

	Vector2 m_stageSize;

	Scenes m_scenes;
	Scenes m_dialogs;
	CScene *m_pUpdatedScene = nullptr;
	CScene *m_pLastScene = nullptr;
	CScene *m_pStoredScene = nullptr;

	ResFont *m_pDefaultFont = nullptr;
	
	std::vector<oxygine::spButton> m_Buttons;
	spActor m_pShadows;
	spActor m_pControls;

	CApp();

	void InitInternal();

public:

	~CApp();

	static void Init();
	static void Free();
	static void Update();
	static CApp *Get();

	spCScene getSceneById(int id, bool bIsScene = true);
	spCScene getSceneByName(const std::string &name, bool bIsScene = true);
	spCScene getSceneByName(const char *name, bool bIsScene = true);
	
	spCScene getDialogById(int id);
	spCScene getDialogByName(const std::string& name);
	spCScene getDialogByName(const char* name);

	void SetLastSceneForUpdate() { m_pUpdatedScene = m_pLastScene; }
	void ResetUpdatedScene() { m_pUpdatedScene = nullptr; }
	void SaveSceneUpdate() { m_pStoredScene = m_pLastScene; }
	void RestoreSceneUpdate() { m_pUpdatedScene = m_pStoredScene; }

	const char *getAutoName();
	
	void deleteSceneById(int id);
	void deleteSceneByName(const std::string &name);
	void deleteSceneByName(const char *name);

	spCScene newScene(const char *sceneName, const char *textureName = "rect", Vector2 size = { -1, -1 });
	spCScene newScene(std::function<void(spCScene)> sceneCreator = nullptr);
	void addScene(spCScene scene, bool bIsScene = true);
	
	spCScene newDialog(CScene *pParentScene, const char *dialogName, ETransitionType transitionType = ETransitionType::Fade, const char *textureName = "rect", Vector2 size = { -1, -1 });
	spCScene newDialog(CScene *pParentScene, std::function<void(spCScene)> dialogCreator = nullptr);
	void addDialog(spCScene scene);

	template <class T> spCButton newButton(spCScene scene, const char *Id, Vector2 position = { 0.0f, 0.0f }, Vector2 size = { 150.0f, 44.0f }, const char *Caption = "name", const char *textureName = "rect", bool bIsStd = true);
	template <class T> spCButton newButton(spCScene scene, const char *Id, std::function<void(spCButton)> buttonCreator = nullptr);

	ResAnim *getTexture(const std::string &textureName);
	ResAnim *getTexture(const char *textureName);
	
	ResFont *getFont(const std::string &textureName);
	ResFont *getFont(const char *textureName);
	ResFont *getDefaultFont() { return m_pDefaultFont; }

	void update();
	void render();

	void initScenesAndDialogs();
	void initScenesAndDialogs(const std::string &nameXmlFile);
	void initScenesAndDialogs(const char *nameXmlFile);

	oxygine::spActor GetShadowsActor() const { return m_pShadows; }
	oxygine::spActor GetControlsActor() const { return m_pControls; }

	bool IsUseShadow() const { return m_UseShadow; }
	void SetUseShadow(bool val) { m_UseShadow = val; }
};

class CustomUniformMat : public STDMaterial
{
public:
	MATX(CustomUniformMat);

	Vector4 uniform = Vector4(0.0f, 0.0f, 0.0f, 0.0f);


	void rehash(size_t &hash) const override
	{
		STDMaterial::rehash(hash);
		hash_combine(hash, uniform.x, uniform.y, uniform.z, uniform.w);
	}

	static bool cmp(const CustomUniformMat &a, const CustomUniformMat &b)
	{
		if (!STDMaterial::cmp(a, b))
			return false;

		return a.uniform == b.uniform;
	}

	void xapply() override
	{
		STDMaterial::xapply();
		IVideoDriver::instance->setUniform("Value", uniform);
	}
};