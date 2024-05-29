#include "Character.h"

#include"../../main.h"

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

	Math::Vector3 cameraPos;

	Application::Instance().GetCamera()->ConvertWorldToScreenDetail(nowPos,cameraPos);

	m_cameraMat = Math::Matrix::CreateTranslation(cameraPos);

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
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_cameraMat);
	KdShaderManager::Instance().m_spriteShader.DrawCircle(0,0, 10, &kRedColor);
}


