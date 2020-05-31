.PHONY: all clean unittest config-release config-debug

all:
	@scons -Q

clean:
	@scons -Qc
	@rm -rf build
	@rm -rf .sconf_temp
	@rm -f .sconsign.dblite

unittest: all
	build/test/tests

config-release:
	@echo 'DEBUG = False' > .config.py

config-debug:
	@echo 'DEBUG = True' > .config.py

ifeq ($(suffix ${MAKECMDGOALS}),.o)
${MAKECMDGOALS}::
	@scons -Q $@
endif
