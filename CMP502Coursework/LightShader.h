//
// LightShader.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef LIGHT_SHADER_H
#define LIGHT_SHADER_H

#include "Shader.h"
#include "Model.h"

struct LightBuffer // For pixel shader
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 direction;
	float padding;
};

class LightShader : Shader
{
public:
	LightShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~LightShader();

	HRESULT Initialize();
	bool Render(Model* pModel, Camera* pCamera);

private:
	ID3D11VertexShader* m_pInstancedVertexShader;
	ID3D11InputLayout* m_pInstancedVertexInputLayout;
	ID3D11Buffer* m_pLightBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif
