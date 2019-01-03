//
// ShaderManager.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "ShaderManager.h"

#pragma region Init

ShaderManager::ShaderManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pDevice = &device;
	m_pImmediateContext = &immediateContext;
	m_pVertexShader = nullptr;
	m_pInstancedVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexInputLayout = nullptr;
	m_pInstancedVertexInputLayout = nullptr;
	m_pSamplerState = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pLightBuffer = nullptr;
}

ShaderManager::~ShaderManager()
{
	SAFE_RELEASE(m_pVertexShader)
	SAFE_RELEASE(m_pInstancedVertexShader)
	SAFE_RELEASE(m_pPixelShader)
	SAFE_RELEASE(m_pVertexInputLayout)
	SAFE_RELEASE(m_pInstancedVertexInputLayout)
	SAFE_RELEASE(m_pSamplerState)
	SAFE_RELEASE(m_pMatrixBuffer)
	SAFE_RELEASE(m_pLightBuffer)
}

HRESULT ShaderManager::Initialize()
{
	HRESULT result = S_OK;

	// Compile the vertex shader
	ID3DBlob* pCompiledVertexShader;
	result = CompileShaderFromFile(L"Shaders/VertexShader.hlsl", "VS", "vs_5_0", &pCompiledVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile vertex shader.", result);
		return result;
	}

	const void* ppCompiledVertexShader = pCompiledVertexShader->GetBufferPointer();
	SIZE_T compiledVertexShaderSize = pCompiledVertexShader->GetBufferSize();

	// Create the vertex shader
	result = m_pDevice->CreateVertexShader(
							ppCompiledVertexShader,
							compiledVertexShaderSize,
							nullptr,					// Class linkage interface for dynamic shader linking
							&m_pVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex shader.", result);
		return result;
	}

	// Compile the instanced vertex shader
	ID3DBlob* pCompiledInstancedVertexShader;
	result = CompileShaderFromFile(L"Shaders/InstancedVertexShader.hlsl", "IVS", "vs_5_0", &pCompiledInstancedVertexShader);
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

	// Compile the pixel shader
	ID3DBlob* pCompiledPixelShader;
	result = CompileShaderFromFile(L"Shaders/PixelShader.hlsl", "PS", "ps_5_0", &pCompiledPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile pixel shader.", result);
		return result;
	}

	const void* ppCompiledPixelShader = pCompiledPixelShader->GetBufferPointer();
	SIZE_T compiledPixelShaderSize = pCompiledPixelShader->GetBufferSize();

	// Create the pixel shader
	result = m_pDevice->CreatePixelShader(
							ppCompiledPixelShader,
							compiledPixelShaderSize,
							nullptr,					// Class linkage interface for dynamic shader linking
							&m_pPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create pixel shader.", result);
		return result;
	}

	// Create the vertex input layout (should be the same as Vertex struct)

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

	result = m_pDevice->CreateInputLayout(vertexInputDesc, uiElementCount, ppCompiledVertexShader, compiledVertexShaderSize, &m_pVertexInputLayout);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex input layout.", result);
		return result;
	}

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

	// Create the constant buffers
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(MatrixBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;						
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create matrix buffer.", result);
		return result;
	}

	bufferDesc.ByteWidth = sizeof(LightBuffer); 
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pLightBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create light buffer.", result);
		return result;
	}

	// Release
	SAFE_RELEASE(pCompiledPixelShader)
	SAFE_RELEASE(pCompiledInstancedVertexShader)
	SAFE_RELEASE(pCompiledVertexShader)

	return result;
}

HRESULT ShaderManager::CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiledShader)
{
	HRESULT result = S_OK;

	ID3DBlob* pError = nullptr;
	result = D3DCompileFromFile(
				filename,
				nullptr,										 // Array of shader macros
				nullptr,										 // Include interface the compiler will use if the shader contains #include
				entryPoint,										 // Name of the shader entry point function where shader execution begins
				target,											 // Target set of shader features / effect type
				D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, // Disallow deprecated syntax; insert debug information into the output code
				0,												 // Flags for compiling an effect instead of a shader
				ppCompiledShader,
				&pError);
	if (FAILED(result))
	{
		if (pError)
		{
			// Send a string to the debugger for display
			OutputDebugStringA(reinterpret_cast<const char*>(pError->GetBufferPointer()));
			pError->Release();
		}
		// else shader file is missing
	}

	return result;
}

#pragma endregion

#pragma region Render

bool ShaderManager::Render(Camera* pCamera, Model* pModel)
{
	// Set the vertex input layout
	if (pModel->GetInstanceCount() == 1)
	{
		m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);
	}
	else
	{
		m_pImmediateContext->IASetInputLayout(m_pInstancedVertexInputLayout);
	}

	// Update constant buffers

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Lock the matrix buffer so it can be written to
	HRESULT result = m_pImmediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the matrix buffer.", result);
		return false;
	}

	// Get a pointer to the matrix buffer data
	MatrixBuffer* matrixBufferData = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the matrix buffer
	matrixBufferData->mWorld = XMMatrixTranspose(pModel->GetWorldMatrix());
	matrixBufferData->mView = XMMatrixTranspose(pCamera->GetViewMatrix());
	matrixBufferData->mProjection = XMMatrixTranspose(pCamera->GetProjectionMatrix());

	// Unlock the matrix buffer
	m_pImmediateContext->Unmap(m_pMatrixBuffer, 0);

	// Lock the light buffer so it can be written to
	result = m_pImmediateContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the light buffer.", result);
		return false;
	}

	// Get a pointer to the light buffer data
	LightBuffer* lightBufferData = (LightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the light buffer
	lightBufferData->ambientColor = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	//lightBufferData->diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	lightBufferData->diffuseColor = XMFLOAT4(1.0f, 0.8f, 0.973f, 1.0f); // Light pink
	lightBufferData->direction = XMFLOAT3(0.0f, -0.8f, 0.5f);
	//lightBufferData->direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lightBufferData->padding = 0.0f;

	// Unlock the light buffer
	m_pImmediateContext->Unmap(m_pLightBuffer, 0);

	// Set the constant buffers to be used by the vertex shader
	m_pImmediateContext->VSSetConstantBuffers(
							0,					// Position of the constant buffer in the vertex shader
							1,					// Number of buffers to set
							&m_pMatrixBuffer);	// Array of constant buffers being given to the device

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

	// Set the texture to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, pModel->GetTexture());

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer* psConstantBuffers[1] = { m_pLightBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

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
