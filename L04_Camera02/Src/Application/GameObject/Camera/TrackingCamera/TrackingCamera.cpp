#include "TrackingCamera.h"

void TrackingCamera::Init()
{
	//基準点(ターゲット)からどれだけ離れているか
	m_mLocalPos = Math::Matrix::CreateTranslation(0.0f, 6.0f, -5.0f);

	//どれだけ傾いているか
	m_mRotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(45));

	CameraBase::Init();

}

void TrackingCamera::Update()
{
	//ターゲットの行列
	Math::Matrix _targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}


	m_mWorld = m_mRotation * m_mLocalPos * _targetMat;

	CameraBase::Update();
}
