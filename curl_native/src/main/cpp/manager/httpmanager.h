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
        m_request_managers.clear();
#ifdef CURL_DEBUG
        std::cout<< "~HttpManager" << std::endl;
#endif
    }

    std::shared_ptr<RequestManager> get_request_manager(const std::string &str_key)
    {
        std::shared_ptr<RequestManager> sp;
        if (m_request_managers.find(str_key) == m_request_managers.end())
        {
            sp = std::make_shared<RequestManager>();
            m_request_managers[str_key] = sp;
        }
        else
        {
            sp = m_request_managers.at(str_key);
        }
        return sp;
    }

    void manual_lock()
    {
        try
        {
            m_lock.lock();
        } catch (...)
        {}
    }

    void manual_unlock()
    {
        try
        {
            m_lock.unlock();
        } catch (...)
        {}
    }


private:
    HttpManager() : m_lock(m_mutex, std::defer_lock)
    {}

    using MAP = std::map<std::string, std::shared_ptr<RequestManager> >;
    MAP m_request_managers;
    std::unique_lock<std::mutex> m_lock;
    std::mutex m_mutex;
    static HttpManager *g_manager;
};



#endif //USELIBCURL_HTTPMANAGER_H
