#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_MAX 100

#define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO

extern void Dprintf(int level, const char *fmt, ...);
extern void vDprintf(int level, const char *fmt, va_list ap);
extern int log_level;

#define PRINT(_level, _fmt, _args...) Dprintf(_level, _fmt, ##_args)

#define TSTM(x,y, _fmt, _args...)						\
	do if (!(x)) { 								\
			PRINT(LOG_LEVEL_ERROR,					\
				"Error in %s at %s:%d verifying %s. " _fmt,	\
				__PRETTY_FUNCTION__, __FILE__, __LINE__, 	\
				#x, ##_args);					\
			return y;						\
       } while (0)

#define TST(x,y) TSTM(x,y,"")

#define LOG(_fmt, _args...) \
	PRINT(LOG_LEVEL_DEBUG, "%s: " _fmt, __PRETTY_FUNCTION__, ##_args)

#define INFO(_fmt, _args...) \
	PRINT(LOG_LEVEL_INFO, "%s: " _fmt, __PRETTY_FUNCTION__, ##_args)

#define ERROR(_fmt, _args...) \
	PRINT(LOG_LEVEL_ERROR, "%s: " _fmt,__PRETTY_FUNCTION__,  ##_args)


#endif
