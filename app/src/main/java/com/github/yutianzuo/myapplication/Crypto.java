package com.github.yutianzuo.myapplication;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class Crypto {
    /**
     * 传入文本内容，返回 SHA-256 串
     */
    public String SHA256(final String strText) {
        return SHA(strText, "SHA-256");
    }

    /**
     * 字符串 SHA 加密
     */
    private String SHA(final String strText, final String strType) {
        // 返回值
        String strResult = null;

        // 是否是有效字符串
        if (strText != null && strText.length() > 0) {
            try {
                // SHA 加密开始
                // 创建加密对象 并傳入加密類型
                MessageDigest messageDigest = MessageDigest.getInstance(strType);
                // 传入要加密的字符串
                messageDigest.update(strText.getBytes());
                // 得到 byte 類型结果
                byte byteBuffer[] = messageDigest.digest();

                // 將 byte 轉換爲 string
                StringBuffer strHexString = new StringBuffer();
                // 遍歷 byte buffer
                for (int i = 0; i < byteBuffer.length; i++) {
                    String hex = Integer.toHexString(0xff & byteBuffer[i]);
                    if (hex.length() == 1) {
                        strHexString.append('0');
                    }
                    strHexString.append(hex);
                }
                // 得到返回結果
                strResult = strHexString.toString();
            } catch (NoSuchAlgorithmException e) {
                e.printStackTrace();
            }
        }

        return strResult;
    }

    /**
     * 传入文本内容，返回 SHA-512 串
     */
    public String SHA512(final String strText) {
        return SHA(strText, "SHA-512");
    }

    public byte[] aesEncrypt(String key, String iv, String content) {
        byte[] byteRet = null;
        try {
            SecretKeySpec secretKey = new SecretKeySpec(key.getBytes("UTF-8"), "AES");
            byte[] initParam = iv.getBytes("UTF-8");
            IvParameterSpec ivParameterSpec = new IvParameterSpec(initParam);
            //https://stackoverflow.com/questions/29232705/encrypt-text-to-aes-cbc-pkcs7padding
            //Java only provides PKCS#5 padding, but it is the same as PKCS#7 padding. See this question on Crypto.SE:
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
            cipher.init(Cipher.ENCRYPT_MODE, secretKey, ivParameterSpec);
//            cipher.update(content.getBytes("UTF-8"));
            byteRet = cipher.doFinal(content.getBytes("UTF-8"));
//            strRet = Base64.getEncoder().encodeToString(result);
        } catch (Throwable e) {

        }
        return byteRet;
    }
}
