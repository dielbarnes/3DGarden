//
// SkyPlaneShader.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SKY_PLANE_SHADER_H
#define SKY_PLANE_SHADER_H

#include "Shader.h"
#include "SkyPlane.h"

struct CloudBuffer // For pixel shader
{
	float texture1TranslationX;
	float texture1TranslationZ;
	float texture2TranslationX;
	float texture2TranslationZ;
	float brightness;
	XMFLOAT3 padding;
};

class SkyPlaneShader : public Shader
{
public:
	SkyPlaneShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~SkyPlaneShader();

	HRESULT Initialize();
	bool Render(SkyPlane* pSkyPlane, Camera* pCamera);

private:
	ID3D11Buffer* m_pCloudBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif
