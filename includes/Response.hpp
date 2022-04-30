#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request.hpp"

enum class statusCode
{

	/* 1xx: Informational 
		It means the request has been received and the 
		process is continuing.
	*/
		continueCode= 100,
		switchingProtocols= 101,

	/* 2xx: Success 
		It means the action was successfully received,
		understood, and accepted.
	*/
		ok= 200,
		created= 201,
		accepted= 202,
		nonAuthoritativeInformation= 203,
		noContent= 204,
		resetContent= 205,

	/* 3xx: Redirection 
		It means further action must be taken in order to
		complete the request.
	*/
		multipleChoices= 300,
		movedPermanently= 301,
		found= 302,
		seeOther= 303,
		useProxy= 305,
		temporaryRedirect= 307,

	/* 4xx: Client Error 
		It means the request contains incorrect syntax or 
		cannot be fulfilled.
	*/
		badRequest= 400,
		paymentRequired= 402,
		forbidden= 403,
		notFound= 404,
		methodNotAllowed= 405,
		notAcceptable= 406,
		requestTimeout= 408,
		conflict= 409,
		gone= 410,
		lengthRequired= 411,
		payloadTooLarge= 413,
		URITooLong= 414,
		unsupportedMediaType= 415,
		expectationFailed= 417,
		upgradeRequired= 426,

	/* 5xx: Server Error 
		It means the server failed to fulfill an apparently valid
		request.
	*/
		internalServerError= 500,
		notImplemented= 501,
		badGateway= 502,
		serviceUnavailable= 503,
		gatewayTimeout= 504,
		httpVersionNotSupported= 505
		
};

class Response
{
	private:
		statusCode _statuscode;
		Request	_request;
		std::string _response;
		int _fd;
		fd_set _set;
		bool _is_request_handled;

	public:
		Response(Request	request);
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();

		statusCode getStatusCode() const;
		void setStatusCode(statusCode statuscode);
		
		void setHeader(size_t status_code, std::string const &message);
		void unallowedMethod();
		void badRequest();
		void internalError();
		void httpVersionNotSupported(std::string const &version);
		void ok(std::string const &tmp_path);

		void handleRequest();
		void handleGetRequest();
		void handlePostRequest();
		void handleDeleteRequest();
};

#endif;