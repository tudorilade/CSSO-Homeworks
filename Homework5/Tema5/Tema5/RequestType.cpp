#include "RequestType.h"

RequestType::RequestType(int req_type, char* pathGiven) :req_type(req_type) {
	path = _strdup(pathGiven);
}

int RequestType::getReqType() {
	return this->req_type;
}

char* RequestType::getPath() {
	return this->path;
}


