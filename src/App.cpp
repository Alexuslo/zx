#include "App.h"
#include "Resource.h"
#include "CustomTween.h"
#include "Func.h"
#include "Sound.h"
#include "Scene.h"
#include "Button.h"

CApp *CApp::m_pInstance = nullptr;

CApp::CApp() {}


void CApp::InitInternal()
{
	ObjectBase::__startTracingLeaks();
	core::init_desc desc;
	desc.title = "Project";
	desc.w = 1280;						//display width
	desc.h = 720;						//display height
	desc.mode24bpp = false;				//sets 24 bits per pixel, otherwise sets 16 bits per pixel?
	desc.vsync = false;					//vertical sync
	desc.fullscreen = false;			//fullscreen mode 
	desc.resizable = true;				//can the window be resized
	desc.borderless = false;			//borderless window
	desc.show_window = false;			//will the window be visible
	desc.force_gles = false;			//use OpenGLES driver. Could be used on Windows for emulation OpenGLES via Direct3D
	desc.allow_screensaver = false;		//allow screensaver
	desc.appName = "appName";			//Application name to be used as part of the file system directory for writable storage. If appName is empty files would be written next to working directory
	desc.companyName = "companyName";	//Company name to be used as part of the file system directory for writable storage

	SDL_GL_SetSwapInterval(1);

	core::init(&desc);

	Stage::instance = new Stage();
	getStage()->setSize(core::getDisplaySize());

	DebugActor::show();
	DebugActor::setCorner(0);

	flow::init();

	SoundSystem::create()->init(16);
	SoundPlayer::initialize();

	m_SoundPlayer.setResources(&m_ResSfx);
	m_SoundPlayer.setVolume(1.0f);

	m_ResGfx.loadXML("resources.xml");
	//m_resSfx.loadXML("sounds/res.xml");

#ifdef EMSCRIPTEN
	emscStartSoundsPreloading(resources);
#endif

	m_pDefaultFont = m_ResGfx.getResFont("main");
	m_StageSize = getStage()->getSize();
}

CApp::~CApp() 
{
	Destroy();	
	
	delete m_pInstance;
}

void CApp::Init()
{
	m_pInstance = new CApp;
	m_pInstance->InitInternal();

	m_pInstance->InitScenesAndDialogs();
}

void CApp::Destroy()
{
	ObjectBase::dumpCreatedObjects();
	flow::free();
	core::release();
	ObjectBase::dumpCreatedObjects();
	ObjectBase::__stopTracingLeaks();

	m_pInstance->m_ResGfx.free();
	m_pInstance->m_SoundPlayer.stop();
	m_pInstance->m_ResSfx.free();

	SoundPlayer::free();
	SoundSystem::free();
}

void CApp::Update()
{
	flow::update();
	getStage()->update();

	if (m_pInstance->m_pUpdatedScene)
		m_pInstance->m_pUpdatedScene->update();

	SoundSystem::get()->update();
	m_pInstance->m_SoundPlayer.update();
}

CApp *CApp::Get()
{
	if (!m_pInstance)
		CApp::Init();

	return m_pInstance;
}

CScene *CApp::getSceneById(int id)
{
	return GetSceneInternal(m_Scenes, id);
}

CScene *CApp::getSceneByName(const std::string Name)
{
	return GetSceneInternal(m_Scenes, Name.c_str());
}

CScene *CApp::getSceneByName(const char* Name)
{
	return GetSceneInternal(m_Scenes, Name);
}

CScene *CApp::getDialogById(int id)
{
	return GetSceneInternal(m_Dialogs, id);
}

CScene *CApp::getDialogByName(const std::string Name)
{
	return GetSceneInternal(m_Dialogs, Name.c_str());
}

CScene *CApp::getDialogByName(const char* Name)
{
	return GetSceneInternal(m_Dialogs, Name );
}

void CApp::PushSceneForUpdate()
{
	m_StoredScenes.push_back(m_pUpdatedScene);
	m_pUpdatedScene = nullptr;
}

void CApp::PopSceneForUpdate()
{
	if (!m_StoredScenes.empty())
	{
		m_pUpdatedScene = m_StoredScenes.back();
		m_StoredScenes.pop_back();
	}
}

