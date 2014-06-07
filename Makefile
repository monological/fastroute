IDIR=./src
CC=/usr/bin/clang
CFLAGS=-std=c99 -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -O3 -I$(IDIR) 

ODIR=obj
BDIR=bin
LDIR=../lib

_DEPS = main.h create_db.h uthash.h usage.h database.h  data.h  expander.h  graph.h  path.h  pqueue.h  route.h  util.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJF = main.o usage.o database.o  data.o  expander.o  graph.o  path.o  pqueue.o  route.o  util.o
OBJF = $(patsubst %,$(ODIR)/%,$(_OBJF))

_OBJD = create_db.o usage.o database.o  data.o  expander.o  graph.o  path.o  pqueue.o  route.o  util.o
OBJD = $(patsubst %,$(ODIR)/%,$(_OBJD))


all: fastroute create_db

$(ODIR)/%.o: $(IDIR)/%.c $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

fastroute: $(OBJF)
				$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) -lm -lzmq -lczmq

create_db: $(OBJD)
				$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) -lm -lzmq -lczmq

.PHONY: clean

clean:
				rm -f $(ODIR)/*.o *~ core $(BDIR)/create_db $(BDIR)/fastroute
