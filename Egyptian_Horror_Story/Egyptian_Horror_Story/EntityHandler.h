#ifndef ENTITYHANDLER_H
#define ENTITYHANDLER_H
#include <vector>
#include "EntityRenderer.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Treasure.h"
#include "FBXLoader.h"
#include "AudioManager.h"
#include "AIHandler.h"

#define WALLTEXTURE L"pyramidStone.png"

class EntityHandler
{
private:
	std::vector<Entity*> mEntities;
	std::vector<Trap*> mTraps;
	std::vector<Treasure*> mTreasures;
	AudioManager* mAudioManager;
	Entity* mFlashlightModel;
	Entity* mLevel;
	Player* mPlayer;
	Enemy* mEnemy;
	
	FBXLoader mLoader;

	//Will be removed in GraphicsHandler
	EntityRenderer* mEntityRenderer;

	ID3D11Device *device;
	ID3D11DeviceContext *context;

	bool footstepsPlaying;
	int mNrOfKeys;

	/* LUA BUILDING */
	Entity *toBuild;
	bool victory; //this is stupid

	void loadEntityModel(std::string modelName, wchar_t* textureName, Entity* entity, ID3D11Device* device);
	void updateAudio();
	void updateCollision();
	void detectCloseTreasures();
	void createBuildCube();

	float getPlayerGroundY(DirectX::SimpleMath::Vector3 const &pos) const;
public:
	EntityHandler();
	virtual ~EntityHandler();

	void loadMap(ID3D11Device* device);
	void setDevice(ID3D11Device *device);
	void setDeviceContext(ID3D11DeviceContext *context);

	void setupTraps(AIHandler* ai, ID3D11Device* device, ID3D11DeviceContext* context);
	void setupPlayer(ID3D11Device* device, ID3D11DeviceContext* context, CameraClass* camera);
	void setupEntities(ID3D11Device* device);
	void setupAudioManager(AudioManager* manager);

	void update(ID3D11DeviceContext* context, float dt, GAMESTATE state);

	int addBlock(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size,
		AABB *aabb, bool solid, int textureId);
	int updateBuildBlock(DirectX::SimpleMath::Vector3 position,
		DirectX::SimpleMath::Vector3 size,
		int textureId);
	void removeEntity(int key);

	EntityRenderer* getEntityRenderer();
	Player* getPlayer();
	Enemy* getEnemy();
	Entity* getEntity(int index) const;

	void setToBuildEntity(Entity *entity);
	Entity *getToBuildEntity() const;

	void setVictory(bool victory);
	bool hasVictory() const;
};


#endif