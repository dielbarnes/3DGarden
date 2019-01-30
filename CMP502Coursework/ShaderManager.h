//
// ShaderManager.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "LightShader.h"
#include "ParticleShader.h"
#include "SkyDomeShader.h"

class ShaderManager
{
public:
	ShaderManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ShaderManager();

	HRESULT InitializeShaders();
	bool RenderModel(Model* pModel, Camera* pCamera);
	bool RenderParticles(ParticleSystem *pParticleSystem, Camera* pCamera);
	bool RenderSkyDome(SkyDome *pSkyDome, Camera* pCamera);

private:
	LightShader* m_pLightShader;
	ParticleShader* m_pParticleShader;
	SkyDomeShader* m_pSkyDomeShader;
};

#endif
