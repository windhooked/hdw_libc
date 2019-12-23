#include <stdio.h>
#include <stdlib.h>
#include "Hashtable.h"
/* 
 * gcc -I../include hash_tab_test.c hash_tab.c logger.c xmalloc.c -lm -o ./htab_test 
 */
Hashtable *test_tab = NULL;

char *logfile = { "hash_test.log" };
char *log_file = { "hash_test.log" };

int param_log_level = 0;
char *param_exclude_msg_source = { "" };
struct test_key
{
  unsigned long key1;
  unsigned long key2;
};
struct test
{
  unsigned long key1;
  unsigned long key2;
  char text[50];
};

/*
*
*/
unsigned int
hashfromkey (struct test_key *k)
{
  unsigned int key;
  key = k->key1;
  return (((k->key1 << 17) | (k->key1 >> 15)) ^ k->key1) + 140;

/*
2654435761 is the golden ratio of 2^32. The right shift of 3 bits assumes that all objects are aligned on the 8 byte boundary. If a system aligns on the 4 byte boundary, then a right shift of 2 bits should be done. 
//	  return (key >> 3) * 2654435761;
	  */
}


/*
*
*/
int
equalkeys (void *k1, void *k2)
{
  return (0 == memcmp (k1, k2, sizeof (struct test_key)));
}

/*
*
*/
void
tab_init ()
{
  test_tab = (Hashtable *) Hashtable_create (10, hashfromkey, equalkeys);
  Hashtable_set_name (test_tab, "Test");
}

/*
*
*/
void
tab_clear ()
{
  Hashtable_clear (test_tab);
}

/*
*
*/
void
test_tab_dump (void)
{
  struct test *m;
  unsigned long count = 0;
  count = Hashtable_count (test_tab);
  if (count > 0)

    {
      m = (struct test *) Hashtable_first (test_tab);
      while (m != NULL)

	{
	  printf ("%d %x %x %s\n", count--, m->key1, m->key2, &m->text);
	  m = (struct test *) Hashtable_next (test_tab);
    }}
} int

main ()
{
  unsigned long i, dup = 0, count = 0;
  struct test *t, *r;
  struct test_key *k;
  char s[50];

  tab_init ();

  //for (i = 0xbfffa; i<0xffffe; i += 0x1){
  for (i = 0xa; i < 0xf; i += 0x1)
    {
      count++;
      t = malloc (sizeof (struct test));
      k = malloc (sizeof (struct test_key));
      t->key1 = i;
      k->key1 = i;
      t->key2 = 1;
      k->key2 = 1;
      sprintf (s, "test %d\0", i * 100);
      strcpy (t->text, s);
      if (Hashtable_add (test_tab, t, k, 0) < 0)
	{

//              fprintf(stderr,"dup %x =? %x\n",r->key1,t->key1);
	  dup++;
	  free (k);
	  free (t);
	}
      else
	{
	  r = Hashtable_get (test_tab, k);
	  if (0 != memcmp (r, t, sizeof (struct test)))
	    fprintf (stderr, "%x =? %x\n", r->key1, t->key1);
    }
    } 
  fprintf (stderr, "count %d dup %d\n", count, dup);
  Hashtable_status (test_tab);
  test_tab_dump ();
}
