/*
 * Title: Viterbi + Forward algorithms
 * Author: José González
*/

#include <stdio.h>
#include <stdlib.h>

typedef enum {FALSE,TRUE} BOOL;

typedef struct MarkovModel{
	int  *Q;
	int numStates; // Se cuentan el estado inicial y el final en la matriz //
	char *E;
	int numCodewords;
	double **A;
	double **B;
}MarkovModel;

typedef struct MarkovModels{
	MarkovModel *markovModels;
}MarkovModels;


void setMarkovModels();
BOOL initMarkovModels(struct MarkovModels* mm,int n);
BOOL setMarkovStates(struct MarkovModels* mm,int n);
BOOL setMarkovCodebook(struct MarkovModels* mm,int n,char* codebook);
/** Modificar manejo de matrices dinámicas (si no se limita el tamaño de las matrices) **/
BOOL initMarkovGraph(struct MarkovModels* mm,int n,double adjacencyMatrix[5][5]);
BOOL initCastProbabilities(struct MarkovModels* mm,int n,double castProbabilities[3][3]);
BOOL freeMarkovModels(struct MarkovModels* mm,int n);
int getCodewordIndex(struct MarkovModels* mm,int p,char codeword);
