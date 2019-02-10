//
// ParticleShader.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 39: Particle Systems (http://www.rastertek.com/dx11tut39.html)
//

#include "ParticleShader.h"

#pragma region Init

ParticleShader::ParticleShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext) : Shader(device, immediateContext)
{
	m_pSamplerState = nullptr;
}

ParticleShader::~ParticleShader()
{
	SAFE_RELEASE(m_pSamplerState)
}

HRESULT ParticleShader::Initialize()
{
	HRESULT result = S_OK;

	// Compile and create the vertex shader
	// Compile and create the pixel shader
	// Create the vertex input layout
	// Create the matrix constant buffer

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	result = Shader::Initialize(L"Shaders/ParticleVertexShader.hlsl", "VS", L"Shaders/ParticlePixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	if (FAILED(result))
	{
		return result;
	}

	// Create the texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create texture sampler state.", result);
		return result;
	}

	return result;
}

#pragma endregion

#pragma region Render

bool ParticleShader::Render(ParticleSystem *pParticleSystem, Camera* pCamera)
{
	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(pParticleSystem->GetWorldMatrix(), pCamera);

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);

	// Set the texture to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, pParticleSystem->GetTexture());

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// Render triangles
	m_pImmediateContext->DrawIndexed(pParticleSystem->GetIndexCount(), 0, 0);

	return true;
}

#pragma endregion
