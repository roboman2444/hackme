CC = gcc
LDFLAGS =
CFLAGS = -O0 -g
OBJECTS = hackme.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

hackme: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f hackme
