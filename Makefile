all: project

CC=c++

INCLUDE=.

#--- Development flags
#CFLAGS=-g -Wall -ansi

#--- Release flags
CFLAGS=-O -Wall -ansi
PROGRAM=links.fcgi

project:
	$(CC) $(CFLAGS) main.cc -lfcgi -L/usr/local/lib -lxapian  \
			-I/usr/local/include -o $(PROGRAM)

clean:
	rm $(PROGRAM)

install:
	sudo /etc/init.d/httpd stop             
	rm -f ../bin/$(PROGRAM)
	cp $(PROGRAM) ../bin;
	sudo /etc/init.d/httpd start

