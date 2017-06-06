#include "Game.h"
#include "ParticleRenderer.h"

#define DEFAULT_MAP_PATH "../Resource/Map/mapTest.lwm"

void Game::setupRenderers()
{
	mGuiRenderer = new GUIRenderer();
	this->mGraphics->addRenderer(mGuiRenderer);

	this->mGraphics->addRenderer(new ParticleRenderer(this->mCamera, mStateHandler->getState()));
	this->mGraphics->addRenderer(this->mEntityHandler->getEntityRenderer());

	this->mGraphics->setupRenderers();
	this->mGraphics->setupDSAndSRViews();

	mGuiRenderer->loadButtons(this->mMenuHandler); // Find better place to load buttons
}

void Game::setupEntityHandler()
{

	//this->mEntityHandler->setupAudioManager(&this->mAudioManager);
	this->mEntityHandler->setupPlayer(this->mGraphics->getDevice(),
		this->mGraphics->getDeviceContext(),
		this->mCamera);

	this->mEntityHandler->setupEntities(this->mGraphics->getDevice());

	//this is neccessary for the traps
//	this->mAIHandler = new AIHandler(mEntityHandler->getEnemy(), mEntityHandler->getPlayer());

	//this->mEntityHandler->setupTraps(this->mAIHandler, this->mGraphics->getDevice(), this->mGraphics->getDeviceContext());

	luaHandler.setEntityHandler(mEntityHandler);
	luaHandler.setPlayer(mEntityHandler->getPlayer());
	mapLoader.setupMapLoader(mEntityHandler, &luaHandler);
}

Game::Game(GraphicsHandler* mGraphicsHandler, OptionsHandler* options) {
	this->mStateHandler = new StateHandler();
	this->mEntityHandler = new EntityHandler();

	this->mStateHandler->setState(GAMESTATE::MAIN_MENU);

	this->mGraphics = mGraphicsHandler;
	this->mOptionHandler = options;

	this->mOptionHandler->setup(this->mGraphics->getDevice());
	
	this->mCamera = new CameraClass(this->mGraphics->getDevice(),
		this->mEntityHandler->getEntityRenderer()->getGraphicsData(),
		this->mOptionHandler->getGraphicSettings());

	this->setupEntityHandler();

	this->setupRenderers();
}

Game::~Game()
{
	delete this->mStateHandler;
	delete this->mCamera;
	delete this->mEntityHandler;
}

void Game::updateGame(float dt)
{
	this->mEntityHandler->update(this->mGraphics->getDeviceContext(), dt, mStateHandler->getState());
	this->mCamera->update(this->mGraphics->getDeviceContext());

	this->mCamera->updateRotation(this->mGraphics->getDeviceContext());
	this->luaHandler.update();

	if (this->mEntityHandler->hasVictory())
		mStateHandler->setState(GAMESTATE::VICTORY);

	if (this->mEntityHandler->getPlayer()->getPosition().y < -250.f)
		mStateHandler->setState(GAMESTATE::MAIN_MENU);
}

void Game::draw() {
	this->mGraphics->clear();
	this->mGraphics->renderRenderers(this->mCamera->getMatrixBuffer(),
									this->mEntityHandler->getEntityRenderer()->getGraphicsData()->getConstantBuffer(301),
									mStateHandler->getState());
	this->mGraphics->present();
}

void Game::update(float dt) {
	this->mStateHandler->update(this, dt);
}

bool Game::handleKeyboardPress(SDL_KeyboardEvent const& key)
{
	this->mEntityHandler->getPlayer()->handleKeyboardPress(key);
	this->mOptionHandler->handleButtonPress(key, this->mGraphics->getDeviceContext());

	switch (key.keysym.scancode)
	{
	case FOVUPKEY:
	case FOVDOWNKEY:
		this->mCamera->updateProjection(
			this->mGraphics->getDeviceContext(), 
			this->mOptionHandler->getGraphicSettings());
		break;
	case BUILDKEY:
		{
			GAMESTATE state = mStateHandler->getState();
			if (state == GAMESTATE::PLAY)
				mStateHandler->setState(GAMESTATE::BUILDING);
			else if (state == GAMESTATE::BUILDING)
				mStateHandler->setState(GAMESTATE::PLAY);
			break;
		}
	case SAVEKEY:
		{
			if (mapLoader.isMapLoaderLoaded())
				mapLoader.saveMap(DEFAULT_MAP_PATH);
			break;
		}
	case SDL_SCANCODE_1:
		updateLua();
		break;
	}

	if (mStateHandler->getState() == GAMESTATE::BUILDING)
		this->mapLoader.handleMouseEvents(
			key.keysym.scancode,
			this->mCamera->getForward(),
			this->mEntityHandler->getPlayer()->getPosition()
		);

	return true;
}

bool Game::handleKeyboardRelease(SDL_KeyboardEvent const& key)
{
	this->mEntityHandler->getPlayer()->handleKeyboardRelease(key);
	this->mOptionHandler->handleButtonRelease(key, this->mGraphics->getDeviceContext());

	return true;
}

void Game::handleMouseMotion(SDL_MouseMotionEvent const &motion)
{
	this->mEntityHandler->getPlayer()->handleMouseMotion(motion);
	this->mMenuHandler.mouseMotion(motion);
}

bool Game::handleMousePress(SDL_MouseButtonEvent const &button) {
	this->mStateHandler->setState(
		this->mMenuHandler.mousePress(button, this->mStateHandler->getState())
	);

	if (this->mStateHandler->getState() == PLAY) {
		updateLua();
		mEntityHandler->getPlayer()->setPosition(
			DirectX::SimpleMath::Vector3::Zero);
		mEntityHandler->getPlayer()->resetGravity();
		return true;
	} else
		return false;
}

void Game::updateLua() {
	luaHandler.reset();
	luaHandler.loadLua("scripts/ScriptObjects.lua");
	mapLoader.loadLuaFile();
	mapLoader.loadMap(DEFAULT_MAP_PATH);
}

GAMESTATE Game::StateHandler::getState() {
	return this->state;
}

void Game::StateHandler::setState(GAMESTATE state) {
	this->state = state;
}

void Game::StateHandler::update(Game* g, float dt) {
	switch (this->state) {
	case GAMESTATE::DEFAULT:
		break;
	case GAMESTATE::MAIN_MENU:
		break;
	case GAMESTATE::PLAY:
		g->updateGame(dt);
		break;
	case GAMESTATE::BUILDING:
		g->updateGame(dt);
		g->mapLoader.buildingModeUpdate(
			g->mCamera->getForward(),
			g->mEntityHandler->getPlayer()->getPosition()
		);
		break;
	}
}

void Game::setWindowSize(SDL_Window* window) {
	int width = this->mOptionHandler->getGraphicSettings().width;
	int height = this->mOptionHandler->getGraphicSettings().height;
	SDL_SetWindowSize(window, width, height);
	this->mMenuHandler.setWindowResolution(width, height);
}

void Game::victory() {
	mStateHandler->setState(GAMESTATE::VICTORY);
}