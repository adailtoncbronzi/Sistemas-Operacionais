/*
------------------------------------------------------------------------------------------------------------------------------
				CODIGO REFERENTE AO TRABALHO DE SISTEMAS OPERACIONAIS, MINISTRADA PELO PROFESSOR LEON GRADVOHL.
												1o SEMESTRE DE 2019.

					DESENVOLVIDO POR GABRIEL FERREIRA, JULIANA MORRONI E LARISSA BENEVIDES.
------------------------------------------------------------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h> 

/*
------------------------------------------------------------------------------------------------------------------------------
						DECLARACAO DA STRUCT QUE SERA PASSADA COMO PARAMETRO DA THREAD
------------------------------------------------------------------------------------------------------------------------------ */

typedef struct Matrix {
    int coordenada_x;
    int coordenada_y;
    int repeticoes;
    int ordem_matriz;
    double ** matrizprincipal;
    double ** matrizsuperior;
    double ** matrizinferior;

}matrix;

pthread_t outrosTIDs[16];
matrix threads_argumento[16];
/*
------------------------------------------------------------------------------------------------------------------------------
										FUNCAO DO TEMPO DE PROCESSAMENTO
------------------------------------------------------------------------------------------------------------------------------ */

float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

/*
------------------------------------------------------------------------------------------------------------------------------
					FUNCAO DA THREAD QUE DIVIDE AS MATRIZES EM SUPERIOR E INFERIOR
------------------------------------------------------------------------------------------------------------------------------ */

void * dividir(void * arg) {

    int parada = 0, col = 0;
    matrix * information = arg;
    col = information->coordenada_y;
    for (int lin = information->coordenada_x; lin < information->ordem_matriz; lin++) {
        while (col < information->ordem_matriz) {
            if (col >= lin) {
                information->matrizsuperior[lin][col] = information->matrizprincipal[lin][col];
                information->matrizinferior[lin][col] = 0;
                parada++;
                if (parada >= information -> repeticoes)
                    return 0;
            }
            else {
                information->matrizsuperior[lin][col] = 0;
                information->matrizinferior[lin][col] = information->matrizprincipal[lin][col];
                parada++;
                if (parada >= information->repeticoes)
                    return 0;
            }
            col++;
        }
        col = 0;
    }
}

