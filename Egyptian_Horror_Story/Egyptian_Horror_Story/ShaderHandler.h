#pragma once
#include "Direct3DHeader.h"
#include <map>

class ShaderHandler {
private:
	std::map<int, ID3D11VertexShader*> mVertexShaders;
	std::map<int, ID3D11GeometryShader*> mGeometryShaders;
	std::map<int, ID3D11PixelShader*> mPixelShaders;
	std::map<int, ID3D11InputLayout*> mInputLayouts;
public:
	ShaderHandler();
	ShaderHandler(ShaderHandler const &handler) = delete;
	~ShaderHandler();

	// input layout & vertex shader have same key
	HRESULT setupVertexShader(ID3D11Device *dev, int key,
		wchar_t *name, char *entryPoint, D3D11_INPUT_ELEMENT_DESC *desc = nullptr, UINT nrOfElements = 0);
	HRESULT setupPixelShader(ID3D11Device *dev, int key,
		wchar_t *name, char *entryPoint);
	HRESULT setupGeometryShader(ID3D11Device *dev, int key,
		wchar_t *name, char *entryPoint);

	ID3D11VertexShader* getVertexShader(int key);
	ID3D11GeometryShader* getGeometryShader(int key);
	ID3D11PixelShader* getPixelShader(int key);
	ID3D11InputLayout* getInputLayout(int key);

	ShaderHandler* operator=(ShaderHandler const &handler) = delete;
};