#include "Character.h"

#include "../../main.h"
#include "../Camera/CameraBase.h"

void Character::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		//m_spModel->SetModelData("Asset/Models/Robot/Robot.gltf");
		m_spModel->SetModelData("Asset/Models/SkinMeshMan/SkinMeshMan.gltf");
	
		//初期のアニメーションをセットする
		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spModel->GetAnimation("Walk"));
	}
}

void Character::Update()
{
	// キャラクターの移動速度(真似しちゃダメですよ)
	float			_moveSpd = 0.05f;
	Math::Vector3	_nowPos	= GetPos();

	Math::Vector3 _moveVec = Math::Vector3::Zero;
	if (GetAsyncKeyState('D')) { _moveVec.x =  1.0f; }
	if (GetAsyncKeyState('A')) { _moveVec.x = -1.0f; }
	if (GetAsyncKeyState('W')) { _moveVec.z =  1.0f; }
	if (GetAsyncKeyState('S')) { _moveVec.z = -1.0f; }

	const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		_moveVec = _moveVec.TransformNormal(_moveVec, _spCamera->GetRotationYMatrix());
	}
	_moveVec.Normalize();
	_moveVec *= _moveSpd;
	_nowPos += _moveVec;

	// キャラクターの回転行列を創る
	UpdateRotate(_moveVec);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = _rotation * Math::Matrix::CreateTranslation(_nowPos);
}

void Character::PostUpdate()
{
	//アニメーションの更新
	m_spAnimetor->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Character::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Character::UpdateRotate(const Math::Vector3& srcMoveVec)
{
	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f) { return; }

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Backward();

	// 移動方向のベクトル
	Math::Vector3 _targetDir = srcMoveVec;

	_nowDir.Normalize();
	_targetDir.Normalize();

	float _nowAng = atan2(_nowDir.x, _nowDir.z);
	_nowAng = DirectX::XMConvertToDegrees(_nowAng);

	float _targetAng = atan2(_targetDir.x, _targetDir.z);
	_targetAng = DirectX::XMConvertToDegrees(_targetAng);

	// 角度の差分を求める
	float _betweenAng = _targetAng - _nowAng;
	if (_betweenAng > 180)
	{
		_betweenAng -= 360;
	}
	else if (_betweenAng < -180)
	{
		_betweenAng += 360;
	}

	float rotateAng = std::clamp(_betweenAng, -8.0f, 8.0f);
	m_worldRot.y += rotateAng;
}