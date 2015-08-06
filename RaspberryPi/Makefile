subdirs := KiiThingSDK/KiiThingSDK mqttcd

PHONY_TARGETS = all distclean

.PHONY: all clean $(subdirs) $(PHONY_TARGETS)

all: $(subdirs)

$(subdirs):
	$(MAKE) -C $@

clean:
	for d in $(subdirs); \
		do \
			$(MAKE) --directory=$$d clean; \
		done
