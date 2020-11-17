//
// Created by yutianzuo on 2018/5/8.
//

#ifndef USELIBCURL_POSTREQUEST_H
#define USELIBCURL_POSTREQUEST_H

#include "request.h"

class HttpPostFormDataRequest : public HttpRequest<HttpPostFormDataRequest>
{
public:
    HttpPostFormDataRequest(HttpPostFormDataRequest &&) = delete;

    void operator=(HttpPostFormDataRequest &&) = delete;

    HttpPostFormDataRequest &operator=(const HttpPostFormDataRequest &src) = delete;

    HttpPostFormDataRequest(const HttpPostFormDataRequest &src) = delete;

    HttpPostFormDataRequest(bool form_or_json) : HttpRequest<HttpPostFormDataRequest>(), m_form_or_json(form_or_json)
    {
    }
    ~HttpPostFormDataRequest()
    {
#ifdef CURL_DEBUG
        std::cout<< "~HttpPostFormDataRequest" << std::endl;
#endif
    }

    void set_postformdata(const std::string &str_data)
    {
        if (!is_valid())
        {
            return;
        }
        curl_easy_setopt(m_curl_handle, CURLOPT_POSTFIELDSIZE, str_data.size());
        curl_easy_setopt(m_curl_handle, CURLOPT_COPYPOSTFIELDS, str_data.c_str());
    }
    int get_request_type(){return m_http_type;}

    void config_curl()
    {
        if (is_valid())
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_POST, 1L);
            if (m_form_or_json)
            {
                m_headers = curl_slist_append(m_headers,
                                              "Content-Type: application/x-www-form-urlencoded;charset=utf-8");
                m_http_type = HTTPREQUEST_POSTFORM;
            }
            else
            {
                m_headers = curl_slist_append(m_headers,
                                              "Content-Type: application/json;charset=utf-8");
                m_http_type = HTTPREQUEST_POSTJSON;
            }
        }
    }

    void set_form_or_json(bool form_or_json)
    {
        m_form_or_json = form_or_json;
    }

private:
    bool m_form_or_json;
};

#endif //USELIBCURL_POSTREQUEST_H
