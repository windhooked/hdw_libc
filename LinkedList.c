#include <stdio.h>
#include "LinkedList.h"
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


LinkedList_t *
LinkedList_new ()
{
  LinkedList_t *new = (LinkedList_t *) malloc (sizeof (LinkedList_t));
  new->head = NULL;
  new->iter = NULL;
  return new;
}


void
insert (LinkedList_t * l, void *value)
{
  /* The node that should precede the new node in the list. */
  Node *previous = NULL;

  /* The node we are currently comparing to our new value */
  Node *current = l->head;

  /* The new node */
  Node *new = newNode (value);

  /* Find the nodes that should precede and follow the new node. */
  //while (current != NULL && current->value < value) {
  while (current != NULL)
    {
      previous = current;
      current = current->next;
    }

  /* Special case the situation where we are adding the first
     node to the list */
  if (previous == NULL)
    {
      l->head = new;
    }
  else
    {
      previous->next = new;
    }

  /* Set the node that follows the new node. */
  new->next = current;
}


int
removeValue (LinkedList_t * l, void *value)
{
  /* The node that precedes the node we are removing */
  Node *previous = NULL;

  /* The node we are examining */
  Node *current = l->head;

  /* Keep looking until there are no more nodes or we have found
     or gone past the value we are looking for. */
  //while (current != NULL && current->value < value) {
  while (current != NULL)
    {
      previous = current;
      current = current->next;
    }

  /* If we reached the end of the list or we found a value 
     higher than what we wanted, then the value is not in the 
     list */
  //if (current == NULL || current->value > value) {
  if (current == NULL)
    {
      return -1;
    }
  if (previous == NULL)
    {
      /* Removing the head of the list. */
      l->head = current->next;
    }
  else
    {
      /* Remove the current element by having its predecessor
         skip it. */
      previous->next = current->next;
    }

  /* Free the memory associated with the node we just removed. */
  free (current);
  return 0;

}

void
LinkedList_loop (LinkedList_t * l, int (*fncb) (void *))
{
  Node *current;
  for (current = l->head; current != NULL; current = current->next)
    {
      fncb (current->value);

    }
}

void
LinkedList_free (LinkedList_t * l)
{
  Node *current;

  for (current = l->head; current != NULL; current = current->next)
    {
      free (current);
    }

  free (l);
}

Node *
newNode (void *value)
{
  Node *new = (Node*) malloc (sizeof (Node));
  new->value = value;
  new->next = NULL;
  return new;
}

#if defined TEST
int
main ()
{
  LinkedList *l;
  char *d = { "h" };
  l = new_LinkedList ();
  insert (l, d);
  removeValue (l, d);
  printf ("%s\n", d);
}
#endif
