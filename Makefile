all: jlink-test

.PHONY: test

CFLAGS = -std=gnu99 -Wall -Werror
DGFLAGS = -MMD -MP -MT $@

-include config.mak

-include jlink-test.d

jlink-test: main.c jlink.c
	$(CC) -o $@ $(CPPFLAGS) $(DGFLAGS) $(CFLAGS) main.c jlink.c $(LDFLAGS) -lusb-1.0

test: jlink-test
	./jlink-test

clean:
	-rm jlink-test *.d
