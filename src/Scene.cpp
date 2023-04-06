#include "Scene.h"
#include "App.h"
#include "Func.h"

CScene::CScene(CApp *pApp, CScene *pParentScene/* = nullptr*/) :
	m_pApp(pApp),
	m_pParentScene(m_pParentScene)
{
	m_pApp = pApp;
	m_stageSize = getStage()->getSize();
	_dialog = true;/////////////////////////////////////////////////////////////////////////////////
	m_pSdFont = nullptr;
	m_self = new Actor;
	m_self->setSize(getHolder()->getSize());
	m_self->attachTo(getHolder());
	m_clipArea = new ClipRectActor;
	m_clipArea->setAnchor(0.5f, 0.5f);
	m_clipArea->attachTo(m_self);
	m_clipArea->setSize(m_self->getSize());
	m_clipArea->setPosition(m_self->getSize() / 2);

	m_Shadows = new Actor;
	m_Controls = new Actor;
	int Priority = m_self->getPriority();
	m_Shadows->attachTo(m_self);
	m_Controls->attachTo(m_self);
	m_Shadows->setPosition(0.0f, 0.0f);
	m_Controls->setPosition(0.0f, 0.0f);
	m_Shadows->setPriority(Priority + ShadowPriority);
	m_Controls->setPriority(Priority + ButtonPriority);
}

CScene::~CScene()
{
	m_sprites.clear();
	m_buttons.clear();
	m_textFields.clear();

	m_pPrevScene = nullptr;
	m_pNextScene = nullptr;
	m_pSdFont = nullptr;
	m_view = nullptr;
	m_pApp = nullptr;
	SetBase(nullptr);
}

void CScene::show(CScene *scene)
{
	m_pApp->getSceneByName(scene->getName());
	flow::show(scene, [](Event *event) { logs::messageln("scene closed"); });
}

void CScene::show()
{
	flow::show(this, [](Event *event) { logs::messageln("scene closed"); });
}

spSprite CScene::addBackSprite(spSprite sprite, Color color/* = {255, 255, 255, 255}*/)
{
	sprite->setAnchor(0.5f, 0.5f);
	sprite->setSize(getSelfSize());
	sprite->setColor(color);
	sprite->setPosition(getSelfSize() / 2.0f);

	sprite->attachTo(getSelf());

	if (!m_backSprites.empty())
		sprite->setVisible(false);
	else
	{
		m_indexBackSprite = 0;
		m_currentBackSprite = (sprite).get();
	}

	m_backSprites.push_back(sprite);
	return sprite;
}

spSprite CScene::addBackSprite(ResAnim *texture, Vector2 position, Vector2 scale, bool bIsSaveRatio/* = true*/, unsigned char alpha /* = 255*/)
{
	spSprite sprite;
	
	if (texture)
	{
		sprite = new Sprite;
		sprite->setAnimFrame(texture);
	}
	else
	{
		sprite = new ColorRectSprite;
	}
	
	return addBackSprite(sprite, position, scale, bIsSaveRatio, alpha);
}

spSprite CScene::addBackSprite(spSprite sprite, Vector2 position, Vector2 scale, bool bIsSaveRatio/* = true*/, unsigned char alpha /* = 255*/)
{
	sprite->setAnchor(0.5f, 0.5f);

	Vector2 selfSize = getSelfSize();
	Vector2 spriteSize = sprite->getSize();
	float scaleX = selfSize.x / spriteSize.x;
	float scaleY = selfSize.y / spriteSize.y;

	if (scale == Vector2{ -1.0f, -1.0f })
	{
		if (!bIsSaveRatio)
			scale = Vector2(scaleX, scaleY);

		else
			if (scaleX < 1.0f && scaleY < 1.0f)
				if (scaleX <= scaleY)
					scale = Vector2(scaleY, scaleY);
				else
					scale = Vector2(scaleX, scaleX);
			else if (scaleX < 1.0f)
				scale = Vector2(scaleY, scaleY);
			else if (scaleY < 1.0f)
				scale = Vector2(scaleX, scaleX);
			else
				if (scaleX <= scaleY)
					scale = Vector2(scaleX, scaleX);
				else
					scale = Vector2(scaleY, scaleY);

	}
	else if (scale.x == -1.0f)
	{
		scale.x = scaleX;
		if (bIsSaveRatio)
			scale.y = scaleX;
	}
	else if (scale.y == -1.0f)
	{
		scale.y = scaleY;
		if (bIsSaveRatio)
			scale.x = scaleY;
	}

	sprite->setScale(scale);

	if (position.x < 0.0f)
		position.x = selfSize.x / 2;

	if (position.y < 0.0f)
		position.y = selfSize.y / 2;

	sprite->setPosition(position);
	sprite->setAlpha(alpha);
	sprite->attachTo(getSelf());

	if (!m_backSprites.empty())
		sprite->setVisible(false);
	else
	{
		m_indexBackSprite = 0;
		m_currentBackSprite = (sprite).get();
	}

	m_backSprites.push_back(sprite);
	return sprite;
}