void CApp::SwapSceneForUpdateWithTopStack()
{
	if (!m_StoredScenes.empty())
	{
		CScene *&pScene = m_StoredScenes.back();
		std::swap(pScene, m_pUpdatedScene);
	}
}

CScene *CApp::GetSceneInternal(std::vector<spCScene> &Collection, CApp::SceneData Data)
{
	bool bIdValid = Data.m_Id > 0;
	bool bNameValid = Data.m_Name != "";

	std::vector<spCScene>::iterator It = std::find_if(Collection.begin(), Collection.end(), [Data, bIdValid, bNameValid](auto pObject) {

		return	(bIdValid && pObject->getId() == Data.m_Id) ||
			(bNameValid && pObject->getName() == Data.m_Name);
	});

	if (It != Collection.end())
	{
		m_pLastScene = It->get();
		return m_pLastScene;
	}

	return nullptr;
}

void CApp::DeleteSceneById(int id)
{
	DeleteSceneInternal(m_Scenes, id);
}

void CApp::DeleteSceneByName(const std::string Name)
{
	DeleteSceneInternal(m_Scenes, Name.c_str());
}

void CApp::DeleteSceneByName(const char *Name)
{
	DeleteSceneInternal(m_Scenes, Name);
}

void CApp::DeleteDialogById(int id)
{
	DeleteSceneInternal(m_Dialogs, id);
}

void CApp::DeleteDialogByName(const std::string Name)
{
	DeleteSceneInternal(m_Dialogs, Name.c_str());
}

void CApp::DeleteDialogByName(const char *Name)
{
	DeleteSceneInternal(m_Dialogs, Name);
}

void CApp::DeleteSceneInternal(std::vector<spCScene> &Collection, CApp::SceneData Data)
{
	bool bIdValid = Data.m_Id > 0;
	bool bNameValid = Data.m_Name != "";

	auto It1 = std::find_if(Collection.begin(), Collection.end(), [Data, bIdValid, bNameValid](auto pObject) {

		return	(bIdValid && pObject->getId() == Data.m_Id) ||
			(bNameValid && pObject->getName() == Data.m_Name);
	});

	if (It1 != Collection.end())
	{
		if (m_pLastScene == It1->get())
			m_pLastScene = nullptr;

		if (!m_StoredScenes.empty())
		{
			auto It2 = std::find(m_StoredScenes.begin(), m_StoredScenes.end(), *It1);

			if (It2 != m_StoredScenes.end())
				m_StoredScenes.erase(It2);
		}

		Collection.erase(It1);
	}
}

// template <class T>
// std::vector<T>::iterator  CApp::FindScene(std::vector<T> &Collection, CApp::SceneData Data)
// {
// 	bool bIdValid = Data.m_Id > 0;
// 	bool bNameValid = Data.m_Name != "";
// 
// 	std::vector<T>::iterator It = std::find_if(Collection.begin(), Collection.end(), [Data, bIdValid, bNameValid](auto pObject) {
// 
// 		return	(bIdValid && pObject->getId() == Data.m_Id) ||
// 			(bNameValid && pObject->getName() == Data.m_Name);
// 	});
// 
// 	return It;
// }

std::string CApp::GetAutoName()
{
	return std::string("object_" + std::to_string(m_IdSuffix++));
}

spCScene CApp::newScene(const char *sceneName, const char *textureName/* = "rect"*/, Vector2 size/* = { -1, -1 }*/)
{
	spCScene s = new CScene(this);

	if (size == Vector2(-1, -1))
		size = Vector2(getStage()->getWidth(), getStage()->getHeight());

	s->getHolder()->setAnchor(0.5f, 0.5f);
	s->getHolder()->setSize(size);
	s->getHolder()->setPosition(getStage()->getSize() / 2);

	if (sceneName != "")
		s->setName(sceneName);
	else
		s->setName(GetAutoName());

	if (textureName == "rect")
		s->addBackSprite(new ColorRectSprite, Color(25, 25, 25, 200));
	else if (textureName == "box sprite")
		s->addBackSprite(new Box9Sprite, Color(25, 25, 25, 200));
	else
		s->addBackSprite(GetTexture(textureName), { -1.0f, -1.0f }, { -1.0f, -1.0f }, true);

	s->getHolder()->setSize(getStage()->getSize());
	//flow::TransitionFade::assign(s);
	//flow::TransitionShutters::assign(s);

	AddScene(s);

	return s;
}

