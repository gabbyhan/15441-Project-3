IDIR=include
CC=gcc 
CFLAGS= -std=gnu99 -I$(IDIR)

ODIR=build
ODIR_DNS=build_dns
SDIR=src
SDIR_DNS = src_dns

LIBS=-lm

_DEPS = proxy.h httpparser.h customsocket.h log.h mydns.h dns_msg.h dns_msg2.h pq.h dijkstra.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = proxy.o httpparser.o customsocket.o log.o mydns.o dns_msg.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DNS_OBJ = nameserver.o pq.o dijkstra.o dns_msg2.o
DNS_OBJ = $(patsubst %,$(ODIR_DNS)/%,$(_DNS_OBJ))

BINS = proxy nameserver

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR_DNS)/%.o: $(SDIR_DNS)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: proxy nameserver

proxy: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

nameserver: $(DNS_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

customsocket: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

httpparser: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

log: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mydns: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dns_msg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dns_msg2: $(DNS_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

pq: $(DNS_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dijkstra: $(DNS_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(BINS) $(IDIR)/*~ 
