# $Id$
# Works with both GNU and BSD make 
#

PROGNAME=	cdpc

OTHER_OBJ=	main.o cdp.o output_plain.o output_csv.o

CFLAGS+= -g -Wall
LDFLAGS+= -lpcap

OBJECTS= ${OTHER_OBJ}

all:	${OBJECTS}
	${CC} ${LDFLAGS} -o ${PROGNAME} ${OBJECTS}

clean:
	rm -f *.o *~ ${PROGNAME} ${PROGNAME}.core

