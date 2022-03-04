#ifndef ERR_MNG_H
#define ERR_MNG_H

#include <stdlib.h>
#include <stdio.h>


#define flag \
{ \
fprintf(stderr,"flag \tat line %d \tin file %s",__LINE__,__FILE__);\
}

#define end \
{\
fprintf(stderr,"end \tat line %d \tin file %s",__LINE__,__FILE__);\
exit(EXIT_FAILURE);\
}

#define stop \
{\
fprintf(stderr,"stop \tat line %d \tin file %s",__LINE__,__FILE__); \
fprintf(stdout,"end program?\n Yes(Y) / No (N) : "); \
char macro_stop; \
do { macro_stop = getchar(); } while(macro_stop!='N' && macro_stop != 'Y'); \
if(macro_stop == 'Y') exit(EXIT_FAILURE);\
}\

#define assert( cond, if_true, if_false ) \
{\
if((cond))\
{\
 fprintf(stderr,"true "); \
 if_true; \
}\
else\
{\
 fprintf(stderr,"false "); \
 if_false; \
}\
fprintf(stderr,"\n");\
}

#define report(string,variable) \
{\
fprintf(stderr,"report \tat line %d \tin file %s \t",__LINE__,__FILE__); \
fprintf(stderr,string,variable); \
fprintf(stderr,"\n"); \
}

#endif
