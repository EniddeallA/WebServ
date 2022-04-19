#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>

class statusCode
{
	private:
		statusCode();
		~statusCode();

	public:
	/* 1xx: Informational 
		It means the request has been received and the 
		process is continuing.
	*/
		int continue(){return 100;};
		int switchingProtocols(){return 101;};

	/* 2xx: Success 
		It means the action was successfully received,
		understood, and accepted.
	*/
		int ok(){return 200;};
		int created(){return 201;};
		int accepted(){return 202;};
		int nonAuthoritativeInformation(){return 203;};
		int noContent(){return 204;};
		int resetContent(){return 205;};
		int partialContent(){return 206;};

	/* 3xx: Redirection 
		It means further action must be taken in order to
		complete the request.
	*/
		int multipleChoices(){return 300;};
		int movedPermanently(){return 301;};
		int found(){return 302;};
		int seeOther(){return 303;};
		int notModified(){return 304;};
		int useProxy(){return 305;};
		int temporaryRedirect(){return 307;};

	/* 4xx: Client Error 
		It means the request contains incorrect syntax or 
		cannot be fulfilled.
	*/
		int badRequest(){return 400;};
		int unauthorized(){return 401;};
		int paymentRequired(){return 402;};
		int forbidden(){return 403;};
		int notFound(){return 404;};
		int methodNotAllowed(){return 405;};
		int notAcceptable(){return 406;};
		int proxyAuthenticationRequired(){return 407;};
		int requestTimeout(){return 408;};
		int conflict(){return 409;};
		int gone(){return 410;};
		int lengthRequired(){return 411;};
		int preconditionFailed(){return 412;};
		int requestEntityTooLarge(){return 413;};
		int requestURITooLong(){return 414;};
		int unsupportedMediaType(){return 415;};
		int requestedRangeNotSatisfiable(){return 416;};
		int expectationFailed(){return 417;};
	
	/* 5xx: Server Error 
		It means the server failed to fulfill an apparently valid
		request.
	*/
		int internalServerError(){return 500;};
		int notImplemented(){return 501;};
		int badGateway(){return 502;};
		int serviceUnavailable(){return 503;};
		int gatewayTimeout(){return 504;};
		int httpVersionNotSupported(){return 505;};
		
};

class Response
{
	private:
		int _statuscode;

	public:
		Response(int statuscode);
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();

		int getStatusCode();
		void handleRequest(Request& request);
};

#endif;