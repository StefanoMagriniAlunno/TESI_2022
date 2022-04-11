#include "../hdr/evidence.h"
#include "../hdr/err_mng.h"

#include <math.h>
#include <limits.h>

typedef struct t_index_ev
{
	unsigned* ref;
	size_t r;
	size_t c;
} t_index_ev;			// tipo dell'indice
typedef unsigned** t_mat;	// tipo della matrice

size_t size_gram_ev = 1;
unsigned FILTER_ev = 1;

void setting_ev( void )
{
	assert(size_gram_ev <= CHAR_BIT * sizeof(unsigned), flag, stop);
	FILTER_ev = ~((~FILTER_ev)<<( size_gram_ev - 1 ));
}	

void push_ev( unsigned* ref_line_gram, const char bit)
{
	*ref_line_gram = ((*ref_line_gram) << 1) ^ bit;
	*ref_line_gram &= FILTER_ev;
}

int cmp_line_gram_ev(unsigned* a, unsigned* b)
{
	if(*a < *b ) return -1;
	return *a > *b;
}

int cmp_ev(const void* a, const void* b)
{
	unsigned* ref_line_gram_a = (((t_index_ev*)a)->ref);
	unsigned* ref_line_gram_b = (((t_index_ev*)b)->ref);
	for(size_t k = 0; k<size_gram_ev; k++)
	{
		int ret = cmp_line_gram_ev(ref_line_gram_a,ref_line_gram_b);
		if(ret < 0) return -1;
		else if( ret > 0) return 1;
		ref_line_gram_a++;
		ref_line_gram_b++;
	}
	return 0;
}

void jmp_magic_number_ev(FILE* f)
{
	while( fgetc(f) != '\n' );
}
	
void insert_ev(t_index_ev* I, size_t k, const size_t cnt, size_t** NEW_MATRIX)
{
	for (size_t i = 0; i <= cnt; i++)
		NEW_MATRIX[I[k - i].r][I[k - i].c] = cnt;
}

t_mat new_matrix_ev( const size_t row, const size_t column)
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

void insert_line_ev( unsigned* ref_line, char* buffer, const size_t b)
{
	for(size_t c = 0; c < b; c++)
	{
		push_ev(ref_line, *buffer);
		buffer++;
		ref_line++;
	}
}

void insert_next_line_ev( unsigned* ref_pred_line, unsigned* ref_line, char* buffer,const size_t b)
{
	for(size_t c = 0; c<b;c++)
	{
		*ref_line = *ref_pred_line;
		push_ev(ref_line, *buffer);
		buffer++;
		ref_line++;
		ref_pred_line++;
	}
}

void ftor_ev(const size_t b, const size_t N, t_mat M, FILE* f)
{
	char* buffer = (char*)calloc(b,sizeof(char));
	assert(buffer!=NULL,flag,end);
	for( size_t r = 0; r<size_gram_ev; r++)
	{
		for( size_t c = 0; c < b; c++)
		{
			buffer[c] = fgetc(f)-'0';
			fgetc(f); //salto lo spazio dopo il bit
		}
		fgetc(f); //salto il line feed
		insert_line_ev(M[0],buffer,b);
	}
	for( size_t r = 1; r < N; r++)
	{
		for( size_t c = 0; c < b; c++)
		{
			buffer[c] = fgetc(f)-'0';
			fgetc(f); //salto lo spazio dopo il bit
		}
		fgetc(f); //salto il line feed
		insert_next_line_ev(M[r-1],M[r],buffer,b);
	}
	free(buffer);
}

size_t** inMAT(const size_t index, t_index_ev* I, size_t row, size_t column)
{
	size_t** NEW_MATRIX = (size_t**)calloc(row,sizeof(size_t*));
	assert(NEW_MATRIX!=NULL, flag, end);
	for(size_t i = 0;i<row;i++)
	{
		NEW_MATRIX[i]=(size_t*)calloc(column, sizeof(size_t));
		//assert(M[i]!=NULL, flag, end);
	}
	
	size_t cnt = 0;
	for(size_t k = 1; k<index; k++)
	{
		cnt++;
		if(cmp_ev(&(I[k-1]), &(I[k])) != 0)
		{
			insert_ev(I,k,cnt,NEW_MATRIX);
			cnt = 0;
		}
	}
	//assert((size_t)(-1)-1>=cnt, flag , stop);
	cnt++;
	insert_ev(I,index-1,cnt,NEW_MATRIX);
	return NEW_MATRIX;
}

void loc_evidence(const size_t b, const size_t a, t_mat M, t_index_ev* I, FILE* f, FILE* f_out)
{
	ftor_ev(b+size_gram_ev-1, a, M, f);
	size_t I_dim = a*b;
	qsort(I,I_dim,sizeof(t_index_ev),cmp_ev);
	size_t** NEW_MATRIX = inMAT(I_dim, I, a, b );
	fprintf(f_out,"P2\n%lu %lu\n255\n",b,a);
	for(size_t r = 0;r<a;r++)
	{
		for (size_t c = 0; c < b; c++)
			fprintf(f_out, "%lu ", (unsigned long) (255.*log((double)(NEW_MATRIX[r][c])) / log(a * b)));
		free(NEW_MATRIX[r]);
		fprintf(f_out,"\n");
	}
	free(NEW_MATRIX);
}

void main_evidence(FILE* f, FILE* f_out)
{
	jmp_magic_number_ev(f);
	size_t colonne_file = 0;
	size_t righe_file = 0;
	assert(fscanf(f,"%lu %lu\n", &colonne_file, &righe_file)>=0, flag, end);
	righe_file -= size_gram_ev - 1;
	t_mat M = new_matrix_ev(righe_file,colonne_file);
	colonne_file-=size_gram_ev-1;
	size_t dim_I = righe_file*colonne_file;
	t_index_ev* I = (t_index_ev*)calloc(dim_I, sizeof(t_index_ev));
	assert(I!=NULL, flag,end);
	{ // inizializzo I
		size_t k = 0;
		for(size_t r = 0;r<righe_file;r++)
			for(size_t c= 0;c < colonne_file; c++)
			{
				I[k].ref=M[r]+c;
				I[k].r=r;
				I[k].c=c;
				k++;
			}
	}
	loc_evidence(colonne_file, righe_file, M, I, f, f_out);
	for(size_t i = 0; i<righe_file;i++) free(M[i]);
	free(M);
	free(I);
}

void evidence( const char* name_file_in, const size_t SIZE_GRAM)
{
	FILE *file_out = NULL,*file_in = NULL;
	FILTER_ev = 1;
	size_gram_ev = SIZE_GRAM;
	setting_ev();
	{ // apro i file
		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_in = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_in!=NULL,flag,end);
		assert(sprintf(percorso_out,"../EVIDENCE/%s",name_file_in)>=0,flag,end);
		assert(sprintf(percorso_in,"../DATA/%s",name_file_in)>=0,flag,end);
		remove(percorso_out);
		file_out = fopen(percorso_out,"wb");
		file_in = fopen(percorso_in,"r");
		assert(file_out,flag,end);
		assert(file_in,flag,end);
		free(percorso_out);
		free(percorso_in);
	}
	report("%u ", FILTER_ev);
	report("%s ", name_file_in);
	main_evidence(file_in,file_out);
	fclose(file_out);
	fclose(file_in);
}

