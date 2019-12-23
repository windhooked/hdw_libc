/*
*
*/
struct entry
{
  void *k, *v;
  unsigned int h;
  struct entry *next;
};

typedef struct hashtable
{
  char *name;
  struct entry **table;
  unsigned int tablelength;     /* number of bins */
  unsigned int index;
  unsigned int startindex;
  unsigned int chainindex;
  unsigned int entrycount;
  unsigned int updcount; /* update count */
  unsigned int dupcount; /* duplicate count */
  unsigned int primeindex;
  unsigned int loadlimit;
  unsigned int (*hashfn) (void *k);
  int (*eqfn) (void *k1, void *k2);
  unsigned int resize_count;
  unsigned int high_water;
  unsigned int low_water;
  int status;
} Hashtable;

void Hashtable_set_name (Hashtable *h,char *name) ;
Hashtable* Hashtable_create (unsigned int minsize,
unsigned int (*hashf) (void *), int (*eqf) (void *, void *));
//void Hashtable (Hashtable *h);
void *Hashtable_first (Hashtable *h);
void *Hashtable_next (Hashtable *h);
void  Hashtable_clear (Hashtable * h);
void *Hashtable_del (Hashtable * h, void *k);
void Hashtable_destroy (Hashtable * h, int free_values);
unsigned int hash (Hashtable * h, void *k);
int Hashtable_expand (Hashtable *h);
unsigned int Hashtable_count (Hashtable *h);
int Hashtable_add (Hashtable  *h, void *k, void *v, int allow_dup);
void * Hashtable_get (Hashtable *h, void *k);
int Hashtable_loop (Hashtable * h, void (*callback) (void *));


#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))

#define MALLOC 1000
