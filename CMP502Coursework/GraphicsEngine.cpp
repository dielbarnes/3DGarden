//
// GraphicsEngine.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "GraphicsEngine.h"

#pragma region Init

GraphicsEngine::GraphicsEngine()
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
	m_pParticleSystem = nullptr;
	m_pAlphaEnabledBlendState = nullptr;
	m_pAlphaDisabledBlendState = nullptr;
}

GraphicsEngine::~GraphicsEngine()
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
	SAFE_DELETE(m_pParticleSystem)
	SAFE_DELETE(m_pAlphaEnabledBlendState)
	SAFE_DELETE(m_pAlphaDisabledBlendState)
}

bool GraphicsEngine::Initialize(int& iScreenWidth, int& iScreenHeight, HWND hWindow)
{
	// Initialize camera
	//XMFLOAT3 position = XMFLOAT3(0.0f, 3.7f, -12.0f); // Close up statue
	XMFLOAT3 position = XMFLOAT3(0.0f, 8.0f, -22.0f);
	float fAspectRatio = iScreenWidth / (FLOAT)iScreenHeight;
	m_pCamera = new Camera(position, fAspectRatio);

	// Create device, swap chain, render target view, depth stencil state, depth stencil view, and rasterizer state
	// Setup the viewport
	if (FAILED(InitDirect3D(iScreenWidth, iScreenHeight, hWindow)))
	{
		return false;
	}

	// Load textures and models
	m_pResourceManager = new ResourceManager(*m_pDevice, *m_pImmediateContext);
	if (!m_pResourceManager->LoadResources())
	{
		return false;
	}

	// Compile and create vertex and pixel shaders
	// Create vertex input layouts
	// Create constant buffers
	// Create the texture sampler state
	m_pShaderManager = new ShaderManager(*m_pDevice, *m_pImmediateContext);
	if (FAILED(m_pShaderManager->InitializeShaders()))
	{
		return false;
	}

	// Initialize the particle system
	m_pParticleSystem = new ParticleSystem();
	if (!m_pParticleSystem->Initialize(m_pDevice))
	{
		return false;
	}
	m_pParticleSystem->SetTexture(*m_pResourceManager->GetTexture(TextureResource::ParticleTexture));
	m_pParticleSystem->SetWorldMatrix(XMMatrixTranslation(0.0f, 5.5f, -7.5f));

	return true;
}

HRESULT GraphicsEngine::InitDirect3D(int& iScreenWidth, int& iScreenHeight, HWND hWindow)
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
		if (pDisplayModes[i].Width == (unsigned int)iScreenWidth && pDisplayModes[i].Height == (unsigned int)iScreenHeight)
		{
			uiNumerator = pDisplayModes[i].RefreshRate.Numerator;
			uiDenominator = pDisplayModes[i].RefreshRate.Denominator;
			break;
		}
	}

	// Create device and swap chain
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = iScreenWidth;
	swapChainDesc.BufferDesc.Height = iScreenHeight;
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

	// Create the render target view

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
	depthBufferDesc.Width = iScreenWidth;
	depthBufferDesc.Height = iScreenHeight;
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

	// Bind the render target view and the depth stencil view to the output merger stage
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
	viewport.Width = (FLOAT)iScreenWidth;
	viewport.Height = (FLOAT)iScreenHeight;
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

void GraphicsEngine::HandleKeyboardInput(const float& fDeltaT)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pCamera->MoveForward(fDeltaT, 5.0f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pCamera->MoveForward(fDeltaT, -5.0f);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pCamera->Strafe(fDeltaT, -5.0f);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pCamera->Strafe(fDeltaT, 5.0f);
	}
}

void GraphicsEngine::OnMouseDown(int x, int y, HWND hWindow)
{
	m_mousePosition.x = x;
	m_mousePosition.y = y;

	// Capture mouse input either when the mouse is over the capturing window, 
	// or when the mouse button was pressed while the mouse was over the capturing window and the button is still down
	SetCapture(hWindow);
}

void GraphicsEngine::OnMouseUp()
{
	// Release the mouse capture from a window in the current thread and restore normal mouse input processing
	ReleaseCapture();
}

