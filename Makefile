ifndef V
	V=0
endif

all:
	@scons V=$V

clean:
	@scons -c
