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
extern int abestiaSortu(FILE *pfSarrera, FILE *pfIrteera, char *izenburua, int akordeakIdatzi);
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
 * Latex fitxategitik habiatuta, PDF fitxategia sortzen du.
 * @param fitxategia Sortuko den fitxategiaren izena.
 */
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


/**
 * Scandir funtzioan erabiltzeko. Fitxategi bat .txt erakoa
 * denentz erabakitzen du.
 * @return FALSE, ez baldin bada .txt erako fitxategia. Bestela, beste edozein balio bueltatzen du.
 */
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
