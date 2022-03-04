all:
	gcc -std=c99 -c ../src/sorting.c ../hdr/types.h ../hdr/sorting.h
	gcc -std=c99 -c ../src/distance.c ../hdr/types.h ../hdr/distance.h
	gcc -std=c99 -c ../src/sintex.c ../hdr/types.h ../hdr/sintex.h
	gcc -std=c99 -c ../src/main.c ../hdr/types.h ../hdr/sintex.h ../hdr/distance.h ../hdr/err_mng.h ../hdr/sorting.h
	gcc -std=c99 -o main main.o sintex.o distance.o sorting.o