int main(int argc, char * argv[]) {

/*
------------------------------------------------------------------------------------------------------------------------------
											DECLARACAO DAS VARIAVEIS
------------------------------------------------------------------------------------------------------------------------------ */

    int num_threads = atoi(argv[2]), ordem_matriz = atoi(argv[1]), indice_x, indice_y, lin = ordem_matriz,
    col = ordem_matriz, resto = 0, id, j;
    double **MATRIZ, **diagsup, **diaginf;
    FILE *matriz, *diag1, *diag2;
    struct timeval t0;
    struct timeval t1;
    float elapsed;
 

/*
------------------------------------------------------------------------------------------------------------------------------
										CONDICAO DE NUMERO MAXIMO DE THREADS
------------------------------------------------------------------------------------------------------------------------------ */

    if(num_threads > ordem_matriz)
    	num_threads = ordem_matriz;
/*
------------------------------------------------------------------------------------------------------------------------------
											ABERTURA DO ARQUIVO MATRIZ.TXT
------------------------------------------------------------------------------------------------------------------------------ */

    matriz = fopen(argv[3], "r");
    if (matriz == NULL)
    {
        printf("ERRO! Impossivel abrir o arquivo matriz!\n");
        return 1;
    }
/*
------------------------------------------------------------------------------------------------------------------------------
						ALOCACAO DINAMICA DA MATRIZ PRINCIPAL E LEITURA DOS VALORES NO ARQUIVO
------------------------------------------------------------------------------------------------------------------------------ */

    MATRIZ = (double ** ) malloc(sizeof(double * ) * (col * lin));
    if (MATRIZ == NULL) {
        printf("I cannot allocate memory (1)\n");
        exit(EXIT_FAILURE);
        return 2;
    }
    for (indice_x = 0; indice_x < lin; indice_x++)
        MATRIZ[indice_x] = (double * ) malloc(col * sizeof(double));

    for (indice_x = 0; indice_x < lin; indice_x++)
        for (indice_y = 0; indice_y < col; indice_y++)
            fscanf(matriz, "%lf", &MATRIZ[indice_x][indice_y]);

/*
------------------------------------------------------------------------------------------------------------------------------
				ALOCACAO DINAMICA DAS MATRIZES AUXILIARES (MATRIZ DIAGONAL SUPERIOR E DIAGONAL INFERIOR)
------------------------------------------------------------------------------------------------------------------------------ */

    diagsup = (double ** ) malloc((col * lin) * sizeof(double * ));
    if (diagsup == NULL) {
        printf("I cannot allocate memory (2)\n");
        exit(EXIT_FAILURE);
        return 2;
    }
    for (indice_x = 0; indice_x < lin; indice_x++)
        diagsup[indice_x] = (double * ) malloc(col * sizeof(double));

    diaginf = (double ** ) malloc((col * lin) * sizeof(double * ));
    if (diaginf == NULL) {
        printf("I cannot allocate memory (2)\n");
        exit(EXIT_FAILURE);
        return 2;
    }

    for (indice_x = 0; indice_x < lin; indice_x++)
        diaginf[indice_x] = (double * ) malloc(col * sizeof(double));
/*
------------------------------------------------------------------------------------------------------------------------------
						DEFININDO OS VALORES DAS VARIAVEIS DA STRUCT E CRIACAO DA THREAD
------------------------------------------------------------------------------------------------------------------------------ */

    gettimeofday(&t0, 0);
    for (id = 0; id < num_threads; id++) {
        threads_argumento[id].ordem_matriz = ordem_matriz;
        threads_argumento[id].matrizprincipal = MATRIZ;
        threads_argumento[id].matrizsuperior = diagsup;
        threads_argumento[id].matrizinferior = diaginf;
        threads_argumento[id].coordenada_x = (((ordem_matriz * ordem_matriz) / num_threads) * id) / ordem_matriz;
        threads_argumento[id].coordenada_y = (((ordem_matriz * ordem_matriz) % num_threads) * id) % ordem_matriz;
        threads_argumento[id].repeticoes = (ordem_matriz * ordem_matriz) / num_threads;
        if (id == num_threads - 1) {
            resto = (ordem_matriz * ordem_matriz) % num_threads;
            threads_argumento[id].repeticoes = ((ordem_matriz * ordem_matriz) / num_threads) + resto;
        }
        pthread_create(&outrosTIDs[id], NULL, dividir, (void * ) &threads_argumento[id]);
    }
/*
------------------------------------------------------------------------------------------------------------------------------
											JUNCAO DAS THREADS CRIADAS
------------------------------------------------------------------------------------------------------------------------------ */

    for (id = 0; id < num_threads; id++)
        pthread_join(outrosTIDs[id], NULL);

/*
------------------------------------------------------------------------------------------------------------------------------
											TEMPO DE PROCESSAMENTO
------------------------------------------------------------------------------------------------------------------------------ */
    gettimeofday(&t1, 0);
    elapsed = timedifference_msec(t0, t1);
    printf("Code executed in %f milliseconds.\n", elapsed);
/*
------------------------------------------------------------------------------------------------------------------------------
									ESCREVENDO O RESULTADO DA DIVISAO NOS ARQUIVOS
------------------------------------------------------------------------------------------------------------------------------ */

    diag2 = fopen("Matriz.diag2", "w");
    if (diag2 == NULL)
    {
        printf("ERRO! Impossivel abrir o arquivo diag2!");
        return 1;
    }

    for (lin = 0; lin < ordem_matriz; lin++) {
        for (col = 0; col < ordem_matriz; col++) {
                fprintf(diag2, "%f ", diaginf[lin][col]);
        }
        fprintf(diag2, "\n");
    }

    diag1 = fopen("Matriz.diag1", "w");

    if (diag1 == NULL)
    {
        printf("ERRO! Impossivel abrir o arquivo diag1!");
        return 1;
    }

    for (lin = 0; lin < ordem_matriz; lin++) {
        for (col = 0; col < ordem_matriz; col++) {
            fprintf(diag1, "%f ", diagsup[lin][col]);
        }
        fprintf(diag1, "\n");
    }

    fclose(matriz);
    fclose(diag1);
    fclose(diag2);
    return 0;
}