#ifndef PWDLOG_H
#define PWDLOG_H

void pwdLog(int lvl, const char *format, ...);

#define PWD_LOG_LVL_DEBUG   0
#define PWD_LOG_LVL_INFO    1
#define PWD_LOG_LVL_WARN    2
#define PWD_LOG_LVL_ERROR   3
#define PWD_LOG_LVL_ASSERT  4

#ifndef PWD_LOG_LVL
#ifdef DEBUG
#define PWD_LOG_LVL     PWD_LOG_LVL_DEBUG
#else
#define PWD_LOG_LVL     PWD_LOG_LVL_ERROR
#endif
#endif

#if PWD_LOG_LVL_DEBUG >= PWD_LOG_LVL
#define PWD_LOG_DEBUG(...)      pwdLog(PWD_LOG_LVL_DEBUG, __VA_ARGS__)
#else
#define PWD_LOG_DEBUG(...)
#endif

#if PWD_LOG_LVL_INFO >= PWD_LOG_LVL
#define PWD_LOG_INFO(...)       pwdLog(PWD_LOG_LVL_INFO, __VA_ARGS__)
#else
#define PWD_LOG_INFO(...)
#endif

#if PWD_LOG_LVL_WARN >= PWD_LOG_LVL
#define PWD_LOG_WARN(...)       pwdLog(PWD_LOG_LVL_WARN, __VA_ARGS__)
#else
#define PWD_LOG_WARN(...)
#endif

#if PWD_LOG_LVL_ERROR >= PWD_LOG_LVL
#define PWD_LOG_ERROR(...)      pwdLog(PWD_LOG_LVL_ERROR, __VA_ARGS__)
#else
#define PWD_LOG_ERROR(...)
#endif

#if PWD_LOG_LVL_ASSERT >= PWD_LOG_LVL
#define PWD_LOG_ASSERT(...)     pwdLog(PWD_LOG_LVL_ASSERT, __VA_ARGS__)
#else
#define PWD_LOG_ASSERT(...)
#endif

#endif // PWDLOG_H
