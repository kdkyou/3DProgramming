#pragma once

class CameraBase;
class Character : public KdGameObject
{
public:
	Character()							{}
	~Character()			override	{}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;
	void DrawLit()			override;

	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	std::shared_ptr<KdModelWork>		m_spModel		= nullptr;
	std::shared_ptr<KdAnimator>			m_spAnimetor	= nullptr;

	std::weak_ptr<CameraBase>			m_wpCamera;

	Math::Vector3						m_worldRot;
};