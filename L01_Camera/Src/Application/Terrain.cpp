#include"Terrain.h"

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

void Terrain::Init()
{
	m_spModel = std::make_shared<KdModelData>();
	m_spModel->Load("Asset/Data/LessonData/Terrain/Terrain.gltf");
}

void Terrain::Update()
{
}

void Terrain::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel);

}
