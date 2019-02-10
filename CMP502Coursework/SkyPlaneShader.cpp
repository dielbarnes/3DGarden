//
// SkyPlaneShader.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 11: Bitmap Clouds (http://www.rastertek.com/tertut11.html)
//

#include "SkyPlaneShader.h"

#pragma region Init

SkyPlaneShader::SkyPlaneShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext) : Shader(device, immediateContext)
{
	m_pCloudBuffer = nullptr;
	m_pSamplerState = nullptr;
}

SkyPlaneShader::~SkyPlaneShader()
{
	SAFE_RELEASE(m_pCloudBuffer)
	SAFE_RELEASE(m_pSamplerState)
}

HRESULT SkyPlaneShader::Initialize()
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
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	result = Shader::Initialize(L"Shaders/SkyPlaneVertexShader.hlsl", "VS", L"Shaders/SkyPlanePixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	if (FAILED(result))
	{
		return result;
	}

	// Create the cloud constant buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(CloudBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pCloudBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create cloud buffer.", result);
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
		return false;
	}

	return result;
}

#pragma endregion

#pragma region Render

bool SkyPlaneShader::Render(SkyPlane* pSkyPlane, Camera* pCamera)
{
	HRESULT result = S_OK;

	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(pSkyPlane->GetWorldMatrix(), pCamera);

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);

	// Update the cloud constant buffer

	// Lock the cloud buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_pImmediateContext->Map(m_pCloudBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the cloud buffer.", result);
		return false;
	}

	// Get a pointer to the cloud buffer data
	CloudBuffer* cloudBufferData = (CloudBuffer*)mappedResource.pData;

	// Copy the cloud variables into the cloud buffer
	cloudBufferData->texture1TranslationX = pSkyPlane->GetTexture1TranslationX();
	cloudBufferData->texture1TranslationZ = pSkyPlane->GetTexture1TranslationZ();
	cloudBufferData->texture2TranslationX = pSkyPlane->GetTexture2TranslationX();
	cloudBufferData->texture2TranslationZ = pSkyPlane->GetTexture2TranslationZ();
	cloudBufferData->brightness = pSkyPlane->GetBrightness();
	cloudBufferData->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Unlock the cloud buffer
	m_pImmediateContext->Unmap(m_pCloudBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer* psConstantBuffers[1] = { m_pCloudBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the textures to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, pSkyPlane->GetTexture1());
	m_pImmediateContext->PSSetShaderResources(1, 1, pSkyPlane->GetTexture2());

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// Render triangles
	m_pImmediateContext->DrawIndexed(pSkyPlane->GetIndexCount(), 0, 0);

	return true;
}

#pragma endregion
