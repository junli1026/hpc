Object=Astar
SRC=src


CC=mpicc
LDFLAGS= -lmpich -lpthread -lm 
INCLUDE= -I/home/junli/roomy-install/include
LIB= -L/home/junli/roomy-install/lib  /home/junli/roomy-install/lib/libroomy.a


_Astar=Astar.o
Astar=$(patsubst %, $(SRC)/%, $(_Astar))

%.o: %.c  
	$(CC) $(INCLUDE) -c $<


$(Object):$(Astar)
	$(CC) $^  -o $@  $(INCLUDE) $(LIB) $(LDFLAGS)

clean:
	rm -f  $(SRC)/$(Temp)

