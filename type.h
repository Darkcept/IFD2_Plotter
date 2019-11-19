#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED
#define longIndiv 8
#define pCroise 0.5
#define A -1
#define B 1
enum bool {FALSE=0,TRUE=1};

typedef unsigned char Bit;

typedef struct Bit
{
    Bit valeur;
    struct Bit* suivant;
}Bitlink;
typedef Bitlink* liste_bit;

typedef struct Indiv
{
    Bit* tete;
    struct Indiv* suivant;
}Individu;
typedef Individu* liste_indiv;

typedef struct Pop
{
    Individu* tete;
    struct Pop* suivant;
}Population;
typedef Population* liste_pop;

#endif // TYPE_H_INCLUDED
