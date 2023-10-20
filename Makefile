CC:=gcc
OUTPUT_FOLDER:=bin

all: clean
	@mkdir $(OUTPUT_FOLDER)
	$(CC) -Iinclude/ -Wall src/main.c -o $(OUTPUT_FOLDER)/main `pkg-config --cflags --libs xcb`

clean:
	rm -rf $(OUTPUT_FOLDER)
