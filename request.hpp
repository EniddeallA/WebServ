# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <vector>
# include <algorithm>
# include <map>
# include "Utils.hpp"
# include <cstdio>
#ifndef REQUEST

# define REQUEST

# define HTTP_VERSION "HTTP/1.1"
# define BAD_REQUEST 400
# define HTTP_VERSION_NOT_SUPPORTED 505
# define TOO_LARGE 413
# define METHOD_NOT_ALLOWED 405

typedef std::map<std::string, std::string> t_headers;

class Request{
    public:
        Request();
        Request(Request const &src);
        Request &operator=(Request const &src);
        ~Request();
    


        size_t getError(void) { return _error; }
        void    errorHandler(){};
        void    printData( void );

        void    Parse(std::string &req);
        bool    isRequestCompleted() { return _requestEnd; }
        void    clear( void );
        /** WARNING
         * getValidServer
        */
        std::string getRequestMethod( void ) const;
        std::string getRequestTarget( void ) const;
        std::string getRequestQuery( void ) const;
        std::string getHost( void ) const;
        t_headers getHeaders( void ) const;
        std::string getBodyName( void ) const;
        bool    getKeepAlive( void ) const;
        int getError( void ) const;
        size_t getContentLength( void ) const;

    private:
        void parseHeaders(std::string headers);
        void parseFirstLine(std::vector<std::string> vec);
        void parseBody(std::string &req);
        void toChuncked(std::string &req);
        void preBody( void );

    private:
        int                         _error;
        std::string                 _requestMethod, _requestTarget, _requestQuery;
        std::string                 _host, _bodyName, _str;
        bool                        _hasBody, _keepAlive;
        bool                        _headersEnd, _requestEnd;
        std::vector<std::string>    _allowedMethods;
        t_headers _headers;
        std::ofstream               _bodyFile;
        size_t                      _bodySize, _contentLength;


    private:
        std::string    _bodyToFile();
};

/*

HTTP-message   =    start-line
                    *( header-field CRLF )
                    CRLF
                    [ message-body ]

*/
#endif