spSprite CScene::addBackSprite(Sprite *sprite, Vector2 position, Vector2 scale, bool bIsSaveRatio/* = true*/, unsigned char alpha /* = 255*/)
{
	spSprite s = sprite;
	return addBackSprite(s, position, scale, bIsSaveRatio, alpha);
}

spSprite CScene::getBackSpriteByIndex(int index)
{
	if (!m_backSprites.empty() && index >= 0 && index < static_cast<int>(m_backSprites.size()))
		return m_backSprites[index];

	return nullptr;
}

void CScene::changeBackSpriteByIndex(int index)
{
	m_currentBackSprite->addTween(Actor::TweenAlpha(0), 500)->addDoneCallback([backSprite = m_currentBackSprite](Event *) {

		backSprite->setVisible(false);
		backSprite->setAlpha(255);
		});

	m_currentBackSprite = (m_backSprites[index]).get();

	m_currentBackSprite->setAlpha(0);
	m_currentBackSprite->setVisible(true);
	m_currentBackSprite->addTween(Actor::TweenAlpha(255), 500, 1, false, 250);
}

void CScene::nextBackSprite()
{
	if (m_backSprites.size() < 2)
		return;

	m_indexBackSprite++;

	if (static_cast<int>(m_backSprites.size()) < m_indexBackSprite + 1)
		m_indexBackSprite %= static_cast<int>(m_backSprites.size());

	changeBackSpriteByIndex(m_indexBackSprite);
}

void CScene::prevBackSprite()
{
	if (m_backSprites.size() < 2)
		return;

	if (m_indexBackSprite == 0)
		m_indexBackSprite = m_backSprites.size() - 1;
	else
		m_indexBackSprite--;

	changeBackSpriteByIndex(m_indexBackSprite);
}

void CScene::firstBackSprite()
{
	if (m_backSprites.size() < 2)
		return;

	if (m_indexBackSprite == 0)
		return;

	m_indexBackSprite = 0;

	changeBackSpriteByIndex(m_indexBackSprite);
}

void CScene::lastBackSprite()
{
	if (m_backSprites.size() < 2)
		return;

	if (m_indexBackSprite == m_backSprites.size() - 1)
		return;

	m_indexBackSprite = m_backSprites.size() - 1;

	changeBackSpriteByIndex(m_indexBackSprite);
}

spSprite CScene::addSprite(ResAnim *texture, Vector2 position, Vector2 scale, unsigned char alpha /* = 255*/, bool bIsScale /* = true*/, bool bIsSaveRatio /* = true*/)
{
	spSprite sprite;
	
	if (texture)
	{
		sprite = new Sprite;
		sprite->setAnimFrame(texture);
	}
	else
		sprite = new ColorRectSprite;
	
	return addSprite(sprite, position, scale, alpha, bIsScale, bIsSaveRatio);
}

spSprite CScene::addSprite(spSprite sprite, Vector2 position, Vector2 scale, unsigned char alpha /* = 255*/, bool bIsScale /* = true*/, bool bIsSaveRatio /* = true*/)
{
	if (bIsScale)
	{
		sprite->setScale(scale);
	}

	sprite->setPosition(position);
	sprite->setAlpha(alpha);
	sprite->setAnchor(0.5f, 0.5f);
	sprite->attachTo(getSelf());

	m_sprites.push_back(sprite);
	return sprite;
}

