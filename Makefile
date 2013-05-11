CC = g++
CFLAGS = -g
INCFLAGS = -I./FreeImage 
LDFLAGS = -lfreeimage

RM = /bin/rm -f

all: Quilt.o Image.o
	$(CC) $(CFLAGS) -o quilt Quilt.o Image.o $(INCFLAGS) $(LDFLAGS)

