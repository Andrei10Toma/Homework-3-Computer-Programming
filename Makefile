build: bmp.c
	gcc bmp.c bmp_header.h -o bmp -Wall
run: build
	./bmp
clean:
	rm -f bmp

