#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define N 4
#define T 8

//Teste
typedef struct Matrix{
    int coordx;
    int coordy;
    int var;
    double **matrizprinc;
    double **matrizsup;
    double **matrizinf;

}matrix;

pthread_t outrosTIDs[16];
matrix threads_argumento[16];

int calculacoordy(int var, int ordem)
{ 
  return var%ordem;
}

int calculacoordx(int var, int ordem)
{
  return var/ordem;
}
void *dividir(void *arg){

  int m=0, col=0;
  matrix *information = arg;
 information->matrizprinc[0][0];
            col = information->coordy;
          for(int lin=information->coordx; lin < N ; lin++)
          {
            while ( col < N)
            {
                if(col >= lin)
                {
                    information->matrizsup [lin][col] = information->matrizprinc[lin][col];
                    information->matrizinf [lin][col] = 0;
                    m++;
                }

                else
                {
                    information->matrizsup [lin][col] = 0;
                    information->matrizinf [lin][col] = information->matrizprinc[lin][col];
                    m++;
                }
                col++;
            }
            col = 0;
            if(m <= information->var)
            {
              break;
            }
          }

}
int main(){

    int x, y, lin = N, col= N, ordem; 
    int i;
    int pontos = 0;     // Marcador de pontos
    double **MATRIZ, **diagsup, **inf;        
    FILE *matriz, *diag1, *diag2; // Arquivo do campo minado


    matriz = fopen("Matriz.txt", "r"); // Lê documento Tabuleiro.txt

    if (matriz == NULL) // Caso não consiga ler
    {
        printf("ERRO! Impossivel abrir o arquivo matriz!\n");
        return 1; // Finaliza com 1
    }



    MATRIZ = (double**)malloc(sizeof(double *)*(col*lin));	
   if (MATRIZ == NULL)
 	{
     printf("I cannot allocate memory 1\n");
     exit(EXIT_FAILURE);
     return 2;
 	} 
 	for (x = 0; x < lin; x++){ //Percorre as linhas do Vetor de Ponteiros
       MATRIZ[x] = (double*) malloc(col * sizeof(double));
   }

    for(x =0; x < lin; x++){
        for(y=0; y< col; y++){
        //  MATRIZ [x][y] = 10;
            fscanf(matriz, "%lf", &MATRIZ[x][y]);
                //printf("%lf", MATRIZ[x] [y]);

        }
            //printf("\n");
    }




diagsup = (double**)malloc((col*lin)*sizeof(double *));
    if (diagsup == NULL)
  {
     printf("I cannot allocate memory 2\n");
     exit(EXIT_FAILURE);
     return 2;
  }
  for (x = 0; x < lin; x++){ //Percorre as linhas do Vetor de Ponteiros
       diagsup[x] = (double*) malloc(col * sizeof(double));
   }



inf = (double**)malloc((col*lin)*sizeof(double *));
    if (inf == NULL)
  {
     printf("I cannot allocate memory 2\n");
     exit(EXIT_FAILURE);
     return 2;
  } 
             // var = threads_argumento.var = N/T;
              for (x = 0; x < lin; x++){ //Percorre as linhas do Vetor de Ponteiros
                  inf[x] = (double*) malloc(col * sizeof(double));
                }

                for (i=0; i<T; i++){
                  threads_argumento[i].var = N/T;

                  threads_argumento[i].matrizprinc = MATRIZ;
                  threads_argumento[i].matrizsup = diagsup;
                  threads_argumento[i].matrizinf = inf;
                  threads_argumento[i].coordx = 0;
                  threads_argumento[i].coordy = 0;
                  threads_argumento[i].var = (N*N)/T;
                  pthread_create(&outrosTIDs[i], NULL, dividir,(void *)&threads_argumento[i]);

                  threads_argumento[i].coordx = calculacoordx(threads_argumento[i].var, ordem);
                  threads_argumento[i].coordy = calculacoordy(threads_argumento[i].var, ordem);
                }
                

    for (i=0; i<T; i++){

    pthread_join(outrosTIDs[i], NULL);
    }

    diag2 = fopen("diag2.txt", "w");
    if (diag2 == NULL) // Caso não consiga ler
    {
        printf("ERRO! Impossivel abrir o arquivo diag2!");
        return 1; // Finaliza com 1
    }


    for(lin=0; lin < N ; lin++)
    {
        for(col=0 ; col < N ; col++)
        {
            if(col < lin)
            {
                inf[lin][col] = MATRIZ[lin][col];
                fprintf(diag2, "%f ", inf[lin][col]);
                  }
           else
            {
                inf[lin][col] = 0;
                fprintf(diag2, "%f ", inf[lin][col]);
            }
        }
        fprintf(diag2, "\n");
    }

    diag1 = fopen("diag1.txt", "w");


    if (diag1 == NULL) // Caso não consiga ler
    {
        printf("ERRO! Impossivel abrir o arquivo diag1!");
        return 1; // Finaliza com 1
    }

     for(lin=0; lin < N ; lin++)
    {
        for(col=0 ; col < N ; col++)
        {
            fprintf(diag1, "%f ", diagsup[lin][col]);

        }
        fprintf(diag1, "\n");
    }


    fclose(matriz); // Fecha o documento
    fclose(diag1);
    fclose(diag2);
    return 0;
}
