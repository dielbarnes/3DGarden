//
// GraphicsEngine.h
// Copyright � 2018 Diel Barnes. All rights reserved.
//

#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "Camera.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "ParticleSystem.h"
#include "Utils.h"

// Link necessary libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

using namespace DirectX;

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Initialize(int& iScreenWidth, int& iScreenHeight, HWND hWindow);
	void OnMouseDown(int x, int y, HWND hWindow);
	void OnMouseUp();
	void OnMouseMove(WPARAM buttonState, int x, int y);
	bool Render(const float& fDeltaT, float fFrameTime);

private:
	ID3D11Device* m_pDevice;
	IDXGISwapChain* m_pSwapChain;
	ID3D11DeviceContext* m_pImmediateContext; // Performs rendering onto a buffer
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilState* m_pDepthDisabledStencilState;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState* m_pRasterizerState;
	ID3D11RasterizerState* m_pRasterizerStateNoCulling;
	Camera* m_pCamera;
	POINT m_mousePosition;
	ResourceManager* m_pResourceManager;
	ShaderManager* m_pShaderManager;
	ParticleSystem* m_pParticleSystem;
	ID3D11BlendState* m_pAlphaEnabledBlendState1; // Render target pre-blend operation inverts alpha data
	ID3D11BlendState* m_pAlphaEnabledBlendState2; // No render target pre-blend operation
	ID3D11BlendState* m_pAlphaDisabledBlendState;

	HRESULT InitDirect3D(int& iScreenWidth, int& iScreenHeight, HWND hWindow);
	void HandleKeyboardInput(const float& fDeltaT);
};

#endif
