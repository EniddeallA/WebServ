#include "Response.hpp"

Response::Response(statusCode statuscode):
	_statuscode(statuscode)
{}

Response::Response(const Response& other)
{
	_statuscode = other._statuscode;
}

Response& Response::operator=(const Response& other)
{
	_statuscode = other.getStatusCode();
	return *this;
}

Response::~Response(){}

statusCode Response::getStatusCode() const
{
	return _statuscode;
}

void Response::setStatusCode(statusCode statuscode)
{
	_statuscode = statuscode;
}

void handleRequest(Request& request)
{
	
}