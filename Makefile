
CFLAGS += -Iev -O0 -g3 -Wall

obj += util.c
obj += sfp_opt.c
obj += sfp.o

#topor_ev.o: CFLAGS = -Iev -O2

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

sfp: $(obj)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) sfp tags
