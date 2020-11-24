//
// Created by yutianzuo on 2018/5/14.
//

#ifndef USELIBCURL_REQUESTMANAGER_H
#define USELIBCURL_REQUESTMANAGER_H

#include <map>
#include <string>
#include <deque>
#include <memory>
#include "threadpool.h"

#include "../request/getrequest.h"
#include "../request/postfilerequest.h"
#include "../request/postrequest.h"
#include "../request/downloadrequest.h"
#include "../request/putrequest.h"

/*
 * 对curl链接进行了cache，只cache了postform、postjson、get、put四种请求，这四种请求是业务种最频繁常用的，值得cache的。
 * 每一种类型的请求cache数量阈值定义为线程池传入大小。时间阈值，在request中定义。
 * cache可以充分利用http1.1复用链接的好处，也可以充分利用tls一次session的重用，节省时间和算力。
 */
class RequestManager : public std::enable_shared_from_this<RequestManager>
{
public:
    template<typename T>
    using MAP = std::map<T, T>;

    using STRING_MAP = MAP<std::string>;
    using STRING_MAP_IT = STRING_MAP::iterator;
    using CONST_STRING_MAP_IT = STRING_MAP::const_iterator;


    static void init(int n_threadpool_size)
    {
        POOL_SIZE = n_threadpool_size;
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

    ~RequestManager()
    {
        clear_pool();
#ifdef CURL_DEBUG
        std::cout<< "~RequestManager" << std::endl;
#endif
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

        std::shared_ptr<HttpGetRequest> get =
                get_cached_url<HttpGetRequest>(HttpRequest<HttpGetRequest>::HTTPREQUEST_GET, m_get_url_pool);
        if (!get)
        {
            get = std::make_shared<HttpGetRequest>();
        }
        inner_add_headers(get.get(), headers);
        std::string str_url = get_url(str_path, url_params);
        get->set_url(str_url);
        get->set_proxy(m_proxy_path);
        get->set_cert(m_cert_path);
        get->set_callback(call_back);
        get->set_request_seq(seq);

        std::shared_ptr<RequestManager> self = shared_from_this();
        g_threadpool->commit([get, self]() -> void
                             {
                                 get->go();
                                 get->reuse_url();
                                 self->add_cache_url<HttpGetRequest>(HttpRequest<HttpGetRequest>::HTTPREQUEST_GET, get,
                                                               self->m_get_url_pool);
                             });

    }

    template<typename Callback>
    void post_form(const std::string &str_path, const STRING_MAP &headers,
                   const STRING_MAP &form_params, Callback call_back, size_t seq)
    {
        std::shared_ptr<HttpPostFormDataRequest> post =
                get_cached_url<HttpPostFormDataRequest>(HttpRequest<HttpPostFormDataRequest>::HTTPREQUEST_POSTFORM,
                                                        m_post_url_pool);
        if (!post)
        {
            post = std::make_shared<HttpPostFormDataRequest>(true);
        }
        else
        {
            post->set_form_or_json(true);
        }
        inner_add_headers(post.get(), headers);
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

        std::shared_ptr<RequestManager> self = shared_from_this();
        g_threadpool->commit([post, self]() -> void
                             {
                                 post->go();
                                 post->reuse_url();
                                 self->add_cache_url<HttpPostFormDataRequest>(
                                         HttpRequest<HttpPostFormDataRequest>::HTTPREQUEST_POSTFORM, post,
                                         self->m_post_url_pool);
                             });
    }

    template<typename Callback>
    void post_json(const std::string &str_path, const STRING_MAP &headers,
                   const std::string &json, Callback call_back, size_t seq)
    {
        std::shared_ptr<HttpPostFormDataRequest> post =
                get_cached_url<HttpPostFormDataRequest>(HttpRequest<HttpPostFormDataRequest>::HTTPREQUEST_POSTJSON,
                                                        m_post_url_pool);
        if (!post)
        {
            post = std::make_shared<HttpPostFormDataRequest>(false);
        }
        else
        {
            post->set_form_or_json(false);
        }
        inner_add_headers(post.get(), headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        post->set_url(str_url);
        post->set_proxy(m_proxy_path);
        post->set_cert(m_cert_path);
        post->set_request_seq(seq);
        post->set_postformdata(json);
        post->set_callback(call_back);

        std::shared_ptr<RequestManager> self = shared_from_this();
        g_threadpool->commit([post, self]() -> void
                             {
                                 post->go();
                                 post->reuse_url();
                                 self->add_cache_url<HttpPostFormDataRequest>(
                                         HttpRequest<HttpPostFormDataRequest>::HTTPREQUEST_POSTJSON, post,
                                         self->m_post_url_pool);
                             });
    }

    template<typename Callback>
    void post_file(const std::string &str_path, const STRING_MAP &headers,
                   const std::string &str_formname, const STRING_MAP &form_params,
                   const std::string &str_jsonname, const std::string json,
                   const std::string &str_filekeyname, const std::string &str_filename,
                   const std::string &str_filepath, Callback call_back, size_t seq)
    {
        std::shared_ptr<HttpPostFileRequest> post = std::make_shared<HttpPostFileRequest>();
        inner_add_headers(post.get(), headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        post->set_url(str_url); //should be invoke first
        post->set_proxy(m_proxy_path);
        post->set_cert(m_cert_path);
        post->set_request_seq(seq);
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
        if (str_filepath.size())
        {
            post->set_filepath(str_filekeyname, str_filepath, str_filename, true);
        }
        post->set_callback(call_back);
        g_threadpool->commit([post]() -> void
                             {
                                 post->go();
                             });

    }

    template<typename Callback>
    void put(const std::string &str_path, const std::string json,
             const STRING_MAP &headers, Callback callback, size_t seq
    )
    {
        std::shared_ptr<HttpPutJsonRequest> put =
                get_cached_url<HttpPutJsonRequest>(HttpRequest<HttpPutJsonRequest>::HTTPREQUEST_PUT,
                                                   m_put_url_pool);
        if (!put)
        {
            put = std::make_shared<HttpPutJsonRequest>();
        }
        inner_add_headers(put.get(), headers);
        std::string str_url = get_url(str_path, STRING_MAP());
        put->set_url(str_url);
        put->set_proxy(m_proxy_path);
        put->set_json(json);
        put->set_cert(m_cert_path);
        put->set_callback(callback);
        put->set_request_seq(seq);

        std::shared_ptr<RequestManager> self = shared_from_this();
        g_threadpool->commit([put, self]() -> void
                             {
                                 put->go();
                                 put->reuse_url();
                                 self->add_cache_url<HttpPutJsonRequest>(
                                         HttpRequest<HttpPutJsonRequest>::HTTPREQUEST_PUT, put,
                                         self->m_put_url_pool);
                             });
    }


    template<typename Callback>
    void download(const std::string &str_path, const STRING_MAP &headers, const STRING_MAP &url_params,
                  const std::string &str_download_filepath,
                  Callback callback, size_t seq)
    {
        std::shared_ptr<HttpGetDownloadRequest> get = std::make_shared<HttpGetDownloadRequest>();
        inner_add_headers(get.get(), headers);
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

    template<class T>
    std::shared_ptr<T> get_cached_url(int http_type, std::deque<std::shared_ptr<T> >& pool)
    {
        std::lock_guard<std::mutex> lock(m_pool_lock);
        if (http_type == HttpRequest<T>::HTTPREQUEST_GET || http_type == HttpRequest<T>::HTTPREQUEST_POSTFORM ||
            http_type == HttpRequest<T>::HTTPREQUEST_POSTJSON || HttpRequest<T>::HTTPREQUEST_PUT)
        {
            while (pool.size() > 0)
            {
                if (pool.front()->can_reuse())
                {
                    std::shared_ptr<T> p = pool.front();
                    pool.pop_front();
#ifdef CURL_DEBUG
                    std::cout << "reuse http_request" << std::endl;
#endif
                    return p;
                }
                else
                {
                    pool.pop_front();
                    continue;
                }
            }
        }
#ifdef CURL_DEBUG
        std::cout << "no reuse http_request" << std::endl;
#endif
        return std::shared_ptr<T>();
    }

    template<class T>
    void add_cache_url(int http_type, const std::shared_ptr<T>& p, std::deque<std::shared_ptr<T> >& pool)
    {
        std::lock_guard<std::mutex> lock(m_pool_lock);
        if (http_type == HttpRequest<T>::HTTPREQUEST_GET || http_type == HttpRequest<T>::HTTPREQUEST_POSTFORM ||
                http_type == HttpRequest<T>::HTTPREQUEST_POSTJSON || HttpRequest<T>::HTTPREQUEST_PUT)
        {
            if (pool.size() >= POOL_SIZE)
            {
                pool.pop_back();
            }
            pool.push_front(p);
        }
    }

    void clear_pool()
    {
        std::lock_guard<std::mutex> lock(m_pool_lock);
        m_get_url_pool.clear();
        m_put_url_pool.clear();
        m_post_url_pool.clear();
    }


    std::string m_host;
    std::string m_cert_path;
    std::string m_proxy_path;
    STRING_MAP m_basic_params;
    STRING_MAP m_basic_headers;

    std::mutex m_pool_lock;

    ///just cache get post and put.
    std::deque<std::shared_ptr<HttpGetRequest> > m_get_url_pool;
    std::deque<std::shared_ptr<HttpPostFormDataRequest> > m_post_url_pool;
    std::deque<std::shared_ptr<HttpPutJsonRequest> > m_put_url_pool;

    static stdx::ThreadPool *g_threadpool;
    static int POOL_SIZE;

};



#endif //USELIBCURL_REQUESTMANAGER_H
