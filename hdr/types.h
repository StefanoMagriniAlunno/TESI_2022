#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdio.h>

typedef struct prlg
{
  size_t total;			// numero totale di gram
  size_t counter;		// varietà dei gram
  size_t size_gram;		// grandezza dei gram
} prlg;

typedef struct t_key
{
  unsigned* gram;
  size_t occurrence;
} t_key;

typedef struct t_key_s
{
	unsigned* gram;
	size_t occurrence;
} t_key_s;

typedef struct t_key_d
{
	char name_file[FILENAME_MAX];
	double distance;
} t_key_d;

#endif