spSprite CScene::addSprite(Sprite *sprite, Vector2 position/* = { 0.0f, 0.0f }*/, Vector2 scale/* = { 1.0f, 1.0f }*/, unsigned char alpha/* = 255*/, bool bIsScale/* = true*/, bool bIsSaveRatio/* = true*/)
{
	spSprite s = sprite;
	return addSprite(s, position, scale, alpha, bIsScale, bIsSaveRatio);
}

spSprite CScene::getSpriteByIndex(int index)
{
	if (!m_sprites.empty() && index >= 0 && index < static_cast<int>(m_sprites.size()))
		return m_sprites[index];

	return nullptr;
}

spButton &CScene::addButton(spButton button)
{
	//button->attachTo(getHolder());
	button->attachTo(m_Controls);
	m_buttons.push_back(button);

	return m_buttons.back();
}

spButton CScene::getButtonByName(const std::string &name)
{
	return getButtonByName(name.c_str());
}

spButton CScene::getButtonByName(const char *name)
{
	auto it = std::find_if(m_buttons.begin(), m_buttons.end(), [this, name](spButton button) {

		if (button->getName() == name)
			return true;

		return false;
		});

	if (it != m_buttons.end())
	{
		m_currentButton = it->get();
		return *it;
	}

	m_currentButton = nullptr;
	return nullptr;
}

void CScene::addTextField(const char *text, Vector2  pos/* = { 0.0f, 0.0f }*/, int size/* = 14*/)
{
	TextField *textField = new TextField();
	textField->setAnchor(0.5f, 0.5f);
	textField->attachTo(m_Controls);
	textField->setFontSize(size);

	ResFont *font = m_pApp->GetDefaultFont();

	TextStyle style = TextStyle(font).withColor(Color::White).alignMiddle();
	textField->setStyle(style);

	textField->setText(text);

	if (pos.x == 0.0f)
		pos.x = m_self->getSize().x / 2;

	if(pos.y == 0.0f)
		pos.y = m_self->getSize().y / 2;
	
	textField->setPosition(pos);

	addTextField(textField);
}

void CScene::addTextField(spTextField textField)
{
	m_textFields.push_back(textField);
}

spTextField CScene::getTextFieldByName(const std::string &name)
{
	return getTextFieldByName(name.c_str());
}

spTextField CScene::getTextFieldByName(const char *name)
{
	auto it = std::find_if(m_textFields.begin(), m_textFields.end(), [this, name](spTextField textField) {

		if (textField->getName() == name)
			return true;

		return false;
		});

	if (it != m_textFields.end())
	{
		m_currentTextField = it->get();
		return *it;
	}

	m_currentTextField = nullptr;
	return nullptr;
}

void CScene::update()
{
	if (m_autoFunc)
		m_autoFunc();

	if (m_pBase)
		m_pBase->Update();

	if (!m_Updateables.empty())
	{
		const UpdateState us;
		for (auto &i : m_Updateables)
			i->update(us);
	}

	auto spr = getBackSpriteByIndex(0);

	static float value = 0.0f;
	static float inc = 1.0f / 30.0f;

	value += inc;

	if (value >= 1.0f)
	{
		value = 1.0f;
		inc *= -1.0f;
	}

	if (value <= 0.0f)
	{
		value = 0.0f;
		inc *= -1.0f;
	}

	//Vector4 val( static_cast<float>((rand() % 255)) / 255.0f, static_cast<float>((rand() % 255)) / 255.0f, static_cast<float>((rand() % 255)) / 255.0f, static_cast<float>((rand() % 255)) / 255.0f); 
	Vector4 val(value, value, value, value);

	if (dynamic_cast<CustomUniformMat *>(spr->_mat.get()))
	{
		CustomUniformMat &my = *safeCast<CustomUniformMat *>(spr->_mat.get());
		my.uniform = val;

		spr->_mat = mc().cache(my);
	}
// 	CustomUniformMat *ptr = static_cast<CustomUniformMat *>(spr->_mat.get());
// 
// 	ptr->uniform = val;

	//spr->_mat = mc().cache(my);
}

void CScene::AddToUpdateByScene(Actor *obj)
{
	if (obj) m_Updateables.push_back(obj);
}

