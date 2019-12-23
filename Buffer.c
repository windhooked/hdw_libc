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
 * Buffer module
 *
 * @log:
 * 4 July 2006 - version 1.0
 *
 *
 */

/*
 *
 */
Buffer *
Buffer_create ()
{
  return (Buffer *) malloc (sizeof (Buffer));
}
/*
 *
 */
unsigned int
Buffer_seek (Buffer * buffer, int type)
{

}
/*
 *
 */
unsigned int
Buffer_length (Buffer * buffer)
{
  return buffer->count;
}
/*
 *
 */
int
Buffer_rewind (Buffer * buffer)
{

  buffer->index = 0;
  buffer->peek_index = 0;
}
/*
 *
 */
int
Buffer_peek_reset (Buffer * buffer)
{

  buffer->peek_index =  buffer->index;
}

/*
 *
 */
int
Buffer_add (Buffer * buffer, char *buf, unsigned int len)
{

  if (buffer->end < (buffer->index + len))
    {
      fprintf (stderr, "buffer_add: index out of bounds %d %d %d \n", len,
	       buffer->index, buffer->end);
      return -1;
    }
  if (buffer->buf == NULL)
    {
      fprintf (stderr, "buffer_add: buffer not allocated");
      return -2;
    }
  if (memcpy ((buffer->buf + buffer->index), buf, len) == NULL)
    {
      fprintf (stderr, "buffer_add: memcpy fail");
      return -3;
    }
  buffer->index += len;
  buffer->count += len;
  return buffer->index;
}

/*
 * same as buffer_get but without incrementing the index pointer
 */
unsigned int
Buffer_peek (Buffer * buffer, char *buf, unsigned int len)
{
  if (buffer->end < (buffer->peek_index + len))
    {
      fprintf (stderr, "buffer_peek: index out of bounds");
      return -1;
    }

  if (memcpy (buf, buffer->buf + buffer->peek_index, len) == NULL)
    {
      fprintf (stderr, "buffer_peek: memcpy fail");
      return -2;
    }

  buffer->peek_index += len;
  return buffer->peek_index;
}


/*
 * Get data from buffer, repetitive calls iterate through buffer
 * use use buffer_rewind if done or if the buffer index ipointer 
 * is unknown
 */

unsigned int
Buffer_get (Buffer * buffer, char *buf, unsigned int len)
{
  if (buffer->end < (buffer->index + len))
    {
      loge ("buffer_get: index out of bounds");
      return -1;
    }
  if (buffer->buf == NULL)
    {
      loge("buffer_add: buffer not allocated");
      return -2;
    }


  if (memcpy (buf, buffer->buf + buffer->index, len) == NULL)
    {
      loge ("buffer_get: memcpy fail");
      return -2;
    }

  buffer->index += len;
  return buffer->index;
}

void
Buffer_init (Buffer * buffer, unsigned int size, unsigned int type_size)
{
  buffer->alloc = size;
  buffer->buf = malloc (size * sizeof (char));
  buffer->index = 0;
  buffer->peek_index = 0;
  buffer->flag = 0;
  buffer->end = size;
}

void
Buffer_free (Buffer * buffer)
{
  memset (buffer->buf, 0, buffer->alloc);
  free (buffer->buf);
  free (buffer);
}

/*
 */

void
Buffer_clear (Buffer * buffer)
{
  memset (buffer->buf, 0, buffer->alloc);
  buffer->index = 0;
  buffer->peek_index = 0;
  buffer->count = 0;
  buffer->flag = 0;
}

/* Appends data to the buffer, expanding it if necessary. */

void
Buffer_append (Buffer * buffer, const char *data, unsigned int len)
{
  char *cp;
  //buffer_append_space(buffer, &cp, len);
  memcpy (cp, data, len);
}

char *
Buffer_ptr (Buffer * buffer)
{
  return buffer->buf + buffer->index;
}

int
Buffer_copy (Buffer * in, Buffer * out, unsigned int n)
{
  char *temp;
  temp = (char *) malloc (n);
  buffer_get (in, temp, n);
  buffer_add (out, temp, n);
  buffer_rewind(out);
  free (temp);
}


void
Buffer_dump (Buffer * buffer)
{
  int i;
  unsigned char *ucp = (unsigned char *) buffer->buf;

  for (i = buffer->index; i < buffer->end; i++)
    {

      if (((ucp[i] > 31) && (ucp[i] < 126)))
	{
	  fprintf (stdout, "%c", ucp[i]);
	}
      else
	{
	  fprintf (stdout, "%c", ucp[i]);
	  //fprintf (stdout, ".%02x", ucp[i]);
	}
    }

}