spCScene CApp::newScene(std::function<void(spCScene)> sceneCreator /*= nullptr*/)
{
	spCScene s;
	if (sceneCreator)
	{
		sceneCreator(s);
		AddScene(s);
		return s;
	}

	return newScene("unnamed", "rect", { -1, -1 });
}

void CApp::AddScene(spCScene pScene)
{
	AddSceneInternal(m_Scenes, pScene);
}

void CApp::AddSceneInternal(std::vector<spCScene> &Collection, spCScene pScene)
{
	Collection.push_back(pScene);
}


spCScene CApp::newDialog(CScene *pParentScene, const char *dialogName, ETransitionType transitionType/* = ETransitionType::Fade*/,const char *textureName/* = "rect"*/, Vector2 size/* = { -1, -1 }*/)
{
	spCScene s = new CScene(this, pParentScene);
	
	spSprite sprite;

	if (size == Vector2(-1, -1))
		size = Vector2(getStage()->getWidth() / 2, getStage()->getHeight() / 2);

	s->setAsDialog();
	s->getHolder()->setAnchor(0.5f, 0.5f);
	s->getHolder()->setSize(size);
	s->getSelf()->setSize(size);
	s->getHolder()->setPosition(getStage()->getSize() / 2);

	if (dialogName != "")
		s->setName(dialogName);
	else
		s->setName(GetAutoName());

	if (textureName == "rect")
		s->addBackSprite(new ColorRectSprite, Color(25, 25, 25, 200));
	else if (textureName == "box sprite")
		s->addBackSprite(new Box9Sprite, Color(25, 25, 25, 200));
	else
		s->addBackSprite(GetTexture(textureName), { -1.0f, -1.0f }, { -1.0f, -1.0f }, true, 150);
	
	switch (transitionType)
	{
	case ETransitionType::Move:
		{
			TransitionMove *t = nullptr;
			flow::TransitionMove::assign(s);

			float x = getStage()->getWidth() / 2;
			float y = getStage()->getHeight();
			static_cast<TransitionMove *>(s->getTransitionIn().get())->setSrcPositionIn(Vector2(x, -y));
			static_cast<TransitionMove *>(s->getTransitionOut().get())->setSrcPositionOut(Vector2(x, y * 2));
			static_cast<TransitionMove *>(s->getTransitionOut().get())->setDestPosition(getStage()->getSize() / 2);
			s->getHolder()->setAnchor(0.5f, 0.5f);
			s->getHolder()->setPosition(x, y);
			s->setTransitionType(ETransitionType::Move);
		}
		break;

	case ETransitionType::Fade:
		flow::TransitionFade::assign(s);
		s->setTransitionType(ETransitionType::Fade);
		break;

	case ETransitionType::Shutters:
		flow::TransitionShutters::assign(s);
		s->setTransitionType(ETransitionType::Shutters);
		break;

	case ETransitionType::Quads:
		flow::TransitionQuads::assign(s);
		s->setTransitionType(ETransitionType::Quads);
		break;

	case ETransitionType::FadeFromPoint:
		flow::TransitionFadeFromPoint::assign(s);
		s->getHolder()->setAnchor(0.5f, 0.5f);
		s->getHolder()->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 2);
		s->setTransitionType(ETransitionType::FadeFromPoint);
		break;
	}
		
	AddDialog(s);

	return s;
}

spCScene CApp::newDialog(CScene *pParentScene, std::function<void(spCScene)> dialogCreator /*= nullptr*/)
{
	spCScene s;
	
	if (dialogCreator)
	{
		dialogCreator(s);
		AddDialog(s);
		return s;
	}

	return newDialog(pParentScene, "unnamed", ETransitionType::Fade, "rect", { -1, -1 });
}

