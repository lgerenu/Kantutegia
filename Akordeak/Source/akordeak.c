/** @file akordeak.c
 *
 */

/* Fitxategiak sartu */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "akordeak.h"

/* Funtzioen prototipoak */
int abestiaSortu(FILE *pfSarrera, FILE *pfIrteera, char *izenburua, int akordeakIdatzi);
void latex_putc(FILE * Pfitxategia, char *karaktereak);
void latex_verbatim_putc(FILE * Pfitxategia, char *karaktereak);
void hasiVerbatim(FILE * pFitxategia);
void bukatuVerbatim(FILE * pFitxategia);
void izenburuaLatex(FILE * pFitxategia, char * izenburua);
void sortuPdf(char * fitxategia);
int txtDa(const struct dirent *entry);

int main(int argc, char **argv)
{
  /* Aldagaiak erazagutu */
  FILE *pfSarrera; /* Sarrerako fitxategia */
  FILE *pfIrteera;

  char izenburua[IZENBURU_LUZERA]; /* Abestiaren izenburua gordetzeko */
  char irteerakoFitxategia[IZENBURU_LUZERA]; /* Irteerako fitxategiaren izena */

  int akordeakIdatzi = FALSE;   /* TRUE akordeak idatzi behar baldin badira */

  /* Ez bada fitxategi baten izena ematen, itzuli */
  if (argc < 2)
  {
    printf("\nEz duzu irteerako fitxategi izena adierazi...\n");
    return (1);
  }


  /*
   * .txt fitxategiak lortu
   */
  struct dirent **emaitzak=NULL;
  int emaitzaKopurua;

  /*
   * Akordeak bai edo ez?
   */
  char erantzuna[100];
  printf("Akordeak inprimatu? ");
  scanf("%s", erantzuna);
  printf("%s", erantzuna);
  akordeakIdatzi = strcmp(erantzuna, "ez");

  /*
   * Irteerako fitxategia ireki
   */
  strcpy(irteerakoFitxategia, argv[1]);
  strcat(irteerakoFitxategia, ".tex");
  pfIrteera = fopen(irteerakoFitxategia, "w");
  if (pfIrteera == NULL )
  {
    printf("Ezin izan da abestia.txt fitxategia ireki...\n");
    return (1);
  }

  /* Irteerako fitxategiari LaTeX goiburua gehitu*/
  fprintf(pfIrteera, "\\documentclass[a4page]{article}\n");
  fprintf(pfIrteera, "\\textheight = 25cm\n\\textwidth = 16cm\n\\topmargin = -2cm\n\\oddsidemargin = 0cm\n");
  fprintf(pfIrteera, "\\usepackage[basque]{babel}\n\\usepackage[utf8]{inputenc}\n\\usepackage{multicol}\n");
  fprintf(pfIrteera, "\\addto\\captionsbasque{\n\\def\\contentsname{Abestiak}\n}\n");
  fprintf(pfIrteera, "\\title{%s}\n", argv[1]);
  fprintf(pfIrteera, "\\begin{document}\n");
  fprintf(pfIrteera, "\\begin{titlepage}\n\\maketitle\n\n\\end{titlepage}\n");
  fprintf(pfIrteera, "\\begin{multicols}{2}\n");
  fprintf(pfIrteera, "\\tableofcontents\n\\setcounter{tocdepth}{1}\n");
  fprintf(pfIrteera, "\\end{multicols}\n");
  fprintf(pfIrteera, "\\newpage\n");
  if (ZUTABE_KOPURUA > 1)
    fprintf(pfIrteera, "\\begin{multicols}{%i}\n", ZUTABE_KOPURUA);

  emaitzaKopurua = scandir("./", &emaitzak, txtDa, alphasort);
  printf("\n******************\nEmaitza kopurua = %i\n", emaitzaKopurua);
  /*
   * Fitxategi guztiak begiratu
   */
  int i;
  for(i=0; i<emaitzaKopurua; i++)
  {
    strcpy(izenburua, emaitzak[i]->d_name);
    printf("Fitxategi izena: %s\n", izenburua);
    /* Sarrerako fitxategia ireki */
    pfSarrera = fopen(izenburua, "r");
    if (pfSarrera == NULL )
    {
      printf("Ezin izan da %s fitxategia ireki...\n", izenburua);
      return (1);
    }
    if (strlen(izenburua) >= 4)
      izenburua[strlen(izenburua)-4] = 0;
    abestiaSortu(pfSarrera, pfIrteera, izenburua, akordeakIdatzi);
    /* Sarrerako fitxategiak itxi */
    fflush(pfSarrera);
    fclose(pfSarrera);
    printf("Sarrerako fitxategia itxita...\n");
  }
  for (i = 0; i < emaitzaKopurua; ++i)
  {
    free(emaitzak[i]);
    emaitzak[i] = NULL;
  }
  free(emaitzak);
  emaitzak = NULL;
  printf("\n*******************\n");

  /* LaTeX bukaera idatzi */
  if (ZUTABE_KOPURUA > 1)
    fprintf(pfIrteera, "\\end{multicols}\n");
  fprintf(pfIrteera, "\\end{document}");



  /* Fitxategiak itxi */
  fflush(pfIrteera);
  fclose(pfIrteera);
  printf("Irteerako fitxategia itxita...\n");

  /* PDF fitxategia sortu */
  printf("PDFa sortzeko prest...\n");
  sortuPdf(irteerakoFitxategia);
  sortuPdf(irteerakoFitxategia);
  return (0);
}


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

