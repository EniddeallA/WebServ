#include "response.hpp"

Response::Response(int statuscode)
{
	_statuscode = statuscode;
}

Response::Response(const Response& other)
{
	_statuscode = other._statuscode;
}

Response& Response::operator=(const Response& other)
{
	_statuscode = other.getStatusCode();
	return *this;
}

~Response::Response(){}

int getStatusCode()
{
	return _statuscode;
}

void Response::handleRequest(Request& request)
{

}