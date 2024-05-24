#include"Moon.h"
#include"../Earth/Earth.h"

void Moon::Init()
{
	m_model.Load("Asset/Data/LessonData/Planets/Moon.gltf");
	m_transMat = Math::Matrix::CreateTranslation(-7, 0, 0);
	m_rotY = 0.0f;
	m_mWorld = m_transMat;
}

void Moon::Update()
{
	m_rotY += 0.01f;
	if (m_rotY >= 360)
	{
		m_rotY -= 360;
	}
	m_rotYMat = Math::Matrix::CreateRotationY(m_rotY);
	m_transMat = Math::Matrix::CreateTranslation(-2, 0, 0);

	m_mWorld = m_rotYMat * m_transMat * m_rotYMat *m_earth->GetMatrix();
}

void Moon::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(m_model, m_mWorld);
}
