/*
 * @Author: Hannes de Waal (c) 2006
 *
 * @Description: 
 * Hash module
 *
 * @log:
 * Somewhere 2004 - version 1.0
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Hashtable.h"
#include "Logger.h"
#include "Malloc.h"

static const unsigned int primes[] = {
    19, 31, 53, 97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};
const unsigned int prime_table_length = sizeof(primes) / sizeof(primes[0]);
const float max_load_factor = 0.65;

unsigned int hash(Hashtable * h, void *k);

/*****************************************************************************/
/* indexFor */
static inline unsigned int
indexFor(unsigned int tablelength, unsigned int hashvalue)
{
    // fprintf(stderr,"hash    == %u \n",hashvalue % tablelength); 
    return (hashvalue % tablelength);
};

/* Only works if tablelength == 2^N */
/*static inline unsigned int
indexFor(unsigned int tablelength, unsigned int hashvalue)
{
    return (hashvalue & (tablelength - 1u));
}
*/

#define freekey(X) free(X)

/*======================================================== Hashtable_set_name
 *
*/
void Hashtable_set_name(Hashtable * h, char *name)
{
// fprintf(stderr,"Allocate name %s, length [%d]\n",name,strlen(name));
    h->name = (char *) Malloc(strlen(name));
    strcpy(h->name, name);
}

/*****************************************************************************
* Hashtable_create()  - create a hash table 
*
*  size, hash function , hash equal function 
*
*****************************************************************************/
Hashtable *Hashtable_create(unsigned int minsize,
			    unsigned int (*hashf) (void *),
			    int (*eqf) (void *, void *))
{
    Hashtable *h;
    unsigned int pindex, size = primes[0];
    /* Check requested Hashtable isn't too large */
    if (minsize > (1u << 30))
	return NULL;
    /* Enforce size as prime */
    for (pindex = 0; pindex < prime_table_length; pindex++) {
	if (primes[pindex] > minsize) {
	    size = primes[pindex];
	    break;
	}
    }
    h = (Hashtable *) Malloc(sizeof(Hashtable));
    if (NULL == h) {
	h->status = MALLOC + __LINE__;
	return NULL;
    }
    h->table = (struct entry **) Malloc(sizeof(struct entry *) * size);
    if (NULL == h->table) {
	free(h);
	return NULL;
    } else {
	logi("Allocated hash table at [0x%x], of size: h->tablelength == %u", &h->table, size);
    }
    memset(h->table, 0, size * sizeof(struct entry *));
    h->tablelength = size;
    h->index = 0;
    h->startindex = size;
    h->chainindex = 0;
    h->primeindex = pindex;
    h->entrycount = 0;
    h->updcount = 0;
    h->dupcount = 0;
    h->hashfn = hashf;
    h->eqfn = eqf;
    h->loadlimit = (unsigned int) ceil(size * max_load_factor);
    h->status = 0;
    h->resize_count = 0;
    h->low_water = 0;
    h->high_water = 0;
    return h;
}

/*****************************************************************************
* Hashtable_stat    - print statistics about the table
*
*****************************************************************************/
void Hashtable_status(Hashtable * h)
{
    printf("%s", h->name);
    fprintf(stdout,
	    ": length %u, entries %u, update count %u, duplicates  %u, loadlimit = %u, resize count %u, high water %u, low water %u, status %u \n",
	    h->tablelength, h->entrycount, h->updcount, h->dupcount,
	    h->loadlimit, h->resize_count, h->high_water, h->low_water,
	    h->status);

}

/*****************************************************************************
*   Hashtable_first  - position at the first item in the table
*
*****************************************************************************/
void *Hashtable_first(Hashtable * h)
{
    struct entry *e = NULL;
    int i;
    for (i = h->startindex; i < h->tablelength; i++) {
	e = h->table[i];
	if (e != NULL) {
	    logd("Found entry at h->table[%u] --> &entry 0x%x", i, e);
	    h->index = i;
	    break;
	}
    }
    if (e == NULL)
	h->index = 0;
    return e->v;

}

/*****************************************************************************

*   Hashtable_next   - move the position to the next item in the table
*
*
*****************************************************************************/
void *Hashtable_next(Hashtable * h)
{
    /* TODO: next iteration through chained list */
    int i, index, cindex;
    struct entry *e = NULL, *en = NULL;

    if (h->index > h->tablelength) {
	h->index = 0;
	return NULL;
    }
// if ( h->table[h->index] ==  NULL ) return Hashtable_first(h);
    index = h->index;
    e = h->table[index];
    en = (e != NULL) ? e->next++ : NULL;
    if (en != NULL)
	return en->v;

/*
 cindex = h->chainindex;
 if (cindex != 0) {
	en = e->next++;
	h->chainindex += ( en == NULL ) ? 0 : 1;
	return  ( en == NULL ) ? NULL : en->v;
 }
*/
    logd("h->table[?] index %u <= length %u", index, h->tablelength);
    for (i = ++index; i <= h->tablelength - 1; i++) {
	e = h->table[i];
	if (e != NULL) {
	    logd("Found entry at h->table[%u] --> &entry 0x%x", i, e);
	    h->index = i;
	    h->chainindex = (e->next == NULL) ? 0 : 1;
	    break;
	}
    }
    logd("h->table[?] i %u <= length %u", i, h->tablelength);
    if (e == NULL)
	h->index = 0;
    return (e == NULL) ? NULL : e->v;
}

