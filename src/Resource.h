#pragma once
#include "oxygine-framework.h"

using namespace oxygine;

class CResource
{
private:
	Resources resource_;

public:
	CResource();
	~CResource();

	Resources& GetManager()  { return resource_; }
};

