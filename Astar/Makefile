CC=mpicc
LDFLAGS= -lmpich -lpthread -lm 
INCLUDE= -I/home/jli/roomy-install/include
LIB= -L/home/jli/roomy-install/lib  /home/jli/roomy-install/lib/libroomy.a


Object=Astar
SRC=src
Temp= Astar.o test.o $(SRC)/Astar.h
%.o:$(SRC)/%.c 
	$(CC) $(INCLUDE) -c $<

$(Object):$(Temp)
	$(CC) $^ -o $@  $(INCLUDE) $(LIB) $(LDFLAGS)

clean:
	rm -f  *.o
	rm -f  $(Object)

