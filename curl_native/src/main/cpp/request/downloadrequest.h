//
// Created by yutianzuo on 2018/5/9.
//

#ifndef USELIBCURL_DOWNLOADREQUEST_H
#define USELIBCURL_DOWNLOADREQUEST_H

#include <iostream>
#include "request.h"

class HttpGetDownloadRequest : public HttpRequest<HttpGetDownloadRequest>
{
public:
    HttpGetDownloadRequest(HttpGetDownloadRequest &&) = delete;

    void operator=(HttpGetDownloadRequest &&) = delete;

    HttpGetDownloadRequest(const HttpGetDownloadRequest &src) = delete;

    HttpGetDownloadRequest &operator=(const HttpGetDownloadRequest &src) = delete;

    HttpGetDownloadRequest() : HttpRequest<HttpGetDownloadRequest>()
    {

    }

    ~HttpGetDownloadRequest()
    {
        closefile();
    }

    void config_curl()
    {
        if (!is_valid())
        {
            return;
        }
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, write_file_callback);
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT, 600);
    }

    void set_download_filepath(const std::string &str_path)
    {
        m_file = ::fopen(str_path.c_str(), "w+b");
    }

    void closefile()
    {
        if (m_file)
        {
            ::fclose(m_file);
            m_file = nullptr;
        }
    }

private:
    FILE *m_file = nullptr;
    size_t m_file_len = 0;

    static size_t write_file_callback(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        auto p = (HttpGetDownloadRequest *) stream;
        if (!p || !p->m_curl_handle || !p->m_file)
        {
            return 0;
        }
        curl_off_t cl;
        curl_easy_getinfo(p->m_curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
        FILE *fp = p->m_file;
        size_t n_ret = ::fwrite(ptr, size, nmemb, fp);
        if (n_ret <= 0)
        {
            return 0;
        }
        else
        {
            p->m_file_len += n_ret;
            float persent = 0.0f;
            if (n_ret > 0 && n_ret <= cl)
            {
                persent = p->m_file_len / (float) cl;
                persent *= 100.0f;
                if (p->m_callback)
                {
                    p->m_callback(HttpGetDownloadRequest::RESULT_TYPE_SUCCEES_DOWNLOAD_PROGRESS, p->m_buffer, persent,
                    p->m_request_seq, 0, nullptr);
                }
            }
            return n_ret;
        }
    }
};

#endif //USELIBCURL_DOWNLOADREQUEST_H
