#include "Game.h"

Game::Game(GraphicsHandler* mGraphicsHandler, float width, float height)
{
	this->mGraphics = mGraphicsHandler;
	this->mCamera = new CameraClass(this->mGraphics->getDevice(), width, height);
	this->mPlayer = new Player(this->mCamera, this->mGraphics->getDevice());

	this->mGraphics->setupShadow(this->mPlayer->getLight());
	
	this->mGraphics->setupTestData();
	this->mGraphics->setupFloor();

	this->mPlayer->setPosition(DirectX::SimpleMath::Vector3(0, 0, -5));

	this->mWall = new Wall(
		DirectX::SimpleMath::Vector3(-1.f, -1.f, 0.f),
		DirectX::SimpleMath::Vector3(2.f, 0.f, 0.f), 
		DirectX::SimpleMath::Vector3(0.f, 2.f, 0.f),
		DirectX::SimpleMath::Vector3(0.f, 0.f, 2.f));
}

Game::~Game()
{
	delete this->mCamera;
	delete this->mPlayer;
}

void Game::update()
{
	this->mCamera->update(this->mGraphics->getDeviceContext());
	this->mPlayer->updatePosition();

	//test
	if (this->mWall->getOBB().obbVSPoint(this->mPlayer->getPosition()))

	this->mGraphics->render(this->mCamera->getMatrixBuffer());
	this->mGraphics->present();
}

bool Game::handleMouseKeyPress(SDL_KeyboardEvent const& key)
{
	return this->mPlayer->handleMouseKeyPress(key);
}

bool Game::handleMouseKeyRelease(SDL_KeyboardEvent const& key)
{
	return this->mPlayer->handleMouseKeyRelease(key);
}

void Game::handleMouseMotion(SDL_MouseMotionEvent const &motion)
{
	this->mPlayer->handleMouseMotion(motion);
	this->mCamera->updateRotation(this->mGraphics->getDeviceContext());
}