void GraphicsEngine::OnMouseMove(WPARAM buttonState, int x, int y)
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

bool GraphicsEngine::Render(const float& fDeltaT, float fFrameTime)
{
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
	HandleKeyboardInput(fDeltaT);

	// Update camera
	m_pCamera->Update();

	// Create blend state
	D3D11_BLEND_DESC blendStateDesc = {};
	blendStateDesc.AlphaToCoverageEnable = TRUE;										 // Use alpha-to-coverage as a multisampling technique when setting a pixel to a render target
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;							 // Operation to perform on the RGB value that the pixel shader outputs
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				 // Operation to perform on the current RGB value in the render target
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;						 // How to combine the SrcBlend and DestBlend operations
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;						 // Operation to perform on the alpha value that the pixel shader outputs
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					 // Operation to perform on the current alpha value in the render target
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;					 // How to combine the SrcBlendAlpha and DestBlendAlpha operations
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // Identify which components of each pixel of a render target are writable during blending
	HRESULT result = m_pDevice->CreateBlendState(&blendStateDesc, &m_pAlphaEnabledBlendState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create alpha enabled blend state.", result);
		return false;
	}

	float blendFactor[4] = COLOR_F4(0.0f, 0.0f, 0.0f, 0.0f)
	UINT sampleMask = 0xffffffff;

	// Set the vertex and index buffers and the primitive topology
	m_pResourceManager->RenderModel(ModelResource::StatueModel);
	// Set the vertex input layout, constant buffers, texture, sampler state, and shaders
	// Draw
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::StatueModel), m_pCamera))
	{
		return false;
	}

	/*m_pResourceManager->RenderModel(ModelResource::LionModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::LionModel), m_pCamera))
	{
		return false;
	}*/

	/*m_pResourceManager->RenderModel(ModelResource::VaseModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::VaseModel), m_pCamera))
	{
		return false;
	}*/
	
	m_pResourceManager->RenderModel(ModelResource::PillarModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::PillarModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::FountainModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::FountainModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::LavenderModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::LavenderModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::GroundModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::GroundModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::HedgeModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::HedgeModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::HedgeModel2);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::HedgeModel2), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::BalustradeModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::BalustradeModel), m_pCamera))
	{
		return false;
	}

	m_pResourceManager->RenderModel(ModelResource::BalustradeModel2);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::BalustradeModel2), m_pCamera))
	{
		return false;
	}

	// Turn on alpha blending
	m_pImmediateContext->OMSetBlendState(
							m_pAlphaEnabledBlendState,
							blendFactor,				// Array of blend factors, one for each RGBA component; modulate values for the pixel shader, render target, or both
							sampleMask);				// Determines which samples get updated in all the active render targets

	m_pResourceManager->RenderModel(ModelResource::LupineModel);
	if (!m_pShaderManager->RenderModel(m_pResourceManager->GetModel(ModelResource::LupineModel), m_pCamera))
	{
		return false;
	}

	// Change the render target blend operation
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	result = m_pDevice->CreateBlendState(&blendStateDesc, &m_pAlphaEnabledBlendState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to change the render target blend operation.", result);
		return false;
	}
	m_pImmediateContext->OMSetBlendState(m_pAlphaEnabledBlendState, blendFactor, sampleMask);

	// Run the frame processing for the particle system
	m_pParticleSystem->Update(fFrameTime, m_pImmediateContext);
	// Render particles
	m_pParticleSystem->Render(m_pImmediateContext);
	if (!m_pShaderManager->RenderParticles(m_pParticleSystem, m_pCamera))
	{
		return false;
	}

	// Turn off alpha blending
	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
	result = m_pDevice->CreateBlendState(&blendStateDesc, &m_pAlphaDisabledBlendState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create alpha disabled blend state.", result);
		return false;
	}
	m_pImmediateContext->OMSetBlendState(m_pAlphaDisabledBlendState, blendFactor, sampleMask);

	// Present the back buffer to the front buffer
	m_pSwapChain->Present(
					0,	// Sync interval
					0);	// Present a frame from each buffer starting with the current buffer to the output

	return true;
}

#pragma endregion
