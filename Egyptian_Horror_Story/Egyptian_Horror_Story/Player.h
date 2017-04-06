#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "CameraClass.h"
#include "Light.h"

#include <SDL.h>

class Player : public Entity
{
private:
	CameraClass* mCamera;
	DirectX::SimpleMath::Vector2 mDirection;

	float mSpeed, jumpingVelocity;
	bool mJumping;
	DirectX::SimpleMath::Vector3 mVelocity;

	Light* mLight;

public:
	Player(CameraClass* camera, ID3D11Device* device, int grapchicsKey);
	virtual ~Player();

	void updatePosition();
	void handleJumping();

	bool handleMouseKeyPress(SDL_KeyboardEvent const &key);
	bool handleMouseKeyRelease(SDL_KeyboardEvent const &key);
	void handleMouseMotion(SDL_MouseMotionEvent const &motion);
	Light* getLight();

	void setPosition(DirectX::SimpleMath::Vector3 pos);

};

#endif