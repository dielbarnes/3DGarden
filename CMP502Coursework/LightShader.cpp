//
// LightShader.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
//

#include "LightShader.h"

#pragma region Init

LightShader::LightShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext) : Shader(device, immediateContext)
{
	m_pInstancedVertexShader = nullptr;
	m_pInstancedVertexInputLayout = nullptr;
	m_pLightBuffer = nullptr;
	m_pSamplerState = nullptr;
}

LightShader::~LightShader()
{
	SAFE_RELEASE(m_pInstancedVertexShader)
	SAFE_RELEASE(m_pInstancedVertexInputLayout)
	SAFE_RELEASE(m_pLightBuffer)
	SAFE_RELEASE(m_pSamplerState)
}

HRESULT LightShader::Initialize()
{
	HRESULT result = S_OK;

	// Compile and create the vertex shader
	// Compile and create the pixel shader
	// Create the vertex input layout
	// Create the matrix constant buffer

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] =
	{
		{ "POSITION",					// Semantic name
		  0,							// Semantic index (only needed when there is more than one element with the same semantic)
		  DXGI_FORMAT_R32G32B32_FLOAT,	// 96-bit format that supports 32 bits per color channel
		  0,							// Input slot (index of vertex buffer the GPU should fetch ranging form 0 to 15)
		  0,							// Offset in bytes between each element (tells the GPU the memory location to start fetching the data for this element)
		  D3D11_INPUT_PER_VERTEX_DATA,  // Input classification
		  0 },							// Number of instances to draw using the same per-instance data before advancing in the buffer by one element (must be 0 for an element that contains per-vertex data)
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // D3D11_APPEND_ALIGNED_ELEMENT defines the current element directly after the previous one
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	result = Shader::Initialize(L"Shaders/LightVertexShader.hlsl", "VS", L"Shaders/LightPixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	if (FAILED(result))
	{
		return result;
	}

	// Compile the instanced vertex shader
	ID3DBlob* pCompiledInstancedVertexShader;
	result = CompileShaderFromFile(L"Shaders/LightInstancedVertexShader.hlsl", "IVS", "vs_5_0", &pCompiledInstancedVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile instanced vertex shader.", result);
		return result;
	}

	const void* ppCompiledInstancedVertexShader = pCompiledInstancedVertexShader->GetBufferPointer();
	SIZE_T compiledInstancedVertexShaderSize = pCompiledInstancedVertexShader->GetBufferSize();

	// Create the instanced vertex shader
	result = m_pDevice->CreateVertexShader(ppCompiledInstancedVertexShader, compiledInstancedVertexShaderSize, nullptr, &m_pInstancedVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create instanced vertex shader.", result);
		return result;
	}

	// Create the instanced vertex input layout

	D3D11_INPUT_ELEMENT_DESC instancedVertexInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	uiElementCount = ARRAYSIZE(instancedVertexInputDesc);

	result = m_pDevice->CreateInputLayout(instancedVertexInputDesc, uiElementCount, ppCompiledInstancedVertexShader, compiledInstancedVertexShaderSize, &m_pInstancedVertexInputLayout);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create instanced vertex input layout.", result);
		return result;
	}

	// Create the light constant buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(LightBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pLightBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create light buffer.", result);
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

	// Release
	SAFE_RELEASE(pCompiledInstancedVertexShader)

	return result;
}

#pragma endregion

#pragma region Render

bool LightShader::Render(Model* pModel, Camera* pCamera)
{
	HRESULT result = S_OK;

	// Set the vertex input layout
	if (pModel->GetInstanceCount() == 1)
	{
		m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);
	}
	else
	{
		m_pImmediateContext->IASetInputLayout(m_pInstancedVertexInputLayout);
	}

	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(pModel->GetWorldMatrix(), pCamera);

	// Set the vertex shader to the device
	if (pModel->GetInstanceCount() == 1)
	{
		m_pImmediateContext->VSSetShader(
								m_pVertexShader,
								nullptr,		// Array of class instance interfaces used by the vertex shader
								0);				// Number of class instance interfaces
	}
	else
	{
		m_pImmediateContext->VSSetShader(m_pInstancedVertexShader, nullptr, 0);
	}

	// Update the light constant buffer

	// Lock the light buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_pImmediateContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the light buffer.", result);
		return false;
	}

	// Get a pointer to the light buffer data
	LightBuffer* lightBufferData = (LightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the light buffer
	lightBufferData->ambientColor = pModel->GetAmbientColor();
	lightBufferData->diffuseColor = pModel->GetDiffuseColor();
	lightBufferData->direction = pModel->GetLightDirection();
	lightBufferData->padding = 0.0f;

	// Unlock the light buffer
	m_pImmediateContext->Unmap(m_pLightBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer* psConstantBuffers[1] = { m_pLightBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the texture to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, pModel->GetTexture());

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(
							m_pPixelShader,
							nullptr,		// Array of class instance interfaces used by the pixel shader 
							0);				// Number of class instance interfaces

	// Render triangles
	if (pModel->GetInstanceCount() == 1)
	{
		m_pImmediateContext->DrawIndexed(
								pModel->GetIndexCount(),
								0,							// Location of the first index read by the GPU from the index buffer
								0);							// Value added to each index before reading a vertex from the vertex buffer
	}
	else
	{
		m_pImmediateContext->DrawInstanced(pModel->GetIndexCount(), pModel->GetInstanceCount(), 0, 0);
	}

	return true;
}

#pragma endregion
