#ifndef PLAYER_H
#define PLAYER_H

#define SPEED 15.f

#include "Entity.h"
#include "CameraClass.h"
#include "Light.h"
#include "Treasure.h"

#include <SDL.h>

class Player : public Entity
{
private:
	// Camera and Position var
	CameraClass* mCamera;
	DirectX::SimpleMath::Vector2 mDirection;
	DirectX::SimpleMath::Vector3 mPrevPos;

	// Movement Var
	float mSpeed, mJumpingVelocity, mStamina, mMaxStamina;
	bool mJumping, mSprinting, mSneaking;
	DirectX::SimpleMath::Vector3 mVelocity;
	float mSneakTime;

	// Treasure Var
	Treasure* mPickupableTres;
	bool mIsPickingTres;
	int mScore;

	// Light Var
	Light* mLight;

	// Health var
	bool damaged;

	void updateLightPosition();
	void computeVelocity(bool noclip = false);
	void handleSprinting(float dt);
	float handleSneaking(float dt);
	void startSprint();
	void startSneaking();
	float getMovementMultiplier();

	void updateTreasureGrabbing(float dt);
public:
	//WTF
	Capsule* col;

	Player(CameraClass* camera, ID3D11Device* device, ID3D11DeviceContext* context, int grapchicsKey, GraphicsData* gData);
	virtual ~Player();

	void updatePosition(float dt, float groundY, bool noclip = false);
	void handleJumping(float dt, float groundY);

	bool handleKeyboardPress(SDL_KeyboardEvent const &key);
	bool handleKeyboardRelease(SDL_KeyboardEvent const &key);
	void handleMouseMotion(SDL_MouseMotionEvent const &motion);

	void setPickuppableTreasure(Treasure* tres);
	void resetGravity();
	
	Light* getLight();
	CameraClass* getCamera();
	DirectX::SimpleMath::Vector3 getPrevPos() const;
	DirectX::SimpleMath::Vector3 getVelocity() const;

	void setPosition(DirectX::SimpleMath::Vector3 pos);
	void setPrevPos(DirectX::SimpleMath::Vector3 pos);
	void setSpeed(float speed = SPEED);

	void damage();
	bool isDamaged() const;
	bool isJumping() const;
	bool isRunning() const;
};

#endif