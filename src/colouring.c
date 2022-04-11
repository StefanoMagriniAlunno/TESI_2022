#include "../hdr/colouring.h"
#include "../hdr/types.h"
#include "../hdr/err_mng.h"

#include <math.h>
#include <limits.h>

typedef struct t_index_col
{
	unsigned* ref;
	size_t r;
	size_t c;
} t_index_col;			// tipo dell'indice
typedef unsigned** t_mat;	// tipo della matrice

size_t size_gram_col = 1;
unsigned FILTER_col = 1;

void setting_col( void )
{
	assert(size_gram_col <= CHAR_BIT * sizeof(unsigned), flag, stop);
	FILTER_col = ~((~FILTER_col)<<( size_gram_col - 1 ));
}	

void push_col( unsigned* ref_line_gram, const char bit)
{
	*ref_line_gram = ((*ref_line_gram) << 1) ^ bit;
	*ref_line_gram &= FILTER_col;
}

int cmp_line_gram_col(unsigned* a, unsigned* b)
{
	if(*a < *b ) return -1;
	return *a > *b;
}

int cmp_col(const void* a, const void* b)
{
	unsigned* ref_line_gram_a = (((t_key*)a)->gram);
	unsigned* ref_line_gram_b = (((t_key*)b)->gram);
	for(size_t k = 0; k<size_gram_col; k++)
	{
		int ret = cmp_line_gram_col(ref_line_gram_a,ref_line_gram_b);
		if(ret < 0) return -1;
		else if( ret > 0) return 1;
		ref_line_gram_a++;
		ref_line_gram_b++;
	}
	return 0;
}
/*
t_key* search(void* val, t_key* VEC, size_t counter)
{
	for (int i = 0; i < counter; i++)
	{
		if (cmp_col(&(VEC[i]), val) == 0) return &(VEC[i]);
	}
	return NULL;
}
*/
void insert_line_col( unsigned* ref_line, char* buffer, const size_t b)
{
	for(size_t c = 0; c < b; c++)
	{
		push_col(ref_line, *buffer);
		buffer++;
		ref_line++;
	}
}

void insert_next_line_col( unsigned* ref_pred_line, unsigned* ref_line, char* buffer,const size_t b)
{
	for(size_t c = 0; c<b;c++)
	{
		*ref_line = *ref_pred_line;
		push_col(ref_line, *buffer);
		buffer++;
		ref_line++;
		ref_pred_line++;
	}
}

