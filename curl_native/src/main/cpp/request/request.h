//
// Created by yutianzuo on 2018/5/8.
//

#ifndef USELIBCURL_REQUEST_H
#define USELIBCURL_REQUEST_H

#include "curl.h"
#include <string>
#include "../tools/timeutils.h"


/// typedef void(*CallBackFunc)(int, const std::string &, float, size_t, int, void *);

template<typename Derive>
class HttpRequest
{
public:
    enum
    {
        HTTPREQUEST_UNKNOWN = -1,
        HTTPREQUEST_GET = 0,
        HTTPREQUEST_POSTJSON = 1,
        HTTPREQUEST_POSTFORM = 2,
        HTTPREQUEST_PUT = 3,
        HTTPREQUEST_POSTFILE = 4,
        HTTPREQUEST_DOWNLOAD = 5,
        HTTPALIVE_TIME = 20 * 1000
    };
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

    std::function<void(int, const std::string &, float, size_t, int, void *)> m_callback;

    size_t m_request_seq;

    std::string m_cert_path;

    std::string m_proxy_path;

    int m_http_type = HTTPREQUEST_UNKNOWN;

    TimeUtils::elapsed_milli::rep m_url_reuse_time = 0;

public:
    enum
    {
        RESULT_TYPE_FAILED = -1,
        RESULT_TYPE_SUCCEES_ALL = 0,
        RESULT_TYPE_SUCCEES_DOWNLOAD_PROGRESS = 1,
        RESULT_TYPE_SUCCEES_UPLOAD_PROGRESS = 2,
        RESULT_TYPE_GET_DATA_PROGRESS = 3,
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

    bool can_reuse()
    {
        return is_valid() && TimeUtils::get_current_time() - m_url_reuse_time < HTTPALIVE_TIME;
    }

    void reuse_url()
    {
        clear_headers();
        if (is_valid())
        {
            curl_easy_reset(m_curl_handle);
        }
        m_url_reuse_time = TimeUtils::get_current_time();
    }

    void interrupt()
    {
        if (m_curl_handle)
        {
            curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT_MS, 1);
        }
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
        curl_easy_setopt(m_curl_handle, CURLOPT_AUTOREFERER, 1);
        curl_easy_setopt(m_curl_handle, CURLOPT_FOLLOWLOCATION, 1L); //follow location. e.g. 301/302/203
        curl_easy_setopt(m_curl_handle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL); // 301 302 303 post won't switch to get
        //https enable http2.0 by default;
        curl_easy_setopt(m_curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

#ifdef CURL_DEBUG
        curl_easy_setopt(m_curl_handle, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl_handle, CURLOPT_DEBUGFUNCTION, HttpRequest::debug_callback);
#endif

        (static_cast<Derive *>(this))->config_curl();
    }

#ifdef CURL_DEBUG

    static int debug_callback(CURL *curl, curl_infotype type, char *data, size_t size, void *userptr)
    {
        if (type == CURLINFO_TEXT || type == CURLINFO_HEADER_IN || type == CURLINFO_HEADER_OUT)
        {
            if (data != NULL && size > 0 && data[size - 1] == '\n')
            {
                size = size - 1;
            }
            std::cout << "libcurl: " << std::string(data, size);
        }
        return 0;
}

#endif

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

    template<typename T>
    void set_callback(T&& call_back)
    {
        m_callback = std::forward<T>(call_back);
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
