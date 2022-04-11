#include "../hdr/sintex.h"
#include "../hdr/types.h"

#include <limits.h>

typedef unsigned* t_index;	// tipo dell'indice
typedef unsigned** t_mat;	// tipo della matrice

size_t size_gram = 1;
unsigned FILTER = 1;
int glob_value = 0;

void setting( void )
{
	assert(size_gram <= CHAR_BIT * sizeof(unsigned), flag, stop);
	FILTER = ~((~FILTER)<<( size_gram - 1 ));
}	

void push( unsigned* ref_line_gram, const char bit)
{
	*ref_line_gram = ((*ref_line_gram) << 1) ^ bit;
	*ref_line_gram &= FILTER;
}

int cmp_line_gram(unsigned* a, unsigned* b)
{
	if(*a < *b ) return -1;
	return *a > *b;
}

int cmp(const void* a, const void* b)
{
	t_index ref_line_gram_a = *(t_index*)a;
	t_index ref_line_gram_b = *(t_index*)b;
	for(size_t k = 0; k<size_gram; k++)
	{
		int ret = cmp_line_gram(ref_line_gram_a,ref_line_gram_b);
		if(ret < 0) return -1;
		else if( ret > 0) return 1;
		ref_line_gram_a++;
		ref_line_gram_b++;
	}
	return 0;
}

void jmp_magic_number(FILE* f)
{
	while( fgetc(f) != '\n' );
}

t_mat new_matrix( const size_t row, const size_t column)
{
	t_mat M = (unsigned**)calloc(row,sizeof(unsigned*));
	assert(M!=NULL, flag, end);
	for(size_t i = 0;i<row;i++)
	{
		M[i]=(unsigned*)calloc(column, sizeof(unsigned));
		//assert(M[i]!=NULL, flag, end);
	}
	return M;
}

void insert_line( unsigned* ref_line, char* buffer, const size_t b)
{
	for(size_t c = 0; c < b; c++)
	{
		push(ref_line, *buffer);
		buffer++;
		ref_line++;
	}
}

void insert_next_line( unsigned* ref_pred_line, unsigned* ref_line, char* buffer,const size_t b)
{
	for(size_t c = 0; c<b;c++)
	{
		*ref_line = *ref_pred_line;
		push(ref_line, *buffer);
		buffer++;
		ref_line++;
		ref_pred_line++;
	}
}

void ftor(const size_t b, const size_t a, t_mat M, FILE* f)
{
	char* buffer = (char*)calloc(b,sizeof(char));
	assert(buffer!=NULL,flag,end);
	for( size_t r = 0; r<size_gram; r++)
	{
		for( size_t c = 0; c < b; c++)
		{
			buffer[c] = fgetc(f)-'0';
			fgetc(f); //salto lo spazio dopo il bit
		}
		fgetc(f); //salto il line feed
		insert_line(M[0],buffer,b);
	}
	for( size_t r = 1; r < a; r++)
	{
		for( size_t c = 0; c < b; c++)
		{
			buffer[c] = fgetc(f)-'0';
			fgetc(f); //salto lo spazio dopo il bit
		}
		fgetc(f); //salto il line feed
		insert_next_line(M[r-1],M[r],buffer,b);
	}
	free(buffer);
}

void inFILE(const size_t dim_I, t_index* I, FILE* file_out,  prlg* format )
{
	size_t cnt = 0;
	t_key* VEC = (t_key*)calloc(format->total, sizeof(t_key));
	assert(VEC != NULL, flag, end);
	for(size_t k = 1; k< dim_I; k++)
	{
		cnt++;
		if(cmp(&(I[k-1]), &(I[k])) != 0)
		{	
			VEC[format->counter].occurrence = cnt;
			VEC[format->counter].gram = I[k - 1];
			format->counter++;
			cnt = 0;
		}
	}
	//assert((size_t)(-1)-1>=cnt, flag , stop);
	cnt++;
	VEC[format->counter].occurrence = cnt;
	VEC[format->counter].gram = I[dim_I - 1];
	format->counter++;
	assert(fwrite(format, sizeof(prlg), 1, file_out)==1,flag,end);
	for (size_t i = 0; i < format->counter; i++)
	{
		fwrite(VEC[i].gram, sizeof(unsigned), size_gram, file_out);
		fwrite(&(VEC[i].occurrence), sizeof(size_t), 1, file_out);
	}
	free(VEC);
}

void main_sintex(FILE* file_in, FILE* file_out)
{
	jmp_magic_number(file_in);
	size_t colonne_file = 0;
	size_t righe_file = 0;
	assert(fscanf(file_in,"%lu %lu\n", &colonne_file, &righe_file)>=0, flag, end);
	righe_file -= size_gram - 1;
	t_mat M = new_matrix(righe_file,colonne_file);
	colonne_file -= size_gram - 1;
	size_t dim_I = righe_file *colonne_file;
	t_index* I = (t_index*)calloc(dim_I, sizeof(t_index));
	assert(I!=NULL, flag,end);
	{ // inizializzo I
		size_t k = 0;
		for(size_t r = 0;r<righe_file;r++)
			for(size_t c= 0;c < colonne_file; c++)
			{
				I[k]=M[r]+c;
				k++;
			}
	}
	ftor(colonne_file + size_gram - 1, righe_file, M, file_in);
	qsort(I, dim_I, sizeof(t_index), cmp);
	prlg format = { dim_I,0,size_gram };
	inFILE(dim_I, I, file_out, &format);
	for(size_t i = 0; i<righe_file;i++) free(M[i]);
	free(M);
	free(I);
}

void sintex( const char* name_file_in, const size_t SIZE_GRAM)
{
	FILE *file_out = NULL,*file_in = NULL;
	FILTER = 1;
	size_gram = SIZE_GRAM;
	setting();
	{ // apro i file
		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_in = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_in!=NULL,flag,end);
		assert(sprintf(percorso_out,"../SINTEX/%s",name_file_in)>=0,flag,end);
		assert(sprintf(percorso_in,"../DATA/%s",name_file_in)>=0,flag,end);
		remove(percorso_out);
		file_out = fopen(percorso_out,"wb");
		file_in = fopen(percorso_in,"r");
		assert(file_out,flag,end);
		assert(file_in,flag,end);
		free(percorso_out);
		free(percorso_in);
	}
	report("%u ", FILTER);
	report("%s ", name_file_in);
	main_sintex(file_in,file_out);
	fclose(file_out);
	fclose(file_in);
}










