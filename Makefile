ROOT_DIR:=$(shell pwd)
DBUS_RELEASE_VERSION := 1.10.8
DBUS_SRC_PATH := $(ROOT_DIR)/src/dbus-$(DBUS_RELEASE_VERSION)
LIBEXPAT_SRC_PATH := $(ROOT_DIR)/src/expat-2.1.0/
BUILD_DIR := $(ROOT_DIR)/build
DBUS_INSTALL_DIR := $(BUILD_DIR)/dbus/
LIBEXPAT_INSTALL_DIR := $(BUILD_DIR)/libexpat/

MKDIR := mkdir
MAKE := make
RM := rm
CC := $(CC)
HOST := $(HOST)
TARGET := $(TARGET)

.PHONY:all clean prepare dbus

all: prepare dbus 

prepare:
	$(MKDIR) -p $(BUILD_DIR)	
	
dbus: prepare libexpat
	@echo "[2]Building $@."
	cd $(DBUS_SRC_PATH); ./configure CC=$(CC) --host=$(HOST) --target=$(TARGET) --prefix=$(DBUS_INSTALL_DIR) CPPFLAGS=-I$(LIBEXPAT_INSTALL_DIR)/include LIBS=-lexpat LDFLAGS=-L$(LIBEXPAT_INSTALL_DIR)/lib --without-x  --disable-tests  
	$(MAKE) -C $(DBUS_SRC_PATH) && $(MAKE) -C $(DBUS_SRC_PATH) install 
	
libexpat:
	@echo "[1]Building $@."
	cd $(LIBEXPAT_SRC_PATH); ./configure CC=$(CC) --host=$(HOST) --target=$(TARGET) --prefix=$(LIBEXPAT_INSTALL_DIR)  
	$(MAKE) -C $(LIBEXPAT_SRC_PATH) && $(MAKE) -C $(LIBEXPAT_SRC_PATH) install 
	
clean:
	$(MAKE) -C $(LIBEXPAT_SRC_PATH) clean 
	$(MAKE) -C $(DBUS_SRC_PATH) clean 
	$(RM) -fr $(BUILD_DIR)
	
	
	
	