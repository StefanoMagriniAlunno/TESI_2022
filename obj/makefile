all:
	gcc -c ../src/sorting.c ../hdr/types.h ../hdr/sorting.h
	gcc -c ../src/distance.c ../hdr/types.h ../hdr/distance.h
	gcc -c ../src/sintex.c ../hdr/types.h ../hdr/sintex.h
	gcc -c ../src/main.c ../hdr/types.h ../hdr/sintex.h ../hdr/distance.h ../hdr/err_mng.h ../hdr/sorting.h
	gcc -o main main.o sintex.o distance.o sorting.o
