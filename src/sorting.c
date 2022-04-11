#include "../hdr/sorting.h"
#include "../hdr/err_mng.h"
#include "../hdr/types.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int cmp_sintex(const void* a, const void* b)
{
	size_t a_occurrence = ((t_key_s*)a)->occurrence;
	size_t b_occurrence = ((t_key_s*)b)->occurrence;
	if(a_occurrence < b_occurrence) return -1;
	return a_occurrence > b_occurrence;
}
int cmp_distance(const void* a, const void* b)
{
	double a_distance = ((t_key_d*)a)->distance;
	double b_distance = ((t_key_d*)b)->distance;
	if(a_distance<b_distance) return -1;
	return a_distance>b_distance;
}
void sort_sintex(const char* name_file_in)
{
	FILE *file_out = NULL,*file_in = NULL;
	{ // apro i file
		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_in = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_in!=NULL,flag,end);
		assert(sprintf(percorso_out,"../SORT_SINTEX/%s",name_file_in)>=0,flag,end);
		assert(sprintf(percorso_in,"../SINTEX/%s",name_file_in)>=0,flag,end);
		remove(percorso_out);
		file_out = fopen(percorso_out,"w");
		file_in = fopen(percorso_in,"rb");
		assert(file_out,flag,end);
		assert(file_in,flag,end);
		free(percorso_out);
		free(percorso_in);
	}
  
	prlg format = {0};
	assert(fread(&format,sizeof(prlg),1,file_in)==1,flag,end);
	t_key_s* VEC = (t_key_s*)calloc(format.counter,sizeof(t_key_s));
	assert(VEC,flag,end);
	{ // inizializzazione del vettore di chiavi
		for(size_t i = 0;i<format.counter;i++)
		{
			VEC[i].gram = (unsigned*)calloc(format.size_gram, sizeof(unsigned));	//alloco il gram
			// dereferenzazione del puntatore non eseguita
	  		fread(VEC[i].gram,sizeof(unsigned),format.size_gram,file_in); //senza assert
	  		fread(&(VEC[i].occurrence),sizeof(size_t),1,file_in); //senza assert
	  	}
	}
	qsort(VEC,format.counter,sizeof(t_key_s),cmp_sintex);
	fprintf(file_out, "%lu\n", (unsigned long)(format.counter)); //senza assert
	for(size_t i=0;i<format.counter;i++)
	{
		for(size_t k=0;k<format.size_gram;k++)
			fprintf(file_out,"%u\t",VEC[i].gram[k]); //senza assert
		free(VEC[i].gram);
		fprintf(file_out,"%lu\n",(unsigned long)(VEC[i].occurrence)); //senza assert
	}
	free(VEC);
	fclose(file_in);
	fclose(file_out);
}

void sort_distance(const char* name_file_in, const size_t num_file)
{
	FILE *file_out = NULL,*file_in = NULL;
	{ // apro i file
		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_in = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_in!=NULL,flag,end);
		assert(sprintf(percorso_out,"../SORT_DISTANCE/%s",name_file_in)>=0,flag,end);
		assert(sprintf(percorso_in,"../DISTANCE/%s",name_file_in)>=0,flag,end);
		remove(percorso_out);
		file_out = fopen(percorso_out,"w");
		file_in = fopen(percorso_in,"r");
		assert(file_out,flag,end);
		assert(file_in,flag,end);
		free(percorso_out);
		free(percorso_in);
	}
	
	t_key_d* VEC = (t_key_d*)calloc(num_file,sizeof(t_key_d));
	assert(VEC,flag,end);
	{ // inizializzazione del vettore di chiavi
		t_key_d* iter = VEC;
		for(size_t i = 0;i<num_file;i++,iter++)
	  		assert(fscanf(file_in,"%s \t%lf\n",iter->name_file,&(iter->distance))>=0,flag,end);
	}
	qsort(VEC,num_file,sizeof(t_key_d),cmp_distance);
	for(size_t i=0;i<num_file;i++)
		assert(fprintf(file_out, "%s \t%.15lf\n",VEC[i].name_file, VEC[i].distance)>=0,flag,end);
	free(VEC);
	fclose(file_in);
	fclose(file_out);
}
