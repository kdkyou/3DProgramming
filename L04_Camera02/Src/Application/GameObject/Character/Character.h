#pragma once

class Character : public KdGameObject
{
public:
	Character() {}
	virtual ~Character()	override {}

	void Init()				override;
	void Update()			override;
	void DrawLit()			override;

private:
	std::shared_ptr<KdSquarePolygon>	m_spPoly	= nullptr;

};