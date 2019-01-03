//
// ResourceManager.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "ResourceManager.h"

#pragma region Init

ResourceManager::ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pDevice = &device;
	m_pImmediateContext = &immediateContext;
}

ResourceManager::~ResourceManager()
{
	for (auto& texture : m_textures)
	{
		SAFE_RELEASE(texture);
	}
	for (auto& model : m_models)
	{
		SAFE_DELETE(model);
	}
}

bool ResourceManager::LoadResources()
{
	// Loading of resources should be in the same order as the enum

	// Statue

	HRESULT result = LoadTexture(TextureResource::StatueTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load statue texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::StatueModel))
	{
		MessageBox(0, "Failed to load statue model.", "", 0);
		return false;
	}

	m_models[ModelResource::StatueModel]->SetTexture(*m_textures[TextureResource::StatueTexture]);

	// Stone

	result = LoadTexture(TextureResource::StoneTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load stone texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::PillarModel))
	{
		MessageBox(0, "Failed to load pillar model.", "", 0);
		return false;
	}

	m_models[ModelResource::PillarModel]->SetTexture(*m_textures[TextureResource::StoneTexture]);

	if (!LoadModel(ModelResource::FountainModel))
	{
		MessageBox(0, "Failed to load fountain model.", "", 0);
		return false;
	}

	m_models[ModelResource::FountainModel]->SetTexture(*m_textures[TextureResource::StoneTexture]);

	// Lupine

	result = LoadTexture(TextureResource::LupineTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load lupine texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::LupineModel))
	{
		MessageBox(0, "Failed to load lupine model.", "", 0);
		return false;
	}

	m_models[ModelResource::LupineModel]->SetTexture(*m_textures[TextureResource::LupineTexture]);

	// Ground

	result = LoadTexture(TextureResource::GroundTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load ground texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::GroundModel))
	{
		MessageBox(0, "Failed to load ground model.", "", 0);
		return false;
	}

	m_models[ModelResource::GroundModel]->SetTexture(*m_textures[TextureResource::GroundTexture]);

	// Initialize the vertex, index, and instance buffers

	result = m_models[ModelResource::StatueModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the statue vertex and index buffers.", result);
		return false;
	}

	int iPillarsCount = 8;
	XMMATRIX mPillarRotation = XMMatrixRotationRollPitchYaw(XM_PI * 0.5f, XM_PI * 0.5f, XM_PI * 0.5f);
	XMMATRIX mPillarScaling = XMMatrixScaling(12.0f, 12.0f, 12.0f);
	Instance* pillarInstances = new Instance[iPillarsCount];
	pillarInstances[0].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.8f, -0.8f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[1].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.8f, -0.4f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[2].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.5f, -0.1f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[3].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.2f, 0.2f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[4].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.2f, 0.2f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[5].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.5f, -0.1f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[6].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.8f, -0.4f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[7].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.8f, -0.8f, 0.0f) * mPillarRotation * mPillarScaling);
	result = m_models[ModelResource::PillarModel]->InitializeBuffers(m_pDevice, iPillarsCount, pillarInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the pillar vertex and index buffers.", result);
		return false;
	}

	result = m_models[ModelResource::FountainModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the fountain vertex and index buffers.", result);
		return false;
	}

	int iLupineCount = 8;
	XMMATRIX mLupineScaling = XMMatrixScaling(0.017f, 0.017f, 0.017f);
	Instance* lupineInstances = new Instance[iLupineCount];
	lupineInstances[0].mWorld = XMMatrixTranspose(XMMatrixTranslation(-540.0f, 0.0f, -525.0f) * mLupineScaling);
	lupineInstances[1].mWorld = XMMatrixTranspose(XMMatrixTranslation(-540.0f, 0.0f, -250.0f) * mLupineScaling);
	lupineInstances[2].mWorld = XMMatrixTranspose(XMMatrixTranslation(-315.0f, 0.0f, -50.0f) * mLupineScaling);
	lupineInstances[3].mWorld = XMMatrixTranspose(XMMatrixTranslation(-100.0f, 0.0f, 155.0f) * mLupineScaling);
	lupineInstances[4].mWorld = XMMatrixTranspose(XMMatrixTranslation(100.0f, 0.0f, 155.0f) * mLupineScaling);
	lupineInstances[5].mWorld = XMMatrixTranspose(XMMatrixTranslation(315.0f, 0.0f, -50.0f) * mLupineScaling);
	lupineInstances[6].mWorld = XMMatrixTranspose(XMMatrixTranslation(540.0f, 0.0f, -250.0f) * mLupineScaling);
	lupineInstances[7].mWorld = XMMatrixTranspose(XMMatrixTranslation(540.0f, 0.0f, -525.0f) * mLupineScaling);
	result = m_models[ModelResource::LupineModel]->InitializeBuffers(m_pDevice, iLupineCount, lupineInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lupine vertex and index buffers.", result);
		return false;
	}

	result = m_models[ModelResource::GroundModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the fountain vertex and index buffers.", result);
		return false;
	}

	// Transform models

	//XMMATRIX mRotation = XMMatrixRotationRollPitchYaw(XM_PI * 0.5f, XM_PI * 0.5f, XM_PI * 0.5f);

	XMMATRIX mFountainTranslation = XMMatrixTranslation(0.0f, 125.0f, -375.0f);
	XMMATRIX mFountainScaling = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	m_models[ModelResource::FountainModel]->TransformWorldMatrix(mFountainTranslation, XMMatrixIdentity(), mFountainScaling);

	XMMATRIX mGroundScaling = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	m_models[ModelResource::GroundModel]->TransformWorldMatrix(XMMatrixIdentity(), XMMatrixIdentity(), mGroundScaling);

	return true;
}

