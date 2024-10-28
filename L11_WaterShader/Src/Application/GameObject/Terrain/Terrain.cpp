#include "Terrain.h"

void Terrain::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/LessonData/Stage/StageMap.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);
	}
}

void Terrain::Update()
{
}

void Terrain::DrawLesson()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_LessonShader.DrawModel(*m_spModel->GetData());
}

