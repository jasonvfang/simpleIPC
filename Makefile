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
CONFIG_FLAG := .configured


.PHONY:all clean prepare dbus

all: prepare dbus 

prepare:
	$(MKDIR) -p $(BUILD_DIR);\
	if [ ! -f .configured ]; then \
		cd $(LIBEXPAT_SRC_PATH); ./configure CC=$(CC) --host=$(HOST) --target=$(TARGET) --prefix=$(LIBEXPAT_INSTALL_DIR); \
		cd $(DBUS_SRC_PATH); ./configure CC=$(CC) --host=$(HOST) --target=$(TARGET) --prefix=$(DBUS_INSTALL_DIR) CPPFLAGS=-I$(LIBEXPAT_INSTALL_DIR)/include LIBS=-lexpat LDFLAGS=-L$(LIBEXPAT_INSTALL_DIR)/lib --without-x  --disable-tests; \
	fi;\
	touch $(CONFIG_FLAG) 
	
	
dbus: prepare libexpat
	@echo "[0;32m"
	@echo "--------------------------"
	@echo "[*]Building $@."
	@echo "--------------------------"
	@echo "[0m"
	$(MAKE) -C $(DBUS_SRC_PATH) && $(MAKE) -C $(DBUS_SRC_PATH) install 
	
libexpat:
	@echo "[0;32m"
	@echo "--------------------------"
	@echo "[*]Building $@."
	@echo "--------------------------"
	@echo "[0m"
	$(MAKE) -C $(LIBEXPAT_SRC_PATH) && $(MAKE) -C $(LIBEXPAT_SRC_PATH) install 
	
clean:
	$(MAKE) -C $(LIBEXPAT_SRC_PATH) clean 
	$(MAKE) -C $(DBUS_SRC_PATH) clean 
	$(RM) -fr $(BUILD_DIR) $(CONFIG_FLAG) 
	
	
	
	