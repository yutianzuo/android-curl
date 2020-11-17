//
// Created by yutianzuo on 2018/5/8.
//

#ifndef USELIBCURL_GETREQUEST_H
#define USELIBCURL_GETREQUEST_H

#include "request.h"

class HttpGetRequest : public HttpRequest<HttpGetRequest>
{
public:
    HttpGetRequest(HttpGetRequest &&rvalue) = delete;

    void operator=(HttpGetRequest &&) = delete;

    HttpGetRequest(const HttpGetRequest &src) = delete;

    HttpGetRequest &operator=(const HttpGetRequest &src) = delete;

    HttpGetRequest() : HttpRequest()
    {
        m_http_type = HTTPREQUEST_GET;
    }
    ~HttpGetRequest()
    {
#ifdef CURL_DEBUG
        std::cout<< "~HttpGetRequest" << std::endl;
#endif
    }
    int get_request_type(){return m_http_type;}
};

#endif //USELIBCURL_GETREQUEST_H
