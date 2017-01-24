#
# Makefile: Build and clean the program
# Copyright (C) 2016  Matt Miller
#
# Based on example from:
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

#IDIR =../include
CC=gcc
#CFLAGS=-I$(IDIR)
CFLAGS=-Wall -Werror -g

ODIR=obj

#LIBS=-lm
LIBS=

NAME = c_pthreads

_OBJ = $(NAME).o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: $(NAME)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core 
