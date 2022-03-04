/// legge le immagini in DATA
/// realizza per ognuna di esse un file binario con le sintesi in SINTEX

/// avvia il calcolo delle distanze scegliendo i file a coppie da 2
/// i nomi dei file con rispettive directory sono lette da register.txt
/// i file di output sono inseriti in binario in DISTANCE

/// avvia una sequenza di sorting
/// legge i file da SINTEX e da DISTANCE
/// per quelli in SINTEX riordina per frequenza 
/// per quelli in DISTANCE riordina per distanza

/// il main prende in input la grandezza dei gram

#include "../hdr/sintex.h"
#include "../hdr/distance.h"
#include "../hdr/sorting.h"
#include "../hdr/err_mng.h"
#include <stdlib.h>
#include <stdio.h>

int main( int argc, char** argv )
{
  assert(freopen("../err_report.txt","w",stderr),flag,end); //esso stesso non deve essere un errore
  ///lettura del registro
  FILE* reg = fopen("../register.txt","r");
  assert(reg!=NULL,flag,end);
  unsigned num_file = 0;
  assert(fscanf(reg,"%u\n",&num_file)!=EOF,flag,end); ///leggo il numero di file nel registro
  char** file = (char**)calloc(num_file,sizeof(char*));
  assert(file!=NULL,flag,end);
  assert(fprintf(stdout,"<<lettura del registro in corso...\n")>=0,flag,end);
  for(unsigned i = 0;i<num_file;i++)
  {
    file[i] = (char*)calloc(FILENAME_MAX,sizeof(char));
    assert(file!=NULL,flag,end);
    assert(fscanf(reg,"%s\n",file[i])!=EOF,flag,end);
  }
  assert(fprintf(stdout,"lettura del registro terminata>>\n\
--------------------------------\n")>=0,flag,end);
  ///sintesi
  assert(argc == 2, flag, end);
  unsigned SIZE_GRAM = (unsigned)atoi(argv[1]);
  assert(fprintf(stdout,"<<fase di sintesi in corso..\n")>=0,flag,end);
  //#pragma omp parallel for
  for(unsigned i=0;i<num_file;i++)
  {
    assert(fprintf(stdout," :: sintesi del file %s\n",file[i])>=0,flag,end);
    sintex(file[i], SIZE_GRAM);
  }
  assert(fprintf(stdout,"fase di sintesi terminata>>\n\
--------------------------------\n")>=0,flag,end);
  ///calcolo distanze
  assert(fprintf(stdout,"<<fase di calcolo delle distanze in corso..\n")>=0,flag,end);
  //#pragma omp parallel for
  for(unsigned i = 0;i<num_file;i++)
  {
    assert(fprintf(stdout," :: file soggetto %s in elaborazione\n",file[i])>=0,flag,end);
    for(unsigned j=0;j<num_file;j++)
    {
      distance(file[i],file[j]);
    }
  }
  assert(fprintf(stdout,"calcolo distanze terminato>>\n\
--------------------------------\n")>=0,flag,end);
  ///fase di sorting
  assert(fprintf(stdout,"<<fase di sorting in corso..\n")>=0,flag,end);
  for(unsigned i = 0;i<num_file;i++)
  {
    assert(fprintf(stdout," :: file %s in elaborazione\n",file[i])>=0,flag,end);
    sort_sintex(file[i]);
    sort_distance(file[i],num_file);
  }
  assert(fprintf(stdout,"fase di sorting terminata>>\n\
--------------------------------\n")>=0,flag,end);
  ///termine del programma
  assert(fprintf(stdout,"programma terminato\n\
  ################################\n")>=0,flag,end);
  for(unsigned i = 0;i<num_file;i++)
    free(file[i]);
  free(file);
  return EXIT_SUCCESS;
}
