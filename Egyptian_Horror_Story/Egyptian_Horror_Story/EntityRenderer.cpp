#include "EntityRenderer.h"

EntityRenderer::EntityRenderer()
{
}

EntityRenderer::~EntityRenderer()
{
}

void EntityRenderer::setup(ID3D11Device* device, ShaderHandler& shaderHandler)
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HRESULT hr = shaderHandler.setupVertexShader(device, 20, L"EntityVS.hlsl", "main", desc, ARRAYSIZE(desc));

	shaderHandler.setupPixelShader(device, 20, L"EntityPS.hlsl", "main");
}

void EntityRenderer::render(ID3D11DeviceContext* context, ShaderHandler& shaderHandler)
{
	shaderHandler.setShaders(context, 20, 20, -1);

	context->IASetInputLayout(shaderHandler.getInputLayout(20));
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	int key = 0;

	for (auto const &item : *this->mGraphicsData.getVertexMap()) 
	{
		key = item.first;

		ID3D11Buffer* temp = this->mGraphicsData.getBuffer(key);
		UINT stride = sizeof(EntityStruct::VertexStruct), offset = 0;

		context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);

		temp = mGraphicsData.getBuffer(300);
		context->PSSetConstantBuffers(0, 1, &temp);
		
		ID3D11ShaderResourceView* texTemp = this->mGraphicsData.getSRV(key);
		context->PSSetShaderResources(0, 1, &texTemp);

		context->Draw(this->mGraphicsData.getNrOfVertices(key), 0);
	}
}

bool EntityRenderer::loadObject(ID3D11Device* device, int key, EntityStruct::VertexStruct* vertices, int nrOfVertices, wchar_t* texturePath)
{
	this->mGraphicsData.createVerticeArray(key, vertices, nrOfVertices);

	

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = this->mGraphicsData.getVertices(key);

	this->mGraphicsData.loadTexture(key, texturePath, device);

	if (FAILED(this->mGraphicsData.createVertexBuffer(key, this->mGraphicsData.getNrOfVertices(key) * sizeof(EntityStruct::VertexStruct), &data, device)))
	{
		MessageBox(0, L"Entity vertex buffer creation failed", L"error", MB_OK);
		return false;
	}

	return true;
}

GraphicsData* EntityRenderer::getGraphicsData()
{
	return &this->mGraphicsData;
}