//
// Created by yutianzuo on 2018/6/28.
//

#ifndef ANDROID_CURL_SHA_H
#define ANDROID_CURL_SHA_H

#include <string>
#include <openssl/sha.h>

inline
std::string sha256(const std::string str)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string NewString = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        NewString = NewString + buf;
    }
    return NewString;
}

#endif //ANDROID_CURL_SHA_H
