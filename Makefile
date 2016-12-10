CC=gcc
CFLAGS = -I.
DEPS = buffer_mag.h parser.h timer_mag.h telnet-client.h rotaryEncoder.h
OBJ = buffer_mag.o parser.o timer_mag.o telnet-client.o rotaryEncoder.o main.o 
LIBS = -lpthread -lwiringPi -lm -ltelnet -Wl,-rpath,/usr/local/lib/

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emullation: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
