#include "CCTVCamera.h"

void CCTVCamera::Init()
{
	CameraBase::Init();

	m_mRotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(45));
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 6.0f, -10.0f);
	
	m_mWorld = m_mRotation*m_mLocalPos;
	//m_mWorld = m_mLocalPos;

	m_angle = 0;
}

void CCTVCamera::PostUpdate()
{
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	//カメラの回転
	UpdateLookAtRotate(_targetMat.Translation());
}

void CCTVCamera::UpdateLookAtRotate(const Math::Vector3& targetPos)
{
	Math::Matrix _shadowVP = DirectX::XMMatrixLookAtLH(GetPos(), targetPos, Math::Vector3::Up);

	m_mWorld = _shadowVP.Invert();
}
