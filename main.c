#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "type.h"
#include <math.h>
int main()
{
    printf("Hello world!\n");
    return 0;
}

int val_ent_binaire(const liste_bit b)
{
    int r,i=0;
    while(b->suivant!=NULL)
    {
        r+=pow(b->valeur,i);
        b=b->suivant;
        i++;
    }
    return r;
}
