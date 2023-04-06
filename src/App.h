#pragma once
#include "common.h"
#include "sound/oxygine-sound.h"


class CApp : public ref_counter
{
protected:

	struct SceneData
	{
		int m_Id = -1;
		const char* m_Name = "";

		SceneData(int Id) : m_Id(Id) { }
		SceneData(const char *Name) : m_Name(Name) { }
	};


	static CApp *m_pInstance;

	bool m_UseShadow = false;

	int m_IdSuffix = 0;
	
	Resources m_ResGfx;
	Resources m_ResSfx;
	SoundPlayer m_SoundPlayer;

	Vector2 m_StageSize;

	TScenes m_Scenes;
	TDialogs m_Dialogs;

	CScene *m_pUpdatedScene = nullptr;
	CScene *m_pLastScene = nullptr;

	std::vector<CScene *> m_StoredScenes;
	ResFont *m_pDefaultFont = nullptr;
	
	std::vector<oxygine::spButton> m_Buttons;
	spActor m_pShadows;
	spActor m_pControls;

	CApp();

	void InitInternal();

	CScene *GetSceneInternal(std::vector<spCScene> &Collection, CApp::SceneData Data);
	void DeleteSceneInternal(std::vector<spCScene> &Collection, CApp::SceneData Data);

	void AddSceneInternal(std::vector<spCScene> &Collection, spCScene pScene);

public:

	~CApp();

	static void Init();
	static void Destroy();
	static void Update();
	static CApp *Get();

	CScene *getSceneById(int id);
	CScene *getSceneByName(const std::string name);
	CScene *getSceneByName(const char *name);

	CScene *getDialogById(int id);
	CScene *getDialogByName(const std::string name);
	CScene *getDialogByName(const char* name);

	void SetLastSceneForUpdate() { m_pUpdatedScene = m_pLastScene; }
	void ResetUpdatedScene() { m_pUpdatedScene = nullptr; }
	void PushSceneForUpdate();
	void PopSceneForUpdate();
	void SwapSceneForUpdateWithTopStack();

	std::string GetAutoName();
	
	void DeleteSceneById(int id);
	void DeleteSceneByName(const std::string name);
	void DeleteSceneByName(const char *name);

	void DeleteDialogById(int id);
	void DeleteDialogByName(const std::string name);
	void DeleteDialogByName(const char *name);

	spCScene newScene(const char *sceneName, const char *textureName = "rect", Vector2 size = { -1, -1 });
	spCScene newScene(std::function<void(spCScene)> sceneCreator = nullptr);
	void AddScene(spCScene pScene);
	
	spCScene newDialog(CScene *pParentScene, const char *dialogName, ETransitionType transitionType = ETransitionType::Fade, const char *textureName = "rect", Vector2 size = { -1, -1 });
	spCScene newDialog(CScene *pParentScene, std::function<void(spCScene)> dialogCreator = nullptr);
	void AddDialog(spCScene pScene);

	template <class T> spCButton NewButton(spCScene scene, std::string Id, Vector2 position = { 0.0f, 0.0f }, Vector2 size = { 150.0f, 44.0f }, std::string Caption = "name", std::string = textureName = "rect", bool bIsStd = true);
	template <class T> spCButton NewButton(spCScene scene, std::string Id, std::function<void(spCButton)> buttonCreator = nullptr);

	ResAnim *GetTexture(const std::string &TextureName);
	ResAnim *GetTexture(const char *TextureName);
	
	ResFont *GetFont(const std::string &FontName);
	ResFont *GetFont(const char *FontName);
	ResFont *GetDefaultFont() { return m_pDefaultFont; }

	void Render();

	void InitScenesAndDialogs();
	void InitScenesAndDialogs(const std::string &nameXmlFile);
	void InitScenesAndDialogs(const char *nameXmlFile);

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