#include "EntityHandler.h"
#define EPSILON 0.00001f
#define ENEMY_KEY 400
#define BASE_TRAP_KEY 1000
using namespace DirectX::SimpleMath;

void EntityHandler::setDevice(ID3D11Device *device) {
	this->device = device;
}

void EntityHandler::setDeviceContext(ID3D11DeviceContext *context) {
	this->context = context;
}

int EntityHandler::addBlock(Vector3 position, Vector3 size, AABB *aabb, bool solid, int texId) {
	std::vector<EntityStruct::VertexStruct> temp;
	mLoader.loadMesh(temp, std::string("cube.fbx"));

	Entity *block = new Entity(mNrOfKeys++, aabb, solid);
	block->setPosition(position);
	block->setScale(size);

	wchar_t *textures[] = { L"coin.png", L"block.png" };
	if (texId > ARRAYSIZE(textures)) texId = 0;

	mEntityRenderer->loadObject(device, block->getKey(), temp.data(), temp.size(),
		sizeof(DirectX::XMFLOAT4X4), textures[texId],
		(Matrix::CreateScale(size) * Matrix::CreateTranslation(position)).Transpose());

	this->mEntities.push_back(block);

	return mEntities.size() - 1;
}

int EntityHandler::updateBuildBlock(DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Vector3 size,
	int textureId) {
	wchar_t *textures[] = { L"coin.png", L"block.png" };
	if (textureId > ARRAYSIZE(textures)) textureId = 0;

	Matrix newM = Matrix::CreateScale(size) * Matrix::CreateTranslation(position);

	ID3D11Buffer* buffer = this->mEntityRenderer->getGraphicsData()->
		getConstantBuffer(EntityRenderer::BUILD_KEY);

	D3D11_MAPPED_SUBRESOURCE map;
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &newM.Transpose(), sizeof(newM));
	context->Unmap(buffer, 0);

	return 0;
}

void EntityHandler::loadEntityModel(std::string modelName, wchar_t* textureName, Entity* entity, ID3D11Device* device)
{
	std::vector<EntityStruct::VertexStruct> temp;

	this->mLoader.loadMesh(temp, modelName);

	this->mEntityRenderer->loadObject(
		device,
		entity->getKey(),
		temp.data(),
		temp.size(),
		sizeof(DirectX::XMFLOAT4X4),
		textureName,
		Matrix::CreateTranslation(entity->getPosition())
		);

}

void EntityHandler::updateAudio()
{
	//Updating emitters
	DirectX::SimpleMath::Vector3 feet = this->mPlayer->getPosition();
	feet.x -= 465.5f;

	//this->mAudioManager->updateEmitter(1, feet);
	this->mAudioManager->updateListener(this->mPlayer->getPosition(),
		this->mPlayer->getCamera()->getForward(),
		this->mPlayer->getCamera()->getUp());


	//Playing footsteps when player walks. could be moved into
	//player class. Right now more of a proof of concept
	if (abs(this->mPlayer->getVelocity().x) + abs(this->mPlayer->getVelocity().z) >= 0.3f
		&& !this->footstepsPlaying
		&& this->mPlayer->getVelocity().y == 0)
	{
		//Pitch should wary to make it less repetetive
		this->mAudioManager->playInstance(1, true, -0.5f);
		this->footstepsPlaying = true;
	}

	else if ((abs(this->mPlayer->getVelocity().x) + abs(this->mPlayer->getVelocity().z) < 0.3f
		|| this->mPlayer->getVelocity().y != 0)
		&& this->footstepsPlaying
		)
	{
		this->mAudioManager->stopInstance(1, false);
		this->footstepsPlaying = false;
	}
}

void EntityHandler::updateCollision()
{
	//Wall intersection test
	for (Entity *wall : this->mEntities)
	{
		Wall* ptr = dynamic_cast<Wall*>(wall);

		if (ptr && ptr->getAABB() && ptr->getAABB()->aabbVSCapsule(*this->getPlayer()->col)) {
			DirectX::SimpleMath::Vector3 ptop = this->mPlayer->getPrevPos() - this->mPlayer->getPosition();



			DirectX::SimpleMath::Vector3 norm = ptr->getNormal();
			//DirectX::SimpleMath::Vector3 norm = ptr->getAABB().calculateNormal(*this->mPlayer->col);

			//DirectX::SimpleMath::Vector3 norm = ptr->getAABB().getNormal(*this->getPlayer()->col);
			//DirectX::SimpleMath::Vector3 diff;
			//diff = mPlayer->getPosition() - ptr->getAABB().mPoint;
			//
			//DirectX::SimpleMath::Vector3 tmp1 = diff;
			//DirectX::SimpleMath::Vector3 tmp2 = norm;
			//
			//tmp1.Normalize();
			//tmp2.Normalize();
			//
			//diff = (tmp2.Dot(tmp1) * diff); //STÄMMER KAPPA
			//diff = norm - diff;
			//norm.Normalize();
			//this->mPlayer->setPosition(mPlayer->getPosition() + diff + norm * mPlayer->col->mRadius);



			//ptop = ptop - (norm.Dot(ptop) * norm);

			ptop = ptop - (ptr->getNormal().Dot(ptop) * norm);

			//this->mPlayer->setPrevPos(this->mPlayer->getPrevPos() + ptop);

			ptop = -ptop;
			this->mPlayer->setPosition(this->mPlayer->getPrevPos() + ptop);

			//this->mPlayer->setPosition(ptr->getOBB().mPoint);



		}
	}

}

