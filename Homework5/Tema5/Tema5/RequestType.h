#pragma once
#include "constants.h"
#include <string.h>

class RequestType
{

	private:
		int req_type;
		char* path;
		unsigned int route;
		char* valueFromRoute;

	public:
		RequestType(int, char*, unsigned int);
		int getReqType();
		unsigned int getRoute();
		char* getPath();
		char* getValueFromRoute();
};	
