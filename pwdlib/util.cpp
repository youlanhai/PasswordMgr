#include "util.h"
#include <cstdlib>
#include <cassert>

#include <openssl/aes.h>

namespace pwd
{
    void disorder(pwdstring & pwd)
    {
        size_t n = pwd.size();
        if (n == 0) return;

        for (size_t i = 0; i < n - 1; ++i)
        {
            int k = i + rand() % (n - i);
            std::swap(pwd[i], pwd[k]);
        }
    }

    void randomChoice(pwdstring & out, size_t n, const pwdstring & source)
    {
        out.clear();
        if (n == 0) return;

        size_t srcLen = source.size();
        if (srcLen == 0) return;

        for (size_t i = 0; i < n; ++i)
        {
            out.push_back(source[rand() % srcLen]);
        }
    }

    void genPwd(pwdstring & out, size_t n, const pwdstring & srcMust, const pwdstring & srcDefault)
    {
        if (n == 0)
        {
            //throw(std::invalid_argument("the num must not be 0!"));
            return;
        }
        if (srcMust.empty() && srcDefault.empty())
        {
            //throw(std::invalid_argument("srcMust and srcDefault, at least one is not empty!"));
            return;
        }

        out.clear();

        if (srcMust.size() > n)
        {
            randomChoice(out, n, srcMust);
        }
        else if (srcMust.size() < n)
        {
            if (!srcDefault.empty())
                randomChoice(out, n - srcMust.size(), srcDefault);
            else
                randomChoice(out, n - srcMust.size(), srcMust);
            out += srcMust;
        }
        else
        {
            out = srcMust;
        }

        disorder(out);
    }

    void genPwdByTypeRandom(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust)
    {
        if (n == 0)
        {
            //throw(std::invalid_argument("the num must not be 0!"));
            return;
        }

        pwdstring source;

        if (type & CT_LetterS) source += LetterS;
        if (type & CT_LetterB) source += LetterB;
        if (type & CT_Digit) source += Digit;
        if (type & CT_Special) source += Special;

        if (source.empty() && srcMust.empty())
        {
            //throw(std::invalid_argument("the type and srcMust, at least one must not be none!"));
            return;
        }

        disorder(source);
        genPwd(out, n, srcMust, source);
    }

    void genPwdByTypeStrict(pwdstring & out, size_t n, ChType type, const pwdstring & srcMust)
    {
        if (n == 0)
        {
            //throw(std::invalid_argument("the num must not be 0!"));
            return;
        }

        pwdstring source;
        pwdstring temp;

        if (type & CT_LetterS)
        {
            randomChoice(temp, 1, LetterS);
            source += temp;
        }

        if (type & CT_LetterB)
        {
            randomChoice(temp, 1, LetterB);
            source += temp;
        }

        if (type & CT_Digit)
        {
            randomChoice(temp, 1, Digit);
            source += temp;
        }

        if (type & CT_Special)
        {
            randomChoice(temp, 1, Special);
            source += temp;
        }

        if (source.empty() && srcMust.empty())
        {
            //throw(std::invalid_argument("the type and srcMust, at least one must not be none!"));
            return;
        }

        source += srcMust;//source与srcMust同级。
        disorder(source);

        if (source.size() > n)//srcMust太多，则从中随机选择。
        {
            randomChoice(out, n, source);
        }
        else if (source.size() < n)
        {
            genPwdByTypeRandom(out, n, type, source);
        }
        else
        {
            out = source;
        }
    }

    static const uchar EncryptVector[AES_BLOCK_SIZE] = {1, 2, 54, 32, 65, 235, 92, 43, 87, 4, 65, 12, 76, 34, 20, 84};

    static size_t alignSize(size_t size, size_t align)
    {
        assert(align > 0);
        return size_t((size + align - 1) / align) * align;
    }

    bool encryptData(streambuffer &inoutBuffer, const std::string &password)
    {
        std::string alignedPwd = password;
        alignedPwd.resize(16);

        AES_KEY key;
        if(AES_set_encrypt_key((const uchar*)alignedPwd.c_str(), 128, &key) < 0)
        {
            return false;
        }

        uchar iv[AES_BLOCK_SIZE];
        memcpy(iv, EncryptVector, sizeof(EncryptVector));

        size_t alignedSize = alignSize(inoutBuffer.size(), AES_BLOCK_SIZE);
        streambuffer temp(alignedSize);
        AES_cbc_encrypt(inoutBuffer.data(), temp.data(), inoutBuffer.size(), &key, iv, AES_ENCRYPT);
        temp.resize(inoutBuffer.size());

        inoutBuffer.swap(temp);
        return true;
    }

    bool decryptData(streambuffer &inoutBuffer, const std::string &password)
    {
        std::string alignedPwd = password;
        alignedPwd.resize(16);

        AES_KEY key;
        if(AES_set_decrypt_key((const uchar*)alignedPwd.c_str(), 128, &key) < 0)
        {
            return false;
        }

        uchar iv[AES_BLOCK_SIZE];
        memcpy(iv, EncryptVector, sizeof(EncryptVector));

        size_t alignedSize = alignSize(inoutBuffer.size(), AES_BLOCK_SIZE);
        streambuffer temp(alignedSize);
        AES_cbc_encrypt(inoutBuffer.data(), temp.data(), inoutBuffer.size(), &key, iv, AES_DECRYPT);
        temp.resize(inoutBuffer.size());

        inoutBuffer.swap(temp);
        return true;
    }

    void test_util()
    {
        streambuffer buffer = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e', 'f'};
        std::string password = "0123456789abcdefg";

        streambuffer temp = buffer;
        assert(temp == buffer);

        encryptData(temp, password);
        decryptData(temp, password);
        assert(temp == buffer);
    }
}
