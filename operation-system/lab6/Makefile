CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=200809L
TARGETS = 6_1_1_unsync_shm 6_1_6_sync_shm 6_2_1_pipe 6_2_3_pipe_redirect

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $

clean:
	rm -f $(TARGETS)

.PHONY: all clean
