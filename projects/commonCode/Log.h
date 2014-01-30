/** @file log.h
*  @brief loging functionalities
*
*	@author Bartlomiej Filipek 
*/

#pragma once

/** logging will be by using simple "printf" function */
//#define UTILS_LOG_WITH_PRINTF
#define UTILS_LOG_WITH_OUTPUT_DBG_STRING

#ifdef UTILS_LOG_WITH_PRINTF
    #define LOG(msg, ...)         { printf(msg, ##__VA_ARGS__); printf("\n"); }
    #define LOG_SUCCESS(msg, ...) { printf("SUCCESS: "); printf(msg, ##__VA_ARGS__); printf("\n"); }
    #define LOG_ERROR(msg, ...)   { printf("ERR in %s at line %d: ", __FUNCTION__, __LINE__); printf(msg, ##__VA_ARGS__); printf("\n"); }
#elif defined (UTILS_LOG_WITH_OUTPUT_DBG_STRING) && defined(WIN32)
    #define LOG(msg, ...)         { logger::winOutDebugString(0, 0, 0, 0, msg, ##__VA_ARGS__); }
    #define LOG_SUCCESS(msg, ...) { logger::winOutDebugString(0, 0, 0, "SUCCESS", msg, ##__VA_ARGS__); }
    #define LOG_ERROR(msg, ...)   { logger::winOutDebugString(__FUNCTION__, __FILE__, __LINE__, "ERR", msg, ##__VA_ARGS__); }
#else
    #define LOG(msg, ...)         __noop
    #define LOG_SUCCESS(msg, ...) __noop
    #define LOG_ERROR(msg, ...)   __noop
#endif

namespace logger
{
    inline const char *fileNameFromPath(const char *filePath);

#ifdef WIN32
    void winOutDebugString(const char *func, const char * file, int line, const char *msg, const char *userMsg, ...);
#endif
} // namespce log	


//
// inline
//

inline const char *logger::fileNameFromPath(const char *filePath)
{
    const char *s = strrchr(filePath, '\\');
    if (s == NULL) 
        s = strrchr(filePath, '/');
    return s == NULL ? filePath : s+1;
}
