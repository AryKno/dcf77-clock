#
# Makefile pour generer la bibliotheque de fonctions de communication
#
CC=gcc
AR=ar
BIN=dcf77decoder
LIBNAME=GPIOaccess
CFLAGS+=-Wall -Wextra -Os
LIBDIR=.
LIB+=-l$(LIBNAME) -lrt

all: lib$(LIBNAME).a $(BIN)
#
# La cible de nettoyage
#
clean: 
	rm -f core *.o lib$(LIBNAME).a $(BIN)

#
# Les cibles pour la bibliotheque
#
lib$(LIBNAME).o: $(LIBNAME).c lib$(LIBNAME).h
# ici qu'on fait la compilation
	$(CC) -o $@ -c $< $(CFLAGS)
lib$(LIBNAME).a: lib$(LIBNAME).o
	$(AR) rcs $@ $<

$(BIN): $(BIN).c
	$(CC) -o $@ $< -L$(LIBDIR) $(LIB) $(CFLAGS)
#	rm -f $@
#	$(CLIB) $@ $+