HRESULT ResourceManager::LoadTexture(TextureResource resource)
{
	HRESULT result = S_OK;

	// Create texture
	ID3D11ShaderResourceView* texture;
	switch (resource)
	{
	case StatueTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/statue_d.dds", nullptr, &texture, 0, nullptr);
		break;
	case StoneTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/stone.dds", nullptr, &texture, 0, nullptr);
		break;
	case LupineTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/lupine.dds", nullptr, &texture, 0, nullptr);
		break;
	case GroundTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/grass.dds", nullptr, &texture, 0, nullptr);
		break;
	}
	if (FAILED(result))
	{
		return result;
	}

	// Store texture in array
	m_textures.push_back(texture);

	return result;
}

bool ResourceManager::LoadModel(ModelResource resource)
{
	// Create model
	Model *model = new Model();
	
	// Open file

	std::ifstream file;
	char input;

	switch (resource)
	{
	case StatueModel:
		file.open("Resources/statue.txt");
		break;
	case PillarModel:
		file.open("Resources/pillar.txt");
		break;
	case FountainModel:
		file.open("Resources/fountain.txt");
		break;
	case LupineModel:
		file.open("Resources/lupine.txt");
		break;
	case GroundModel:
		file.open("Resources/ground.txt");
		break;
	}
	if (file.fail())
	{
		return false;
	}

	// Read up to the value of the vertex count
	file.get(input);
	while (input != ':')
	{
		file.get(input);
	}

	// Read the vertex count
	int iVertexCount;
	file >> iVertexCount;
	model->SetVertexCount(iVertexCount);
	model->SetIndexCount(iVertexCount);

	// Read up to the beginning of the data
	file.get(input);
	while (input != ':')
	{
		file.get(input);
	}
	file.get(input);
	file.get(input);

	// Read the vertex data
	ModelData* modelData = new ModelData[iVertexCount];
	for (int i = 0; i < iVertexCount; i++)
	{
		file >> modelData[i].x >> modelData[i].y >> modelData[i].z;
		file >> modelData[i].tu >> modelData[i].tv;
		file >> modelData[i].nx >> modelData[i].ny >> modelData[i].nz;
	}
	model->SetModelData(modelData);

	// Close file
	file.close();

	// Store model in array
	m_models.push_back(model);

	return true;
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* ResourceManager::GetTexture(TextureResource resource)
{
	return m_textures[resource];
}

Model* ResourceManager::GetModel(ModelResource resource)
{
	return m_models[resource];
}

#pragma endregion

#pragma region Render

void ResourceManager::RenderModel(ModelResource resource)
{
	m_models[resource]->Render(m_pImmediateContext);
}

#pragma endregion
