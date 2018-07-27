//
// Created by yutianzuo on 2018/5/14.
//

#ifndef USELIBCURL_REQUESTMANAGER_H
#define USELIBCURL_REQUESTMANAGER_H

#include <map>
#include <string>
#include "threadpool.h"

#include "../request/getrequest.h"
#include "../request/postfilerequest.h"
#include "../request/postrequest.h"
#include "../request/downloadrequest.h"
#include "../request/putrequest.h"

class RequestManager
{
public:
    template<typename T>
    using MAP = std::map<T, T>;

    using STRING_MAP = MAP<std::string>;
    using STRING_MAP_IT = STRING_MAP::iterator;
    using CONST_STRING_MAP_IT = STRING_MAP::const_iterator;

    static void init(int n_threadpool_size)
    {
        if (!g_threadpool)
        {
            g_threadpool = new stdx::ThreadPool(n_threadpool_size, 16);
        }
    }


    static void uninit()
    {
        if (g_threadpool)
        {
            delete g_threadpool;
            g_threadpool = nullptr;
        }
    }


    /////////////////////////////////////////////////////////
    void add_basic_url_params(const std::string &param_key, const std::string &param_value)
    {
        m_basic_params[param_key] = param_value;
    }

    void add_basic_headers(const std::string &header_key, const std::string &header_value)
    {
        m_basic_headers[header_key] = header_value;
    }

    void set_host(const std::string &host)
    {
        m_host = host;
    }

    void set_cert_path(const std::string& str_cert)
    {
        m_cert_path = str_cert;
    }

    void set_proxy_path(const std::string& str_proxy)
    {
        m_proxy_path = str_proxy;
    }

    template<typename Callback>
    void get(const std::string &str_path, const STRING_MAP &headers,
             const STRING_MAP &url_params, Callback call_back, size_t seq)
    {
        HttpGetRequest *get = new HttpGetRequest();
        inner_add_headers(get, headers);
        std::string str_url = get_url(str_path, url_params);
        get->set_url(str_url);
        get->set_proxy(m_proxy_path);
        get->set_cert(m_cert_path);
        get->set_callback(call_back);
        get->set_request_seq(seq);

        g_threadpool->commit([get]() -> void
                             {
                                 get->go();
                                 delete get;
                             });

    }

