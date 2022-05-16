#include <exception>
#include "StatusCode.hpp"

class StatusCodeException : public std::exception {
    private:
        HttpStatus::statusCode _code;

    public:
        StatusCodeException(HttpStatus::statusCode code) : _code(code) {}
        const char * what() const throw ()
        {
            return HttpStatus::reasonPhrase(_code);
        }
    	virtual ~StatusCodeException() throw() {return ;}

        HttpStatus::statusCode getStatusCode() const {
            return _code;
        }
};