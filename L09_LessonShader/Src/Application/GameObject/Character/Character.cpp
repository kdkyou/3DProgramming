#include "Character.h"

#include "../../main.h"
#include "../Camera/CameraBase.h"

void Character::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Robot/Robot.gltf");

		// 初期のアニメーションをセットする
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stand"));
	}

	m_Gravity = 0;
	SetPos({ 0, 5.0f, 0 });

	// 初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionIdle>());
}

void Character::Update()
{
	// 重力の更新(いかなる状態でも影響するでしょ！？)
	m_Gravity += 0.01f;
	m_mWorld._42 -= m_Gravity;

	// 各種「状態」に応じた更新処理を実行する
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}

	// キャラクターの座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();
}

void Character::PostUpdate()
{
	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Character::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Character::GenerateDepthMapFromLight()
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

void Character::UpdateCollision()
{
	// 地面判定

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高い所から飛ばす(段差の許容範囲)
	rayInfo.m_pos.y += 0.2f;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = m_Gravity + 0.2f;

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = Math::Vector3::Zero;
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			// 何かしらの上に乗ってる
			if (hit)
			{
				SetPos(hitPos);
				m_IsGround	= true;
				m_Gravity	= 0;
			}
			else
			{
				m_IsGround = false;
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo spherInfo;
	spherInfo.m_sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
	spherInfo.m_sphere.Radius = 0.5f;
	spherInfo.m_type = KdCollider::TypeBump;

	// ②HIT対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> weGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = weGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retBumpList;
			spGameObj->Intersects(spherInfo, &retBumpList);

			// ③結果を使って座標を補完する
			for (auto& ret : retBumpList)
			{
				Math::Vector3 newPos = GetPos() + (ret.m_hitDir * ret.m_overlapDistance);
				SetPos(newPos);
			}
		}
	}
}

// こっからステートパターン関係
void Character::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}

// ↓待機状態！
void Character::ActionIdle::Enter(Character& owner){
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stand"));
}
void Character::ActionIdle::Update(Character& owner)
{
	// ジャンプ検知
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		owner.ChangeActionState(std::make_shared<ActionJump>());
		return;
	}

	// 移動検知
	Math::Vector3 _moveVec = Math::Vector3::Zero;
	if (GetAsyncKeyState('D')) { _moveVec.x = 1.0f;		}
	if (GetAsyncKeyState('A')) { _moveVec.x = -1.0f;	}
	if (GetAsyncKeyState('W')) { _moveVec.z = 1.0f;		}
	if (GetAsyncKeyState('S')) { _moveVec.z = -1.0f;	}

	if (_moveVec.LengthSquared() > 0)
	{
		// 「移動状態」へ切り替える
		owner.ChangeActionState(std::make_shared<ActionWalk>());
	}
}
void Character::ActionIdle::Exit(Character& owner){}

// ↓ジャンプ状態！
void Character::ActionJump::Enter(Character& owner)
{
	owner.m_Gravity = -0.5f;
}
void Character::ActionJump::Update(Character& owner)
{
	if (owner.m_IsGround)
	{
		owner.ChangeActionState(std::make_shared<ActionIdle>());
	}
}
void Character::ActionJump::Exit(Character& owner) {}

// ↓移動状態！
void Character::ActionWalk::Enter(Character& owner) 
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Walk"));
}
void Character::ActionWalk::Update(Character& owner)
{
	// キャラクターの移動速度(真似しちゃダメですよ)
	float			_moveSpd	= 0.05f;
	Math::Vector3	_nowPos		= owner.GetPos();

	Math::Vector3 _moveVec = Math::Vector3::Zero;
	if (GetAsyncKeyState('D')) { _moveVec.x = 1.0f;		}
	if (GetAsyncKeyState('A')) { _moveVec.x = -1.0f;	}
	if (GetAsyncKeyState('W')) { _moveVec.z = 1.0f;		}
	if (GetAsyncKeyState('S')) { _moveVec.z = -1.0f;	}

	// 「移動状態」へ切り替える
	if (_moveVec.LengthSquared() == 0)
	{
		owner.ChangeActionState(std::make_shared<ActionIdle>());
	}

	const std::shared_ptr<const CameraBase> _spCamera = owner.m_wpCamera.lock();
	if (_spCamera)
	{
		_moveVec = _moveVec.TransformNormal(_moveVec, _spCamera->GetRotationYMatrix());
	}
	_moveVec.Normalize();
	_moveVec *= _moveSpd;
	_nowPos += _moveVec;

	// キャラクターの回転行列を創る
	owner.UpdateRotate(_moveVec);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation	= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(owner.m_worldRot.y));
	owner.m_mWorld			= _rotation * Math::Matrix::CreateTranslation(_nowPos);
}
void Character::ActionWalk::Exit(Character& owner) {}