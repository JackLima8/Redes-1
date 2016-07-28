All: ftp.c lib_ftp.o
	gcc -o ftp ftp.c  lib_ftp.o -lm
lib_ftp.o: lib_ftp.c
	gcc -o lib_ftp.o -c lib_ftp.c
clean:
	rm -rf *.o
	rm -rf *.c~
	rm -rf *.h~
	rm -rf *~
	rm -rf ftp
	rm -rf teste.txt
