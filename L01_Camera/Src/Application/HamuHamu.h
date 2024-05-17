#pragma once

class HamuHamu :public KdGameObject
{
public:
	HamuHamu();
	~HamuHamu()override;


	void Init() override;
	void Update() override;
	void DrawLit() override;
	

private:

	//板ポリゴン
	std::shared_ptr<KdSquarePolygon> m_spPoly = nullptr;



};