ifndef V
	V=0
endif

ifndef LIBC
	LIBC=1
endif

all:
	@scons V=$V LIBC=$(LIBC)

clean:
	@scons -c
