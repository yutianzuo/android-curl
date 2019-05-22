//
// Created by yutianzuo on 2018/5/15.
//

#ifndef USELIBCURL_HTTPMANAGER_H
#define USELIBCURL_HTTPMANAGER_H

#include "requestmanager.h"
#include <map>
#include <mutex>

class HttpManager
{
public:
    /**
     * NOT thread-safe, better init this at the beginning of application
     */
    static HttpManager *get_instance()
    {
        if (!g_manager)
        {
            g_manager = new HttpManager();
        }
        return g_manager;
    }

    HttpManager(const HttpManager &) = delete;

    static void init(std::size_t threadpool)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        RequestManager::init(threadpool);
    }

    /**
     * may block,depends threads condition,so call this when application will exit, better invoke in a workthread
     * or
     * dont invoke this, kill process instead
     */
    static void uninit()
    {
        RequestManager::uninit();
        curl_global_cleanup();
        if (g_manager)
        {
            delete g_manager;
            g_manager = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~HttpManager()
    {
        for (auto p : m_request_managers)
        {
            if (p.second)
            {
                delete (p.second);
            }
        }
        m_request_managers.clear();
    }

    RequestManager *get_request_manager(const std::string &str_key)
    {
        RequestManager *p;
        if (m_request_managers.find(str_key) == m_request_managers.end())
        {
            p = new RequestManager();
            m_request_managers[str_key] = p;
        }
        else
        {
            p = m_request_managers.at(str_key);
        }
        return p;
    }

private:
    HttpManager()
    {}

    using MAP = std::map<std::string, RequestManager *>;
    MAP m_request_managers;
    std::mutex m_mutex;
    static HttpManager *g_manager;
};

HttpManager *HttpManager::g_manager = nullptr;

#endif //USELIBCURL_HTTPMANAGER_H