void CApp::AddDialog(spCScene pScene)
{
	AddSceneInternal(m_Dialogs, pScene);
}

template <class T> spCButton CApp::NewButton(spCScene Scene, std::string Name, Vector2 position /*= {0.0f, 0.0f}*/, Vector2 size/* = { 150.0f, 44.0f }*/, std::string Caption /*= "no press!"*/, std::string TextureName /*= "rect"*/, bool bIsStd/* = true*/)
{
	ResAnim *Texture = nullptr;
	if (TextureName != "rect")
	{
		Texture = m_ResGfx.getResAnim(TextureName);
	}
	else
	{
		spSprite s = new ColorRectSprite;
		s->setSize(size);
		s->setColor(Color(Color::YellowGreen));
		Texture = const_cast<ResAnim *>(s->getResAnim());
	}

	spCButton b = new T(this, (Scene).get(), Name, Caption, std::string("Default"), Texture);

	b->setSize(size);
	b->setAnchor(0.5f, 0.5f);
	b->setText(Caption);

	if (bIsStd)
	{
		b->AddOverIn();
		b->AddOverOut();
		b->AddClick();
	}

	if (position == Vector2(0.0f, 0.0f))
	{
		if (Scene)
		{
			position.x = Scene->getSelfSize().x / 2.0f;
			position.y = Scene->getSelfSize().y - b->getHeight();
		}
	}

	b->setPosition(position);
	b->UpdateShadow();

	if (Scene)
		Scene->addButton(b);
		
	else
		m_Buttons.push_back(b);

	return b;
}

template <class T> spCButton CApp::NewButton(spCScene scene, std::string Name, std::function<void(spCButton)> buttonCreator /*= nullptr*/)
{
	spCButton b;
	if (buttonCreator)
	{
		buttonCreator(b);
		if (scene)
			scene->addButton(b);

		return b;
	}

	return  newButton<T>(scene, Name, { 0.0f, 0.0f }, { 150.0f, 44.0f }, "name", "rect", true);
}

oxygine::ResAnim* CApp::GetTexture(const char* textureName)
{
	return m_ResGfx.getResAnim(textureName);
}

oxygine::ResAnim* CApp::GetTexture(const std::string &textureName)
{
	return GetTexture(textureName.c_str());
}

oxygine::ResFont *CApp::GetFont(const char *textureName)
{
	return m_ResGfx.getResFont(textureName);
}

oxygine::ResFont *CApp::GetFont(const std::string &textureName)
{
	return GetFont(textureName.c_str());
}

void CApp::Render() 
{
	
}

void CApp::InitScenesAndDialogs()
{
	InitScenesAndDialogs("none");
}

void CApp::InitScenesAndDialogs(const std::string& nameXmlFile /* = "none"*/)
{
	InitScenesAndDialogs(nameXmlFile.c_str());
}

