#include "EntityRenderer.h"
#define ENTITY_SHADER 20
#define BUILD_ENTITY_SHADER 21

using namespace DirectX::SimpleMath;

EntityRenderer::EntityRenderer(GAMESTATE identifier) : Renderer(identifier)
{
	this->shadowPass = false;
	mBlendState = nullptr;
	mRastState = nullptr;
}

EntityRenderer::~EntityRenderer()
{
	if (mRastState)
		mRastState->Release();
	if (mBlendState)
		mBlendState->Release();
}

void EntityRenderer::createBlendState(ID3D11Device* device) {
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	D3D11_RENDER_TARGET_BLEND_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(tDesc));
	tDesc.BlendEnable = true;
	tDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tDesc.DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
	tDesc.BlendOp = D3D11_BLEND_OP_ADD;
	tDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	tDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	tDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tDesc.RenderTargetWriteMask = 0xf;
	desc.RenderTarget[0] = tDesc;

	device->CreateBlendState(&desc, &mBlendState);

	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));
	rDesc.CullMode = D3D11_CULL_NONE;
	//	rDesc.CullMode = D3D11_CULL_BACK;
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&rDesc, &mRastState);
}

void EntityRenderer::setup(ID3D11Device* device, ShaderHandler& shaderHandler)
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 6 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HRESULT hr = shaderHandler.setupVertexShader(device, ENTITY_SHADER, L"EntityVS.hlsl", "main", desc, ARRAYSIZE(desc));

	shaderHandler.setupPixelShader(device, ENTITY_SHADER, L"EntityPS.hlsl", "main");
	shaderHandler.setupPixelShader(device, BUILD_ENTITY_SHADER, L"BuildEntityPS.hlsl", "main");

	p1.normal = Vector4(1, 1, 1, 0);
	p1.normal.Normalize();
	p1.position = Vector4(0, 0, 0, 1);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &p1;

	mGraphicsData.createConstantBuffer(PLANE_TEST, sizeof(p1), &data, device, true);

	createBlendState(device);
}

void EntityRenderer::render(ID3D11DeviceContext* context, ShaderHandler& shaderHandler, GAMESTATE const &state)
{
	if (state == BUILDING || state == PLAY) {
		UINT stride = sizeof(EntityStruct::VertexStruct), offset = 0;

		ID3D11Buffer* temp;

		shaderHandler.setVertexShader(context, ENTITY_SHADER);
		context->IASetInputLayout(shaderHandler.getInputLayout(ENTITY_SHADER));
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		shaderHandler.setGeometryShader(context, ShaderHandler::UNBIND_SHADER);

		if (state == BUILDING) {
			// update buffers
			ID3D11Buffer *fx = mGraphicsData.getConstantBuffer(PLANE_TEST);
			p1.position += p1.normal * 0.05f;
			if (p1.position.Length() > 32) p1.position = -(p1.normal * 10);
			D3D11_MAPPED_SUBRESOURCE map;
			context->Map(fx, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			memcpy(map.pData, &p1, sizeof(p1));
			context->Unmap(fx, 0);

			// Set blending and RS
			context->OMSetBlendState(mBlendState, NULL, 0xFFFFFF);
			context->RSSetState(mRastState);

			// Set shaders and buffers
			shaderHandler.setPixelShader(context, BUILD_ENTITY_SHADER);
			context->PSSetConstantBuffers(3, 1, &fx);

			// Draw block
			temp = this->mGraphicsData.getVertexBuffer(BUILD_KEY);
			context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
			temp = this->mGraphicsData.getConstantBuffer(BUILD_KEY);
			context->VSSetConstantBuffers(1, 1, &temp);

			ID3D11ShaderResourceView *view = this->mGraphicsData.getSRV(BUILD_KEY);
			context->PSSetShaderResources(0, 1, &view);
			context->Draw(this->mGraphicsData.getNrOfVertices(BUILD_KEY), 0);

			// Unset blending and RS
			context->OMSetBlendState(nullptr, NULL, 0xFFFFFF);
			context->RSSetState(nullptr);
		}
		shaderHandler.setPixelShader(context, ENTITY_SHADER);

		if (this->shadowPass)
		{
			shaderHandler.setPixelShader(context, ShaderHandler::UNBIND_SHADER);
		}

		else
		{
			temp = mGraphicsData.getConstantBuffer(300);
			context->PSSetConstantBuffers(0, 1, &temp);

			temp = mGraphicsData.getConstantBuffer(302);
			context->PSSetConstantBuffers(1, 1, &temp);
		}

		int key = 0;

		for (auto const &item : *this->mGraphicsData.getVertexBufferMap())
		{
			key = item.first;
			if (key == BUILD_KEY) continue;

			temp = this->mGraphicsData.getVertexBuffer(key);
			context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);

			temp = this->mGraphicsData.getConstantBuffer(key);
			context->VSSetConstantBuffers(1, 1, &temp);


			if (!this->shadowPass)
			{
				ID3D11ShaderResourceView* texTemp = this->mGraphicsData.getSRV(key);
				context->PSSetShaderResources(0, 1, &texTemp);
			}

			context->Draw(this->mGraphicsData.getNrOfVertices(key), 0);
		}
	}
}

bool EntityRenderer::loadObject(ID3D11Device *device, int key, EntityStruct::VertexStruct* vertices,
	int nrOfVertices, UINT cbufferSize,
	wchar_t* texturePath, DirectX::SimpleMath::Matrix mat, bool isDynamic)
{
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertices;

	this->mGraphicsData.loadTexture(key, texturePath, device);
	this->mGraphicsData.setNrOfVertices(key, nrOfVertices);

	if (FAILED(this->mGraphicsData.createVertexBuffer(key, nrOfVertices * sizeof(EntityStruct::VertexStruct), &data, device)))
	{
		MessageBox(0, L"Entity vertex buffer creation failed", L"error", MB_OK);
		return false;
	}

	if (cbufferSize == sizeof(DirectX::XMFLOAT4X4))
	{
		data.pSysMem = &mat;
	}

	this->mGraphicsData.createConstantBuffer(key, cbufferSize, &data, device, isDynamic);

	return true;
}

void EntityRenderer::setShadowPass(bool value)
{
	this->shadowPass = value;
}

GraphicsData* EntityRenderer::getGraphicsData()
{
	return &this->mGraphicsData;
}
