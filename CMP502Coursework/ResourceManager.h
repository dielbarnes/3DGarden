//
// ResourceManager.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DDSTextureLoader.h"
#include <fstream>
#include <vector>
#include "SkyDome.h"
#include "SkyPlane.h"
#include "Utils.h"

enum TextureResource : int
{
	StatueTexture = 0,
	//LionTexture,
	StoneTexture,
	LupineTexture,
	LavenderTexture,
	GroundTexture,
	HedgeTexture,
	ParticleTexture,
	CloudTexture1,
	CloudTexture2
};

enum ModelResource : int
{
	StatueModel = 0,
	//LionModel,
	//VaseModel,
	PillarModel,
	FountainModel,
	LupineModel,
	LavenderModel,
	GroundModel,
	HedgeModel,
	HedgeModel2,
	BalustradeModel,
	BalustradeModel2,
	SkyDomeModel,	  // Not in models array
};

class ResourceManager
{
public:
	ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ResourceManager();

	bool LoadResources();
	ID3D11ShaderResourceView* GetTexture(TextureResource resource);
	Model* GetModel(ModelResource resource);
	SkyDome* GetSkyDome();
	SkyPlane* GetSkyPlane();
	void RenderModel(ModelResource resource);
	void RenderSkyPlane();

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	std::vector<ID3D11ShaderResourceView*> m_textures;
	std::vector<Model*> m_models;
	SkyDome *m_pSkyDome;
	SkyPlane *m_pSkyPlane;

	HRESULT LoadTexture(TextureResource resource);
	bool LoadModel(ModelResource resource);
};

#endif
