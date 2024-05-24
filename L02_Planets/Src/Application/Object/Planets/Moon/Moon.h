#pragma once

class Earth;

class Moon :public KdGameObject
{
public:
	Moon() {}
	~Moon()override {}

	void Init()override;
	void Update()override;
	void DrawLit()override;

	void SetEarth(std::shared_ptr<Earth> _earth) { m_earth = _earth; }

private:

	Math::Matrix m_transMat, m_rotYMat;

	float m_rotY;

	KdModelData m_model;

	std::shared_ptr<Earth> m_earth;
};