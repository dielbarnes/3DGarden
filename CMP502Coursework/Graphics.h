//
// Graphics.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "Camera.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "ParticleShader.h"
#include "ParticleSystem.h"
#include "Utils.h"

// Link necessary libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

using namespace DirectX;

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Initialize(int& rScreenWidth, int& rScreenHeight, HWND hWindow);
	void OnMouseDown(int x, int y, HWND hWindow);
	void OnMouseUp();
	void OnMouseMove(WPARAM buttonState, int x, int y);
	bool Render(const float& rDeltaT, float frameTime);

private:
	ID3D11Device* m_pDevice;
	IDXGISwapChain* m_pSwapChain;
	ID3D11DeviceContext* m_pImmediateContext;		// Performs rendering onto a buffer
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState* m_pRasterizerState;
	Camera* m_pCamera;
	POINT m_mousePosition;

	ResourceManager* m_pResourceManager;
	ShaderManager* m_pShaderManager;
	ParticleShader* m_ParticleShader;
	ParticleSystem* m_ParticleSystem;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	HRESULT InitDirect3D(int& rScreenWidth, int& rScreenHeight, HWND hWindow);
	void HandleKeyboardInput(const float& rDeltaT);
};

#endif
