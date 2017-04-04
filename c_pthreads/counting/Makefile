#
# Makefile: Build and clean the program
# Copyright (C) 2016  Matt Miller
#
# Based on example from:
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

#IDIR =../include
#CC=gcc
CC=clang
#CFLAGS=-I$(IDIR)
CFLAGS=-Wall -Werror -g
COPTS=-fsanitize=address -g

ODIR=obj

#LIBS=-lm
LIBS=

NAME = counting

_OBJ = $(NAME).o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) $(COPTS) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) $(COPTS) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(NAME)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core
