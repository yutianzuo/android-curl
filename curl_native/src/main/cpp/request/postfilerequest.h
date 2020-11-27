//
// Created by yutianzuo on 2018/5/8.
//

#ifndef USELIBCURL_POSTFILEREQUEST_H
#define USELIBCURL_POSTFILEREQUEST_H

#include "request.h"
#include <fstream>

class HttpPostFileRequest : public HttpRequest<HttpPostFileRequest>
{
public:
    HttpPostFileRequest(HttpPostFileRequest &&) = delete;

    void operator=(HttpPostFileRequest &&) = delete;

    HttpPostFileRequest &operator=(const HttpPostFileRequest &src) = delete;

    HttpPostFileRequest(const HttpPostFileRequest &src) = delete;

    HttpPostFileRequest() : HttpRequest<HttpPostFileRequest>()
    {
        m_http_type = HTTPREQUEST_POSTFILE;
    }

    ~HttpPostFileRequest()
    {
        if (m_mime)
        {
            curl_mime_free(m_mime);
            m_mime = nullptr;
        }
        if (m_file.is_open())
        {
            m_file.close();
        }
    }
    int get_request_type(){return m_http_type;}
    void set_formdata(const std::string &str_name, const std::string &str_data)
    {
        if (!is_valid() || !is_mime_valid())
        {
            return;
        }
        curl_mimepart *part = curl_mime_addpart(m_mime);
        curl_mime_data(part, str_data.c_str(), str_data.size());
        curl_mime_type(part, "application/x-www-form-urlencoded;charset=utf-8");
        curl_mime_name(part, str_name.c_str());
    }

    void set_jsondata(const std::string &str_name, const std::string &str_data)
    {
        if (!is_valid() || !is_mime_valid())
        {
            return;
        }
        curl_mimepart *part = curl_mime_addpart(m_mime);
        curl_mime_data(part, str_data.c_str(), str_data.size());
        curl_mime_type(part, "application/json;charset=utf-8");
        curl_mime_name(part, str_name.c_str());
    }

    void set_filepath(const std::string &str_name, const std::string &str_path, const std::string &str_filename,
                      bool need_persent = false)
    {
        if (!is_valid() || !is_mime_valid())
        {
            return;
        }
        curl_mimepart *part = curl_mime_addpart(m_mime);
        curl_mime_name(part, str_name.c_str());
        if (need_persent)
        {
            m_file.open(str_path);
            if (m_file.is_open())
            {
                m_file.seekg(0, std::ios::seekdir::end);
                m_filesize = m_file.tellg();
                m_uploadsize = 0;
                m_file.seekg(0);
                curl_mime_data_cb(part, m_filesize, readfunc, nullptr, nullptr, this);
                curl_mime_type(part, "application/octet-stream");
            }
        }
        else
        {
            curl_mime_filedata(part, str_path.c_str());
        }
        curl_mime_filename(part, str_filename.c_str());
        curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT, 600);
    }

    void config_curl()
    {
        if (!is_valid())
        {
            return;
        }
        m_mime = curl_mime_init(m_curl_handle);
        if (!is_mime_valid())
        {
            return;
        }
        curl_easy_setopt(m_curl_handle, CURLOPT_MIMEPOST, m_mime);
    }

private:
    static size_t readfunc(char *buffer, size_t size, size_t nitems, void *arg)
    {
        auto p = (HttpPostFileRequest *) arg;
        if (p && p->m_file)
        {
            p->m_file.read(buffer, size * nitems);
            size_t n_ret = p->m_file.gcount();

            if (n_ret == 0)
            {
                p->m_file.close();
            }
            else
            {
                p->m_uploadsize += n_ret;
                if (p->m_callback)
                {
                    float persent = p->m_uploadsize / (float) p->m_filesize;
                    persent *= 100.0f;
                    p->m_callback(HttpPostFileRequest::RESULT_TYPE_SUCCEES_UPLOAD_PROGRESS, p->m_buffer,
                                  persent, p->m_request_seq, 0, nullptr);
                }
            }
            return n_ret;
        }
        else
        {
            return 0;
        }

    }

    bool is_mime_valid()
    {
        return m_mime != nullptr;
    }

    curl_mime *m_mime = nullptr;
    std::ifstream m_file;
    long m_filesize = 0;
    long m_uploadsize = 0;

};

#endif //USELIBCURL_POSTFILEREQUEST_H
