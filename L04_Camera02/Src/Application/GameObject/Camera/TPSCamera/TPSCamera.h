#pragma once

#include"../CameraBase.h"

class TPSCamera :public CameraBase
{
public:
	TPSCamera() {}
	~TPSCamera() override {}

	void Init()		  override;
	void Update()	  override;

private:

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

};
