#include "Character.h"

#include"../../main.h"
#include"../Terrain/Terrain.h"

void Character::Init()
{
	if (!m_spPoly)
	{
		m_spPoly = std::make_shared<KdSquarePolygon>();
		m_spPoly->SetMaterial("Asset/Data/LessonData/Character/Hamu.png");
		m_spPoly->SetPivot(KdSquarePolygon::PivotType::Center_Bottom);
	}

}

void Character::Update()
{

	//①マウスの位置を取得
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		POINT _mousePos;
		GetCursorPos(&_mousePos);
		ScreenToClient(Application::Instance().GetWindowHandle(),&_mousePos);

		std::shared_ptr<KdCamera> _cam = m_wpCam.lock();
		if (_cam)
		{
			//2D座標を3D座標に変換
			Math::Vector3 m_camPos=_cam->GetCameraMatrix().Translation();
			Math::Vector3 _rayDir=Math::Vector3::Zero;
			float _rayRange = 1000.0f;
			//➁レイの発射方向を求める
			_cam->GenerateRayInfoFromClientPos(_mousePos, m_camPos, _rayDir, _rayRange);

			//➂実際にレイを飛ばして衝突位置を求める
			const std::shared_ptr<KdGameObject> _terrain = m_wpTerrain.lock();
			if (_terrain)
			{
				Math::Vector3 _endRayPos = m_camPos+(_rayDir*_rayRange);
				KdCollider::RayInfo _rayInfo(KdCollider::TypeGround,m_camPos,_endRayPos);
				
				//実際の当たり判定の処理
				std::list<KdCollider::CollisionResult>_results;
				_terrain->Intersects(_rayInfo,&_results);
				
				//　結果が一つでも帰って来ていたら
				if (_results.size())
				{
					for (KdCollider::CollisionResult result : _results)
					{
						m_TargetPos = result.m_hitPos;
					}
				}
			}
		}
	}

	// キャラクターの移動速度(真似しちゃダメですよ)
	float			moveSpd = 0.05f;
	Math::Vector3	nowPos	= GetPos();
	Math::Vector3	moveVec = m_TargetPos - nowPos;
	if (moveVec.Length() < moveSpd)moveSpd = moveVec.Length();
	moveVec.Normalize();
	moveVec *= moveSpd;
	nowPos  += moveVec;



	// キャラクターのワールド行列を創る処理
	m_mWorld = Math::Matrix::CreateTranslation(nowPos);

}

//void Character::PostUpdate()
//{
//	//=====================
//	//当たり判定　(レイ判定)
//	//=====================
//
//	//レイ判定用に変数を作成
//	KdCollider::RayInfo ray;
//	//レイの発射位置(座標)を設定
//	ray.m_pos = m_mWorld.Translation();		//自分の足元
//	//レイの発射方向を設定
//	ray.m_dir = Math::Vector3::Down;
//	//少し高いところから飛ばす
//	ray.m_pos.y += 0.1f;
//	//段差の許容範囲を設定
//	float enableStepHigh = 0.2f;
//	ray.m_pos.y += m_gravity + enableStepHigh;
//	//レイの長さを設定
//	ray.m_range = enableStepHigh;
//	//当たり判定をしたいタイプを設定
//	ray.m_type = KdCollider::TypeGround;
//
//
//	//レイに当たったオブジェクト情報を格納するリスト
//	std::list<KdCollider::CollisionResult>retRayList;
//
//	//レイと当たり判定！！
//	for (auto& obj : Application::Instance().GetObjList())
//	{
//		obj->Intersects(ray, &retRayList);
//	}
//
//	//レイに当たったリストから一番近いオブジェクトを検出
//	float maxOverLap = 0;	//はみ出たレイの長さ
//	Math::Vector3 hitPos;	//レイが遮断された座標(当たっている場所)
//	bool isHit = false;		//当たっていたらtrue
//	for (auto& ret : retRayList)
//	{
//		//レイを遮断し、オーバーした長さが一番長いものを探す
//		if (maxOverLap < ret.m_overlapDistance)
//		{
//			maxOverLap = ret.m_overlapDistance;
//			hitPos = ret.m_hitPos;
//			isHit = true;
//		}
//	}
//
//	if (isHit)
//	{
//		//地面に当たっている
//		m_mWorld = Math::Matrix::CreateTranslation(hitPos + Math::Vector3(0.0f, -0.10f, 0.0f));
//		//m_mWorld = Math::Matrix::CreateTranslation(hitPos);
//		m_gravity = 0.0f;
//	}
//}

void Character::DrawLit()
{
	if (!m_spPoly) return;

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPoly, m_mWorld);
}

void Character::DrawSprite()
{
	//	↓この赤い点が常にハムスターの原点(足元)に追従するように!
	//　①ハムスターの3D座標を2D座標へ変換する！
	/*Math::Vector3 cameraPos = {};

	if (m_camera.expired() == false)
	{
		m_camera.lock()->ConvertWorldToScreenDetail(GetPos(), cameraPos);
		m_cameraMat = Math::Matrix::CreateTranslation(cameraPos);
	}

	KdShaderManager::Instance().m_spriteShader.DrawCircle(cameraPos.x,cameraPos.y, 10, &kRedColor);*/
	
	Math::Vector3 _result = Math::Vector3::Zero;
	Math::Vector3 _resultTex = Math::Vector3::Zero;
	std::shared_ptr<KdCamera> _cam = m_wpCam.lock();
	if (_cam)
	{
		_cam->ConvertWorldToScreenDetail(GetPos(), _result);
		_cam->ConvertWorldToScreenDetail(GetPos() + Math::Vector3{ 0.0f,1.0f,0.0f }, _resultTex);
	}
	
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::CreateTranslation(_result.x,_result.y,0.0f));
	KdShaderManager::Instance().m_spriteShader.DrawCircle(0.0f,0.0f, 10.0f, &kRedColor);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	KdTexture tex;
	tex.Load("Asset/Textures/hk.png");
	Math::Rectangle rect = { 0,0,40,20 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&tex,_resultTex.x, _resultTex.y,&rect);

	
}



