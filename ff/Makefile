all: ../web/ff.js

rebuild: clean all

CC = emcc
CFLAGS = -O3 -I./include
LIBOBJS := ./lib/libavcodec.a ./lib/libavutil.a ./lib/libavformat.a
EMCCFLAGS = -gsource-map -g --bind -s ASYNCIFY -s WASM=1 -s ALLOW_TABLE_GROWTH=1 -s ALLOW_MEMORY_GROWTH=1 -s FILESYSTEM=0 -s ASSERTIONS=1

../web/ff.js: ff.cpp  qve_ff.cpp  wasm_http_protocol.c FFConfig.cpp $(LIBOBJS)
	$(CC) $(EMCCFLAGS) $(CFLAGS) -o $@ $^


clean:
	rm -f ff.js ff.wasm
