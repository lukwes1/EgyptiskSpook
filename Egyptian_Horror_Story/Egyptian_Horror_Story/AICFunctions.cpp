#include "AICFunctions.h"

#include <vector>
#include <SDL.h>

#include "Trap.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

int AICFunctions::setEnemySpeed(lua_State *state) {
	Enemy *enemy = static_cast<Enemy*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	if (lua_isnumber(state, -1))
		enemy->setSpeed(LTF((lua_tonumber(state, -1))));

	return 0;
}

int AICFunctions::damagePlayer(lua_State *state) {
	Player *player = static_cast<Player*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	player->damage();

	return 0;
}

int AICFunctions::setEntityPosition(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	if (lua_isnumber(state, -1) && lua_isnumber(state, -2)
		&& lua_isnumber(state, -3))
		entity->setPosition(Vector3(LTF(lua_tonumber(state, -3)),
			LTF(lua_tonumber(state, -2)), LTF(lua_tonumber(state, -1))));

	return 0;
}

int AICFunctions::setEntityRotation(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	if (lua_isnumber(state, -1) && lua_isnumber(state, -2)
		&& lua_isnumber(state, -3))
		entity->setRotation(Vector3(LTF(lua_tonumber(state, -3)),
									LTF(lua_tonumber(state, -2)),
									LTF(lua_tonumber(state, -1))));

	return 0;
}

int AICFunctions::setEntityOffsetRotation(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	if (lua_isnumber(state, -1) && lua_isnumber(state, -2)
		&& lua_isnumber(state, -3))
		entity->setOffsetRotation(
			Vector3(LTF(lua_tonumber(state, -3)),
			LTF(lua_tonumber(state, -2)),
			LTF(lua_tonumber(state, -1)))
		);

	return 0;
}

int AICFunctions::getEntityPosition(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));
	Vector3 position = entity->getPosition();
	lua_pushnumber(state, position.x);
	lua_pushnumber(state, position.y);
	lua_pushnumber(state, position.z);
	return 3;
}

int AICFunctions::slowEntity(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	// ! this is bad design, extends Entity class or maybe make moveable Entity class !
	if (lua_isnumber(state, -1)) {
		if (Enemy *enemy = dynamic_cast<Enemy*> (entity)) {
			enemy->setSpeed(enemy->getVelocity().Length() * LTF(lua_tonumber(state, -1)));
		}
		else if (Player *player = dynamic_cast<Player*> (entity)) {
			// TODO
		}
	}

	return 0;
}

int AICFunctions::stunEntity(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	entity->setPosition(entity->getPosition() * 3); //TODO

	return 0;
}

int AICFunctions::pushbackEntity(lua_State *state) {
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));

	if (lua_isnumber(state, -4) && lua_isnumber(state, -3)
		&& lua_isnumber(state, -2) && lua_isnumber(state, -1)) {
		Vector3 coord(LTF(lua_tonumber(state, -4)),
			LTF(lua_tonumber(state, -3)),
			LTF(lua_tonumber(state, -2)));

		float length = LTF(lua_tonumber(state, -1));

		Vector3 newPos = (entity->getPosition() - coord) ;
		newPos.y = 0; //Just to make sure y is the same
		newPos.Normalize();
		newPos *= length;

		entity->setPosition(entity->getPosition() + newPos);
	}

	return 0;
}

int AICFunctions::getDistanceBetween(lua_State *state) {
	Entity *entity1 = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(1)));
	Entity *entity2 = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(2)));

	Vector3 eToE2 = entity1->getPosition() - entity2->getPosition();

	lua_pushnumber(state, eToE2.Length());

	return 1;
}

int AICFunctions::entitySeesEntity(lua_State *state) {
	Enemy *entity1 = static_cast<Enemy*>
		(lua_touserdata(state, lua_upvalueindex(1)));
	Entity *entity2 = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(2)));
	NavMesh *navMesh = static_cast<NavMesh*>
		(lua_touserdata(state, lua_upvalueindex(3)));

	Vector3 e1Pos = entity1->getPosition();
	Vector3 e2Pos = entity2->getPosition();

	lua_pushboolean(state, navMesh->canSeeFrom(static_cast<int> (e1Pos.x), 
		static_cast<int> (e1Pos.z), 
		static_cast<int> (e2Pos.x), 
		static_cast<int> (e2Pos.z)));

	return 1;
}

int AICFunctions::loadPathToEntity(lua_State *state) {
	Enemy *enemy = static_cast<Enemy*>
		(lua_touserdata(state, lua_upvalueindex(1)));
	Entity *entity = static_cast<Entity*>
		(lua_touserdata(state, lua_upvalueindex(2)));
	NavMesh *navMesh = static_cast<NavMesh*>
		(lua_touserdata(state, lua_upvalueindex(3)));

	loadPath(enemy, entity, navMesh);

	return 0;
}

void AICFunctions::loadPath(Enemy *enemy, Entity *entity, NavMesh *navMesh) {
	Vector3 enemyPos = enemy->getPosition();
	Vector3 entityPos = entity->getPosition();

	navMesh->loadPathToCoord(
		enemy,
		static_cast<int> (enemyPos.x),
		static_cast<int> (enemyPos.z),
		static_cast<int> (entityPos.x),
		static_cast<int> (entityPos.z)
	);
}

int AICFunctions::loadPathToPoint(lua_State *state) {
	Enemy *enemy = static_cast<Enemy*>
		(lua_touserdata(state, lua_upvalueindex(1)));
	NavMesh *navMesh = static_cast<NavMesh*>
		(lua_touserdata(state, lua_upvalueindex(2)));

	Vector3 enemyPos = enemy->getPosition();

	if (lua_isinteger(state, -1) && lua_isinteger(state, -2)) {
		int x = static_cast<int> (lua_tonumber(state, -2)), z = static_cast<int> (lua_tonumber(state, -1));
		navMesh->loadPathToCoord(
			enemy,
			static_cast<int> (enemyPos.x), static_cast<int> (enemyPos.z),
			x, z
		);
	}

	return 0;
}


int AICFunctions::log(lua_State *state) {
	if (lua_isstring(state, -1)) {
		SDL_Log(lua_tostring(state, -1));
	}

	return 0;
}