INCLUDES	+= ./
CFLAGS	+= -I$(INCLUDES)
OBJCFLAGS	+= $(CFLAGS)

OBJDIR	= obj

_DEPS	= hashtable.h
_SOURCE	= hashtable.c

DEPS	= $(_DEPS)
OBJ	= $(patsubst %, $(OBJDIR)/%, $(_SOURCE:.c=.o))

.PHONY:	all
.PHONY:	clean

all:	$(OBJDIR) test test_auto measure measure_auto measure_maptable

$(OBJDIR):	
	mkdir -p $(OBJDIR)

test:	$(OBJ) $(OBJDIR)/test.o
	$(CC) -o $@ $^ $(CFLAGS)
	
test_auto:	$(OBJ) $(OBJDIR)/test_auto.o
	$(CC) -o $@ $^ $(CFLAGS)

measure:	$(OBJ) $(OBJDIR)/measure.o
	$(CC) -o $@ $^ $(CFLAGS)
	
measure_auto:	$(OBJ) $(OBJDIR)/measure_auto.o
	$(CC) -o $@ $^ $(CFLAGS)

ifeq ($(shell uname -s),Darwin)
measure_maptable:	$(OBJ) $(OBJDIR)/measure_maptable.o
	$(CC) -framework Foundation -o $@ $^ $(OBJCFLAGS)
else
.PHONY:	measure_maptable
measure_maptable:
endif

$(OBJDIR)/%.o:	%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR)/%.o:	%.m $(DEPS)
	$(CC) -c -o $@ $< $(OBJCFLAGS)

clean:
	rm -f $(OBJDIR)/*.o test measure measure_maptable measure_auto \
	test_auto