/*****************************************************************************/
/*****************************************************************************/
int Hashtable_loop(Hashtable * h, void (*callback) (void *))
{
    int i, index, cindex;
    struct entry *e = NULL, *en = NULL;

    if (h->index > h->tablelength)
	h->index = h->startindex;
    index = h->index;

    for (i = index; i <= h->tablelength - 1; i++) {
	e = h->table[i];
	while (e != NULL) {
	    (*callback) (e->v);
	    e = e->next;
	}

    }
    return 0;
}

/*****************************************************************************/
/* Hashtable_clear() - clear hash contents */
/*****************************************************************************/
void Hashtable_clear(Hashtable * h)
{
// Hashtable_destroy();
// Hashtable_create();
}

/*****************************************************************************/
/*  Hashtable_del    - delete an item from the table */
/* returns value associated with key */
/*****************************************************************************/
void *Hashtable_del(Hashtable * h, void *k)
{
    /* TODO: consider compacting the table when the load factor drops enough,
     *       or provide a 'compact' method. */

    struct entry *e;
    struct entry **pE;
    void *v;
    unsigned int hashvalue, index;

    hashvalue = hash(h, k);
    index = indexFor(h->tablelength, hash(h, k));
    pE = &(h->table[index]);
    e = *pE;
    while (NULL != e) {
	/* Check hash value to short circuit heavier comparison */
	if ((hashvalue == e->h) && (h->eqfn(k, e->k))) {
	    *pE = e->next;
	    h->entrycount--;
	    v = e->v;
	    freekey(e->k);
	    free(e);
	    return v;
	}
	pE = &(e->next);
	e = e->next;
    }
    h->low_water = MIN(h->low_water, h->entrycount);
    return NULL;
}


/*****************************************************************************/
/* destroy */
/*****************************************************************************/
void Hashtable_destroy(Hashtable * h, int free_values)
{
    unsigned int i;
    struct entry *e, *f;
    struct entry **table = h->table;
    if (free_values) {
	for (i = 0; i < h->tablelength; i++) {
	    e = table[i];
	    while (NULL != e) {
		f = e;
		e = e->next;
		freekey(f->k);
		free(f->v);
		free(f);
	    }
	}
    } else {
	for (i = 0; i < h->tablelength; i++) {
	    e = table[i];
	    while (NULL != e) {
		f = e;
		e = e->next;
		freekey(f->k);
		free(f);
	    }
	}
    }
    free(h->name);
    free(h->table);
    free(h);
}

/*****************************************************************************/
/* */
/*****************************************************************************/
unsigned int hash(Hashtable * h, void *k)
{
    /* Aim to protect against poor hash functions by adding logic here */
    unsigned int i = 0;
    i = h->hashfn(k);
    logd("hashfn  == %u", i);

//i = tw32_hash(i);
//i = tw_mul_hash(i);
//  logd("hash    == %u",i); 
    return i;
}

/*****************************************************************************/
/* logic taken from java 1.4 has Hashtable source */
/*****************************************************************************/
unsigned int j_hash(unsigned int i)
{
    i += ~(i << 9);
    i ^= ((i >> 14) | (i << 18));	/* >>> */
    i += (i << 4);
    i ^= ((i >> 10) | (i << 22));	/* >>> */
    return i;
}

/*****************************************************************************/
/* Thomas Wang's 32 bit Mix Function */
/* http://www.concentric.net/~Ttwang/tech/inthash.htm */
/*****************************************************************************/
unsigned int tw32_hash(unsigned int key)
{
    key += ~(key << 15);
    key ^= (key >> 10);		/* >>> */
    key += (key << 3);
    key ^= (key >> 6);		/* >>> */
    key += ~(key << 11);
    key ^= (key >> 16);		/* >>> */
    return key;
}

/*****************************************************************************/
/* Thomas Wang's 64 bit Mix Function */
/*****************************************************************************/

unsigned long tw64_hash(unsigned long long key)
{
    key += ~(key << 32);
    key ^= (key >> 22);		/* >>> */
    key += ~(key << 13);
    key ^= (key >> 8);		/* >>> */
    key += (key << 3);
    key ^= (key >> 15);		/* >>> */
    key += ~(key << 27);
    key ^= (key >> 31);		/* >>> */
    return key;
}


