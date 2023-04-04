#pragma once
#include "common.h"
#include "sound/oxygine-sound.h"

using namespace oxygine;
using namespace oxygine::flow;
using namespace pugi;

class CApp;
class CBase;

class CScene : public oxygine::flow::Scene
{
private:
	bool m_bIsDialogSwitchable = false;
	
	int m_sceneId = 0;
	int m_indexBackSprite = -1;

	spActor m_self;
	spActor m_Shadows;
	spActor m_Controls;

	spClipRectActor m_clipArea;

	Vector2 m_stageSize;

	CScene *m_pParentScene = nullptr;
	CScene *m_pPrevScene = nullptr;
	CScene *m_pNextScene = nullptr;

	Sprite *m_currentBackSprite = nullptr;
	Sprite *m_currrentSprite = nullptr;
	Button *m_currentButton = nullptr;
	TextField *m_currentTextField = nullptr;

	std::vector<oxygine::spSprite> m_backSprites;
	std::vector<oxygine::spSprite> m_sprites;
	std::vector<oxygine::spButton> m_buttons;
	std::vector<oxygine::spTextField> m_textFields;

	std::vector<oxygine::spActor> m_Updateables;

	oxygine::ResFontBM *m_pSdFont = nullptr;
	oxygine::spActor m_view;

	ETransitionType m_TransitionType = ETransitionType::None;

	CApp *m_pApp = nullptr;
	CBase *m_pBase = nullptr;

	std::function<void()> m_autoFunc = nullptr;

public:
	CScene(CApp *pApp, CScene *pParentScene = nullptr);
	~CScene();

	ETransitionType getTransitionType() const { return m_TransitionType; }
	void setTransitionType(ETransitionType val) { m_TransitionType = val; }
	oxygine::spActor getSelf() const { return m_self; }
	Vector2 getSelfOffset() const { return m_self->getPosition(); }
	Vector2 getSelfSize() const { return m_self->getSize(); }
	void setSelfSize(Vector2 size) { m_self->setSize(size); }

	oxygine::spActor GetShadowsActor() const { return m_Shadows; }
	oxygine::spActor GetControlsActor() const { return m_Controls; }
	
	CBase *GetBase() const { return m_pBase; }
	void SetBase(CBase *val) { m_pBase = val; }
	
	void setId(int id) { m_sceneId = id; };
	int getId() const { return m_sceneId; };

	void addAutoFunc(std::function<void()> func) { m_autoFunc = std::move(func); }
	void delAutoFunc() { m_autoFunc = nullptr; }

	CScene *getNext() { return m_pNextScene; }
	void setNext(CScene *pScene) { m_pNextScene = pScene; }
	CScene *getPrev() { return m_pPrevScene; }
	void setPrev(CScene *pScene) { m_pPrevScene = pScene; }

	void gotoNext() { show(m_pNextScene); }
	void gotoPrev() { show(m_pPrevScene); }

	void show();
	void show(CScene *scene);
	void close() { this->finish(); }

	spSprite addBackSprite(spSprite sprite, Color color /*= { 255, 255, 255, 255 }*/);
	spSprite addBackSprite(ResAnim *texture, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, bool bIsSaveRatio = true, unsigned char alpha = 255);
	spSprite addBackSprite(spSprite sprite, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, bool bIsSaveRatio = true, unsigned char alpha = 255);
	spSprite addBackSprite(Sprite *sprite, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, bool bIsSaveRatio = true, unsigned char alpha = 255);

	spSprite getBackSpriteByIndex(int index);
	void changeBackSpriteByIndex(int index);
	void nextBackSprite();
	void prevBackSprite();
	void firstBackSprite();
	void lastBackSprite();

	spSprite addSprite(ResAnim *texture, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, unsigned char alpha = 255, bool bIsScale = true, bool bIsSaveRatio = true);
	spSprite addSprite(spSprite s, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, unsigned char alpha = 255, bool bIsScale = true, bool bIsSaveRatio = true);
	spSprite addSprite(Sprite *s, Vector2 position = { 0.0f, 0.0f }, Vector2 scale = { 1.0f, 1.0f }, unsigned char alpha = 255, bool bIsScale = true, bool bIsSaveRatio = true);
	
	spSprite getSpriteByIndex(int index);

	spButton &addButton(spButton button);
	spButton getButtonByName(const std::string &name);
	spButton getButtonByName(const char *name);

	void addTextField(const char *text, Vector2  pos = { 0.0f, 0.0f }, int size = 14);
	void addTextField(spTextField textField);
	spTextField getTextFieldByName(const std::string &name);
	spTextField getTextFieldByName(const char *name);

	void setAsDialog() { _dialog = true; }
	void setOffset(Vector2 offset) { getSelf()->setPosition(offset); }
	void setOffset(float x = 0.0f, float y = 0.0f) { setOffset(Vector2(x, y)); }
	void setOffset(int x = 0, int y = 0) { setOffset(Vector2(static_cast<float>(x), static_cast<float>(y))); }

	spClipRectActor getClipRect() const { return m_clipArea; }
	void addToClipRect(spActor actor) { m_clipArea->addChild(actor); }

	CScene *GetParentScene() { return m_pParentScene; }
	void SetpParentScene(CScene *pParentScene) { m_pParentScene = pParentScene; }

	void update();

	void AddToUpdateByScene(Actor *obj);
};

