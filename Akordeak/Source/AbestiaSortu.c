/** @file AbestiaSortu.c
 *
 */

/* Fitxategiak sartu */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "akordeak.h"


/**
 * Funtzio honek sarrerako fitxategi batetik irakurri eta irteerako
 * fitxategi batean formateatzen ditu abestiak. Sarrerako fitxategi
 * bakoitzak abesti bat dauka eta izenburua eta ia akordeak
 * idatzi behar diren edo ez esan behar zaio.
 * @param pfSarrera Sarrerako fitxategia.
 * @param pfIrteera Irteerako fitxategia.
 * @param izenburua Abestiaren izenburua.
 * @param akordeakIdatzi Akordeak irteerako fitxategian idatzi behar diren edo ez.
 * @return FALSE bere eginkizuna ondo bete badu.
 */
int abestiaSortu(FILE *pfSarrera, FILE *pfIrteera, char *izenburua, int akordeakIdatzi)
{
  /* Aldagaiak erazagutu */
  FILE *pfAkordeak; /**< Akordeak idazteko fitxategia */
  FILE *pfHitzak; /**< Hitzak idazteko fitxategia */

  int karakterea; /**< Irakurritako karakterea */
  char akordeaDa = 0; /**< TRUE akorde bat irakurtzen ari baldin bagara */
  char akordeaZai = 0; /**< TRUE akorde bat idazteko baldin badago */
  int tempAkordea[9]; /**< Akordea gordetzeko lekua */
  int i = 0;
  int j = 0;
  char akordeaId = 1; /**< Akordea idazten */
  char letraId = 0; /**< Letra idazten */
  char bukatu = 0;

  /* Akordeen eta letren fitxategiak sortu */
  pfAkordeak = fopen("/tmp/akordeak.txt", "w");
  if (pfAkordeak == NULL )
  {
    printf("Ezin izan da /tmp/akordeak.txt fitxategia sortu...\n");
    return (1);
  }
  printf("/tmp/akordeak.txt fitxategia sortu da...\n");
  pfHitzak = fopen("/tmp/hitzak.txt", "w");
  if (pfHitzak == NULL )
  {
    printf("Ezin izan da /tmp/hitzak.txt fitxategia sortu...\n");
    return (1);
  }
  printf("/tmp/hitzak.txt fitxategia sortu da...\n");

  /* Hasi sarrerako fitxategia irakurtzen eta guztia beste bi fitxategietan gorde */
  while ((karakterea = fgetc(pfSarrera)) != EOF)
  {
    /* Karakterea '[' hizkia baldin bada akorde bat irakurtzen ari gara */
    if (karakterea == '[')
    {
      if (akordeaDa) /* Fitxategi okerra... */
      {
        printf("Akorde bat beste baten barruan. Fitxategi okerra...\n");
        return (1);
      }
      akordeaDa = TRUE;
    }
    else if (karakterea == ']') /* Akordea bukatzen da */
    {
      if (!akordeaDa) /* Fitxategi okerra... */
      {
        printf("Akordea hasi aurretik bukatzen da. Fitxategi okerra...\n");
        return (1);
      }
      akordeaDa = FALSE;
      akordeaZai = TRUE;
    }
    else if (akordeaDa) /* Akordea irakurtzen ari baldin bagara */
    {
      tempAkordea[i] = karakterea;
      i++;
    }
    else if (akordeaZai) /* Akordea idazten ari baldin bagara */
    {
      if (karakterea != 0x0A)
      {
        putc(karakterea, pfHitzak);
        putc(tempAkordea[j], pfAkordeak);
        j++;
      }
      else /* Akordeen luzera hitzena baino luzeagoa baldin bada */
      {
        while (j < i)
        {
          putc(' ', pfHitzak);
          putc(tempAkordea[j], pfAkordeak);
          j++;
        }
        putc(0x0A, pfHitzak);
        putc(0x0A, pfAkordeak);
      }
      if (j >= i)
      {
        i = 0;
        j = 0;
        akordeaZai = FALSE;
      }
    }
    else
    {
      putc(karakterea, pfHitzak);
      if (karakterea != 0x0A)
        putc(' ', pfAkordeak);
      else
        putc(0x0A, pfAkordeak);
    }
  }

  /* Fitxategiak itxi */
  fflush(pfAkordeak);
  fclose(pfAkordeak);
  fflush(pfHitzak);
  fclose(pfHitzak);

  izenburuaLatex(pfIrteera, izenburua);
  if (akordeakIdatzi)
    hasiVerbatim(pfIrteera);

  /* Akordeen eta letren fitxategiak ireki */
  pfAkordeak = fopen("/tmp/akordeak.txt", "r");
  if (pfAkordeak == NULL )
  {
    printf("Ezin izan da /tmp/akordeak.txt fitxategia ireki...\n");
    return (1);
  }
  pfHitzak = fopen("/tmp/hitzak.txt", "r");
  if (pfHitzak == NULL )
  {
    printf("Ezin izan da /tmp/hitzak.txt fitxategia ireki...\n");
    return (1);
  }

  /* Guztia irteerako fitxategia idatzi */
  i = 0;
  while (!bukatu)
  {
    if (akordeaId)
    {
      karakterea = fgetc(pfAkordeak);
      if (karakterea != EOF)
      {
        if (akordeakIdatzi)
        {
          char strTmp[50];
          sprintf(strTmp, "%c", karakterea);
          latex_verbatim_putc(pfIrteera, strTmp);
        }
      }
      if ((karakterea == 0x0A) || (karakterea == EOF))
      {
        akordeaId = FALSE;
        letraId = TRUE;
      }
    }
    else if (letraId)
    {
      karakterea = fgetc(pfHitzak);
      if (karakterea == EOF)
        bukatu = TRUE;
      if (karakterea != EOF)
      {
        char strTmp[50];
        sprintf(strTmp, "%c", karakterea);
        if (akordeakIdatzi)
          latex_verbatim_putc(pfIrteera, strTmp);
        else
        {
          /* Bihurtu karakterea kate batean */
          latex_putc(pfIrteera, strTmp);
        }
      }
      if (karakterea == 0x0A)
      {
        akordeaId = TRUE;
        letraId = FALSE;
      }
    }
  }
  /* Lerro huts bat idatzi bukaeran */
  fprintf(pfIrteera, "\n");
  if (akordeakIdatzi)
    bukatuVerbatim(pfIrteera);

  /* Fitxategiak itxi */
  fclose(pfAkordeak);
  fclose(pfHitzak);

  return (0);
}
