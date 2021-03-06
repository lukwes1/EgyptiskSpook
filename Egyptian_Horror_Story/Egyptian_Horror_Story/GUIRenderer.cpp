#include "GUIRenderer.h"
#define SHADERS 40

using namespace DirectX::SimpleMath;

GUIRenderer::GUIRenderer() : Renderer(){
	this->mGraphicsData = new GraphicsData();

	// for testing
	navTest = nullptr;
	mDirection = 0;
}

GUIRenderer::~GUIRenderer() {
	delete this->mGraphicsData;

	if (navTest)
		navTest->Release();
}

void GUIRenderer::setup(ID3D11Device *device, ShaderHandler &shaders) {
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIMENSIONS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shaders.setupVertexShader(device, SHADERS, L"GUIVS.hlsl", "main", desc, ARRAYSIZE(desc));
	shaders.setupPixelShader(device, SHADERS, L"GUIPS.hlsl", "main");
	shaders.setupGeometryShader(device, SHADERS, L"GUIGS.hlsl", "main");

	Vector2 dimension = { 0.4f, 0.2f };
	// Add new GUI elements here
	this->mElements.push_back(GUI_ELEMENT{ Vector3(-0.2f, 0.2f, 0), dimension });
	this->mElements.push_back(GUI_ELEMENT{ Vector3(-0.2f, -0.2f, 0), dimension });
	this->mElements.push_back(GUI_ELEMENT{ Vector3(-0.4f, 0.6f, 0), Vector2(0.8f, 0.3f) });
	this->mElements.push_back(GUI_ELEMENT{ Vector3(-0.9f, -0.1f, 0), Vector2(0.6f, 0.6f) });
	this->mElements.push_back(GUI_ELEMENT{ Vector3(0.30f, -0.1f, 0), Vector2(0.6f, 0.6f) });
	this->mElements.push_back(GUI_ELEMENT{ Vector3(-0.4f, 0.1f, 0), Vector2(0.8f, 0.4f) });

	// Add the texture here, texture ID = index of element in mElements, this class got own graphicsdata, so infinite ids is availabe (not inf)
	mGraphicsData->loadTexture(0, L"play.png", device);
	mGraphicsData->loadTexture(1, L"options.png", device);
	mGraphicsData->loadTexture(2, L"splash.png", device);
	mGraphicsData->loadTexture(3, L"helpimage.png", device);
	mGraphicsData->loadTexture(4, L"goalimage.png", device);
	mGraphicsData->loadTexture(5, L"victory.png", device);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &this->mElements[0];

	mGraphicsData->createVertexBuffer(0, sizeof(GUI_ELEMENT) * this->mElements.size(), &data, device, true);
}

void GUIRenderer::loadButtons(MenuHandler &menuHandler) {
	for (size_t i = 0; i < this->mElements.size(); i++)
		menuHandler.addButton(static_cast<int> (i),
			Vector2(this->mElements[i].pos), this->mElements[i].dimensions); //i is pretty hardcoded, change later, change elements to use vector2
}

void GUIRenderer::render(ID3D11DeviceContext *context, ShaderHandler &shaders, GAMESTATE const &state) {
	context->IASetInputLayout(shaders.getInputLayout(SHADERS));
	shaders.setShaders(context, SHADERS, SHADERS, SHADERS);

	if (state == GAMESTATE::PLAY)
		renderHud(context, shaders);
	else if (state == GAMESTATE::MAIN_MENU)
		renderStartMenu(context, shaders);
	else if (state == GAMESTATE::VICTORY)
		renderVictory(context, shaders);
}

void GUIRenderer::renderStartMenu(ID3D11DeviceContext *context, ShaderHandler &shaders) {
	if (mDirection == 0) {
		this->mElements[2].dimensions.x += 0.0005f;
		this->mElements[2].pos.x -= 0.00025f;
		this->mElements[2].dimensions.y += 0.0005f;
		this->mElements[2].pos.y -= 0.00025f;

		if (mElements[2].dimensions.x > 1.0f) {
			mDirection = 1;
		}
	}
	else {
		this->mElements[2].dimensions.x -= 0.0005f;
		this->mElements[2].pos.x += 0.00025f;
		this->mElements[2].dimensions.y -= 0.0005f;
		this->mElements[2].pos.y += 0.00025f;

		if (mElements[2].dimensions.x < 0.6f) {
			mDirection = 0;
		}
	}
	D3D11_MAPPED_SUBRESOURCE res;
	context->Map(mGraphicsData->getVertexBuffer(0), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	memcpy(res.pData, &this->mElements[0], sizeof(GUI_ELEMENT) * mElements.size());
	context->Unmap(mGraphicsData->getVertexBuffer(0), 0);

	UINT stride = sizeof(GUI_ELEMENT), offset = 0;
	ID3D11Buffer *buffer = this->mGraphicsData->getVertexBuffer(0);
	ID3D11ShaderResourceView *srv;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	// Loop through and draw buttons, optimize plz
	for (size_t i = 0; i < mElements.size() - 1; i++) {
		srv = this->mGraphicsData->getSRV(static_cast<int> (i));
		context->PSSetShaderResources(0, 1, &srv);
		context->Draw(1, static_cast<int> (i));
	}
}

void GUIRenderer::renderVictory(ID3D11DeviceContext *context, ShaderHandler &shaders) {
	UINT stride = sizeof(GUI_ELEMENT), offset = 0;
	ID3D11Buffer *buffer = this->mGraphicsData->getVertexBuffer(0);
	ID3D11ShaderResourceView *srv;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	// Loop through and draw buttons, optimize plz
	srv = this->mGraphicsData->getSRV(5);
	context->PSSetShaderResources(0, 1, &srv);
	context->Draw(1, 5);
}


void GUIRenderer::renderHud(ID3D11DeviceContext *context, ShaderHandler &shaders) {
	UINT stride = sizeof(Vector3), offset = 0;
	ID3D11Buffer *buffer = this->mGraphicsData->getVertexBuffer(0);
	ID3D11ShaderResourceView *srv = this->mGraphicsData->getSRV(0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetInputLayout(shaders.getInputLayout(SHADERS));
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->PSSetShaderResources(0, 1, &srv);

	if (navTest)
		context->PSSetShaderResources(0, 1, &navTest);

	// context->Draw(this->mSize, 0); // not used right now
}

void GUIRenderer::setNavigationTest(ID3D11Device *device, void* pixels, int w, int h) {
	if (navTest)
		navTest->Release();
	mGraphicsData->loadTexture(0, L"NAVIGATION_TEST.bmp", device);

	/*
	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	desc.Width = w;
	desc.Height = h;
	desc.ArraySize = desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = pixels;
	data.SysMemPitch = w * 4;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MipLevels = 1;
	
	hr = device->CreateShaderResourceView(tex, &srDesc, &navTest);
	tex->Release();
	*/
}