void colouring( const char* name_file_in_A, const char* name_file_in_B, const char* name_file_out_A, const size_t SIZE_GRAM)
{
	//dato n-gramma in coordinata rc lui cerca questo in B.sintex
	//da questo calcola la distanza
	size_gram_col = SIZE_GRAM;
	FILTER_col = 1;
	setting_col();
	FILE *file_out = NULL,*file_A_sint = NULL,*file_B_sint,*file_imag = NULL;
	{ // apro i file

		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_A_sint= (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_A_sint!=NULL,flag,end);
		char* percorso_B_sint= (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_B_sint !=NULL,flag,end);
		char* percorso_imag = (char*)calloc(FILENAME_MAX, sizeof(char));
		assert(percorso_imag != NULL, flag, end);
		
		assert(sprintf(percorso_out,"../COLOURING/%s",name_file_out_A)>=0,flag,end);
		assert(sprintf(percorso_A_sint,"../SINTEX/%s",name_file_in_A)>=0,flag,end);
		assert(sprintf(percorso_B_sint,"../SINTEX/%s",name_file_in_B)>=0,flag,end);
		assert(sprintf(percorso_imag, "../DATA/%s", name_file_in_B) >= 0, flag, end);
		
		remove(percorso_out);
		file_out = fopen(percorso_out, "w");
		file_A_sint = fopen(percorso_A_sint, "rb");
		file_B_sint = fopen(percorso_B_sint, "rb");
		file_imag = fopen(percorso_imag, "r");

		assert(file_out,flag,end);
		assert(file_A_sint,flag,end);
		assert(file_B_sint,flag,end);
		assert(file_imag, flag, end);
		free(percorso_out);
		free(percorso_A_sint);
		free(percorso_B_sint);
		free(percorso_imag);
	}
	//recupero evidence
	unsigned colonne_A = 0, righe_A = 0;
	assert(fscanf(file_imag, "P1\n%u %u\n", &colonne_A, &righe_A)!=0,flag,end);
	fprintf(file_out, "P3\n%lu %lu\n255\n", colonne_A-SIZE_GRAM+1, righe_A-SIZE_GRAM+1);
	//recupero sintex

	prlg format_sub = { 0 };
	prlg format_obj = { 0 };
	assert(fread(&format_sub, sizeof(prlg), 1, file_A_sint) == 1, flag, end);
	assert(fread(&format_obj, sizeof(prlg), 1, file_B_sint) == 1, flag, end);
	assert(format_sub.size_gram == format_obj.size_gram, flag, stop);
	t_key* VECs = (t_key*)calloc(format_sub.counter, sizeof(t_key));
	assert(VECs, flag, end);
	t_key* VECo = (t_key*)calloc(format_obj.counter, sizeof(t_key));
	assert(VECo, flag, end);
	{ // inizializzazione dei vettori di chiavi
		for (size_t i = 0; i < format_sub.counter; i++)
		{
			VECs[i].gram = (unsigned*)calloc(format_sub.size_gram, sizeof(unsigned));	//alloco il gram
			// senza controllo dereferenzazione di iter->gram
			fread(VECs[i].gram, sizeof(unsigned), format_sub.size_gram, file_A_sint); //senza assert
			fread(&(VECs[i].occurrence), sizeof(size_t), 1, file_A_sint); //senza assert
		}
		for (size_t i = 0; i < format_obj.counter; i++)
		{
			VECo[i].gram = (unsigned*)calloc(format_obj.size_gram, sizeof(unsigned));	//alloco il gram
			// senza controllo dereferenzazione di iter->gram
			fread(VECo[i].gram, sizeof(unsigned), format_obj.size_gram, file_B_sint);
			fread(&(VECo[i].occurrence), sizeof(size_t), 1, file_B_sint);
		}
	}
	//inizializzo v
	unsigned* v = (unsigned*)calloc(colonne_A, sizeof(unsigned));
	assert(v != NULL, flag, end);
	char* buffer = (char*)calloc(colonne_A, sizeof(char));
	assert(buffer != NULL, flag, end);
	for (size_t r = 0; r < SIZE_GRAM; r++)
	{
		for (size_t c = 0; c < colonne_A; c++)
		{
			buffer[c] = fgetc(file_imag) - '0';
			fgetc(file_imag); //salto lo spazio dopo il bit
		}
		fgetc(file_imag); //salto il line feed
		insert_line_col(v, buffer, colonne_A);
	}
	//coloro
	t_key val = { .gram = NULL, .occurrence = 0 };
	for (unsigned row = 0; row < righe_A-SIZE_GRAM+1; row++)
	{
		for (unsigned col = 0; col < colonne_A-SIZE_GRAM+1; col++)
		{
			val.gram = &(v[col]);
			t_key* Io = bsearch(&val, VECo, format_obj.counter, sizeof(t_key), cmp_col);
			t_key* Is = bsearch(&val, VECs, format_sub.counter, sizeof(t_key), cmp_col);
			if (Io != NULL)
			{
				double freq_sub = (double)(Is->occurrence) / format_sub.total;
				double freq_obj = (double)(Io->occurrence) / format_obj.total;
				// assert( freq_sub != 0 || freq_obj != 0, flag, stop);
				double r = freq_sub < freq_obj ? freq_sub / freq_obj : freq_obj / freq_sub; // per evitare divisioni per 0 
				r = (1 - r) / (1 + r); // 0 <= r <= 1, divisione poco suscettibile ad errori
				r *= r;
				fprintf(file_out, "%u %u %u ",
					(unsigned)(127. * (1-log(Is->occurrence) / log(righe_A * colonne_A)) + 127. * (1-r)),
					(unsigned)(255. * log(Is->occurrence) / log(righe_A * colonne_A)),
					(unsigned)(255. * r)
				);
			}
			else
			{
				fprintf(file_out, "%u %u %u ",
					(unsigned)(127. * (1 - log(Is->occurrence) / log(righe_A * colonne_A))),
					(unsigned)(255. * log(Is->occurrence) / log(righe_A * colonne_A)),
					(unsigned)(255.)
				);
			}
		}
		fprintf(file_out, "\n");

		for (size_t c = 0; c < colonne_A; c++)
		{
			buffer[c] = fgetc(file_imag) - '0';
			fgetc(file_imag); //salto lo spazio dopo il bit
		}
		fgetc(file_imag); //salto il line feed
		insert_line_col(v, buffer, colonne_A);
	}

	free(buffer);
	free(v);
	for (int i = 0; i < format_sub.counter; i++) free(VECs[i].gram);
	for (int i = 0; i < format_obj.counter; i++) free(VECo[i].gram);
	free(VECs);
	free(VECo);

	fclose(file_out);
	fclose(file_A_sint);
	fclose(file_B_sint);
	fclose(file_imag);
}










