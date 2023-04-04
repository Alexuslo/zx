#include "Resource.h"

CResource::CResource()
{
	resource_.loadXML("resources.xml");
}

CResource::~CResource()
{
	resource_.free();
}

