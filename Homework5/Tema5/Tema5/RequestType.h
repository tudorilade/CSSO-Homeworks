#pragma once
#include "constants.h"
#include <string.h>

class RequestType
{

	private:
		int req_type;
		char* path;
		unsigned int route;

	public:
		RequestType(int, char*, unsigned int);
		int getReqType();
		char* getPath();
		unsigned int getRoute();
};	

