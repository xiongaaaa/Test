#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <string>

class XHttpResponse
{
public:
    XHttpResponse();
    ~XHttpResponse();

    bool SetRequest(std::string request);
    std::string GetHead();
    int Read(char* buf, int bufsize);  
private:
    int filesize;
    FILE *fp;
};


#endif