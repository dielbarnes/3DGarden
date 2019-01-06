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

	// Stone (pillar & fountain)

	result = LoadTexture(TextureResource::StoneTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load stone texture.", result);
		return false;
	}

	/*if (!LoadModel(ModelResource::VaseModel))
	{
		MessageBox(0, "Failed to load vase model.", "", 0);
		return false;
	}

	m_models[ModelResource::VaseModel]->SetTexture(*m_textures[TextureResource::StoneTexture]);*/

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

	// Lavender

	result = LoadTexture(TextureResource::LavenderTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load lavender texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::LavenerModel))
	{
		MessageBox(0, "Failed to load lavender model.", "", 0);
		return false;
	}

	m_models[ModelResource::LavenerModel]->SetTexture(*m_textures[TextureResource::LavenderTexture]);

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

	/*result = m_models[ModelResource::VaseModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the vase vertex and index buffers.", result);
		return false;
	}*/

	int iPillarsCount = 8;
	XMMATRIX mPillarRotation = XMMatrixRotationRollPitchYaw(XM_PI * 0.5f, XM_PI * 0.5f, XM_PI * 0.5f);
	XMMATRIX mPillarScaling = XMMatrixScaling(12.0f, 12.0f, 12.0f);
	Instance* pillarInstances = new Instance[iPillarsCount];
	pillarInstances[0].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.8f, -0.8f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[1].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.75f, -0.4f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[2].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.55f, -0.07f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[3].mWorld = XMMatrixTranspose(XMMatrixTranslation(-0.225f, 0.2f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[4].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.225f, 0.2f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[5].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.55f, -0.07f, 0.0f) * mPillarRotation * mPillarScaling);
	pillarInstances[6].mWorld = XMMatrixTranspose(XMMatrixTranslation(0.75f, -0.4f, 0.0f) * mPillarRotation * mPillarScaling);
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

	int iLupineCount = 16;
	XMMATRIX mLupineScaling = XMMatrixScaling(0.017f, 0.017f, 0.017f);
	Instance* lupineInstances = new Instance[iLupineCount];
	lupineInstances[0].mWorld = XMMatrixTranspose(XMMatrixTranslation(-155.0f, 0.0f, -330.0f) * mLupineScaling);
	lupineInstances[1].mWorld = XMMatrixTranspose(XMMatrixTranslation(-100.0f, 0.0f, -280.0f) * mLupineScaling);
	lupineInstances[2].mWorld = XMMatrixTranspose(XMMatrixTranslation(-35.0f, 0.0f, -255.0f) * mLupineScaling);
	lupineInstances[3].mWorld = XMMatrixTranspose(XMMatrixTranslation(35.0f, 0.0f, -255.0f) * mLupineScaling);
	lupineInstances[4].mWorld = XMMatrixTranspose(XMMatrixTranslation(100.0f, 0.0f, -280.0f) * mLupineScaling);
	lupineInstances[5].mWorld = XMMatrixTranspose(XMMatrixTranslation(155.0f, 0.0f, -330.0f) * mLupineScaling);
	lupineInstances[6].mWorld = XMMatrixTranspose(XMMatrixTranslation(-155.0f, 0.0f, -555.0f) * mLupineScaling);
	lupineInstances[7].mWorld = XMMatrixTranspose(XMMatrixTranslation(-100.0f, 0.0f, -605.0f) * mLupineScaling);
	lupineInstances[8].mWorld = XMMatrixTranspose(XMMatrixTranslation(-35.0f, 0.0f, -630.0f) * mLupineScaling);
	lupineInstances[9].mWorld = XMMatrixTranspose(XMMatrixTranslation(35.0f, 0.0f, -630.0f) * mLupineScaling);
	lupineInstances[10].mWorld = XMMatrixTranspose(XMMatrixTranslation(100.0f, 0.0f, -605.0f) * mLupineScaling);
	lupineInstances[11].mWorld = XMMatrixTranspose(XMMatrixTranslation(155.0f, 0.0f, -555.0f) * mLupineScaling);
	lupineInstances[12].mWorld = XMMatrixTranspose(XMMatrixTranslation(-180.0f, 0.0f, -405.0f) * mLupineScaling);
	lupineInstances[13].mWorld = XMMatrixTranspose(XMMatrixTranslation(-180.0f, 0.0f, -485.0f) * mLupineScaling);
	lupineInstances[14].mWorld = XMMatrixTranspose(XMMatrixTranslation(180.0f, 0.0f, -405.0f) * mLupineScaling);
	lupineInstances[15].mWorld = XMMatrixTranspose(XMMatrixTranslation(180.0f, 0.0f, -485.0f) * mLupineScaling);
	result = m_models[ModelResource::LupineModel]->InitializeBuffers(m_pDevice, iLupineCount, lupineInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lupine vertex and index buffers.", result);
		return false;
	}

	int iLavenderCount = 27;
	XMMATRIX mLavenderScaling = XMMatrixScaling(0.005f, 0.006f, 0.006f);
	Instance* lavenderInstances = new Instance[iLavenderCount];
	lavenderInstances[0].mWorld = XMMatrixTranspose(XMMatrixTranslation(30.0f, -250.0f, 0.0f) * mLavenderScaling);
	lavenderInstances[1].mWorld = XMMatrixTranspose(XMMatrixTranslation(-100.0f, -250.0f, 0.0f) * mLavenderScaling);
	lavenderInstances[2].mWorld = XMMatrixTranspose(XMMatrixTranslation(100.0f, -250.0f, 0.0f) * mLavenderScaling);
	lavenderInstances[3].mWorld = XMMatrixTranspose(XMMatrixTranslation(-500.0f, -250.0f, 400.0f) * mLavenderScaling);
	lavenderInstances[4].mWorld = XMMatrixTranspose(XMMatrixTranslation(-600.0f, -250.0f, 500.0f) * mLavenderScaling);
	lavenderInstances[5].mWorld = XMMatrixTranspose(XMMatrixTranslation(-700.0f, -250.0f, 500.0f) * mLavenderScaling);
	lavenderInstances[6].mWorld = XMMatrixTranspose(XMMatrixTranslation(500.0f, -250.0f, 400.0f) * mLavenderScaling);
	lavenderInstances[7].mWorld = XMMatrixTranspose(XMMatrixTranslation(600.0f, -250.0f, 500.0f) * mLavenderScaling);
	lavenderInstances[8].mWorld = XMMatrixTranspose(XMMatrixTranslation(700.0f, -250.0f, 500.0f) * mLavenderScaling);
	lavenderInstances[9].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1250.0f, -250.0f, -150.0f) * mLavenderScaling);
	lavenderInstances[10].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1350.0f, -250.0f, -50.0f) * mLavenderScaling);
	lavenderInstances[11].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1450.0f, -250.0f, -50.0f) * mLavenderScaling);
	lavenderInstances[12].mWorld = XMMatrixTranspose(XMMatrixTranslation(1250.0f, -250.0f, -150.0f) * mLavenderScaling);
	lavenderInstances[13].mWorld = XMMatrixTranspose(XMMatrixTranslation(1350.0f, -250.0f, -50.0f) * mLavenderScaling);
	lavenderInstances[14].mWorld = XMMatrixTranspose(XMMatrixTranslation(1450.0f, -250.0f, -50.0f) * mLavenderScaling);
	lavenderInstances[15].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1750.0f, -250.0f, -850.0f) * mLavenderScaling);
	lavenderInstances[16].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1850.0f, -250.0f, -750.0f) * mLavenderScaling);
	lavenderInstances[17].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1950.0f, -250.0f, -750.0f) * mLavenderScaling);
	lavenderInstances[18].mWorld = XMMatrixTranspose(XMMatrixTranslation(1750.0f, -250.0f, -850.0f) * mLavenderScaling);
	lavenderInstances[19].mWorld = XMMatrixTranspose(XMMatrixTranslation(1850.0f, -250.0f, -750.0f) * mLavenderScaling);
	lavenderInstances[20].mWorld = XMMatrixTranspose(XMMatrixTranslation(1950.0f, -250.0f, -750.0f) * mLavenderScaling);
	lavenderInstances[21].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1850.0f, -250.0f, -1650.0f) * mLavenderScaling);
	lavenderInstances[22].mWorld = XMMatrixTranspose(XMMatrixTranslation(-1950.0f, -250.0f, -1550.0f) * mLavenderScaling);
	lavenderInstances[23].mWorld = XMMatrixTranspose(XMMatrixTranslation(-2050.0f, -250.0f, -1550.0f) * mLavenderScaling);
	lavenderInstances[24].mWorld = XMMatrixTranspose(XMMatrixTranslation(1850.0f, -250.0f, -1650.0f) * mLavenderScaling);
	lavenderInstances[25].mWorld = XMMatrixTranspose(XMMatrixTranslation(1950.0f, -250.0f, -1550.0f) * mLavenderScaling);
	lavenderInstances[26].mWorld = XMMatrixTranspose(XMMatrixTranslation(2050.0f, -250.0f, -1550.0f) * mLavenderScaling);
	result = m_models[ModelResource::LavenerModel]->InitializeBuffers(m_pDevice, iLavenderCount, lavenderInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lavender vertex and index buffers.", result);
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

	/*XMMATRIX mVaseTranslation = XMMatrixTranslation(-5.0f, 0.0f, 0.0f);
	XMMATRIX mVaseScaling = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	m_models[ModelResource::VaseModel]->TransformWorldMatrix(mVaseTranslation, XMMatrixIdentity(), mVaseScaling);*/

	XMMATRIX mFountainTranslation = XMMatrixTranslation(3.0f, 125.0f, -375.0f);
	XMMATRIX mFountainScaling = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	m_models[ModelResource::FountainModel]->TransformWorldMatrix(mFountainTranslation, XMMatrixIdentity(), mFountainScaling);

	XMMATRIX mGroundTranslation = XMMatrixTranslation(0.0f, 0.0f, -5.0f);
	XMMATRIX mGroundScaling = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	m_models[ModelResource::GroundModel]->TransformWorldMatrix(mGroundTranslation, XMMatrixIdentity(), mGroundScaling);

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
	case LavenderTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/lavender.dds", nullptr, &texture, 0, nullptr);
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
	/*case VaseModel:
		file.open("Resources/vase.txt");
		break;*/
	case PillarModel:
		file.open("Resources/pillar.txt");
		break;
	case FountainModel:
		file.open("Resources/fountain.txt");
		break;
	case LupineModel:
		file.open("Resources/lupine.txt");
		break;
	case LavenerModel:
		file.open("Resources/lavender.txt");
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
