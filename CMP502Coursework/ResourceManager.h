//
// ResourceManager.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DDSTextureLoader.h"
#include <fstream>
#include <vector>
#include "Model.h"
#include "Utils.h"

enum TextureResource : int
{
	StatueTexture = 0,
	StoneTexture,
	LupineTexture,
	GroundTexture
};

enum ModelResource : int
{
	StatueModel = 0,
	PillarModel,
	FountainModel,
	LupineModel,
	GroundModel
};

class ResourceManager
{
public:
	ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ResourceManager();

	bool LoadResources();
	ID3D11ShaderResourceView* GetTexture(TextureResource resource);
	Model* GetModel(ModelResource resource);
	void RenderModel(ModelResource resource);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	std::vector<ID3D11ShaderResourceView*> m_textures;
	std::vector<Model*> m_models;

	HRESULT LoadTexture(TextureResource resource);
	bool LoadModel(ModelResource resource);
};

#endif
