#include "Character.h"

#include "../../main.h"
#include "../Camera/CameraBase.h"

void Character::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/gltf/Grint/grint.gltf");

		// 初期のアニメーションをセットする
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stand"));
	}

	m_Gravity = 0;
	SetPos({ 0, 5.0f, 0 });
	
	m_scale = Math::Matrix::CreateScale(0.25f);

	SetScale(0.25f);

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionIdle>());
}

void Character::Update()
{
	// 重力の更新(いかなる状態でも影響するでしょ！？)
	m_Gravity += 0.01f;
	m_mWorld._42 -= m_Gravity;

	//各種「状態に応じた」更新処理を実行する
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

void Character::GenerateDepthMapFromLight()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
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
	Math::Vector3 _nowDir = GetMatrix().Forward();

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
				m_Gravity = 0;

				m_isGround = true;
			}
			else
			{
				
				m_isGround = false;
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


//ここからステートパターン関係
void Character::ChangeActionState(std::shared_ptr<ActionStateBase> nextAction)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextAction;
	m_nowAction->Enter(*this);
}

void Character::ActionIdle::Enter(Character& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stand"));
}

void Character::ActionIdle::Update(Character& owner)
{

	if (GetAsyncKeyState(VK_SPACE)) {

		if (owner.m_isGround)
		{
			owner.ChangeActionState(std::make_shared<ActionJump>());
			return;
		}
	}

	bool isMove = false;

	if (GetAsyncKeyState('W')||
		GetAsyncKeyState('A')|| 
		GetAsyncKeyState('S')||
		GetAsyncKeyState('D')) {
		isMove = true;
	}
	if(isMove){
		owner.ChangeActionState(std::make_shared<ActionMove>());
		return;
	}
}

void Character::ActionIdle::Exit(Character& owner)
{
	owner.m_spAnimator->IsAnimationEnd();
}

void Character::ActionJump::Enter(Character& owner)
{
	owner.m_Gravity += -0.5f;

	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Boost"));
}

void Character::ActionJump::Update(Character& owner)
{
	if (owner.m_isGround)
	{
		owner.ChangeActionState(std::make_shared<ActionBoostEnd>());
		return;
	}
	else
	{
		owner.ChangeActionState(std::make_shared<ActionBoostNow>());
		return;
	}

}

void Character::ActionJump::Exit(Character& owner)
{
}

void Character::ActionMove::Enter(Character& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Walk"));
}

void Character::ActionMove::Update(Character& owner)
{

	Math::Vector3 _move;
	Math::Vector3 _nowPos=owner.GetPos();
	float _moveSpd = 0.08f;

	if (GetAsyncKeyState('W')) { _move.z += 1.f;
	}
	if(	GetAsyncKeyState('A')) { _move.x -= 1.f;
	}
	if(	GetAsyncKeyState('S')) { _move.z -= 1.f;
	}
	if(	GetAsyncKeyState('D')){ _move.x += 1.f;
	}

	//移動中に何も入力がなければ待機に移行
	if (_move.LengthSquared() ==0 ) {
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}
	else
	{
		const std::shared_ptr<const CameraBase> _spCamera = owner.m_wpCamera.lock();
		if (_spCamera)
		{
			_move = _move.TransformNormal(_move, _spCamera->GetRotationYMatrix());
		}
		_move.Normalize();
		_move *= _moveSpd;
		_nowPos += _move;
	}

	

	// キャラクターの回転行列を創る
	owner.UpdateRotate(_move);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(owner.m_worldRot.y));
	owner.m_mWorld = owner.m_scale * _rotation * Math::Matrix::CreateTranslation(_nowPos);
}

void Character::ActionMove::Exit(Character& owner)
{
}

void Character::ActionBoostNow::Enter(Character& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Hoboor"));
}

void Character::ActionBoostNow::Update(Character& owner)
{
	if (owner.m_isGround)
	{
		owner.ChangeActionState(std::make_shared<ActionBoostEnd>());
		return;
	}
}

void Character::ActionBoostNow::Exit(Character& owner)
{
}

void Character::ActionBoostEnd::Enter(Character& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("BoostEnd"),false);
}

void Character::ActionBoostEnd::Update(Character& owner)
{
	if (owner.m_spAnimator->IsAnimationEnd())
	{
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}

}

void Character::ActionBoostEnd::Exit(Character& owner)
{
}
