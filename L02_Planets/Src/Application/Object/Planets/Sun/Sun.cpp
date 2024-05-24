#include "Sun.h"

void Sun::Init()
{
	m_model.Load("Asset/Data/LessonData/Planets/Sun.gltf");
	m_mWorld = Math::Matrix::CreateTranslation(0, 0, 0);
}

void Sun::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(m_model, m_mWorld);
}
