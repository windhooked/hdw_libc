// $Id$
// $Name$
// $ProjectName$
//
#include <string.h>
#include <stdio.h>
#include "Buffer.h"
/*
 * @Author: Hannes de Waal (c) 2006
 *
 * @Description: 
 * String routines 
 *
 * @log:
 * 4 Dec 2003 - version 1.0
 *
 *
 */

char *
Strcpy (char *dest, const char *src)
{
  if (strlen (dest) > strlen (src))
    return strncpy (dest, src, strlen (src));
  if (src == NULL)
    return strcpy (dest, "NULL");

  return strcpy (dest, src);

}

/*
*
*/
int
Strcsplit (char *s, char c, char *sa, char *sb)
{
  char *pc;
  pc = strchr (s, c);
  if (pc != NULL)
    {
      *pc = '\0';
      pc++;
      strcpy (sb, pc);
      strcpy (sa, s);
      return 0;
    }
  else
    {
      return -1;
    }
}
/*
 *
 */
char *
strnxcpy (char *dest, char *src, int from, int count)
{
  int i = 0;
  int x = 0;
  //if ( (from + count ) > sizeof(dest)/(char*) || (from +count) > sizeof(src)/(char*) )  return NULL;

  for (i = from; i <= (from + count - 1); i++, x++)
    {
      //printf("%c",src[i]);
      dest[x] = src[i];
    }
  dest[count] = '\0';
  //printf("\n%s\n",dest);

  return dest;
}


/*
 * remove leading and trailing white spaces
 */
char *
trim_space (char *str)
{
  char *string, *p, *mark;

  string = str;
  /* find first non space character */
  for (p = string; *p && isspace (*p); p++);
  /* move characters */
  for ((mark = NULL); (*string = *p); string++, p++)
    {
      if (isspace (*p))
	{
	  if (!mark)
	    mark = string;
	}
      else
	{
	  mark = NULL;
	}

    }

  if (mark)
    *mark = '\0';		/* remove trailing spaces */

  return str;
}

unsigned
trim_trailing_chars (char *line, unsigned len, const char *trimchars)
{
  char *p, *mark;
  unsigned n;

  for (mark = NULL, p = line, n = 0; n < len; n++, p++)
    {
      if (strchr (trimchars, *p))
	{
	  if (!mark)
	    mark = p;
	}
      else
	mark = NULL;
    }

  if (mark)
    {
      *mark = 0;
      return mark - line;
    }
  return len;
}

/****************
 * remove trailing white spaces and return the length of the buffer
 */
unsigned
trim_trailing_ws (char *line, unsigned len)
{
  return trim_trailing_chars (line, len, " \t\r\n");
}

/* skip token */
char *
skip_token (const char *p)
{
  while (isspace (*p))
    p++;
  while (*p && !isspace (*p))
    p++;
  return (char *) p;
}

/* Skip whitespace */
char *
skip_ws (const char *p)
{
  while (isspace (*p))
    p++;
  return (char *) p;
}
