#include "Moon.h"

Moon::Moon()
{
}

Moon::~Moon()
{
	Release();
}

void Moon::Init()
{
	PlanetBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Data/LessonData/Planets/moon.gltf");

		// 原点(回転させたい中心点)からどれだけ離れた位置にいるか
		m_mTrans = Math::Matrix::CreateTranslation(0, 0, -1.5f);
	}
}

void Moon::Update()
{
	Math::Matrix parentTrans = Math::Matrix::Identity;
	const std::shared_ptr<PlanetBase> spPalent = m_wpParent.lock();
	if (spPalent)
	{
		parentTrans = Math::Matrix::CreateTranslation(spPalent->GetPos());
	}

	// 自転
	m_mRotation *= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(4.0f));

	// 公転(移動してから回転)
	m_mTrans *= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(3.0f));

	// 行列の合成（最後に親の行列を合成する！） = 行列の親子関係
	m_mWorld = (m_mRotation * m_mTrans) * parentTrans;
}

void Moon::Release()
{
}
