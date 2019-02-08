//
// ShaderManager.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "ShaderManager.h"

#pragma region Init

ShaderManager::ShaderManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pLightShader = new LightShader(device, immediateContext);
	m_pParticleShader = new ParticleShader(device, immediateContext);
	m_pSkyDomeShader = new SkyDomeShader(device, immediateContext);
	m_pSkyPlaneShader = new SkyPlaneShader(device, immediateContext);
}

ShaderManager::~ShaderManager()
{
	SAFE_DELETE(m_pLightShader)
	SAFE_DELETE(m_pParticleShader)
	SAFE_DELETE(m_pSkyDomeShader)
	SAFE_DELETE(m_pSkyPlaneShader)
}

HRESULT ShaderManager::InitializeShaders()
{
	HRESULT result = m_pLightShader->Initialize();
	if (FAILED(result))
	{
		return result;
	}

	result = m_pParticleShader->Initialize();
	if (FAILED(result))
	{
		return result;
	}

	result = m_pSkyDomeShader->Initialize();
	if (FAILED(result))
	{
		return result;
	}

	result = m_pSkyPlaneShader->Initialize();
	return result;
}

#pragma endregion

#pragma region Render

bool ShaderManager::RenderModel(Model* pModel, Camera* pCamera)
{
	return m_pLightShader->Render(pModel, pCamera);
}

bool ShaderManager::RenderParticles(ParticleSystem *pParticleSystem, Camera* pCamera)
{
	return m_pParticleShader->Render(pParticleSystem, pCamera);
}

bool ShaderManager::RenderSkyDome(SkyDome *pSkyDome, Camera* pCamera)
{
	return m_pSkyDomeShader->Render(pSkyDome, pCamera);
}

bool ShaderManager::RenderSkyPlane(SkyPlane *pSkyPlane, Camera* pCamera)
{
	return m_pSkyPlaneShader->Render(pSkyPlane, pCamera);
}

#pragma endregion
