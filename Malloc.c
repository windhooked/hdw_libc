#include <stdlib.h>
#include "Logger.h"
/*
 
 change to MM_malloc
 change to MM_calloc
 change to MM_realloc
 change to MM_free
 */

void *
Malloc (size_t size)
{
  void *ptr = NULL;

  if (size == 0)
    logf ("Malloc: zero size");

  ptr = malloc (size);

  if (ptr == NULL)
    {
      logf ("Malloc: out of memory (allocating %lu bytes)", (u_long) size);
    }
  else
    {
//      mem_alloc += size;
    }

  return ptr;
}

void *
Realloc (void *ptr, size_t new_size)
{
  void *new_ptr;

  if (new_size == 0)
    logf ("xrealloc: zero size");

  if (ptr == NULL)
    new_ptr = malloc (new_size);
  else
    new_ptr = realloc (ptr, new_size);

  if (new_ptr == NULL)
    {
      logf ("xrealloc: out of memory (new_size %lu bytes)",
	    (u_long) new_size);
    }


  return new_ptr;
}

void
Free (void *ptr)
{
  if (ptr == NULL)
    logf ("xfree: NULL pointer given as argument");

  free (ptr);
}

char *
Strdup (const char *str)
{
  size_t len = strlen (str) + 1;
  char *cp;

  if (len == 0)
    logf ("xstrdup: zero size");

  cp = Malloc (len);
  strncpy (cp, str, len);

  return cp;
}
