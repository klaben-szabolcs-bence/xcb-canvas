CC:=gcc
STD:=-std=gnu99
INCLUDE:=-Iinclude/
OUTPUT_FOLDER:=build
PKG_CONF:=pkg-config --cflags --libs
WRAP:=-Wl,--wrap=xcbcanvas_init_xcb,--wrap=canvas_rendering_context_t

example: lib
	$(CC) $(STD) $(INCLUDE) -Wall src/example.c -o $(OUTPUT_FOLDER)/example \
	$(OUTPUT_FOLDER)/lib-xcb-canvas.a `$(PKG_CONF) xcb x11`

tests: clean canvas.o xcb-canvas.o tests.o ctx.o
	$(CC) $(STD) $(INCLUDE) -Wall -g tests/* \
	$(OUTPUT_FOLDER)/canvas.o $(OUTPUT_FOLDER)/xcb-canvas.o $(OUTPUT_FOLDER)/tests.o $(OUTPUT_FOLDER)/ctx.o \
	-o $(OUTPUT_FOLDER)/tests `$(PKG_CONF) xcb x11 cmocka` $(WRAP) && \
	./$(OUTPUT_FOLDER)/tests

lib: tests clean xcb-canvas.o canvas.o ctx.o
	cd $(OUTPUT_FOLDER); \
	ar rcs lib-xcb-canvas.a xcb-canvas.o canvas.o ctx.o

tests.o:
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/tests.c -o $(OUTPUT_FOLDER)/tests.o `$(PKG_CONF) cmocka` $(WRAP)

xcb-canvas.o:
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/xcb-canvas.c -o $(OUTPUT_FOLDER)/xcb-canvas.o `$(PKG_CONF) xcb x11`

ctx.o:
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/canvas-rendering-context.c -o $(OUTPUT_FOLDER)/ctx.o

canvas.o:
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/canvas.c -o $(OUTPUT_FOLDER)/canvas.o

clean:
	rm -rf $(OUTPUT_FOLDER)
	@mkdir $(OUTPUT_FOLDER)
