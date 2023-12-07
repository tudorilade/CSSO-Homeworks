#pragma once
#include "constants.h"
class RequestType
{

	private:
		int req_type;
		char* path;
		int id;
	public:
		RequestType(int, char*, int);
		int getReqType();
		char* getPath();
		int getId();
};	

