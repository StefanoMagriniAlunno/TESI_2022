
#include "../hdr/distance.h"
#include "../hdr/err_mng.h"
#include "../hdr/types.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

void distance(const char* name_file_sub, const char* name_file_obj)
{
	report("%s", name_file_obj);
	FILE *file_out = NULL,*file_sub =NULL,*file_obj=NULL;
	{ // apro i file
		char* percorso_out = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_out!=NULL,flag,end);
		char* percorso_sub = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_sub!=NULL,flag,end);
		char* percorso_obj = (char*)calloc(FILENAME_MAX,sizeof(char));
		assert(percorso_obj!=NULL,flag,end);
		assert(sprintf(percorso_out,"../DISTANCE/%s",name_file_sub)>=0,flag,end);
		assert(sprintf(percorso_sub,"../SINTEX/%s",name_file_sub)>=0,flag,end);
		assert(sprintf(percorso_obj,"../SINTEX/%s",name_file_obj)>=0,flag,end);
		file_out = fopen(percorso_out,"a");
		file_sub = fopen(percorso_sub,"rb");
		file_obj = fopen(percorso_obj,"rb");
		assert(file_out,flag,end);
		assert(file_sub,flag,end);
		assert(file_obj,flag,end);
		free(percorso_out);
		free(percorso_sub);
		free(percorso_obj);
	}
  
	prlg format_sub = {0};
  	prlg format_obj = {0};
	assert(fread(&format_sub,sizeof(prlg),1,file_sub)==1,flag,end);
	assert(fread(&format_obj,sizeof(prlg),1,file_obj)==1,flag,end);
	assert(format_sub.size_gram == format_obj.size_gram,flag,stop);
	t_key* VECs = (t_key*)calloc(format_sub.counter,sizeof(t_key));
	assert(VECs,flag,end);
	t_key* VECo = (t_key*)calloc(format_obj.counter,sizeof(t_key));
	assert(VECo,flag,end);
	{ // inizializzazione dei vettori di chiavi
		for(size_t i = 0;i<format_sub.counter;i++)
		{
			VECs[i].gram = (unsigned*)calloc(format_sub.size_gram, sizeof(unsigned));	//alloco il gram
			// senza controllo dereferenzazione di iter->gram
	  		fread(VECs[i].gram,sizeof(unsigned),format_sub.size_gram,file_sub); //senza assert
	  		fread(&(VECs[i].occurrence),sizeof(size_t),1,file_sub); //senza assert
	  	}
	  	for(size_t i = 0;i<format_obj.counter;i++)
	  	{
	  		VECo[i].gram = (unsigned*)calloc(format_obj.size_gram, sizeof(unsigned));	//alloco il gram
	  		// senza controllo dereferenzazione di iter->gram
	  		fread(VECo[i].gram,sizeof(unsigned),format_obj.size_gram,file_obj);
	  		fread(&(VECo[i].occurrence),sizeof(size_t),1,file_obj);
	  	}
	}
	double out = 0;
	size_t i = 0,j = 0;
	t_key *iter_sub = VECs, *iter_obj = VECo;
	while(i<format_sub.counter && j<format_obj.counter)
	{
	 	int ret = 0;
	  	// comparazione
	  	{
	  		unsigned* buf_sub = iter_sub->gram;
	  		unsigned* buf_obj = iter_obj->gram;
	  		for(int k = 0;k<format_sub.size_gram;k++)
		  		if(buf_sub[k]<buf_obj[k])
		  		{
		  			ret = -1;
		  			break;
		  		}
		  		else if(buf_sub[k]>buf_obj[k])
		  		{
		  			ret = 1;
		  			break;

				}
		 }
	  	// ret ora vale -1 se iter_sub precede iter_obj, 0 se sono uguali, 1 altrimenti
	  	if(ret==0)
	  	{
	  		double freq_sub = (double)(iter_sub->occurrence)/format_sub.total;
	  		double freq_obj = (double)(iter_obj->occurrence)/format_obj.total;
			// assert( freq_sub != 0 || freq_obj != 0, flag, stop);
	  		double r = freq_sub < freq_obj ? freq_sub / freq_obj : freq_obj / freq_sub; // per evitare divisioni per 0 
	  		r = (1-r) / (1+r); // 0 <= r <= 1, divisione poco suscettibile ad errori
	  		out += r * r;
	  		free(iter_sub->gram);
	  		free(iter_obj->gram);
	  		iter_sub++;
	  		iter_obj++;
	  		i++;
	  		j++;
	  	}
	  	else
	  	{
	  		out++;
	  		if( ret == -1)
	  		{
	  			free(iter_sub->gram);
	  			iter_sub++;
	  			i++;
	  		}
	  		else
	  		{
		  		free(iter_obj->gram);
		  		iter_obj++;
		  		j++;
	  		}
	  	}
	}
	out += format_sub.counter - i;
	out += format_obj.counter - j;
	for(;i<format_sub.counter;i++,iter_sub++) free(iter_sub->gram);
	for(;j<format_obj.counter;j++,iter_obj++) free(iter_obj->gram);
	free(VECs);
	free(VECo);
	out /= format_sub.counter + format_obj.counter;
	assert(fprintf(file_out, "%s \t%.15lf\n",name_file_obj, out)>=0,flag,end);
	fclose(file_out);
	fclose(file_sub);
	fclose(file_obj);
}
