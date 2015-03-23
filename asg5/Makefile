# $Id: Makefile,v 1.2 2015-03-12 23:49:27-07 - - $
# Brad Cardello (bcardell)
# Shayan Farmani (sfarmani)
# asg5

GPP        = g++ -g -O0 -Wall -Wextra -std=gnu++11

DEPFILE    = Makefile.dep
HEADERS    = sockets.h signal_action.h cix_protocol.h logstream.h
CPPLIBS    = sockets.cpp signal_action.cpp cix_protocol.cpp 
CPPSRCS    = ${CPPLIBS} cix-daemon.cpp cix-client.cpp cix-server.cpp
LIBOBJS    = ${CPPLIBS:.cpp=.o}
CLIENTOBJS = cix-client.o ${LIBOBJS}
SERVEROBJS = cix-server.o ${LIBOBJS}
DAEMONOBJS = cix-daemon.o ${LIBOBJS}
OBJECTS    = ${CLIENTOBJS} ${SERVEROBJS} ${DAEMONOBJS}
EXECBINS   = cix-client cix-server cix-daemon
SOURCES    = ${HEADERS} ${CPPSRCS} Makefile README PARTNER

LISTING     = Listing.ps
CLASS       = cmps109-wm.w15
PROJECT     = asg5

all: ${DEPFILE} ${EXECBINS}

cix-client: ${CLIENTOBJS}
	${GPP} -o $@ ${CLIENTOBJS}

cix-server: ${SERVEROBJS}
	${GPP} -o $@ ${SERVEROBJS}

cix-daemon: ${DAEMONOBJS}
	${GPP} -o $@ ${DAEMONOBJS}

%.o: %.cpp
	${GPP} -c $<

ci:
	- checksource ${SOURCES}
	- cid + ${SOURCES}

lis: all ${SOURCES} ${DEPFILE}
	mkpspdf ${LISTING} ${SOURCES} ${DEPFILE}

clean:
	- rm ${LISTING} ${LISTING:.ps=.pdf} ${OBJECTS} Makefile.dep

spotless: clean
	- rm ${EXECBINS}

submit : ${SOURCES}
	- checksource ${SOURCES}
	submit ${CLASS} ${PROJECT} ${SOURCES}

dep:
	- rm ${DEPFILE}
	make --no-print-directory ${DEPFILE}

${DEPFILE}:
	${GPP} -MM ${CPPSRCS} >${DEPFILE}

again: ${SOURCES}
	make --no-print-directory spotless ci all lis

include ${DEPFILE}

