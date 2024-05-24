#pragma once

class Earth:public KdGameObject
{
public:
	Earth() {}
	~Earth()override {}

	void Init()override;
	void Update()override;
	void DrawLit()override;

private:

	Math::Matrix m_transMat, m_rotYMat;
	
	float m_rotY;

	KdModelData m_model;
};