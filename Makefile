CC:=gcc
STD:=-std=c18
INCLUDE:=-Iinclude/
OUTPUT_FOLDER:=build

example: tests
	$(CC) $(STD) $(INCLUDE) -Wall src/example.c -o $(OUTPUT_FOLDER)/example $(OUTPUT_FOLDER)/lib-xcb-canvas.a `pkg-config --cflags --libs xcb`

tests: lib
	$(CC) $(STD) $(INCLUDE) -Wall tests/* -o $(OUTPUT_FOLDER)/tests $(OUTPUT_FOLDER)/lib-xcb-canvas.a `pkg-config --cflags --libs xcb cmocka`; \
	./$(OUTPUT_FOLDER)/tests

lib: clean xcb-canvas.o canvas.o
	cd $(OUTPUT_FOLDER); \
	ar rcs lib-xcb-canvas.a xcb-canvas.o canvas.o

xcb-canvas.o: 
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/xcb-canvas.c -o $(OUTPUT_FOLDER)/xcb-canvas.o `pkg-config --cflags --libs xcb`

canvas.o:
	$(CC) $(STD) $(INCLUDE) -Wall -g -c src/canvas.c -o $(OUTPUT_FOLDER)/canvas.o

clean:
	rm -rf $(OUTPUT_FOLDER)
	@mkdir $(OUTPUT_FOLDER)
