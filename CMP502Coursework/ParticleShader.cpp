#include "ParticleShader.h"

ParticleShader::ParticleShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
}

ParticleShader::~ParticleShader()
{
	SAFE_RELEASE(m_sampleState)
	SAFE_RELEASE(m_matrixBuffer)
	SAFE_RELEASE(m_layout)
	SAFE_RELEASE(m_pixelShader)
	SAFE_RELEASE(m_vertexShader)
}

bool ParticleShader::Initialize(ID3D11Device* device)
{
	// Compile the vertex shader
	ID3DBlob* pCompiledVertexShader;
	HRESULT result = CompileShaderFromFile(L"Shaders/ParticleVertexShader.hlsl", "VS", "vs_5_0", &pCompiledVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile particle vertex shader.", result);
		return result;
	}

	const void* ppCompiledVertexShader = pCompiledVertexShader->GetBufferPointer();
	SIZE_T compiledVertexShaderSize = pCompiledVertexShader->GetBufferSize();

	// Create the vertex shader
	result = device->CreateVertexShader(ppCompiledVertexShader, compiledVertexShaderSize, nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create particle vertex shader.", result);
		return result;
	}

	// Compile the pixel shader
	ID3DBlob* pCompiledPixelShader;
	result = CompileShaderFromFile(L"Shaders/ParticlePixelShader.hlsl", "PS", "ps_5_0", &pCompiledPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile particle pixel shader.", result);
		return result;
	}

	const void* ppCompiledPixelShader = pCompiledPixelShader->GetBufferPointer();
	SIZE_T compiledPixelShaderSize = pCompiledPixelShader->GetBufferSize();

	// Create the pixel shader
	result = device->CreatePixelShader(ppCompiledPixelShader, compiledPixelShaderSize, nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create particle pixel shader.", result);
		return result;
	}

	// Create the vertex input layout description
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, ppCompiledVertexShader, compiledVertexShaderSize, &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
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

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Release
	SAFE_RELEASE(pCompiledPixelShader)
	SAFE_RELEASE(pCompiledVertexShader)

	return true;
}

bool ParticleShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Lock the constant buffer so it can be written to.
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	MatrixBuffer* dataPtr = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->mWorld = XMMatrixTranspose(worldMatrix);
	dataPtr->mView = XMMatrixTranspose(viewMatrix);
	dataPtr->mProjection = XMMatrixTranspose(projectionMatrix);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}

HRESULT ParticleShader::CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiledShader)
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
