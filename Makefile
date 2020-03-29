make: encrypt.c
	gcc encrypt.c -o encrypt -lpthread


clean:
	rm -f encrypt
