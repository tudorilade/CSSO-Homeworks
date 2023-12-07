#include "RequestType.h"

RequestType::RequestType(int req_type, char* path, int id) :req_type(req_type), path(path) , id(id){}

int RequestType::getReqType() {
	return this->req_type;
}

char* RequestType::getPath() {
	return this->path;
}
int RequestType::getId() {
	return this->id;
}

