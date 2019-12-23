
typedef struct node {
      void *value;
      struct node *next;
} Node;

typedef struct {
      Node *head;
      Node *iter;
} LinkedList_t;

Node *newNode (void *value) ;
