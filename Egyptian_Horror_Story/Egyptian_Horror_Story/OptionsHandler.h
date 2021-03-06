#ifndef OPTIONHANDLER_H
#define OPTIONHANDLER_H

#include "Direct3DHeader.h"
#include <SDL.h>
#include <fstream>
#include <string>
#include "Structs.h"
//F�r att kunna anv�nda M_PI
#define _USE_MATH_DEFINES
#include <math.h>


#define DEFAULTWIDTH 1280
#define DEFAULTHEIGHT 720
#define DEFAULTFOV 0.45
#define DEFAULTFARPLANE 100
#define DEFAULTBRIGHTNESS 0

#define FOVUPKEY SDL_SCANCODE_RIGHT
#define FOVDOWNKEY SDL_SCANCODE_LEFT
#define BUILDKEY SDL_SCANCODE_O
#define SAVEKEY SDL_SCANCODE_L

class OptionsHandler
{
private:
	settings::GraphicSettings mGraphics;
	ID3D11Buffer* mBrightnessBuffer;


	void updateBuffer(ID3D11DeviceContext* context);
public:
	OptionsHandler();
	virtual ~OptionsHandler();

	void setup(ID3D11Device* device);

	bool handleButtonPress(SDL_KeyboardEvent const& key, ID3D11DeviceContext* context);
	bool handleButtonRelease(SDL_KeyboardEvent const& key, ID3D11DeviceContext* context);

	settings::GraphicSettings& getGraphicSettings();
	
	ID3D11Buffer* getBrightnessBuffer();
};


#endif
