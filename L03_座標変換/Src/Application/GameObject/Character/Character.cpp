#include "Character.h"

//#include"../../main.h"

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
	// キャラクターの移動速度(真似しちゃダメですよ)
	float moveSpd = 0.05f;
	Math::Vector3 nowPos = m_mWorld.Translation();

	Math::Vector3 moveVec = Math::Vector3::Zero;
	if (GetAsyncKeyState('D')) { moveVec.x = 1.0f; }
	if (GetAsyncKeyState('A')) { moveVec.x = -1.0f; }
	if (GetAsyncKeyState('W')) { moveVec.z = 1.0f; }
	if (GetAsyncKeyState('S')) { moveVec.z = -1.0f; }
	moveVec.Normalize();
	moveVec *= moveSpd;
	nowPos += moveVec;

	Math::Vector3 cameraPos = {};

	if (m_wpCam.expired() == false)
	{
		m_wpCam.lock()->ConvertWorldToScreenDetail(nowPos, cameraPos);
		m_cameraMat = Math::Matrix::CreateTranslation(cameraPos);
	}
	

	// キャラクターのワールド行列を創る処理
	m_mWorld = Math::Matrix::CreateTranslation(nowPos);

}

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
	}
	if (_cam)
	{
		_cam->ConvertWorldToScreenDetail(GetPos() + Math::Vector3{ 0.0f,1.0f,0.0f }, _resultTex);
	}



	//KdShaderManager::Instance().m_spriteShader.SetMatrix(m_cameraMat);
	KdShaderManager::Instance().m_spriteShader.DrawCircle(_result.x, _result.y, 10.0f, &kRedColor);

	KdTexture tex;
	tex.Load("Asset/Textures/hk.png");
	Math::Rectangle rect = { 0,0,40,20 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&tex,_resultTex.x, _resultTex.y,&rect);
}



