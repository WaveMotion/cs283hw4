CC = g++
CFLAGS = -O2
INCFLAGS = -I./include 
LDFLAGS = -L/opt/local/lib -lfreeimage

RM = /bin/rm -f

all: Quilt.o Image.o
	$(CC) $(CFLAGS) -o quilt Quilt.o Image.o $(INCFLAGS) $(LDFLAGS)
Image.o: Image.cpp Image.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Image.cpp 
Quilt.o: Quilt.cpp impl.cpp
	$(CC) $(CFLAGS) $(INCFLAGS) -c Quilt.cpp 
clean:
	$(RM) *.o quilt



