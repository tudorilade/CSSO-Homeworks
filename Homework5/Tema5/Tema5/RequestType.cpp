#include "RequestType.h"

RequestType::RequestType(
	int req_type, char* pathGiven, unsigned int routeType
) : req_type(req_type), route(routeType){
	path = _strdup(pathGiven);
	this->valueFromRoute = _strdup(strchr(pathGiven, '/') + 1);
}

int RequestType::getReqType() {
	return this->req_type;
}

char* RequestType::getPath() {
	return this->path;
}

unsigned int RequestType::getRoute()
{
	return this->route;
}

char* RequestType::getValueFromRoute()
{
	return this->valueFromRoute;
}
