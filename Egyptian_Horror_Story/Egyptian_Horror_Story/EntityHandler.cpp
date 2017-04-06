#include "EntityHandler.h"

EntityHandler::EntityHandler()
{
	this->mEntityRenderer = new EntityRenderer();
}

EntityHandler::~EntityHandler()
{
	delete this->mPlayer;

	for (size_t i = 0; i < this->mEntities.size(); i++)
	{
		delete this->mEntities[i];
	}
}

void EntityHandler::setupPlayer(ID3D11Device* device, CameraClass* camera)
{
	this->mPlayer = new Player(camera, device, this->mNrOfKeys++);
	this->mPlayer->setPosition(DirectX::SimpleMath::Vector3(0, 0, -5));
}

void EntityHandler::setupEntities(ID3D11Device* device)
{
	//test
	for (size_t j = 0; j < 10; j++)
	{
		for (size_t i = 0; i < 10; i++)
		{
			Wall* wall = new Wall(
				DirectX::SimpleMath::Vector3(-10.f + (2 * i), -2.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(2.f, 0.f, 0.f),
				DirectX::SimpleMath::Vector3(0.f, 2.f, 0.f),
				DirectX::SimpleMath::Vector3(0.f, 0.f, 2.f), this->mNrOfKeys++);

			EntityStruct::VertexStruct testData[] = {
				DirectX::SimpleMath::Vector3(-10.f + (2 * i), -2.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 1.f),

				DirectX::SimpleMath::Vector3(-10.f + (2 * i), 0.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 0.f),

				DirectX::SimpleMath::Vector3(-8.f + (2 * i), -2.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 1.f),

				DirectX::SimpleMath::Vector3(-8.f + (2 * i), -2.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 1.f),

				DirectX::SimpleMath::Vector3(-8.f + (2 * i), 0.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 0.f),

				DirectX::SimpleMath::Vector3(-10.f + (2 * i), 0.f + (2 * j), 10.f),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 0.f)
			};
			this->mEntityRenderer->loadObject(device, wall->getKey(), testData, 6, L"../Resource/Textures/normalMap.png");

			this->mEntities.push_back(wall);
		}
	}

	for (size_t j = 0; j < 10; j++)
	{
		for (size_t i = 0; i < 10; i++)
		{
			Wall* wall = new Wall(
				DirectX::SimpleMath::Vector3(12.f, -2.f + (2 * j), 10.f - (2 * i)),
				DirectX::SimpleMath::Vector3(0.f, 0.f, 2.f),
				DirectX::SimpleMath::Vector3(0.f, 2.f, 0.f),
				DirectX::SimpleMath::Vector3(2.f, 0.f, 0.f), this->mNrOfKeys++);

			EntityStruct::VertexStruct testData[] = {
				DirectX::SimpleMath::Vector3(10.f, -2.f + (2 * j), 10.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 1.f),

				DirectX::SimpleMath::Vector3(10.f, 0.f + (2 * j), 10.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 0.f),

				DirectX::SimpleMath::Vector3(10.f, -2.f + (2 * j), 8.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 1.f),

				DirectX::SimpleMath::Vector3(10.f, -2.f + (2 * j), 8.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 1.f),

				DirectX::SimpleMath::Vector3(10.f, 0.f + (2 * j), 8.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(1.f, 0.f),

				DirectX::SimpleMath::Vector3(10.f, 0.f + (2 * j), 10.f - (2 * i)),
				DirectX::SimpleMath::Vector3(),
				DirectX::SimpleMath::Vector2(0.f, 0.f)
			};
			this->mEntityRenderer->loadObject(device, wall->getKey(), testData, 6, L"../Resource/Textures/normalMap.png");

			this->mEntities.push_back(wall);
		}
	}
	

	//another test
	Entity* ent = new Entity(this->mNrOfKeys++);

	EntityStruct::VertexStruct testData2[] = {
		DirectX::SimpleMath::Vector3(-10.f, -2.f, -10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(0.f, 1.f),

		DirectX::SimpleMath::Vector3(-10.f, -2.f, 10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(0.f, 0.f),

		DirectX::SimpleMath::Vector3(10.f, -2.f, 10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(1.f, 0.f),

		DirectX::SimpleMath::Vector3(-10.f, -2.f, -10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(0.f, 1.f),

		DirectX::SimpleMath::Vector3(10.f, -2.f, -10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(1.f, 1.f),

		DirectX::SimpleMath::Vector3(10.f, -2.f, 10.f),
		DirectX::SimpleMath::Vector3(),
		DirectX::SimpleMath::Vector2(1.f, 0.f)
	};

	this->mEntities.push_back(ent);

	this->mEntityRenderer->loadObject(device, ent->getKey(), testData2, 6, L"../Resource/Textures/normalMap.png");
}

void EntityHandler::update()
{
	DirectX::SimpleMath::Vector3 prevPos = this->mPlayer->getPosition();

	this->mPlayer->updatePosition();


	//Wall intersection test
	for (Entity *wall : this->mEntities) 
	{
		Wall* ptr = dynamic_cast<Wall*>(wall);
		if (ptr && ptr->getOBB().obbVSPoint(this->mPlayer->getPosition()))
		{
			this->mPlayer->setPosition(this->mPlayer->getPosition() + ptr->getNormal() * 0.03f);
		}
	}
	
}

EntityRenderer* EntityHandler::getRenderer()
{
	return this->mEntityRenderer;
}

Player* EntityHandler::getPlayer()
{
	return this->mPlayer;
}
