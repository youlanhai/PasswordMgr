#include "pwdlog.h"

#include <cstdarg>
#include <cstdio>

static const char* LogPrefix[] = {
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Assert",
};

const char* logLvl2Prefix(int lvl)
{
    if(lvl < PWD_LOG_LVL_DEBUG)
    {
        lvl = PWD_LOG_LVL_DEBUG;
    }
    else if(lvl > PWD_LOG_LVL_ASSERT)
    {
        lvl = PWD_LOG_LVL_ASSERT;
    }
    return LogPrefix[lvl];
}

void pwdLog(int lvl, const char *format, ...)
{
    printf("[%s]", logLvl2Prefix(lvl));

    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);

    printf("\n");
    fflush(stdout);
}
