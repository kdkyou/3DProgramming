#pragma once
#include "../PlanetBase.h"

class Moon : public PlanetBase
{
public:
	Moon();
	~Moon()			override;

	void Init()		override;
	void Update()	override;

private:

	void Release();
};