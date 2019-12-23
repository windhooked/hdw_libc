typedef struct buffer {
 char * buf;
 unsigned int alloc;
 unsigned int index;
 unsigned int peek_index;
 unsigned int end;
 unsigned int count;
 unsigned int flag;
} Buffer;
