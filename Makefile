CFLAGS  = -c 
LFLAGS  =
CC = gcc
AR = ar
LIB = std 

SRC     = Buffer.c Hashtable.c Malloc.c String.c Logger.c Globals.c Util.c
OBJ     = Buffer.o Hashtable.o Malloc.o String.o Logger.o Globals.o Util.c

all:
	gcc  -c $(SRC) 
	ar -r libhdw$(LIB).so $(OBJ) 

$(SRC) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ)


$(LIB) : $(OBJ)
	$(AR) -r libhdw$(LIB).so $(OBJ) 

test:
	 gcc hash_tab_test.c -L. -lm -lhdw -o hash_tab_test


clean:
	rm -f *.o

