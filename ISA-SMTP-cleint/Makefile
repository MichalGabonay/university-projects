# Makefile for client/server example
# see 'man make' for more details
SMTPKLIENT     = smtpklient
SMTPKLIENT_SOURCES  = smtpklient.c

DEFINES         = 

CFLAGS         = -g -std=gnu99 -lm 

LIBRARIES       = #-llibrary_name

CC              = gcc
SMTPKLIENT_OBJECTS  = $(SMTPKLIENT_SOURCES:.c=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

###########################################

.SUFFIXES: .c .o

.c.o:
		$(CC) $(CFLAGS) -c $<

###########################################

all:		$(SMTPKLIENT)

rebuild:	clean all

$(SMTPKLIENT):	$(SMTPKLIENT_OBJECTS)
		$(CC) $(SMTPKLIENT_OBJECTS) $(LDFLAGS) -o $@


###########################################

clean:
	rm -fr core* *~ $(SMTPKLIENT_OBJECTS) $(SMTPKLIENT) .make.state .sb
