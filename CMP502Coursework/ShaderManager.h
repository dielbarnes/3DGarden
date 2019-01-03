//
// ShaderManager.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "Camera.h"
#include "Model.h"
#include "Utils.h"

// Link library
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

struct MatrixBuffer // For vertex shader
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct LightBuffer // For pixel shader
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 direction;
	float padding;
};

class ShaderManager
{
public:
	ShaderManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ShaderManager();

	HRESULT Initialize();
	bool Render(Camera* pCamera, Model* pModel);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11VertexShader* m_pInstancedVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pVertexInputLayout;
	ID3D11InputLayout* m_pInstancedVertexInputLayout;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pLightBuffer;

	HRESULT CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiledCode);
};

#endif