/*****************************************************************************/
/* Thomas Wang  */
/*****************************************************************************/
/*
unsigned long
tw_mul_hash (unsigned long key)
{
  unsigned long c1 = 0x6e5ea73858134343L;
  unsigned long c2 = 0xb34e8f99a2ec9ef5L;
  key ^= ((c1 ^ key) >> 32);
  key *= c1;
  key ^= ((c2 ^ key) >> 31);
  key *= c2;
  key ^= ((c1 ^ key) >> 32);
  return key;
}
*/

/*****************************************************************************/
/* */
/*****************************************************************************/
int Hashtable_expand(Hashtable * h)
{
    /* Double the size of the table to accomodate more entries */
    struct entry **newtable;
    struct entry *e;
    struct entry **pE;
    unsigned int newsize, i, index;
    /* Check we're not hitting max capacity */
    if (h->primeindex == (prime_table_length - 1))
	return 0;
    newsize = primes[++(h->primeindex)];

    newtable = (struct entry **) Malloc(sizeof(struct entry *) * newsize);
    if (NULL != newtable) {
	memset(newtable, 0, newsize * sizeof(struct entry *));
	/* This algorithm is not 'stable'. ie. it reverses the list
	 * when it transfers entries between the tables */
	for (i = 0; i < h->tablelength; i++) {
	    while (NULL != (e = h->table[i])) {
		h->table[i] = e->next;
		index = indexFor(newsize, e->h);
		e->next = newtable[index];
		newtable[index] = e;
	    }
	}
	free(h->table);
	h->table = newtable;
    }
    /* Plan B: realloc instead */
    else {
	newtable = (struct entry **)
	    realloc(h->table, newsize * sizeof(struct entry *));
	if (NULL == newtable) {
	    (h->primeindex)--;
	    return 0;
	}
	h->table = newtable;
	memset(newtable[h->tablelength], 0, newsize - h->tablelength);
	for (i = 0; i < h->tablelength; i++) {
	    for (pE = &(newtable[i]), e = *pE; e != NULL; e = *pE) {
		index = indexFor(newsize, e->h);
		if (index == i) {
		    pE = &(e->next);
		} else {
		    *pE = e->next;
		    e->next = newtable[index];
		    newtable[index] = e;
		}
	    }
	}
    }
    logd("Resized hash table from %u to %u  ", h->tablelength, newsize);
    h->tablelength = newsize;
    h->resize_count++;
    h->loadlimit = (unsigned int) ceil(newsize * max_load_factor);
    return -1;
}

/*****************************************************************************/
/* */
/*****************************************************************************/
unsigned int Hashtable_count(Hashtable * h)
{
    return h->entrycount;
}

/*****************************************************************************/
/* add value key pair to hash table */
/* allow chaining if allow duplicates */
/*****************************************************************************/
int Hashtable_add(Hashtable * h, void *k, void *v, int allow_dup)
{
    unsigned int index;
    struct entry *e, *es;
    int ok_code = 0;

    if (h == NULL)
	return -1;

    if (++(h->entrycount) > h->loadlimit) {
	Hashtable_expand(h);
    }
    e = (struct entry *) Malloc(sizeof(struct entry));
    if (NULL == e) {
	--(h->entrycount);
	h->status = MALLOC + __LINE__;
	return -2;
    }
    e->h = hash(h, k);
    index = indexFor(h->tablelength, e->h);
    logd("%s, add value key pair at h->table[%u] --> &entry == 0x%x",
	 h->name, index, e);

    /* get bucket at index */
    es = h->table[index];

    /* check if entry exists with identical key */
    if (es == NULL || !h->eqfn(k, es->k) || allow_dup == 1) {

	if (h->table[index] != NULL) {
	    logd("%s, duplicate key, will chain h->table[%u]->next",
		 h->name, index);
	}
	e->k = k;
	e->v = v;
	// save previous bucket on this index to current->next
	e->next = h->table[index];
	// save current bucket on index
	h->table[index] = e;
	h->updcount++;
    } else {
	if (es != NULL && h->eqfn(k, es->k)) {	/* we need to update */
	    // memcpy(es->v,v,sizeof(void *v));
	}
	free(e);
	//h->table[index]->next = NULL;
	--(h->entrycount);
	h->dupcount++;
	ok_code = -1;
    }

    h->startindex = MIN(h->startindex, index);
    h->high_water = MAX(h->high_water, h->entrycount);

    return ok_code;
}

/*****************************************************************************/
/* returns value associated with key */
/*****************************************************************************/
void *Hashtable_get(Hashtable * h, void *k)
{
    struct entry *e;
    unsigned int hashvalue, index;
    if (h == NULL || k == NULL)
	loge("hasHashtablele or key == NULL");
    hashvalue = hash(h, k);
    index = indexFor(h->tablelength, hashvalue);
    e = h->table[index];
    logd("%s, get value key pair at h->table[%u] --> &entry == 0x%x",
	 h->name, index, e);
    while (NULL != e) {
	/* Check hash value to short circuit heavier comparison */
	if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
	    return e->v;
	e = e->next;
    }
    return NULL;
}
