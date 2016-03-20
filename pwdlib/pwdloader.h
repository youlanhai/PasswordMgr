#ifndef PWDLOADER_H
#define PWDLOADER_H

#include <cstdint>

namespace pwd
{
    class Pwd;
    class PwdStream;

    class PwdLoader
    {
    public:
        PwdLoader();
        virtual ~PwdLoader();

        virtual bool load(Pwd &info, PwdStream &stream) = 0;
        virtual bool save(const Pwd &info, PwdStream &stream) = 0;
    };

    class PwdLoaderV0 : public PwdLoader
    {
    public:
        virtual bool load(Pwd &info, PwdStream &stream) override;
        virtual bool save(const Pwd &info, PwdStream &stream) override;
    };

    class PwdLoaderV1 : public PwdLoader
    {
    public:
        virtual bool load(Pwd &info, PwdStream &stream) override;
        virtual bool save(const Pwd &info, PwdStream &stream) override;
    };

    PwdLoader* createLoader(uint32_t version);
}

#endif // PWDLOADER_H
