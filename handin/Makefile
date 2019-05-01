IDIR=include
CC=gcc 
CFLAGS= -std=gnu99 -I$(IDIR)

ODIR=build
SDIR=src

LIBS=-lm

_DEPS = nameserver.h httpparser.h customsocket.h log.h dns_msg.h pq.h dijkstra.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = nameserver.o httpparser.o customsocket.o log.o dns_msg.o pq.o dijkstra.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


BINS = nameserver

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: nameserver

nameserver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

customsocket: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

httpparser: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

log: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dns_msg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

pq: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dijkstra: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(BINS) $(IDIR)/*~ 
