NAME = PiAirRemote

CC = gcc
BUILD = build
BUILD_LIB = lib

TARGETS = register temperature w1-simple

SOURCE_DIR = source
KII_SDK_DIR = KiiThingSDK/KiiThingSDK
MQTTCD_DIR = mqttcd
SUB_DIRS := $(KII_SDK_DIR) $(MQTTCD_DIR)
APP_INFO = appinfo.json

KII_FLAGS_PATH = $(KII_SDK_DIR) $(KII_SDK_DIR)/jansson
CFLAGS = $(addprefix -I, $(KII_FLAGS_PATH))
LDFLAGS = $(addprefix -L, $(KII_FLAGS_PATH)) -lkii -ljansson

PHONY_TARGETS = build $(SUB_DIRS) $(TARGETS)
.PHONY: $(PHONY_TARGETS)

all: prepare $(SUB_DIRS) $(TARGETS)

prepare:
	mkdir -p $(BUILD)/$(BUILD_LIB)

clean:
	for d in $(SUB_DIRS); \
		do \
			$(MAKE) --directory=$$d clean; \
		done
	rm -rf $(BUILD)

install: install-credentials

install-credentials:
	mkdir -p $(HOME)/.$(NAME)
	cp $(APP_INFO) $(HOME)/.$(NAME)

uninstall: uninstall-credentials

uninstall-credentials:
	rm -rf $(HOME)/.$(NAME)

KiiThingSDK/KiiThingSDK:
	$(MAKE) -C $@ build
	cp $@/libkii.so $(BUILD)/$(BUILD_LIB)
	cp $@/jansson/libjansson.so $(BUILD)/$(BUILD_LIB)

mqttcd:
	$(MAKE) -C $@
	cp $@/mqttcd $(BUILD)

$(TARGETS):
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(BUILD)/$@

register: $(addprefix $(SOURCE_DIR)/, register.c config.c)
temperature: $(addprefix $(SOURCE_DIR)/, temperature.c config.c w1.c)
w1-simple: $(addprefix $(SOURCE_DIR)/, w1-simple.c)

