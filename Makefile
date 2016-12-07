CC=gcc
CFLAGS = -I.
DEPS = buffer_mag.h parser.h timer_mag.h
OBJ = buffer_mag.o parser.o timer_mag.o main.o 
LIBS = -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emullation: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
