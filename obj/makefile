all:
	gcc -c ../src/evidence.c ../hdr/evidence.h -lm
	gcc -c ../src/sorting.c ../hdr/types.h ../hdr/sorting.h -lm
	gcc -c ../src/distance.c ../hdr/types.h ../hdr/distance.h -lm
	gcc -c ../src/sintex.c ../hdr/types.h ../hdr/sintex.h -lm
	gcc -c ../src/main.c ../hdr/types.h ../hdr/sintex.h ../hdr/distance.h ../hdr/err_mng.h ../hdr/sorting.h ../hdr/evidence.h -lm
	gcc -o main main.o sintex.o distance.o sorting.o evidence.o -lm
