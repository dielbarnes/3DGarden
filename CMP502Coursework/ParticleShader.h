#ifndef PARTICLESHADER_H
#define PARTICLESHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "ShaderManager.h"
#include "Utils.h"

using namespace DirectX;

class ParticleShader
{
public:
	ParticleShader();
	~ParticleShader();

	bool Initialize(ID3D11Device*);
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	HRESULT CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiledCode);
};

#endif
