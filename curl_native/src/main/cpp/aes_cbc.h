//
// Created by yutianzuo on 2018/7/2.
//

#ifndef ANDROID_CURL_AES_CBC_H
#define ANDROID_CURL_AES_CBC_H

#include <cstdio>
//#include <openssl/evp.h>

/**
 * do_encrypt为1时加密，为0时解密。
 * opssl官方aes示例
 */
inline
int do_crypt(FILE *in, FILE *out, int do_encrypt)
{
    /* Allow enough space in output buffer for additional block */
//    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
//    int inlen, outlen;
//    EVP_CIPHER_CTX *ctx;
//    /*
//     * Bogus key and IV: we'd normally set these from
//     * another source.
//     */
//    unsigned char key[] = "0123456789abcdeF";
//    unsigned char iv[] = "1234567887654321";
//
//    /* Don't set key or IV right away; we want to check lengths */
//    ctx = EVP_CIPHER_CTX_new();
//    EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
//                      do_encrypt);
//    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
//    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
//
//    /* Now we can set key and IV */
//    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, do_encrypt);
//
//    for (;;) {
//        inlen = fread(inbuf, 1, 1024, in);
//        if (inlen <= 0)
//            break;
//        if (!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen)) {
//            /* Error */
//            EVP_CIPHER_CTX_free(ctx);
//            return 0;
//        }
//        fwrite(outbuf, 1, outlen, out);
//    }
//    if (!EVP_CipherFinal_ex(ctx, outbuf, &outlen)) {
//        /* Error */
//        EVP_CIPHER_CTX_free(ctx);
//        return 0;
//    }
//    fwrite(outbuf, 1, outlen, out);
//
//    EVP_CIPHER_CTX_free(ctx);
    return 1;
}

inline
std::string decrypt_aescbc(const unsigned char* entrytion_data, int len, std::string& str_key, std::string& str_iv)
{
//    unsigned char outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
//    int outlen;
//    EVP_CIPHER_CTX *ctx;
//    ctx = EVP_CIPHER_CTX_new();
//    EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
//                      0); //decryption
//    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
//    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
//    EVP_CipherInit_ex(ctx, NULL, NULL, (const unsigned char*)str_key.c_str(), (const unsigned char*)str_iv.c_str(), 0);
////    EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7); //openssl default
//    //assume buff is long enough
//    EVP_CipherUpdate(ctx, outbuf, &outlen, entrytion_data, len);
//    unsigned char* tmp = outbuf + outlen;
//    int final_len;
//    EVP_CipherFinal_ex(ctx, tmp, &final_len);
//    EVP_CIPHER_CTX_free(ctx);
//
//    std::string str_ret((const char*)outbuf, outlen + final_len);
//    return str_ret;
return  "";
}

#endif //ANDROID_CURL_AES_CBC_H
