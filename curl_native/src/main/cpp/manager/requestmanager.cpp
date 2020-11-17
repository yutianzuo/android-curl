//
// Created by tianzuo yu on 2020/11/16.
//

#ifndef USELIBCURL_REQUESTMANAGER_CPP
#define USELIBCURL_REQUESTMANAGER_CPP

#include "requestmanager.h"
#include "httpmanager.h"

HttpManager *HttpManager::g_manager = nullptr;
stdx::ThreadPool *RequestManager::g_threadpool = nullptr;
int RequestManager::POOL_SIZE = 1;

#endif //USELIBCURL_REQUESTMANAGER_CPP
