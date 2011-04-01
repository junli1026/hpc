CC=mpicc
LDFLAGS= -lmpich -lpthread -lm 
INCLUDE= -I/home/junli/roomy-install/include
LIB= -L/home/junli/roomy-install/lib  /home/junli/roomy-install/lib/libroomy.a


Object=Astar
SRC=src
Temp= Astar.o test.o RoomyGraph.o $(SRC)/Astar.h $(SRC)/RoomyGraph.h

%.o:$(SRC)/%.c 
	$(CC) $(INCLUDE) -c $<

$(Object):$(Temp)
	$(CC) $^ -o $@  $(INCLUDE) $(LIB) $(LDFLAGS)

clean:
	rm -f  *.o
	rm -f  $(Object)