void EntityHandler::detectCloseTreasures()
{
	bool exit = false;

	for (int i = 0; i < this->mTreasures.size() && !exit; i++)
	{
		this->mPlayer->setPickuppableTreasure(nullptr);

		if (this->mTreasures[i]->getPickedUp())
		{
			this->mEntityRenderer->getGraphicsData()->removeData(this->mTreasures[i]->getKey());
			
			delete this->mTreasures[i];
			this->mTreasures.erase(this->mTreasures.begin() + i);
			exit = true;
		}

		//Change to squared for performance
		
		else if ((this->mPlayer->getPosition() - this->mTreasures[i]->getPosition()).Length() < 1.f)
		{
			this->mPlayer->setPickuppableTreasure(this->mTreasures[i]);
			exit = true;
		}
	}
}

void EntityHandler::setupTraps(AIHandler* ai, ID3D11Device* device, ID3D11DeviceContext* context)
{
	Trap* test = new Trap(1000, 25, 0, -74);
	ai->addTrap("scripts/TrapHanger.lua", test);

	this->loadEntityModel("fallingRock.fbx", L"", test, device);
	this->mTraps.push_back(test);

	test = new Trap(1001, 5, 12, -5);
	ai->addTrap("scripts/TrapStone.lua", test);

	this->loadEntityModel("fallingRock.fbx", L"", test, device);
	this->mTraps.push_back(test);
	
	ai->setupAI();
}

void EntityHandler::setupPlayer(ID3D11Device* device, ID3D11DeviceContext* context, CameraClass* camera)
{
	this->mPlayer = new Player(camera, device, context, this->mNrOfKeys++, this->mEntityRenderer->getGraphicsData());
	this->mPlayer->setPosition(DirectX::SimpleMath::Vector3(0, 0, 4));
}

EntityHandler::EntityHandler()
{
	this->mEntityRenderer = new EntityRenderer(GAMESTATE::UNDEFINED);
	this->footstepsPlaying = false;
	this->toBuild = nullptr;
}

EntityHandler::~EntityHandler()
{
	delete this->mPlayer;
	delete this->mFlashlightModel;
	delete this->mLevel;

	for (size_t i = 0; i < this->mEntities.size(); i++)
		delete this->mEntities[i];

	for (size_t i = 0; i < this->mTraps.size(); i++)
		delete this->mTraps[i];

	for (size_t i = 0; i < this->mTreasures.size(); i++)
	{
		if (this->mTreasures[i])
			delete this->mTreasures[i];
	}
}

void EntityHandler::loadMap(ID3D11Device* device)
{
	this->mLevel = new Entity(401);

	this->loadEntityModel("LevelDesignTest.fbx", WALLTEXTURE, this->mLevel, device);
}

void EntityHandler::setupEntities(ID3D11Device* device)
{
	setDevice(device);

	this->mFlashlightModel = new Entity(this->mPlayer->getLight()->getGraphicsKey());
	this->loadEntityModel("flashLight.fbx", L"dargon_bump.jpg", mFlashlightModel, device);
	createBuildCube();
}

void EntityHandler::createBuildCube() {
	GraphicsData *gd = mEntityRenderer->getGraphicsData();
	std::vector<EntityStruct::VertexStruct> temp;
	mLoader.loadMesh(temp, std::string("cube.fbx"));
	
	mEntityRenderer->loadObject(device, EntityRenderer::BUILD_KEY, temp.data(),
		temp.size(), sizeof(Matrix));
}

void EntityHandler::setupAudioManager(AudioManager* manager)
{
	this->mAudioManager = manager;
	this->mAudioManager->addSfx(0, L"monster.wav");
	this->mAudioManager->addSfx(1, L"footStepLouder.wav");
	this->mAudioManager->createInstance(1, 1);
	this->mAudioManager->createEmitter(1);
	this->mAudioManager->apply3DToInstance(1, 1);
	this->mAudioManager->playSfx(0);
}

void EntityHandler::update(ID3D11DeviceContext* context, float dt)
{
	setDeviceContext(context);

	DirectX::SimpleMath::Vector3 prevPos = this->mPlayer->getPosition();
	this->mPlayer->updatePosition(dt, getPlayerGroundY(mPlayer->getPosition()));
}

float EntityHandler::getPlayerGroundY(Vector3 const &position) const {
	Vector3 o = position;
	AABB *aabb;
	Vector3 p, s;

	float groundY = 0, temp;

	for (auto& entity : mEntities) {
		if (entity->isSolid()) {
			aabb = entity->getAABB();
			p = aabb->mPoint;
			s = aabb->mScale;
			if (abs(o.x - p.x) <= s.x && abs(o.z - p.z) <= s.z) {
				temp = p.y + s.y + 2.0f;
				if (temp <= o.y + EPSILON && temp > groundY) {
					groundY = temp;
				}
			}
		}
	}

	return groundY;
}

EntityRenderer* EntityHandler::getEntityRenderer()
{
	return this->mEntityRenderer;
}

Player* EntityHandler::getPlayer()
{
	return this->mPlayer;
}

Enemy* EntityHandler::getEnemy()
{
	return this->mEnemy;
}

Entity* EntityHandler::getEntity(int index) const {
	return this->mEntities[index];
}

void EntityHandler::setToBuildEntity(Entity *entity) {
	this->toBuild = entity;
}

Entity *EntityHandler::getToBuildEntity() const {
	return toBuild;
}
