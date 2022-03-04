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

unsigned conta_righe(const unsigned a, const unsigned b)
{
	assert( a>= size_gram && b >= size_gram, flag, stop );
	size_t mem = (size_t)(-1) / b;
	return mem < a-size_gram+1 ? mem : ( a - size_gram + 1 );
}
	
void insert(FILE* f, t_index index, const size_t cnt)
{
	size_t buffer = cnt;
	fwrite(index,sizeof(unsigned),size_gram,f); // senza assert
	fwrite(&buffer,sizeof(size_t),1,f);  // senza assert
}

FILE* conta_elem(size_t* cnt, FILE* f)
{
	if(*cnt != 0) return f; // questo controllo nell'algoritmo rende questa funzione usabile 1 sola volta
	rewind(f);
	FILE* f_out = tmpfile();
	assert(f_out != NULL, flag, end);
	unsigned * buf_line = (unsigned*)calloc(size_gram,sizeof(unsigned));
	assert(buf_line != NULL, flag, end);
	size_t buf_cnt = 0;
	while(1)
	{
		if(fread(buf_line, sizeof(unsigned),size_gram, f) != size_gram) break;
		fread(&buf_cnt, sizeof(size_t),1, f);
		fwrite(buf_line, sizeof(unsigned),size_gram, f_out);
		fwrite(&buf_cnt, sizeof(size_t),1, f_out);
		(*cnt)++;
	}
	free(buf_line);
	fclose(f);
	return f_out;
}

void merge_file(FILE* src1, FILE* src2, FILE* dst, size_t* cnt)
{
	*cnt = 0;
	t_key K1 = {0}, K2 = {0};
	K1.gram = (unsigned*)calloc(size_gram,sizeof(unsigned));
	assert(K1.gram!=NULL,flag,end);
	K2.gram = (unsigned*)calloc(size_gram,sizeof(unsigned));
	assert(K2.gram!=NULL,flag,end);
	int flg[2] = {0};
	while(1)
	{
		
		if(flg[0]==0)
		{
			assert(fread(K1.gram, sizeof(unsigned),size_gram, src1) == size_gram, flag , flag; fprintf(stderr,"\n"); break);
			assert(fread(&(K1.occurrence), sizeof(size_t),1, src1) == 1, flag, end);
		}
		if(flg[1]==0)
		{
			assert(fread(K2.gram, sizeof(unsigned),size_gram, src2) == size_gram, flag , flag; fprintf(stderr,"\n"); break);
			assert(fread(&(K2.occurrence), sizeof(size_t),1, src2) == 1, flag, end);
		}
		int mem = cmp(K1.gram, K2.gram);
		if(mem==0)
		{
			(*cnt)++;
			assert((size_t)(-1) - (K1.occurrence) >= (K2.occurrence), flag, flag)
			insert(dst,K1.gram,(K1.occurrence)+(K2.occurrence));
			flg[0]=0;
			flg[1]=0;
		}
		else if(mem>0)
		{
			(*cnt)++;
			insert(dst,K2.gram,(K2.occurrence));
			flg[0]=1;
			flg[1]=0;
		}
		else
		{
			(*cnt)++;
			insert(dst,K1.gram,(K1.occurrence));
			flg[0]=0;
			flg[1]=1;
		}
	}
	while(1)
	{
		assert(fread(K1.gram, sizeof(unsigned),size_gram, src1) == size_gram, flag , flag; fprintf(stderr,"\n"); break);
		assert(fread(&(K1.occurrence), sizeof(size_t),1, src1) == 1, flag, end);
		(*cnt)++;
		insert(dst,K1.gram,(K1.occurrence));
	}
	while(1)
	{
		assert(fread(K2.gram, sizeof(unsigned),size_gram, src2) == size_gram, flag , flag; fprintf(stderr,"\n"); break);
		assert(fread(&(K2.occurrence), sizeof(size_t),1, src1) == 1, flag, end);
		(*cnt)++;
		insert(dst,K2.gram,(K2.occurrence));
	}
	free(K2.gram);
	free(K1.gram);
}

