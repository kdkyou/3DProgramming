#include "Sun.h"

Sun::Sun()
{
}

Sun::~Sun()
{
	Release();
}

void Sun::Init()
{
	// 親クラスの同処理を呼び出せるよ！
	PlanetBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Data/LessonData/Planets/sun.gltf");
	}
}

void Sun::Update()
{
	m_mRotation *= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(2.0f));
	m_mWorld	= m_mRotation * Math::Matrix::CreateTranslation(0,0,8);
}

void Sun::Release()
{
}
