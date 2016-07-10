ROOT_DIR:=$(shell pwd)
DBUS_RELEASE_VERSION := 1.10.8
DBUS_SRC_PATH := $(ROOT_DIR)/dbus_src/dbus-$(DBUS_RELEASE_VERSION)
LIBEXPAT_SRC_PATH := $(ROOT_DIR)/dbus_src/expat-2.1.0/
BUILD_DIR := $(ROOT_DIR)/build

.PHONY:all clean prepare dbus

prepare:
	mkdir -p $(BUILD_DIR)
	
all: prepare dbus 	
	
dbus: libexpat
	@echo "Building $@."

libexpat:
	@echo "Building $@."


clean:
	make -C $(LIBEXPAT_SRC_PATH) clean 
	make -C $(DBUS_SRC_PATH) clean 
	rm -fr $(BUILD_DIR)
	