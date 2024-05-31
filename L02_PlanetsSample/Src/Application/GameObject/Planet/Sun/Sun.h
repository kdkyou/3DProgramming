#pragma once
#include"../PlanetBase.h"

class Sun : public PlanetBase
{
public:
	Sun();
	~Sun()			override;

	void Init()		override;
	void Update()	override;

private:

	void Release();
};