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

    HttpGetRequest()  : HttpRequest()
    {
        m_http_type = HTTPREQUEST_GET;
    }

    void config_curl()
    {
        if (!is_valid())
        {
            return;
        }
        if (m_is_head)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_NOBODY, 1L);

        }

        if (m_allow_header_info)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_HEADER, 1L); //是否输出header信息
        }
    }

    void set_is_head(bool is_head)
    {
        m_is_head = is_head;
    }

    void set_is_allow_header_info(bool b)
    {
        m_allow_header_info = b;
    }


    void reuse_url()
    {
        m_is_head = false;
        m_allow_header_info = false;
        HttpRequest<HttpGetRequest>::reuse_url();
    }

    ~HttpGetRequest()
    {
#ifdef CURL_DEBUG
        std::cout << "~HttpGetRequest" << std::endl;
#endif
    }


    int get_request_type() { return m_http_type; }

private:
    bool m_is_head = false;
    bool m_allow_header_info = false;
};

#endif //USELIBCURL_GETREQUEST_H