FILE* fondi(FILE** tmp_file, size_t a, size_t b, size_t* cnt)
{
	if(a+1==b) return conta_elem(cnt,tmp_file[a]);
	FILE* tmp_1 = fondi(tmp_file, a, (a+b)/2, cnt);
	FILE* tmp_2 = fondi(tmp_file, (a+b)/2, b, cnt);
	if(tmp_1 == NULL || tmp_2 == NULL) return NULL;
	rewind(tmp_1);
	rewind(tmp_2);
	FILE* tmp_out = tmpfile();
	merge_file(tmp_1,tmp_2,tmp_out,cnt);
	fclose(tmp_1);
	fclose(tmp_2);
	return tmp_out;
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

void ftor(const size_t b, const size_t N, t_mat M, FILE* f)
{
	char* buffer = (char*)calloc(b,sizeof(char));
	assert(buffer!=NULL,flag,end);
	for( size_t r = glob_value; r<size_gram; r++)
	{
		for( size_t c = 0; c < b; c++)
		{
			buffer[c] = fgetc(f)-'0';
			fgetc(f); //salto lo spazio dopo il bit
		}
		fgetc(f); //salto il line feed
		insert_line(M[0],buffer,b);
	}
	for( size_t r = 1; r < N; r++)
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
	glob_value = 1; //viene settato ora per futuri cicli della medesima analisi
}

void inFILE(const size_t index, FILE* f, t_index* I)
{
	size_t cnt = 0;
	for(size_t k = 1; k<index; k++)
	{
		cnt++;
		if(cmp(&(I[k-1]), &(I[k])) != 0)
		{
			insert(f,I[k-1], cnt);
			cnt = 0;
		}
	}
	//assert((size_t)(-1)-1>=cnt, flag , stop);
	cnt++;
	insert(f,I[index-1], cnt);
}

void sum(FILE* f_out, FILE** tmp_file, size_t dim, size_t total)
{
	size_t cnt = 0;
	FILE* f = fondi(tmp_file, 0, dim, &cnt);
	{ // formato
		prlg format = { total, cnt, size_gram };
		assert(fwrite(&format,sizeof(prlg),1,f_out) == 1, flag, end);
	}
	rewind(f);
	size_t* buf_counter = (size_t*)calloc(1,sizeof(size_t));
	assert(buf_counter != NULL, flag, end);
	unsigned* buf_line = (unsigned*)calloc(size_gram,sizeof(unsigned));
	assert(buf_line != NULL, flag, end);
	while(1)
	{
		if(fread( buf_line, sizeof(unsigned), size_gram, f ) != size_gram) break; //senza assert
		fwrite( buf_line, sizeof(unsigned), size_gram, f_out); //senza assert
		fread( buf_counter, sizeof(size_t), 1, f); //senza assert
		fwrite( buf_counter, sizeof(size_t), 1, f_out); //senza assert
	}
	free(buf_counter);
	free(buf_line);
	fclose(f);
}

FILE* loc_sintex(const size_t b, const size_t N, t_mat M, t_index* I, FILE* f)
{
	if(N==0) return NULL;
	ftor(b, N, M, f);
	size_t I_dim = N*(b-size_gram+1);
	qsort(I,I_dim,sizeof(size_t),cmp);
	FILE* tmp = tmpfile();
	assert(tmp != NULL, flag, end);
	inFILE(I_dim, tmp, I );
	for(size_t c = 0; c<b; c++) M[0][c] = M[N-1][c];
	return tmp;
}

void main_sintex(FILE* f, FILE* f_out)
{
	jmp_magic_number(f);
	size_t colonne_file = 0;
	size_t righe_file = 0;
	assert(fscanf(f,"%lu %lu\n", &colonne_file, &righe_file)>=0, flag, end);
	size_t N = conta_righe(righe_file,colonne_file);
	righe_file -= size_gram - 1;
	t_mat M = new_matrix(N,colonne_file);
	size_t dim_I = N*(colonne_file-size_gram+1);
	t_index* I = (t_index*)calloc(dim_I, sizeof(t_index));
	assert(I!=NULL, flag,end);
	{ // inizializzo I
		size_t k = 0;
		for(size_t r = 0;r<N;r++)
			for(size_t c= 0;c <= colonne_file-size_gram; c++)
			{
				I[k]=M[r]+c;
				k++;
			}
	}
	size_t dim_tmp = righe_file / N +((righe_file%N)!= 0); //numero di file temporanei
	report("%lu ",dim_tmp);
	assert(TMP_MAX >= dim_tmp, flag, end);
	FILE** tmp_file = (FILE**)calloc(dim_tmp,sizeof(FILE*));
	assert(tmp_file!=NULL,flag,end);
	for(size_t k = 1; k<dim_tmp;k++)
		tmp_file[k] = loc_sintex(colonne_file,N,M,I,f);
	if(dim_tmp>1)
	{
		size_t k = 0;
		for(size_t r = 0;r<righe_file-N*(dim_tmp-1);r++)
			for(size_t c= 0;c <= colonne_file-size_gram; c++)
			{
				I[k]=M[r]+c;
				k++;
			}
	}
	tmp_file[0] = loc_sintex(colonne_file, righe_file - N * (dim_tmp -1), M, I , f);
	for(size_t i = 0; i<N;i++) free(M[i]);
	free(M);
	free(I);
	colonne_file-=size_gram-1;
	sum(f_out,tmp_file,dim_tmp,righe_file*colonne_file);
	free(tmp_file);
}

void sintex( const char* name_file_in, const size_t SIZE_GRAM)
{
	FILE *file_out = NULL,*file_in = NULL;
	glob_value = 0;
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
	report("%d ", glob_value);
	report("%u ", FILTER);
	report("%s ", nome_file_in);
	main_sintex(file_in,file_out);
	fclose(file_out);
	fclose(file_in);
}










