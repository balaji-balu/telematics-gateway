CC := $(TARGET_CC)
CFLAGS := $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include
LDFLAGS := $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib -lmosquitto -lcoap

SRC := gateway.c
OBJ := $(SRC:.c=.o)

TARGET := telematics-gateway

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)
