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

    }

};

#endif //USELIBCURL_GETREQUEST_H
