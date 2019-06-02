# Makefile fitxategia, lana automatizatzeko

all: server.o readConf.o

server.o:
    gcc -c readConf.c
    gcc -o server server.c

irakurkonf.o:
    gcc -c readConf.c