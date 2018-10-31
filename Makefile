
CFLAGS += -O0 -g3 -Wall
LDFLAGS += -lev

obj += util.o
obj += sfp_opt.o
obj += ringbuffer.o
obj += sfp.o

#topor_ev.o: CFLAGS = -Iev -O2

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

sfp: $(obj)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(obj) sfp tags
