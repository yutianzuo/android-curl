//
// Created by yutianzuo on 2018/5/8.
//

#ifndef USELIBCURL_REQUEST_H
#define USELIBCURL_REQUEST_H

#include "curl.h"
#include <string>


typedef void(*CallBackFunc)(int, const std::string &, float, size_t, int, void *);

template<typename Derive, typename CallBack = CallBackFunc>
class HttpRequest
{
protected:
    HttpRequest()
    {
        m_curl_handle = curl_easy_init();
    }

    ~HttpRequest()
    {
        if (m_curl_handle)
        {
            curl_easy_cleanup(m_curl_handle);
            m_curl_handle = nullptr;
        }
        clear_headers();
    }


    CURL *m_curl_handle = nullptr;
    std::string m_buffer;
    curl_slist *m_headers = nullptr;

    bool is_valid() const
    {
        return m_curl_handle != nullptr;
    }

    void clear_headers()
    {
        if (m_headers)
        {
            curl_slist_free_all(m_headers);
            m_headers = nullptr;
        }
    }

    CallBack m_callback = nullptr;

    size_t m_request_seq;

    std::string m_cert_path;

    std::string m_proxy_path;

public:
    enum
    {
        RESULT_TYPE_FAILED = -1,
        RESULT_TYPE_SUCCEES_ALL = 0,
        RESULT_TYPE_SUCCEES_DOWNLOAD_PROGRESS = 1,
        RESULT_TYPE_SUCCEES_UPLOAD_PROGRESS = 2,
    };


    HttpRequest(HttpRequest &&rvalue) = delete;

    void operator=(HttpRequest &&) = delete;

    HttpRequest &operator=(const HttpRequest &src) = delete;

    HttpRequest(const HttpRequest &src) = delete;

    void add_header(const std::string &str_header)
    {
        if (!is_valid())
        {
            return;
        }
        m_headers = curl_slist_append(m_headers, str_header.c_str());
    }

    void set_url(const std::string &str_url, bool skip_ssl = false)
    {
        if (!is_valid())
        {
            return;
        }
        curl_easy_setopt(m_curl_handle, CURLOPT_URL, str_url.c_str());

        if (skip_ssl)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYHOST, 0);
        }
        else
        {
            if (!m_cert_path.empty())
            {
                curl_easy_setopt(m_curl_handle, CURLOPT_CAINFO, m_cert_path.c_str());
            }
        }
        m_buffer.clear();
        curl_easy_setopt(m_curl_handle, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(m_curl_handle, CURLOPT_ACCEPT_ENCODING, "");/* enable all supported built-in compressions */
        //this enable gzip if support, and auto decompress the gzip reponse
        curl_easy_setopt(m_curl_handle, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(m_curl_handle, CURLOPT_HEADER, 0); //是否输出header信息
        curl_easy_setopt(m_curl_handle, CURLOPT_CONNECTTIMEOUT, 10); // set transport and time out time
        curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT, 15);
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, (void *) &m_buffer);

//        m_headers = curl_slist_append(m_headers, "Accept: text/html;charset=UTF-8");
//        m_headers = curl_slist_append(m_headers, "Accept-Charset: ISO-8859-1");

        (static_cast<Derive *>(this))->config_curl();
    }

    void set_proxy(const std::string& str_proxy)
    {
        m_proxy_path = str_proxy;
        if (!m_proxy_path.empty() && m_proxy_path.find("http") != std::string::npos)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_PROXY, m_proxy_path.c_str());
        }
    }

    void set_cert(const std::string &cert_path)
    {
        m_cert_path = cert_path;
        if (!m_cert_path.empty())
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_CAINFO, m_cert_path.c_str());
        }
    }

    size_t get_request_seq()
    {
        return m_request_seq;
    }

    void set_request_seq(size_t seq)
    {
        m_request_seq = seq;
    }

    /**
     * use tmplate impl polymorphic
     * subclass could overwrite this func
     * to add additional config of curl
     */
    void config_curl()
    {

    }


    void set_callback(CallBack call_back)
    {
        m_callback = call_back;
    }


    void go() const
    {
        if (!is_valid())
        {
            return;
        }
        CURLcode ret;
        if (m_headers)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_HTTPHEADER, m_headers);
        }

        ret = curl_easy_perform(m_curl_handle);
        if (ret == CURLE_OK)
        {
            //deal the respones m_buffer
            if (m_callback)
            {
                m_callback(RESULT_TYPE_SUCCEES_ALL, m_buffer, 0.0f, m_request_seq, ret, nullptr);
            }
        }
        else
        {
            //callback err
            if (m_callback)
            {
                m_callback(RESULT_TYPE_FAILED, m_buffer, 0.0f, m_request_seq, ret, nullptr);
            }
        }
    }

    static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::string *str = (std::string *) stream;
        (*str).append((char *) ptr, size * nmemb);
        return size * nmemb;
    }

};

#endif //USELIBCURL_REQUEST_H
