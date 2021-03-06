#ifndef GAME_H
#define GAME_H

#include "GraphicsHandler.h"
#include "EntityHandler.h"
#include "GUIRenderer.h"
#include "MenuHandler.h"

// Lua Lua
#include "LuaMapLoader.h"
#include "LuaHandler.h"

class Game
{
private:
	class StateHandler {
	private:
		GAMESTATE state;
	public:
		GAMESTATE getState();
		void setState(GAMESTATE state);
		void update(Game* g, float dt);
	}* mStateHandler;

	GraphicsHandler* mGraphics;
	CameraClass* mCamera;
	EntityHandler* mEntityHandler;
	AIHandler* mAIHandler;
	GUIRenderer *mGuiRenderer; //temp
	AudioManager mAudioManager;
	OptionsHandler* mOptionHandler;
	MenuHandler mMenuHandler;

	// LUA LUA
	LuaMapLoader mapLoader;
	LuaHandler luaHandler;

	void setupRenderers();
	void setupEntityHandler();
public:
	Game(GraphicsHandler* graphicsHandler, OptionsHandler* options);
	virtual ~Game();

	void updateGame(float dt);
	void update(float dt);
	void draw();

	bool handleKeyboardPress(SDL_KeyboardEvent const &key);
	bool handleKeyboardRelease(SDL_KeyboardEvent const &key);
	void handleMouseMotion(SDL_MouseMotionEvent const &motion);
	// returns true/false if cursor should be locked / unlocked. not good code but works
	bool handleMousePress(SDL_MouseButtonEvent const &button);
	void updateLua();

	void setWindowSize(SDL_Window* window);
	void victory();
};


#endif