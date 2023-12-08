#include "RequestType.h"

RequestType::RequestType(int req_type, char* path) :req_type(req_type), path(path) {}

int RequestType::getReqType() {
	return this->req_type;
}

char* RequestType::getPath() {
	return this->path;
}