void CApp::InitScenesAndDialogs(const char *nameXmlFile /* = "none"*/)
{
	XmlDocument m_XmlFile;
	XmlParseResult m_Result;

	if (nameXmlFile != "none")
	{
		m_Result = m_XmlFile.load_file(nameXmlFile);

		if (m_Result.status != status_ok)
			return;

		//TO DO
	}

	m_UseShadow = false;

	int idCounter = 1;
	int fontSize = 32;

	spCScene sc;
	spSprite spr;
	spCButton b;
	Vector2 sizeScene;
	Vector2 sizeButton = Vector2(getStage()->getWidth() / 6.0f, getStage()->getHeight() / 14.0f);
	
	float incValue = sizeButton.y * 1.5f;

	oxygine::ResAnim *t = nullptr;

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init root scene
	{
		spr = new ColorRectSprite();
		spr->setSize(getStage()->getSize());
		spr->setColor(Color::Black);
		sc = new CScene(this);
		sc->setName("root scene");
		sc->addBackSprite(spr, { -1.0f, -1.0f }, { -1.0f, -1.0f }, true);

		AddScene(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init splash scene
	{
		sc = new CScene(this);
		sc->setName("splash scene");
		spr = new Sprite();
		spr->setResAnim(GetTexture("logo"));
		sc->addBackSprite(spr, { -1.0f, -1.0f }, { 1.0f, 1.0f }, true);
		spr->setAlpha(0);
		spr->addTween(Sprite::TweenAlpha(255), 1000, 1, false, 1000);
		spr->addTween(Sprite::TweenAddColor(Color(128, 128, 128, 128)), 100, 2, true, 2000);
		spr->addTween(Sprite::TweenAlpha(0), 1000, 1, false, 3000)->addDoneCallback([pApp = this](Event *e) {

			pApp->getSceneByName("splash scene")->finish();
			flow::show(pApp->getSceneByName("main scene"));
			pApp->PushSceneForUpdate();
			//pApp->SetLastSceneForUpdate();
			flow::show(pApp->getDialogByName("main menu"));
			});
		sc->setId(idCounter++);
		flow::TransitionFade::assign(sc);

		AddScene(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init main scene
	{	
		Vector2 sizeButton(getStage()->getHeight() / 9.0f, getStage()->getHeight() / 9.0f);

		sc = new CScene(this);
		sc->setName("main scene");

		spr = new Sprite;
		spr->setResAnim(GetTexture("back"));

		UberShaderProgram *shader = new UberShaderProgram();
		shader->init(STDRenderer::uberShaderBody, 
			R"(
				#define MODIFY_BASE_PRE
				uniform lowp vec4 Value;

				lowp vec4 modify_base_pre(lowp vec4 base)
				{
					lowp float c = (base.r + base.g + base.b) / 3.0;
					return mix(vec4(c, c, c, base.a), base, 1.0 - Value.r);
				}
			)");

// 		shader->init(STDRenderer::uberShaderBody, R"(
//                                                      #define MODIFY_BASE_PRE
//                             uniform lowp vec4 userValue;
//                             lowp vec4 modify_base_pre(lowp vec4 base)
//                             {                            
//                                 lowp vec4 inverted = vec4(1.0, 1.0, 1.0, 1.0) - base;
//                                 lowp vec4 res = mix(inverted * base.a, base, userValue.r);
//                                 return vec4(res.rgb, base.a);
//                             }
//         )");


		CustomUniformMat mat;

		spr->_mat->copyTo(mat);
		mat._uberShader = shader;
		spr->_mat = mc().cache(mat);

		
		sc->addBackSprite(spr, { -1.0f, -1.0f }, { -1.0f, -1.0f }, true);

		CCursorKeys *ck = new CCursorKeys(this, sc.get(), "cursor keys");
		CJoystick *j = new CJoystick(this, sc.get());

		Vector2 pos = sc->getHolder()->getSize();
		b = NewButton<CButton>(sc, GetAutoName(), Vector2(pos.x - sizeButton.x, pos.y - sizeButton.y * 3), sizeButton, "continue\nstart", "button2");
		b->AddVisualFunc();
		b->addClickListener([scene = sc](Event *e) {

			(static_cast<CDevice *>(scene->GetBase()))->SetNeedPressEnterOrNull();
			});

		b = NewButton<CButton>(sc, GetAutoName(), sc->getHolder()->getSize() - sizeButton, sizeButton, "FIRE", "button2");
		b->AddVisualFunc();


		sc->SetBase(new CDevice(sc, ck, j, b));
		sc->GetBase()->Init();

		sc->setUserData(sc->GetBase());
		
		CDevice *device = static_cast<CDevice *>(const_cast<void *>(sc->getUserData()));

		b = NewButton<CSettingsButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - sizeButton.x / 1.5f, sizeButton.y / 1.5f }, sizeButton, "", "settings");
		
		CSettingsButton *sb = static_cast<CSettingsButton *>((b).get());
		b->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			flow::show(pApp->getDialogByName("options"));
			pApp->ResetUpdatedScene();
			});
		
		ck->attachTo(sc->GetControlsActor());
		j->attachTo(sc->GetControlsActor());

		AddScene(sc);
	}

	CScene *pParentScene = (sc).get();
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init options dialog
	{
		sizeScene = Vector2(getStage()->getHeight() * 0.9f, getStage()->getWidth() * 0.5f);
		sc = newDialog(pParentScene, "options", ETransitionType::FadeFromPoint, "rect", sizeScene);

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - 20, 20 }, { 30, 30 }, "X", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		Vector2 position = Vector2(sc->getHolder()->getWidth() / 2, sc->getHolder()->getHeight() - sizeButton.y);
		
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "exit", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			flow::show(pApp->getDialogByName("exit"));
			});

		position.y = sizeButton.y;
		sc->addTextField("Options", position, static_cast<int>(sizeButton.y));

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "new game", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			CDevice *device = static_cast<CDevice *>(pApp->getSceneByName("main scene")->GetBase());
			device->LoadSnapShot("snap.esf");
			device->PopSnapShot();
			device->Run();
			Dialog->finish();
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "load state", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			flow::show(pApp->getDialogByName("load"));
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "save state", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			flow::show(pApp->getDialogByName("save"));
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "sound", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "advanced", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			//auto child = static_cast<CDevice *>(pApp->getSceneByName("main scene")->GetBase());
			//child->SetFullAcceleration(!child->IsFullAcceleration());

			auto spr = pApp->getSceneByName("main scene")->getBackSpriteByIndex(0);
			
			static bool pass = false;

			if (!pass)
			{
				/*spr->_mat->_uberShader->release();*/

				UberShaderProgram *shader = new UberShaderProgram();
				shader->init(STDRenderer::uberShaderBody, R"(
                          #define MODIFY_BASE_PRE
                          uniform lowp vec4 Value;
                          lowp vec4 modify_base_pre(lowp vec4 base)
                          {
                              return base;
                          }
				)");

				CustomUniformMat mat;

				spr->_mat->copyTo(mat);
				mat._uberShader = shader;
				spr->_mat = mc().cache(mat);

				pass = true;
			}
			else
			{
				UberShaderProgram *shader = new UberShaderProgram();
				shader->init(STDRenderer::uberShaderBody, R"(
                          #define MODIFY_BASE_PRE
                          uniform lowp vec4 Value;
                          lowp vec4 modify_base_pre(lowp vec4 base)
                          {
                              lowp float c = (base.r + base.g + base.b) / 3.0;
                              return mix(vec4(c, c, c, base.a), base, Value.r);
                          }
				)");

				CustomUniformMat mat;

				spr->_mat->copyTo(mat);
				mat._uberShader = shader;
				spr->_mat = mc().cache(mat);

				pass = false;
			}

			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "return to game", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			pApp->SetLastSceneForUpdate();
			});

		AddScene(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init main menu dialog
	{
		sizeScene = Vector2(getStage()->getHeight() * 0.7f, getStage()->getWidth() * 0.4f);
		sc = newDialog(pParentScene, "main menu", ETransitionType::FadeFromPoint, "rect", sizeScene);

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - 20, 20 }, { 30, 30 }, "X", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			CDevice *device = static_cast<CDevice *>(pApp->getSceneByName("main scene")->GetBase());
			device->Run();
			Dialog->finish();
			});

		Vector2 position = Vector2(sc->getHolder()->getWidth() / 2, sizeButton.y);

		sc->addTextField("Select for start", position, static_cast<int>(sizeButton.y));
		CDevice *pDevice = static_cast<CDevice *>(getSceneByName("main scene")->GetBase());
			   
		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "continue game", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([device = pDevice, self = b, Dialog = sc](Event *e) {

			device->LoadSnapShot("f/cont.esf");
			device->PopSnapShot();
			device->Run();
			Dialog->finish();
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "new game", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([device = pDevice, self = b, Dialog = sc](Event *e) {

			device->LoadSnapShot("f/snap.esf");
			device->PopSnapShot();
			device->Run();
			Dialog->finish();
			});
		
		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "load game", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([device = pDevice, pApp = this, self = b, Dialog = sc](Event *e) {
			
			device->Run();
			flow::show(pApp->getDialogByName("load"));
			});

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "exit", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([device = pDevice, pApp = this, self = b, Dialog = sc](Event *e) {

			device->Run();
			flow::show(pApp->getDialogByName("exit"));
			});

		AddDialog(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init save dialog
	{
		sizeScene = Vector2(getStage()->getHeight() * 0.9f, getStage()->getWidth() * 0.5f);
		sc = newDialog(pParentScene, "save", ETransitionType::FadeFromPoint, "rect", sizeScene);

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - 20, 20 }, { 30, 30 }, "X", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([self = b, Dialog = sc](Event *e) {

			Dialog->finish();
		});

		Vector2 position = Vector2(sc->getHolder()->getWidth() / 2, sc->getHolder()->getHeight() - sizeButton.y);

		position.y = sizeButton.y;
		sc->addTextField("Save state", position, static_cast<int>(sizeButton.y));

		std::string name = "save slot ";

		for (int i = 1; i <= 5; i++)
		{
			std::string fullName = name + std::to_string(i);
			position.y += incValue;
			b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, fullName.c_str(), "rect");
			b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([num = i, pApp = this, self = b, parentScene = pParentScene, dialog = sc](Event *e) {

				file::makeDirectory("f");
				std::string name = "f/" + std::to_string(num) + ".esf";
				auto device = static_cast<CDevice *>(parentScene->GetBase());
				device->PushSnapShot();
				device->SaveSnapShot(name);
				dialog->finish();
				pApp->getDialogByName("options")->finish();
			});
		}

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "return", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		AddScene(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init load dialog
	{
		sizeScene = Vector2(getStage()->getHeight() * 0.9f, getStage()->getWidth() * 0.5f);
		sc = newDialog(pParentScene, "load", ETransitionType::FadeFromPoint, "rect", sizeScene);

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - 20, 20 }, { 30, 30 }, "X", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		Vector2 position = Vector2(sc->getHolder()->getWidth() / 2, sc->getHolder()->getHeight() - sizeButton.y);

		position.y = sizeButton.y;
		sc->addTextField("Load state", position, static_cast<int>(sizeButton.y));

		std::string name = "Load slot ";

		for (int i = 1; i <= 5; i++)
		{
			std::string fullName = name + std::to_string(i);
			position.y += incValue;
			b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, fullName.c_str(), "rect");
			b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([num = i, pApp = this, self = b, parentScene = pParentScene, dialog = sc](Event *e) {

				std::string name = "f/" +  std::to_string(num) + ".esf";
				auto device = static_cast<CDevice *>(parentScene->GetBase());
				device->LoadSnapShot(name);
				device->PopSnapShot();
				dialog->finish();
				pApp->getDialogByName("options")->finish();
				pApp->getDialogByName("main menu")->finish();
				});
		}

		position.y += incValue;
		b = NewButton<CButton>(sc, GetAutoName(), position, sizeButton, "return", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		AddScene(sc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//init exit dialog
	{
		sc = newDialog(pParentScene, "exit", ETransitionType::Move, "rect", getStage()->getSize() / 3);

		sc->addTextField("Are you leaving us?", { 0.0f, static_cast<float>(fontSize) }, fontSize);

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() - 20, 20 }, { 30, 30 }, "X", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() / 2, sc->getHolder()->getHeight() - 44 }, { 150, 44 }, "cancel", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			Dialog->finish();
			});

		b = NewButton<CButton>(sc, GetAutoName(), { sc->getHolder()->getWidth() / 2, sc->getHolder()->getHeight() - 128 }, { 150, 44 }, "exit", "rect");
		b->AddOverIn()->AddOverOut()->SetClickType(EClickType::AddSpring)->addClickListener([pApp = this, self = b, Dialog = sc](Event *e) {

			CDevice *device = static_cast<CDevice *>(pApp->getSceneByName("main scene")->GetBase());
			device->PushSnapShot();
			device->SaveSnapShot("f/cont.esf");

			pApp->getDialogByName("options")->finish();
			pApp->getDialogByName("main menu")->finish();
			pApp->getSceneByName("main scene")->finish();
			pApp->getSceneByName("root scene")->finish();
			Dialog->finish();
			});

		AddScene(sc);

	}

	//run application
	flow::show(getSceneByName("root scene"));
	flow::show(getSceneByName("splash scene"));
}