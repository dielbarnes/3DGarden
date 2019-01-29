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

	// Lion

	/*result = LoadTexture(TextureResource::LionTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load lion texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::LionModel))
	{
		MessageBox(0, "Failed to load lion model.", "", 0);
		return false;
	}

	m_models[ModelResource::LionModel]->SetTexture(*m_textures[TextureResource::LionTexture]);*/

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

	if (!LoadModel(ModelResource::LavenderModel))
	{
		MessageBox(0, "Failed to load lavender model.", "", 0);
		return false;
	}

	m_models[ModelResource::LavenderModel]->SetTexture(*m_textures[TextureResource::LavenderTexture]);

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

	// Hedge

	result = LoadTexture(TextureResource::HedgeTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load wall texture.", result);
		return false;
	}

	if (!LoadModel(ModelResource::HedgeModel))
	{
		MessageBox(0, "Failed to load wall model.", "", 0);
		return false;
	}

	m_models[ModelResource::HedgeModel]->SetTexture(*m_textures[TextureResource::HedgeTexture]);
	m_models[ModelResource::HedgeModel]->SetLightDirection(-0.5f, -0.8f, 0.5f);

	Model *hedgeModel2 = new Model();
	*hedgeModel2 = *m_models[ModelResource::HedgeModel];
	m_models.push_back(hedgeModel2);

	m_models[ModelResource::HedgeModel2]->SetTexture(*m_textures[TextureResource::HedgeTexture]);
	m_models[ModelResource::HedgeModel2]->SetLightDirection(0.5f, -0.8f, 0.5f);

	// Balustrade

	if (!LoadModel(ModelResource::BalustradeModel))
	{
		MessageBox(0, "Failed to load balustrade model.", "", 0);
		return false;
	}

	m_models[ModelResource::BalustradeModel]->SetTexture(*m_textures[TextureResource::StoneTexture]);
	m_models[ModelResource::BalustradeModel]->SetLightDirection(-0.3f, -0.8f, 0.5f);

	Model *balustradeModel2 = new Model();
	*balustradeModel2 = *m_models[ModelResource::BalustradeModel];
	m_models.push_back(balustradeModel2);

	m_models[ModelResource::BalustradeModel2]->SetTexture(*m_textures[TextureResource::StoneTexture]);
	m_models[ModelResource::BalustradeModel2]->SetLightDirection(0.3f, -0.8f, 0.5f);

	// Particle

	result = LoadTexture(TextureResource::ParticleTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load particle texture.", result);
		return false;
	}

	// Initialize the vertex, index, and instance buffers

	result = m_models[ModelResource::StatueModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the statue vertex and index buffers.", result);
		return false;
	}

	/*result = m_models[ModelResource::LionModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lion vertex and index buffers.", result);
		return false;
	}*/

	/*result = m_models[ModelResource::VaseModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the vase vertex and index buffers.", result);
		return false;
	}*/

	int iPillarsCount = 8;
	XMMATRIX pillarRotationMatrix = XMMatrixRotationRollPitchYaw(XM_PI * 0.5f, XM_PI * 0.5f, XM_PI * 0.5f);
	XMMATRIX pillarScalingMatrix = XMMatrixScaling(12.0f, 12.0f, 12.0f);
	Instance* pillarInstances = new Instance[iPillarsCount];
	pillarInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.8f, -0.8f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.75f, -0.4f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.55f, -0.07f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.225f, 0.2f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.225f, 0.2f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.55f, -0.07f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.75f, -0.4f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
	pillarInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.8f, -0.8f, 0.0f) * pillarRotationMatrix * pillarScalingMatrix);
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
	XMMATRIX lupineScalingMatrix = XMMatrixScaling(0.017f, 0.017f, 0.017f);
	Instance* lupineInstances = new Instance[iLupineCount];
	lupineInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-155.0f, 0.0f, -330.0f) * lupineScalingMatrix);
	lupineInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-100.0f, 0.0f, -280.0f) * lupineScalingMatrix);
	lupineInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-35.0f, 0.0f, -255.0f) * lupineScalingMatrix);
	lupineInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(35.0f, 0.0f, -255.0f) * lupineScalingMatrix);
	lupineInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(100.0f, 0.0f, -280.0f) * lupineScalingMatrix);
	lupineInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(155.0f, 0.0f, -330.0f) * lupineScalingMatrix);
	lupineInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-155.0f, 0.0f, -555.0f) * lupineScalingMatrix);
	lupineInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-100.0f, 0.0f, -605.0f) * lupineScalingMatrix);
	lupineInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-35.0f, 0.0f, -630.0f) * lupineScalingMatrix);
	lupineInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(35.0f, 0.0f, -630.0f) * lupineScalingMatrix);
	lupineInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(100.0f, 0.0f, -605.0f) * lupineScalingMatrix);
	lupineInstances[11].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(155.0f, 0.0f, -555.0f) * lupineScalingMatrix);
	lupineInstances[12].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-180.0f, 0.0f, -405.0f) * lupineScalingMatrix);
	lupineInstances[13].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-180.0f, 0.0f, -485.0f) * lupineScalingMatrix);
	lupineInstances[14].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(180.0f, 0.0f, -405.0f) * lupineScalingMatrix);
	lupineInstances[15].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(180.0f, 0.0f, -485.0f) * lupineScalingMatrix);
	result = m_models[ModelResource::LupineModel]->InitializeBuffers(m_pDevice, iLupineCount, lupineInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lupine vertex and index buffers.", result);
		return false;
	}

	int iLavenderCount = 27;
	XMMATRIX lavenderScalingMatrix = XMMatrixScaling(0.005f, 0.006f, 0.006f);
	Instance* lavenderInstances = new Instance[iLavenderCount];
	lavenderInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(30.0f, -250.0f, 0.0f) * lavenderScalingMatrix);
	lavenderInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-100.0f, -250.0f, 0.0f) * lavenderScalingMatrix);
	lavenderInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(100.0f, -250.0f, 0.0f) * lavenderScalingMatrix);
	lavenderInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-500.0f, -250.0f, 400.0f) * lavenderScalingMatrix);
	lavenderInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-600.0f, -250.0f, 500.0f) * lavenderScalingMatrix);
	lavenderInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-700.0f, -250.0f, 500.0f) * lavenderScalingMatrix);
	lavenderInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(500.0f, -250.0f, 400.0f) * lavenderScalingMatrix);
	lavenderInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(600.0f, -250.0f, 500.0f) * lavenderScalingMatrix);
	lavenderInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(700.0f, -250.0f, 500.0f) * lavenderScalingMatrix);
	lavenderInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1250.0f, -250.0f, -150.0f) * lavenderScalingMatrix);
	lavenderInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1350.0f, -250.0f, -50.0f) * lavenderScalingMatrix);
	lavenderInstances[11].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1450.0f, -250.0f, -50.0f) * lavenderScalingMatrix);
	lavenderInstances[12].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1250.0f, -250.0f, -150.0f) * lavenderScalingMatrix);
	lavenderInstances[13].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1350.0f, -250.0f, -50.0f) * lavenderScalingMatrix);
	lavenderInstances[14].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1450.0f, -250.0f, -50.0f) * lavenderScalingMatrix);
	lavenderInstances[15].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1750.0f, -250.0f, -850.0f) * lavenderScalingMatrix);
	lavenderInstances[16].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1850.0f, -250.0f, -750.0f) * lavenderScalingMatrix);
	lavenderInstances[17].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1950.0f, -250.0f, -750.0f) * lavenderScalingMatrix);
	lavenderInstances[18].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1750.0f, -250.0f, -850.0f) * lavenderScalingMatrix);
	lavenderInstances[19].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1850.0f, -250.0f, -750.0f) * lavenderScalingMatrix);
	lavenderInstances[20].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1950.0f, -250.0f, -750.0f) * lavenderScalingMatrix);
	lavenderInstances[21].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1850.0f, -250.0f, -1650.0f) * lavenderScalingMatrix);
	lavenderInstances[22].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1950.0f, -250.0f, -1550.0f) * lavenderScalingMatrix);
	lavenderInstances[23].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2050.0f, -250.0f, -1550.0f) * lavenderScalingMatrix);
	lavenderInstances[24].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1850.0f, -250.0f, -1650.0f) * lavenderScalingMatrix);
	lavenderInstances[25].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1950.0f, -250.0f, -1550.0f) * lavenderScalingMatrix);
	lavenderInstances[26].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2050.0f, -250.0f, -1550.0f) * lavenderScalingMatrix);
	result = m_models[ModelResource::LavenderModel]->InitializeBuffers(m_pDevice, iLavenderCount, lavenderInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the lavender vertex and index buffers.", result);
		return false;
	}

	result = m_models[ModelResource::GroundModel]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the ground vertex and index buffers.", result);
		return false;
	}

	int iHedgesCount = 2;
	XMMATRIX hedgeScalingMatrix = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	Instance* hedgeInstances = new Instance[iHedgesCount];
	hedgeInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.0f, -20.0f, 20.0f) * XMMatrixRotationRollPitchYaw(XM_PI * -0.5f, XM_PI * -0.5f, 0.0f) * hedgeScalingMatrix);
	hedgeInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, -15.0f, 20.0f) * XMMatrixRotationRollPitchYaw(XM_PI * -0.5f, 0.0f, 0.0f) * hedgeScalingMatrix);
	result = m_models[ModelResource::HedgeModel]->InitializeBuffers(m_pDevice, iHedgesCount, hedgeInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the hedge vertex and index buffers.", result);
		return false;
	}

	result = m_models[ModelResource::HedgeModel2]->InitializeBuffers(m_pDevice, 1);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the hedge vertex and index buffers.", result);
		return false;
	}

	int iBalustradesCount = 6;
	XMMATRIX balustradeScalingMatrix = XMMatrixScaling(11.0f, 11.0f, 11.0f);
	Instance* balustradeInstances = new Instance[iBalustradesCount];
	balustradeInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.84f, -0.125f, 0.95f) * balustradeScalingMatrix);
	balustradeInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.03f, -0.125f, 0.95f) * balustradeScalingMatrix);
	balustradeInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.78f, -0.125f, 0.95f) * balustradeScalingMatrix);
	balustradeInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.45f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * -0.5f, 0.0f) * balustradeScalingMatrix);
	balustradeInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.36f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * -0.5f, 0.0f) * balustradeScalingMatrix);
	balustradeInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-1.17f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * -0.5f, 0.0f) * balustradeScalingMatrix);
	result = m_models[ModelResource::BalustradeModel]->InitializeBuffers(m_pDevice, iBalustradesCount, balustradeInstances);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the balustrade vertex and index buffers.", result);
		return false;
	}

	int iBalustradesCount2 = 3;
	Instance* balustradeInstances2 = new Instance[iBalustradesCount2];
	balustradeInstances2[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-0.5f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * 0.5f, 0.0f) * balustradeScalingMatrix);
	balustradeInstances2[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.31f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * 0.5f, 0.0f) * balustradeScalingMatrix);
	balustradeInstances2[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(1.12f, -0.125f, 1.273f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI * 0.5f, 0.0f) * balustradeScalingMatrix);
	result = m_models[ModelResource::BalustradeModel2]->InitializeBuffers(m_pDevice, iBalustradesCount2, balustradeInstances2);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize the balustrade vertex and index buffers.", result);
		return false;
	}

	// Transform models

	/*XMMATRIX vaseTranslationMatrix = XMMatrixTranslation(-5.0f, 0.0f, 0.0f);
	XMMATRIX vaseScalingMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	m_models[ModelResource::VaseModel]->TransformWorldMatrix(vaseTranslationMatrix, XMMatrixIdentity(), vaseScalingMatrix);*/

	XMMATRIX fountainTranslationMatrix = XMMatrixTranslation(3.0f, 125.0f, -375.0f);
	XMMATRIX fountainScalingMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	m_models[ModelResource::FountainModel]->TransformWorldMatrix(fountainTranslationMatrix, XMMatrixIdentity(), fountainScalingMatrix);

	XMMATRIX groundTranslationMatrix = XMMatrixTranslation(0.0f, 0.0f, -5.0f);
	XMMATRIX groundScalingMatrix = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	m_models[ModelResource::GroundModel]->TransformWorldMatrix(groundTranslationMatrix, XMMatrixIdentity(), groundScalingMatrix);

	XMMATRIX hedgeTranslationMatrix = XMMatrixTranslation(5.0f, -20.0f, 20.0f);
	XMMATRIX hedgeRotationMatrix = XMMatrixRotationRollPitchYaw(XM_PI * -0.5f, XM_PI * 0.5f, 0.0f);
	m_models[ModelResource::HedgeModel2]->TransformWorldMatrix(hedgeTranslationMatrix, hedgeRotationMatrix, hedgeScalingMatrix);

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
	/*case LionTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/lion.dds", nullptr, &texture, 0, nullptr);
		break;*/
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
	case HedgeTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/hedge.dds", nullptr, &texture, 0, nullptr);
		break;
	case ParticleTexture:
		result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, L"Resources/particle.dds", nullptr, &texture, 0, nullptr);
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
	/*case LionModel:
		file.open("Resources/lion.txt");
		break;*/
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
	case LavenderModel:
		file.open("Resources/lavender.txt");
		break;
	case GroundModel:
	case HedgeModel:
		file.open("Resources/plane.txt");
		break;
	case BalustradeModel:
		file.open("Resources/balustrade.txt");
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
