//
// SkyDome.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SKY_DOME_H
#define SKY_DOME_H

#include "Model.h"

struct SkyDomeVertex
{
	XMFLOAT3 position;
};

class SkyDome : public Model
{
public:
	SkyDome();
	~SkyDome();

	bool InitializeBuffers(ID3D11Device* device, int iInstanceCount, Instance* instances = nullptr) override;

	void SetApexColor(XMFLOAT4 apexColor);
	XMFLOAT4 GetApexColor();
	void SetCenterColor(XMFLOAT4 centerColor);
	XMFLOAT4 GetCenterColor();

private:
	XMFLOAT4 m_apexColor;
	XMFLOAT4 m_centerColor;
};

#endif
