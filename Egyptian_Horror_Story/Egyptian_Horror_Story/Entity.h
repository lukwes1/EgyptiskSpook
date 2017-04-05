#ifndef ENTITY_H
#define ENTITY_H

#include "OBB.h"
#include "EntityRenderer.h"

class Entity
{
private:
	DirectX::SimpleMath::Vector3 mPos;
	OBB* mObb;
	EntityRenderer renderer;

public:
	Entity();
	virtual ~Entity();

	void move(DirectX::SimpleMath::Vector3 offset);
	void createOBB(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 U, DirectX::SimpleMath::Vector3 V, DirectX::SimpleMath::Vector3 W);

	virtual void setPosition(DirectX::SimpleMath::Vector3 pos);
	DirectX::SimpleMath::Vector3 getPosition() const;

	OBB getOBB() const;
	EntityRenderer* getRenderer();
};


#endif