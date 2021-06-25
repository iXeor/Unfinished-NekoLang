CC = gcc
CFLAGS = -g -DDEBUG -lm -Wall -I VM -I parser -I include -I main -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -Wincompatible-pointer-types
TARGET = neko
DIRS = include main parser VM
CFILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o,$(CFILES))
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)
clean:
	-$(RM) $(TARGET) $(OBJS)
r: clean $(TARGET)