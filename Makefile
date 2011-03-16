#This is the makefile for one.c
Object=Astar
Temp=  HashTable.o


CC=mpicc
LDFLAGS= -lmpich -lpthread -lm 
INCLUDE= -I/home/junli/roomy-install/include
LIB= -L/home/junli/roomy-install/lib  /home/junli/roomy-install/lib/libroomy.a


$(Object):$(Temp)
	$(CC) $^  -o $@  $(INCLUDE) $(LIB) $(LDFLAGS)

HashTable.o : HashTable.c  
	$(CC) $(INCLUDE) -c $<

#HashTable2.o : HashTable2.c
#	$(CC) $(INCLUDE) -c $<
clean:
	rm -f  $(Object)

