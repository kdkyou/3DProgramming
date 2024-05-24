#include "Earth.h"

void Earth::Init()
{
	m_model.Load("Asset/Data/LessonData/Planets/Earth.gltf");
	m_transMat = Math::Matrix::CreateTranslation(-5, 0, 0);
	m_rotY = 0.0f;
	m_mWorld = m_transMat;
}

void Earth::Update()
{
	m_rotY += 0.005f;
	if (m_rotY >= 360)
	{
		m_rotY -= 360;
	}
	m_rotYMat = Math::Matrix::CreateRotationY(m_rotY);
	m_transMat = Math::Matrix::CreateTranslation(-5, 0, 0);
	m_mWorld = m_rotYMat * m_transMat * m_rotYMat;
}

void Earth::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(m_model, m_mWorld);
}
