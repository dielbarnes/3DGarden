//
// Model.h
// Copyright � 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
// Texturing and Lighting in DirectX 11 (https://www.3dgep.com/texturing-lighting-directx-11)
// RasterTek Tutorial 37: Instancing (http://www.rastertek.com/dx11tut37.html)
//

#ifndef MODEL_H
#define MODEL_H

#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;
	XMFLOAT3 normal;
};

struct Instance
{
	XMMATRIX worldMatrix;
};

struct ModelData
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

class Model
{
public:
	Model();
	~Model();

	bool InitializeBuffers(ID3D11Device* device, int iInstanceCount, Instance* instances = nullptr);
	void Render(ID3D11DeviceContext* immediateContext);

	void SetTexture(ID3D11ShaderResourceView &texture);
	ID3D11ShaderResourceView** GetTexture();
	void SetVertexCount(int iCount);
	void SetIndexCount(int iCount);
	int GetIndexCount();
	int GetInstanceCount();
	void SetModelData(ModelData* modelData);
	XMMATRIX GetWorldMatrix();
	void TransformWorldMatrix(XMMATRIX translationMatrix, XMMATRIX rotationMatrix, XMMATRIX scalingMatrix);
	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	void SetLightDirection(float x, float y, float z);
	XMFLOAT3 GetLightDirection();
	float GetSpecularPower();
	XMFLOAT4 GetSpecularColor();

protected:
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11Buffer* m_pVertexBuffer;
	int m_iVertexCount;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;
	ID3D11Buffer* m_pInstanceBuffer;
	int m_iInstanceCount;
	ModelData* m_modelData;
	XMMATRIX m_worldMatrix;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_lightDirection;
	float m_fSpecularPower;
	XMFLOAT4 m_specularColor;
};

#endif
