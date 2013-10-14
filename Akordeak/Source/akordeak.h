/** @file akordeak.h
 *
 */

#ifndef AKORDEAK_H_
#define AKORDEAK_H_

/* Konstanteak definitu */
#define FALSE	0
#define TRUE	!FALSE
#define	IZENBURU_LUZERA	100	/* Izenburuaren gehienezko luzera */

typedef struct
{
  char izenburua[IZENBURU_LUZERA];	/* Abestiaren izenburua */
  char fitxategia[IZENBURU_LUZERA];	/* Irteerako fitxategiaren izenburua */
} abestia_typ;

/* Kantutegiaren formatua aukeratu */
#define ZUTABE_KOPURUA 2

#endif /* AKORDEAK_H_ */