/**
 * Hizki kate bat Latex eran idazten du fitxategi batean.
 *
 * @param pFitxategia Idatzi beharreko fitxategiari punteroa.
 * @param karaktereak Idatzi beharreko karaktereen katea.
 */
void latex_putc(FILE * pFitxategia, char *karaktereak)
{
  if (!strcmp(karaktereak, "\n"))
    fprintf(pFitxategia, "\\\\\n");
  else if (!strcmp(karaktereak, "#"))
    fprintf(pFitxategia, "\\#");
  else if (!strcmp(karaktereak, "\flat"))
    fprintf(pFitxategia, "$\\flat$");
  else if (!strcmp(karaktereak, "á"))
    fprintf(pFitxategia, "\\'a");
  else if (!strcmp(karaktereak, "é"))
    fprintf(pFitxategia, "\\'e");
  else if (!strcmp(karaktereak, "í"))
    fprintf(pFitxategia, "\\'i");
  else if (!strcmp(karaktereak, "ó"))
    fprintf(pFitxategia, "\\'o");
  else if (!strcmp(karaktereak, "ú"))
    fprintf(pFitxategia, "\\'u");
  else
    fprintf(pFitxategia, "%s", karaktereak);
}

/**
 * Hizki kate bat Latex eran idazten du fitxategi batean,
 * baina verbatim eran.
 *
 * @param pFitxategia Idatzi beharreko fitxategiari punteroa.
 * @param karaktereak Idatzi beharreko karaktereen katea.
 */
void latex_verbatim_putc(FILE * pFitxategia, char *karaktereak)
{
  if (!strcmp(karaktereak, "á"))
    fprintf(pFitxategia, "\\'a");
  else if (!strcmp(karaktereak, "é"))
    fprintf(pFitxategia, "\\'e");
  else if (!strcmp(karaktereak, "í"))
    fprintf(pFitxategia, "\\'i");
  else if (!strcmp(karaktereak, "ó"))
    fprintf(pFitxategia, "\\'o");
  else if (!strcmp(karaktereak, "ú"))
    fprintf(pFitxategia, "\\'u");
  else if (!strcmp(karaktereak, "\flat"))
    fprintf(pFitxategia, "$\\flat$");
  else
    fprintf(pFitxategia, "%s", karaktereak);
}

void hasiVerbatim(FILE * pFitxategia)
{
  fprintf(pFitxategia, "\\begin{verbatim}\n");
}

void bukatuVerbatim(FILE * pFitxategia)
{
  fprintf(pFitxategia, "\\end{verbatim}\n");
}

void izenburuaLatex(FILE * pFitxategia, char * izenburua)
{
  fprintf(pFitxategia, "\n\\section{%s}\n\n", izenburua);
}

void sortuPdf(char * fitxategia)
{
  char sysCommand[100];

  strcpy(sysCommand, "pdflatex ");
  strcat(sysCommand, "\"");
  strcat(sysCommand, fitxategia);
  strcat(sysCommand, "\"");
  printf("%s\n", sysCommand);
  system(sysCommand);
}


int txtDa(const struct dirent *entry)
{
  char izena[100];
  char izenBukaera[100];
  int izenLuzera;

  strcpy(izena, entry->d_name);
  izenLuzera = strlen(izena);
  if (izenLuzera >= 4)
  strcpy(izenBukaera, izena+strlen(izena)-4);
  return(!strcmp(izenBukaera, ".txt"));
}
