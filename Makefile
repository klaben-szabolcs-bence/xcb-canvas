CC:=gcc
OUTPUT_FOLDER:=bin

all:
	@mkdir $(OUTPUT_FODLER)
	$(CC) -Wall src/main.c -o main `pkg-config --cflags --libs xcb`

clean:
	rm $(OUTPUT_FODLER)/*
	rmdir $(OUTPUT_FODLER)
