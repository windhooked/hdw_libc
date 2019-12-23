#include <stdarg.h>
#include <stdlib.h>
/* Hannes de Waal * (c) 2006
 * 10 Sep 2006 - initial version
 */

/*========================================================= strxcat
 *  simple function to concatenate strings in c.
 *  uses realloc memory should be freed with free
 *
 */
char *strxcat(int x, ...)
{

    int l = 0;
    int i = 0;
    int rc = 0;

    va_list ap;
    char *arg = NULL;
    char *s = NULL;
    char *new = NULL;
    char *tmp = NULL;
    tmp = (char*) Calloc (512, sizeof(char));

    va_start(ap, x);

    for (i = 0; i < x; i++) {

	arg = va_arg(ap, char *);
	l += strlen((char *) arg);
	if ( l > 512 ) 
		i = x;

	strcat(tmp, arg);
    }

    va_end(ap);

// truncate if longer
    if ( l > 512 ) 
	    l = 512;

    new = (char*) Calloc (l+1, sizeof(char));
    strncpy(new, tmp,l);
    
    Free(tmp);

    return new;

}
/*  File   : strxcat.c
    Author : Richard A. O'Keefe.
    Updated: 25 may 1984
    Defines: strxcat() 


    strxcat(dst, src1, ..., srcn, NullS)
    moves the concatenation of dst,src1,...,srcn to dst, terminates it
    with a NUL character, and returns the original value of dst.
    It is just like strcat except that it concatenates multiple sources.
    Equivalence: strxcat(d, s1, ..., sn) <=> strxcpy(d, d, s1, ..., sn).
    Beware: the last argument should be the null character pointer.
    Take VERY great care not to omit it!  Also be careful to use NullS
    and NOT to use 0, as on some machines 0 is not the same size as a
    character pointer, or not the same bit pattern as NullS.

*/ 





/*VARARGS*/

char *_strxcat(va_alist)

    va_dcl

    {

      va_list pvar;

      register char *dst, *src;

      char *bogus; 


      va_start(pvar);

      dst = va_arg(pvar, char *);

      bogus = dst;

      while (*dst) dst++;

      src = va_arg(pvar, char *);

      while (src != NullS) {

          while (*dst++ = *src++) ;

          dst--;

          src = va_arg(pvar, char *);

      }

      return bogus;

    }


/* 
 * count token 
 */
int
count_token (const char *p)
{
  int i = 0;
  char *s = (char*) strdup(p);

  strtok(s," ");
  i++;

  while (strtok(NULL," ")) { 
	  i++;
  }

  free(s);
  
  
  return i;
}

/* 
 * skip token 
 */
char *
skip_token (const char *p)
{
  while (isspace (*p))
    p++;
  while (*p && !isspace (*p))
    p++;
  return (char *) p;
}

/* 
 * Skip whitespace 
 */
char *
skip_ws (const char *p)
{
  while (isspace (*p))
    p++;
  return (char *) p;
}

char *substr(char *dst, char *src, int off, int len)
{
	while (--off >= 0)
	    if (!*src++) {		/* We've hit the end */
		*dst = NULL;		/* return empty string */
		return dst;
	    }
	while (--len >= 0)
	    if (!(*dst++ = *src++)) {	/* We've hit the end */
		return dst-1;		/* dst is already terminated */
	    }
	*dst = NULL;			/* terminate dst with NUL */
	return dst;
}

#if defined TEST
int main()
{
    char *root = { "/usr" };
    char *path = { "/share" };
    char *s = NULL;
	s = strxcat(7, "The", "Fox", "Jumps", "Over", "the", "fence", ".");
	printf("%s\n", s);
	s = strxcat(3, "123456789", "abcde", "/");
	printf("%s\n", s);
	s = strxcat(3, root, path, "/");
	printf("%s\n", s);
}
#endif
