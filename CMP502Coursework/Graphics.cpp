//
// Graphics.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "Graphics.h"

#pragma region Init

Graphics::Graphics()
{
	m_pDevice = nullptr;
	m_pSwapChain = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilBuffer = nullptr;
	m_pDepthStencilState = nullptr;
	m_pDepthStencilView = nullptr;
	m_pRasterizerState = nullptr;
	m_pCamera = nullptr;
	m_pResourceManager = nullptr;
	m_pShaderManager = nullptr;
}

Graphics::~Graphics()
{
	SAFE_RELEASE(m_pDevice)
	SAFE_RELEASE(m_pSwapChain)
	SAFE_RELEASE(m_pImmediateContext)
	SAFE_RELEASE(m_pRenderTargetView)
	SAFE_RELEASE(m_pDepthStencilBuffer)
	SAFE_RELEASE(m_pDepthStencilState)
	SAFE_RELEASE(m_pDepthStencilView)
	SAFE_RELEASE(m_pRasterizerState)
	SAFE_DELETE(m_pCamera)
	SAFE_DELETE(m_pResourceManager)
	SAFE_DELETE(m_pShaderManager)
}

bool Graphics::Initialize(int& rScreenWidth, int& rScreenHeight, HWND hWindow)
{
	// Initialize camera
	//XMFLOAT3 position = XMFLOAT3(0.0f, 3.7f, -12.0f); // Close up statue
	XMFLOAT3 position = XMFLOAT3(0.0f, 8.0f, -22.0f);
	float aspectRatio = rScreenWidth / (FLOAT)rScreenHeight;
	m_pCamera = new Camera(position, aspectRatio);

	// Create device, swap chain, render target view, depth stencil view, and rasterizer state
	// Setup the viewport
	if (FAILED(InitDirect3D(rScreenWidth, rScreenHeight, hWindow)))
	{
		return false;
	}

	// Compile and create shaders
	m_pShaderManager = new ShaderManager(*m_pDevice, *m_pImmediateContext);
	if (FAILED(m_pShaderManager->Initialize()))
	{
		return false;
	}

	// Load textures and models
	m_pResourceManager = new ResourceManager(*m_pDevice, *m_pImmediateContext);
	if (!m_pResourceManager->LoadResources())
	{
		return false;
	}

	// Create the particle shader object
	m_ParticleShader = new ParticleShader;
	if (!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object
	if (!m_ParticleShader->Initialize(m_pDevice))
	{
		return false;
	}

	// Create the particle system object
	m_ParticleSystem = new ParticleSystem;
	if (!m_ParticleSystem)
	{
		return false;
	}

	// Initialize the particle system object
	if (!m_ParticleSystem->Initialize(m_pDevice, m_pImmediateContext, 0))
	{
		return false;
	}

	return true;
}

HRESULT Graphics::InitDirect3D(int& rScreenWidth, int& rScreenHeight, HWND hWindow)
{
	HRESULT result = S_OK;

	// Create a DXGI factory
	IDXGIFactory* pFactory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create DXGI factory.", result);
		return result;
	}

	// Get the first available display adapter
	IDXGIAdapter* pAdapter;
	result = pFactory->EnumAdapters(
							0,			// Index of the adapter to enumerate
							&pAdapter);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to get display adapter.", result);
		return result;
	}

	// Get the first available display output
	IDXGIOutput* pOutput;
	result = pAdapter->EnumOutputs(
							0,			// Index of the output 
							&pOutput);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to get display output.", result);
		return result;
	}

	// Get the display modes

	unsigned int uiDisplayModeCount;
	DXGI_MODE_DESC* pDisplayModes;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use 32-bit display format
	
	result = pOutput->GetDisplayModeList(format, 0, &uiDisplayModeCount, nullptr); // Get the number of display modes that fit the format
	if (FAILED(result))
	{
		Utils::ShowError("Failed to get the number of display modes.", result);
		return result;
	}

	pDisplayModes = new DXGI_MODE_DESC[uiDisplayModeCount];

	result = pOutput->GetDisplayModeList(
							format, 
							0,					// Display mode options 
							&uiDisplayModeCount, 
							pDisplayModes);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to get display modes.", result);
		return result;
	}

	// Get the refresh rate

	unsigned int uiNumerator = 60;
	unsigned int uiDenominator = 1;
	
	for (unsigned int i = 0; i < uiDisplayModeCount; i++) // Find the display mode that matches the screen width and height
	{
		if (pDisplayModes[i].Width == (unsigned int)rScreenWidth && pDisplayModes[i].Height == (unsigned int)rScreenHeight)
		{
			uiNumerator = pDisplayModes[i].RefreshRate.Numerator;
			uiDenominator = pDisplayModes[i].RefreshRate.Denominator;
			break;
		}
	}

	// Create device & swap chain
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = rScreenWidth;
	swapChainDesc.BufferDesc.Height = rScreenHeight;
	swapChainDesc.BufferDesc.Format = format;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = uiNumerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = uiDenominator;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Surface usage and CPU access options for the back buffer
	swapChainDesc.OutputWindow = hWindow;
	swapChainDesc.SampleDesc.Count = 1;								// Number of multisamples per pixel
	swapChainDesc.SampleDesc.Quality = 0;							// Image quality level
	swapChainDesc.Windowed = true;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,  // Target features supported by Direct3D 11.0
	};

	result = D3D11CreateDeviceAndSwapChain(
				nullptr,					// Use the default adapter (first adapter enumerated by IDXGIFactory::EnumAdapters)
				D3D_DRIVER_TYPE_HARDWARE,	// Implement Direct3D features in hardware for best performance
				nullptr,					// Software for D3D_DRIVER_TYPE_SOFTWARE
				D3D11_CREATE_DEVICE_DEBUG,	// Runtime layer flags
				featureLevels,
				1,							// Number of feature levels
				D3D11_SDK_VERSION,
				&swapChainDesc,
				&m_pSwapChain,
				&m_pDevice,
				nullptr,					// Don't need to determine which feature level is supported
				&m_pImmediateContext);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create device and swap chain.", result);
		return result;
	}

	// Create a render target view

	ID3D11Texture2D* pBackBuffer;
	result = m_pSwapChain->GetBuffer(
								0,										// Buffer index
								__uuidof(ID3D11Texture2D), 
								reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(result))
	{
		Utils::ShowError("Failed to get back buffer.", result);
		return result;
	}

	result = m_pDevice->CreateRenderTargetView(
							pBackBuffer,
							nullptr,				// Render target view descriptor
							&m_pRenderTargetView);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create render target view.", result);
		return result;
	}

	// Create the depth stencil buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = rScreenWidth;
	depthBufferDesc.Height = rScreenHeight;
	depthBufferDesc.MipLevels = 1;							// Maximum number of mipmap levels in the texture; use 1 for a multisampled texture
	depthBufferDesc.ArraySize = 1;							// Number of textures in the texture array
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil
	depthBufferDesc.SampleDesc.Count = 1;					// Number of multisamples per pixel
	depthBufferDesc.SampleDesc.Quality = 0;					// Image quality level
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;			// The texture is to be read from and written to by the GPU
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Bind the texture as a depth-stencil target for the output-merger stage
	depthBufferDesc.CPUAccessFlags = 0;						// CPU access is not required
	depthBufferDesc.MiscFlags = 0;							// Other resource options
	result = m_pDevice->CreateTexture2D(
							&depthBufferDesc, 
							nullptr,					// Subresources for the texture
							&m_pDepthStencilBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create depth stencil buffer.", result);
		return result;
	}

	// Create the depth stencil state

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create depth stencil state.", result);
		return false;
	}

	// Set the depth stencil state
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
	depthViewDesc.Format = depthBufferDesc.Format;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // The resource should be interpreted as a 2D texture
	depthViewDesc.Texture2D.MipSlice = 0; // Use only the first mipmap level of the render target
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create depth stencil view.", result);
		return result;
	}

	// Bind the render target and the depth stencil buffer to the output merger stage
	m_pImmediateContext->OMSetRenderTargets(
							1,						// Number of render targets to bind
							&m_pRenderTargetView, 
							m_pDepthStencilView);

	// Create the rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {}; // Determines how and what polygons will be drawn
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	result = m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create rasterizer state.", result);
		return false;
	}

	// Set the rasterizer state
	m_pImmediateContext->RSSetState(m_pRasterizerState);

	// Setup the viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (FLOAT)rScreenWidth;
	viewport.Height = (FLOAT)rScreenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(
							1,			// Number of viewports to bind
							&viewport);

	// Release
	SAFE_RELEASE(pBackBuffer)
	SAFE_DELETE_ARRAY(pDisplayModes)
	SAFE_RELEASE(pOutput)
	SAFE_RELEASE(pAdapter)
	SAFE_RELEASE(pFactory)

	return result;
}

