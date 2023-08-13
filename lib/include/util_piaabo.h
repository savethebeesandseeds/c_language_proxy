#ifndef __UTILS_PIAABO
#define __UTILS_PIAABO
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define SOCKET int
#define socket_errno() (errno)

#define LOG_FILE stdout
#define LOG_ERR_FILE stderr
#define LOG_WARN_FILE stdout

#define ANSI_COLOR_ERROR "\x1b[41m" 
#define ANSI_COLOR_SUCCESS "\x1b[42m" 
#define ANSI_COLOR_WARNING "\x1b[43m" 
#define ANSI_COLOR_RESET "\x1b[0m" 
#define ANSI_COLOR_Black "\x1b[30m" 
#define ANSI_COLOR_Red "\x1b[31m"     
#define ANSI_COLOR_Green "\x1b[32m"   
#define ANSI_COLOR_Yellow "\x1b[33m" 
#define ANSI_COLOR_Blue "\x1b[34m"  
#define ANSI_COLOR_Magenta "\x1b[35m" 
#define ANSI_COLOR_Cyan "\x1b[36m"    
#define ANSI_COLOR_White "\x1b[37m" 
#define ANSI_COLOR_Bright_Black_Grey "\x1b[90m" 
#define ANSI_COLOR_Bright_Red "\x1b[91m" 
#define ANSI_COLOR_Bright_Green "\x1b[92m" 
#define ANSI_COLOR_Bright_Yellow "\x1b[93m"     
#define ANSI_COLOR_Bright_Blue "\x1b[94m" 
#define ANSI_COLOR_Bright_Magenta "\x1b[95m" 
#define ANSI_COLOR_Bright_Cyan "\x1b[96m"       
#define ANSI_COLOR_Bright_White "\x1b[97m"

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
/* This log functionality checks if there is a pendding log for the error trasported by the errno.h lib, 
 * WARNING! This functionaly sets the errno=0 if the errno is futher required, this might be not a desired behaviour.
 */
#define wrap_log_sys_err() {\
  if(errno != 0) {\
    pthread_mutex_lock(&log_mutex);\
    fprintf(LOG_ERR_FILE,"[%s0x%lX%s]: %sSYS ERRNO%s: ",\
      ANSI_COLOR_Cyan,pthread_self(),ANSI_COLOR_RESET,\
      ANSI_COLOR_ERROR,ANSI_COLOR_RESET);\
    fprintf(LOG_ERR_FILE,"%s\n", strerror(errno));\
    fflush(LOG_ERR_FILE);\
    pthread_mutex_unlock(&log_mutex);\
    errno = 0;\
  }\
}
/* This log functionality marks the thread id, so that log messages are linked to the request thread */
#define log(...) {\
  wrap_log_sys_err();\
  pthread_mutex_lock(&log_mutex);\
  fprintf(LOG_FILE,"[%s0x%lX%s]: ",\
    ANSI_COLOR_Cyan,pthread_self(),ANSI_COLOR_RESET);\
  fprintf(LOG_FILE,__VA_ARGS__);\
  fflush(LOG_FILE);\
  pthread_mutex_unlock(&log_mutex);\
}
/* This log functionality marks the thread id, so that log messages are linked to the request thread */
#define log_dbg(...) {\
  wrap_log_sys_err();\
  pthread_mutex_lock(&log_mutex);\
  fprintf(LOG_ERR_FILE,"[%s0x%lX%s]: %sDEBUG%s: ",\
    ANSI_COLOR_Cyan,pthread_self(),ANSI_COLOR_RESET,\
    ANSI_COLOR_Bright_Blue,ANSI_COLOR_RESET);\
  fprintf(LOG_ERR_FILE,__VA_ARGS__);\
  fflush(LOG_ERR_FILE);\
  pthread_mutex_unlock(&log_mutex);\
}
/* This log functionality marks the thread id, so that log messages are linked to the request thread */
#define log_err(...) {\
  wrap_log_sys_err();\
  pthread_mutex_lock(&log_mutex);\
  fprintf(LOG_ERR_FILE,"[%s0x%lX%s]: %sERROR%s: ",\
    ANSI_COLOR_Cyan,pthread_self(),ANSI_COLOR_RESET,\
    ANSI_COLOR_ERROR,ANSI_COLOR_RESET);\
  fprintf(LOG_ERR_FILE,__VA_ARGS__);\
  fflush(LOG_ERR_FILE);\
  pthread_mutex_unlock(&log_mutex);\
}
/* This log functionality marks the thread id, so that log messages are linked to the request thread */
#define log_warn(...) {\
  wrap_log_sys_err();\
  pthread_mutex_lock(&log_mutex);\
  fprintf(LOG_WARN_FILE,"[%s0x%lX%s]: %sWARNING%s: ",\
    ANSI_COLOR_Cyan,pthread_self(),ANSI_COLOR_RESET,\
    ANSI_COLOR_WARNING,ANSI_COLOR_RESET);\
  fprintf(LOG_WARN_FILE,__VA_ARGS__);\
  fflush(LOG_WARN_FILE);\
  pthread_mutex_unlock(&log_mutex);\
}
/* this function prints boolean values */
#define print_bool(dinput) dinput? "True":"False"

/* functionaly to abort on Crtl-C 
*  Usage: 
      #include <signal.h>
      signal(SIGINT,exitHandler); // signal from Ctrl+c
      signal(SIGTSTP,exitHandler); // signal from Ctrl+z
*/
void exitHandler(int signal_value){
  signal(signal_value, SIG_IGN);
  log_warn("Abort signal detected, ending program!\n");
  exit(0);
}
/* functionaly to encodebase64 */
static char *encode_base64(const char *message){
  /* valid characters */
  const char *lookup =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  /* temporal variables */
  unsigned long l;
  char *b64;
  size_t length = strlen (message);
  /* allocate return (must be free) */
  b64 = (char*)malloc (length * 2);
  if (NULL == b64)
    return b64;
  /* initialzie return */
  b64[0] = 0;
  /* encode base64 */
  for (size_t i = 0; i < length; i += 3){
    l = (((unsigned long) message[i]) << 16)
      | (((i + 1) < length) ? (((unsigned long) message[i + 1]) << 8) : 0)
      | (((i + 2) < length) ? ((unsigned long) message[i + 2]) : 0);
    strncat (b64, &lookup[(l >> 18) & 0x3F], 1);
    strncat (b64, &lookup[(l >> 12) & 0x3F], 1);
    if (i + 1 < length)
      strncat (b64, &lookup[(l >> 6) & 0x3F], 1);
    if (i + 2 < length)
      strncat (b64, &lookup[l & 0x3F], 1);
  }
  /* standarize */
  if (length % 3)
    strncat (b64, "===", 3 - length % 3);
  /* return base64 */
  return b64;
}

#endif