CC := $(CC)
LD=$(CROSS)ld

INC := ./include
LD_LIBS := -ldbus-1 -lexpat 
LD_PATH := -L./lib
OUT_DIR := ./output/

$(shell mkdir -p $(OUT_DIR))

.PHONY: all clean dclient dserver

all: dclient dserver test_signal test_methodcall
	@echo "compiling test.c"
	@$(CC) -o $(OUT_DIR)test test.c -I$(INC) $(LD_LIBS) $(LD_PATH)
dserver:
	@echo "compiling dbus_test_server.c"
	@$(CC) -o $(OUT_DIR)$@ dbus_test_server.c -I$(INC) $(LD_LIBS) $(LD_PATH)
	
dclient:
	@echo "compiling dbus_test_client.c"
	@$(CC) -o $(OUT_DIR)$@ dbus_test_client.c -I$(INC) $(LD_LIBS) $(LD_PATH)

test_signal:
	@echo "compiling test_recv_signal.c"
	@$(CC) -o $(OUT_DIR)test_server test_recv_signal.c -I$(INC) $(LD_LIBS) $(LD_PATH)
	@echo "compiling test_send_signal.c"
	@$(CC) -o $(OUT_DIR)test_client test_send_signal.c -I$(INC) $(LD_LIBS) $(LD_PATH)
	
test_methodcall:
	@echo "compiling test_send_methodcall.c"
	@$(CC) -o $(OUT_DIR)client_methodcall test_send_methodcall.c -I$(INC) $(LD_LIBS) $(LD_PATH)
	@echo "compiling test_server_methodcall.c"
	@$(CC) -o $(OUT_DIR)server_methodcall test_recv_methodcall.c -I$(INC) $(LD_LIBS) $(LD_PATH)
	
clean:
	rm -fr *.o
	rm -fr $(OUT_DIR)