#pragma endregion

#pragma region Input Handling

void Graphics::HandleKeyboardInput(const float& rDeltaT)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pCamera->MoveForward(rDeltaT, 5.0f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pCamera->MoveForward(rDeltaT, -5.0f);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pCamera->Strafe(rDeltaT, -5.0f);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pCamera->Strafe(rDeltaT, 5.0f);
	}
}

void Graphics::OnMouseDown(int x, int y, HWND hWindow)
{
	m_mousePosition.x = x;
	m_mousePosition.y = y;

	// Capture mouse input either when the mouse is over the capturing window, 
	// or when the mouse button was pressed while the mouse was over the capturing window and the button is still down
	SetCapture(hWindow);
}

void Graphics::OnMouseUp()
{
	// Release the mouse capture from a window in the current thread and restore normal mouse input processing
	ReleaseCapture();
}

void Graphics::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if ((buttonState & MK_LBUTTON) != 0)
	{
		float deltaX = float(x - m_mousePosition.x);
		float deltaY = float(y - m_mousePosition.y);
		m_pCamera->Rotate(deltaX / 5.0f, deltaY / 5.0f);
	}

	m_mousePosition.x = x;
	m_mousePosition.y = y;
}

#pragma endregion

#pragma region Render

bool Graphics::Render(const float& rDeltaT, float frameTime)
{
	// Run the frame processing for the particle system
	m_ParticleSystem->Update(frameTime, m_pImmediateContext);

	// Clear the back buffer
	float color[4] = COLOR_F4(200.0f, 180.0f, 180.0f, 1.0f) // Background color
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// Clear the depth buffer to 1.0 (max depth)
	m_pImmediateContext->ClearDepthStencilView(
							m_pDepthStencilView,
							D3D11_CLEAR_DEPTH,	// Specifies the parts of the depth stencil to clear
							1.0f,				// Clear the depth buffer with this value
							0);					// Clear the stencil buffer with this value

	// Handle keyboard input
	HandleKeyboardInput(rDeltaT);

	// Update camera
	m_pCamera->Update();

	// Set the vertex and index buffers and the primitive topology
	m_pResourceManager->RenderModel(ModelResource::StatueModel);
	// Set the constant buffers and shaders, then draw
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::StatueModel)))
	{
		return false;
	}

	/*m_pResourceManager->RenderModel(ModelResource::VaseModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::VaseModel)))
	{
		return false;
	}*/
	
	m_pResourceManager->RenderModel(ModelResource::PillarModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::PillarModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::FountainModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::FountainModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::LavenderModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::LavenderModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::GroundModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::GroundModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::HedgeModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::HedgeModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::HedgeModel2);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::HedgeModel2)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::BalustradeModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::BalustradeModel)))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::BalustradeModel2);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::BalustradeModel2)))
	{
		return false;
	}




	// Turn on alpha blending.
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Create an alpha enabled blend state description.
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.AlphaToCoverageEnable = TRUE;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	if (FAILED(m_pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState)))
	{
		return false;
	}

	m_pImmediateContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);


	m_pResourceManager->RenderModel(ModelResource::LupineModel);
	if (!m_pShaderManager->Render(m_pCamera, m_pResourceManager->GetModel(ModelResource::LupineModel)))
	{
		return false;
	}


	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	if (FAILED(m_pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState)))
	{
		return false;
	}

	m_pImmediateContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);


	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->Render(m_pImmediateContext);

	// Render the model using the texture shader.
	XMMATRIX mParticleWorld = XMMatrixIdentity();
	mParticleWorld = mParticleWorld * XMMatrixTranslation(0.0f, 5.5f, -7.5f);
	if (!m_ParticleShader->Render(m_pImmediateContext, m_ParticleSystem->GetIndexCount(), mParticleWorld, m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(),
		m_ParticleSystem->GetTexture()))
	{
		return false;
	}


	// Turn off the alpha blending.
	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	if (FAILED(m_pDevice->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState)))
	{
		return false;
	}

	m_pImmediateContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);




	// Present the back buffer to the front buffer
	m_pSwapChain->Present(
					0,	// Sync interval
					0);	// Present a frame from each buffer starting with the current buffer to the output

	return true;
}

#pragma endregion
