CC = g++
CFLAGS = -Wall

all: makemaze findpath

makemaze: Makemaze.cpp
	$(CC) $(CFLAGS) -o $@ Makemaze.cpp

findpath: Findpath.cpp
	$(CC) $(CFLAGS) -o $@ Findpath.cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f makemaze
	rm -f findpath
	rm -f *.ppm
	rm -f *.txt
