#pragma once
#include "../PlanetBase.h"

class Earth : public PlanetBase
{
public:
	Earth();
	~Earth()		override;

	void Init()		override;
	void Update()	override;

private:

	void Release();
};