#ifndef __LOGGER_H_
#define __LOGGER_H_

extern char g_error_file_prefix[64];

extern void logError(const char *format,...);
extern void logErrorEx(const char* prefix, const char* format, ...);
extern void logInfo(const char* prefix, const char* format, ...);

#endif