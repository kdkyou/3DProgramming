#pragma once

class CameraBase;

class Character : public KdGameObject
{
public:
	Character() {}
	virtual ~Character()	override {}

	void Init()				override;
	void Update()			override;
	void DrawLit()			override;

	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

private:
	std::shared_ptr<KdSquarePolygon>	m_spPoly	= nullptr;

	std::weak_ptr<CameraBase>			m_wpCamera;

};