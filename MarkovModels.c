/*
 * Title: Viterbi + Forward algorithms
*/

#include "MarkovModels.h"

void setMarkovModels(struct MarkovModels* mModels,int n){
	/** Ejemplo con un modelo de markov con Q = {1,2,3}, E = {a,b,c} , PI1 = PI2 = 1/2; PI3 = 0 **/
	// Inicializar N modelos de markov //
	initMarkovModels(mModels,n);
	/** Definición del modelo de markov 1 **/
	// Set tamaños //
	mModels->markovModels[0].numStates    = 5; // [0,1,2,3,F] . La primera fila es el nodo "fantasma" y la última el nodo final (por manejar matriz cuadrada) - numEstados + 2 -
	mModels->markovModels[0].numCodewords = 3;
	// Inicializar estados del modelo de markov 0//
	printf("Estados inicializados : %d (error)\n",setMarkovStates(mModels,0));
	// Inicializar alfabeto (codebook) //
	char codebook[4] = {'a','b','c'};
	printf("Codebook inicializado : %d (error)\n",setMarkovCodebook(mModels,0,codebook)); // {a,b,c} //
	// Inicializar matriz de adyacencia del modelo de markov 0 contando con los nodos iniciales y finales //
	double adjacencyMatrix[5][5] = {{0.0,0.5,0.5,0.0,0.0},{0.0,1/4.0,1/2.0,1/4.0,0.0},{0.0,1/3.0,1/3.0,1/3.0,0.0},{0.0,0.0,0.0,1/2.0,1/2.0},{0.0,0.0,0.0,0.0,0.0}};
	printf("Grafo inicializado : %d (error)\n",initMarkovGraph(mModels,0,adjacencyMatrix));
	// Inicializar matriz con las probabilidades de emisión de símbolos 3 estados * 3 simbolos//
	double castProbabilities[3][3] = {{1/2.0,1/3.0,1/6.0},{1/4.0,1/4.0,1/2.0},{1/4.0,1/2.0,1/4.0}};
	printf("Matriz de emision inicializada : %d (error)\n",initCastProbabilities(mModels,0,castProbabilities));
}

BOOL initMarkovModels(struct MarkovModels* mm,int n){ 
	mm->markovModels = (MarkovModel*)malloc(n*sizeof(MarkovModel));
	return mm->markovModels!=NULL;
}

BOOL setMarkovStates(struct MarkovModels* mm,int n){ 
	mm->markovModels[n].Q = (int*)malloc(mm->markovModels[n].numStates*sizeof(int));
	unsigned int i;
	for(i=0;i<mm->markovModels[n].numStates;i++) mm->markovModels[n].Q[i] = i;
	return mm->markovModels[n].Q==NULL;
}

BOOL setMarkovCodebook(struct MarkovModels* mm,int n,char* codebook){ 
	mm->markovModels[n].E = (char*)malloc(mm->markovModels[n].numCodewords*sizeof(char));
	unsigned int i;
	for(i=0;i<mm->markovModels[n].numCodewords;i++) mm->markovModels[n].E[i] = codebook[i];
	return mm->markovModels[n].E==NULL;
}

BOOL initMarkovGraph(struct MarkovModels* mm,int n,double adjacencyMatrix[5][5]){
	unsigned int i,j;
	BOOL error = FALSE;
	mm->markovModels[n].A = (double**)malloc(mm->markovModels[n].numStates*sizeof(double*));
	for(i=0;i<mm->markovModels[n].numStates;i++){
		mm->markovModels[n].A[i] = (double*)malloc(mm->markovModels[n].numStates*sizeof(double));
		for(j=0;j<mm->markovModels[n].numStates;j++){
			
			mm->markovModels[n].A[i][j] = adjacencyMatrix[i][j];
		}
		error = mm->markovModels[n].A[i]==NULL ? 1 : 0;
	}
	return error;
}

BOOL initCastProbabilities(struct MarkovModels* mm,int n,double castProbabilities[3][3]){
	unsigned int i,j;
	BOOL error = FALSE;
	mm->markovModels[n].B = (double**)malloc((mm->markovModels[n].numStates-2)*sizeof(double*));
	for(i=1;i<mm->markovModels[n].numStates-1;i++){
		mm->markovModels[n].B[i-1] = (double*)malloc(mm->markovModels[n].numCodewords*sizeof(double));
		for(j=0;j<mm->markovModels[n].numCodewords;j++){
			mm->markovModels[n].B[i-1][j] = castProbabilities[i-1][j];
		}
		error = mm->markovModels[n].B[i]==NULL ? 1 : 0;
	}
	return error;
}

int getCodewordIndex(struct MarkovModels* mm,int p,char codeword){
	unsigned int i,res=-1;
	for(i=0;i<mm->markovModels[p].numCodewords;i++) if(mm->markovModels[p].E[i]==codeword) res = i;
	return res;
}

BOOL freeMarkovModels(struct MarkovModels* mm,int n){
	unsigned int i,j;
	for(i=0;i<n;i++){
		free(mm->markovModels[i].Q);
		free(mm->markovModels[i].E);
		for(j=0;j<n;j++){
			free(mm->markovModels[i].A[j]);
			free(mm->markovModels[i].B[j]);
		}
		free(mm->markovModels[i].A);
		free(mm->markovModels[i].B);
	}
	free(mm);
	return mm->markovModels[i].Q==NULL && mm->markovModels[i].E==NULL && mm->markovModels[i].A==NULL && mm->markovModels[i].B==NULL;
}
