//
// SkyPlane.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SKY_PLANE_H
#define SKY_PLANE_H

#include <math.h>
#include "Model.h"

struct SkyPlaneVertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;
};

class SkyPlane
{
public:
	SkyPlane();
	~SkyPlane();

	bool Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* immediateContext);

	void SetTexture1(ID3D11ShaderResourceView &texture);
	ID3D11ShaderResourceView** GetTexture1();
	void SetTexture2(ID3D11ShaderResourceView &texture);
	ID3D11ShaderResourceView** GetTexture2();
	int GetIndexCount();
	void SetWorldMatrix(XMMATRIX worldMatrix);
	XMMATRIX GetWorldMatrix();
	float GetTexture1TranslationX();
	float GetTexture1TranslationZ();
	float GetTexture2TranslationX();
	float GetTexture2TranslationZ();
	float GetBrightness();

private:
	ID3D11ShaderResourceView* m_pTexture1;
	ID3D11ShaderResourceView* m_pTexture2;
	ID3D11Buffer* m_pVertexBuffer;
	int m_iVertexCount;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;
	ModelData* m_modelData;
	XMMATRIX m_worldMatrix;
	XMFLOAT4 m_textureTranslation;
	XMFLOAT4 m_textureTranslationSpeed;
	float m_fBrightness;
};

#endif
