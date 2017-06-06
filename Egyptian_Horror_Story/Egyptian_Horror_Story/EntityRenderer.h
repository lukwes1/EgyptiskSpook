#ifndef ENTITYRENDERER_H
#define ENTITYRENDERER_H

#include "Renderer.h"
#include "GraphicsData.h"
#include "SimpleMath.h"

/* LUA TESTING */
#define PLANE_TEST 101000

class EntityRenderer : public Renderer
{
public:
	static const int BUILD_KEY = 50000;
private:
	GraphicsData mGraphicsData;
	bool shadowPass;

	/* For Building */
	struct plane {
		DirectX::SimpleMath::Vector4 normal, position;
	} p1;

	ID3D11BlendState* mBlendState;
	ID3D11RasterizerState *mRastState;

	void createBlendState(ID3D11Device* device);
public:
	EntityRenderer(GAMESTATE identifier);
	virtual ~EntityRenderer();

	void setup(ID3D11Device *device, ShaderHandler &shaderHandler);
	void render(ID3D11DeviceContext *context, ShaderHandler &shaderHandler, GAMESTATE const &state);

	//Cbuffersize should for most cases be size of a 4x4 matrix
	bool loadObject(ID3D11Device *device, int key, EntityStruct::VertexStruct* vertices, int nrOfVertices, UINT cbufferSize, wchar_t* texturePath = L"../Resource/Textures/placeholder.png", DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::Identity, bool isDynamic = true);
	void setShadowPass(bool value);


	GraphicsData* getGraphicsData();

};



#endif