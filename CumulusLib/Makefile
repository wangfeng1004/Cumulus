# source files.
OBJECTS = Address AESEngine AMFObjectWriter AMFReader AMFSimpleObject AMFWriter BinaryReader BinaryStream BinaryWriter Client CookieComputing Cookie Cumulus Flow FlowConnection FlowGroup FlowNull FlowStream FlowWriter Handshake Invoker Listener Logs MemoryStream Message Middle PacketReader PacketWriter Peer PoolThread PoolThreads Publication Publications QualityOfService RTMFP RTMFPReceiving RTMFPSending RTMFPServer ServerSession Session Sessions SocketManager Startable Streams Target Task TaskHandler Trigger Util

CC=g++
ifeq ($(shell uname -s),Darwin)
    LIB=libCumulus.dylib
	SHARED=-dynamiclib -install_name ./../CumulusLib/$(LIB)
else
	LIB=libCumulus.a #libCumulus.so
	SHARED=  #-shared
endif
SOURCES=./sources/
CFLAGS+=-D CUMULUS_LOGS -g -O0 -MD
LIBS ?= -L/usr/local/lib/ -lPocoFoundation -lPocoXML -lPocoUtil -lPocoNet -lcrypto -lssl
INCLUDES = -I/usr/local/include/ -I./include/
OBJECT = $(OBJECTS:%=%.o)

.PHONY: all
all: $(OBJECT)
	echo creating static lib $(LIB)
	#$(CC) $(SHARED) $(OBJECT) -o $(LIB) $(LIBS)
	ar rcs $(LIB) $(OBJECT) 
	ranlib $(LIB)

#$(OBJECT): 
#	echo compiling $(@:%.o=%.cpp)
#	$(CC) -fpic $(CFLAGS) $(INCLUDES) -c -o $(@) $(SOURCES)$(@:%.o=%.cpp)

%.o : sources/%.cpp
	echo compiling $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECT) $(LIB)
