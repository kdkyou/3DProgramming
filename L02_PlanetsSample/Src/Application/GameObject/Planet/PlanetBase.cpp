#include "PlanetBase.h"

PlanetBase::PlanetBase()
{
}

PlanetBase::~PlanetBase()
{
	Release();
}

// 初期化
void PlanetBase::Init()
{
	m_spModel	= nullptr;

	m_mTrans	= Math::Matrix::Identity;
	m_mRotation = Math::Matrix::Identity;
}

// 更新
void PlanetBase::Update()
{
}

// 描画
void PlanetBase::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void PlanetBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

// 解放
void PlanetBase::Release()
{
	m_spModel = nullptr;
}