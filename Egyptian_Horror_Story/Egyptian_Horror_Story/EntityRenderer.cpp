#include "EntityRenderer.h"
#define ENTITY_SHADER 20

EntityRenderer::EntityRenderer()
{
	this->shadowPass = false;
}

EntityRenderer::~EntityRenderer()
{
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
}

void EntityRenderer::render(ID3D11DeviceContext* context, ShaderHandler& shaderHandler)
{
	UINT stride = sizeof(EntityStruct::VertexStruct), offset = 0;

	shaderHandler.setShaders(context, ENTITY_SHADER, ENTITY_SHADER, ShaderHandler::UNBIND_SHADER);

	ID3D11Buffer* temp;

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

	context->IASetInputLayout(shaderHandler.getInputLayout(ENTITY_SHADER));
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int key = 0;

	for (auto const &item : *this->mGraphicsData.getVertexBufferMap()) 
	{
		key = item.first;

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

bool EntityRenderer::loadObject(ID3D11Device *device, int key, EntityStruct::VertexStruct* vertices, int nrOfVertices, UINT cbufferSize, wchar_t* texturePath, bool isDynamic)
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
		DirectX::SimpleMath::Matrix data2 = DirectX::SimpleMath::Matrix::Identity;
		data.pSysMem = &data2;
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
