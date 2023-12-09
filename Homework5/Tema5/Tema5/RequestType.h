#pragma once
#include "constants.h"
#include <string.h>

class RequestType
{

	private:
		int req_type;
		char* path;
	public:
		RequestType(int, char*);
		int getReqType();
		char* getPath();
};	

