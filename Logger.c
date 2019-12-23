#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "Globals.h"
#include "Logger.h"

// $Id$
// $Name$
// $ProjectName$
//
#include <stdio.h>
#include "Buffer.h"
/*
 * @Author: Hannes de Waal (c) 2006
 *
 * @Description: 
 * Logger module
 *
 * @log:
 * 18 May 2004  - version 1.0
 *
 *
 */

char * log_file;
int param_log_level;
char *param_exclude_msg_source;

Logger* Logger_new()
{
	return (Logger*) Malloc(sizeof(Logger));
}


int
Logger_log (const char *file, char *func, int l, int level, char *fmt, ...)
{
  char msg[255];
  va_list a;
  va_start (a, fmt);
  vsprintf (msg, fmt, a);
  va_end (a);
  FILE *fd;

  char *levels[5] = { "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
  };

  //2004-05-18 13:28:16,218 [main] DEBUG n1aa_server

  time_t tcurrent;
  struct tm ts;
  char str[81];
  char *pstr;

  pstr = (char *) &str;
  time (&tcurrent);
  ts = *localtime (&tcurrent);
  strftime (pstr, sizeof (str) - 1, "%Y-%m-%d %H:%M:%S", &ts);

  if (log_file == NULL) {
       fprintf (stderr, "%s, %s[%d] [%s()] %s[%d] - %s\n", pstr, levels[level],
				                    getpid (), func, file, l, msg);
       return 0;
  }

  fd = (FILE *) fopen (log_file, "a+");
  if (fd == NULL)
    {
      // log error
      fprintf (stderr, "Logger.c Can't open log file [%s], ", log_file);
      perror ("open");
      return FILE_OPEN_ERROR;
    }
// fprintf(stderr,"%s, %d [%s()] %s %s - %s\n",  pstr ,l,func,levels[level] ,file,msg);

  if (level >= 4 - arg_verbose
      && (strstr (param_exclude_msg_source, file) == NULL))
    fprintf (stderr, "%s, %s[%d] [%s()] %s[%d] - %s\n", pstr, levels[level],
	     getpid (), func, file, l, msg);

//  fprintf(log_file,"%s, %d [%s()] %s %s - %s\n", pstr ,l,func,levels[level] ,file,msg);

  if (level >= param_log_level
      && (strstr (param_exclude_msg_source, file) == NULL))
    fprintf (fd, "%s, %s[%d] [%s()] %s[%d] - %s\n", pstr, levels[level],
	     getpid (), func, file, l, msg);

  fclose (fd);


  return 0;
}

#ifdef TEST_LOGGER

int 
main (){
	log(DEBUG, "hello");
	loge( "hello %d",1);
	logw( "hello");
	logd( "hello");
	logi( "hello");
	logf( "hello");
}
#endif
