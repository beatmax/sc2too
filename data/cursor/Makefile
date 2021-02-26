HOTSPOT_X=1
HOTSPOT_Y=1

all: cursor.xbm cursor-mask.xbm

%.xbm: %.txt Makefile
	atobm -xhot $(HOTSPOT_X) -yhot $(HOTSPOT_Y) $< > $@