    template<typename Callback>
    void post_form(const std::string &str_path, const STRING_MAP &headers,
                   const STRING_MAP &form_params, Callback call_back, size_t seq)
    {
        HttpPostFormDataRequest *post = new HttpPostFormDataRequest(true);
        inner_add_headers(post, headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        post->set_url(str_url);
        post->set_proxy(m_proxy_path);
        post->set_cert(m_cert_path);
        post->set_request_seq(seq);
        std::string str_form;
        for (CONST_STRING_MAP_IT it = form_params.begin(); it != form_params.end(); ++it)
        {
            str_form.append(it->first);
            str_form.append("=");
            str_form.append(it->second);
            str_form.append("&");
        }
        if (str_form.size())
        {
            str_form.erase(str_form.size() - 1);
        }
        post->set_postformdata(str_form);
        post->set_callback(call_back);


        g_threadpool->commit([post]() -> void
                             {
                                 post->go();
                                 delete post;
                             });
    }

    template<typename Callback>
    void post_json(const std::string &str_path, const STRING_MAP &headers,
                   const std::string &json, Callback call_back, size_t seq)
    {
        HttpPostFormDataRequest *post = new HttpPostFormDataRequest(false);
        inner_add_headers(post, headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        post->set_url(str_url);
        post->set_proxy(m_proxy_path);
        post->set_cert(m_cert_path);
        post->set_request_seq(seq);
        post->set_postformdata(json);
        post->set_callback(call_back);

        g_threadpool->commit([post]() -> void
                             {
                                 post->go();
                                 delete post;
                             });
    }

    template<typename Callback>
    void post_file(const std::string &str_path, const STRING_MAP &headers,
                   const std::string &str_formname, const STRING_MAP &form_params,
                   const std::string &str_jsonname, const std::string json,
                   const std::string &str_filekeyname, const std::string &str_filename,
                   const std::string &str_filepath, Callback call_back, size_t seq)
    {
        HttpPostFileRequest *post = new HttpPostFileRequest();
        inner_add_headers(post, headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        post->set_url(str_url); //should be invoke first
        post->set_proxy(m_proxy_path);
        post->set_cert(m_cert_path);
        post->set_request_seq(seq);
        if (str_filepath.size())
        {
            post->set_filepath(str_filekeyname, str_filepath, str_filename, true);
        }
        if (form_params.size())
        {
            std::string str_form;
            for (CONST_STRING_MAP_IT it = form_params.begin(); it != form_params.end(); ++it)
            {
                str_form.append(it->first);
                str_form.append("=");
                str_form.append(it->second);
                str_form.append("&");
            }
            if (str_form.size())
            {
                str_form.erase(str_form.size() - 1);
            }
            post->set_formdata(str_formname, str_form);
        }
        if (json.size())
        {
            post->set_jsondata(str_jsonname, json);
        }
        post->set_callback(call_back);
        g_threadpool->commit([post]() -> void
                             {
                                 post->go();
                                 delete post;
                             });

    }

    template<typename Callback>
    void put(const std::string &str_path, const std::string json,
             const STRING_MAP &headers, Callback callback, size_t seq
    )
    {
        HttpPutJsonRequest *put = new HttpPutJsonRequest();
        inner_add_headers(put, headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        put->set_url(str_url);
        put->set_proxy(m_proxy_path);
        put->set_json(json);
        put->set_cert(m_cert_path);
        put->set_callback(callback);
        put->set_request_seq(seq);

        g_threadpool->commit([put]() -> void
                             {
                                 put->go();
                                 delete put;
                             });
    }


    template<typename Callback>
    void download(const std::string &str_path, const STRING_MAP &headers, const STRING_MAP &url_params,
                  const std::string &str_download_filepath,
                  Callback callback, size_t seq)
    {
        HttpGetDownloadRequest *get = new HttpGetDownloadRequest();
        inner_add_headers(get, headers);
        std::string str_url = get_url(str_path, url_params);
        get->set_url(str_url);
        get->set_proxy(m_proxy_path);
        get->set_cert(m_cert_path);
        get->set_download_filepath(str_download_filepath);
        get->set_callback(callback);
        get->set_request_seq(seq);

        g_threadpool->commit([get]() -> void
                             {
                                 get->go();
                                 delete get;
                             });
    }


private:
    bool host_valid()
    {
        return m_host.find("http") == 0;
    }

    template<typename Request>
    void inner_add_headers(Request *request, const STRING_MAP &headers)
    {
        for (auto it = m_basic_headers.begin(); it != m_basic_headers.end(); ++it)
        {
            std::string header(it->first);
            header.append(":");
            header.append(it->second);
            request->add_header(header);
        }
        for (auto it = headers.begin(); it != headers.end(); ++it)
        {
            std::string header(it->first);
            header.append(":");
            header.append(it->second);
            request->add_header(header);
        }
    }

    std::string get_url(std::string str_path, const STRING_MAP &params)
    {
        std::string str_url;
        if (host_valid())
        {
            str_url = m_host;
            if (m_host.at(m_host.size() - 1) != '/')
            {
                str_url.append("/");
            }

            if (str_path.size() && str_path.at(0) == '/')
            {
                str_path.erase(0, 1);
            }

            str_url.append(str_path);
        }
        else
        {
            str_url = str_path;
        }

        if (!str_url.empty() && str_url.at(str_url.size() - 1) == '/')
        {
            str_url.erase(str_url.size() - 1);
        }

        if (!str_url.empty() && str_url.at(str_url.size() - 1) != '?')
        {
            str_url.append("?");
        }

        for (auto it = params.begin(); it != params.end(); ++it)
        {
            str_url.append(it->first);
            str_url.append("=");
            str_url.append(it->second);
            str_url.append("&");
        }

        for (auto it = m_basic_params.begin(); it != m_basic_params.end(); ++it)
        {
            str_url.append(it->first);
            str_url.append("=");
            str_url.append(it->second);
            str_url.append("&");
        }


        if (!str_url.empty() && str_url.at(str_url.size() - 1) == '&')
        {
            str_url.erase(str_url.size() - 1);
        }

        if (!str_url.empty() && str_url.at(str_url.size() - 1) == '?')
        {
            str_url.erase(str_url.size() - 1);
        }


        return str_url;
    }

    std::string m_host;
    std::string m_cert_path;
    std::string m_proxy_path;
    STRING_MAP m_basic_params;
    STRING_MAP m_basic_headers;

    static stdx::ThreadPool *g_threadpool;

};

stdx::ThreadPool *RequestManager::g_threadpool = nullptr;

#endif //USELIBCURL_REQUESTMANAGER_H
