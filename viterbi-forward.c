/*
 * Title: Viterbi + Forward algorithms
*/

#include "MarkovModels.c"
#define VITERBI 1
#define FORWARD 0
#define VERBOSE 1

#define MAX(a,b) ((a>b) ? a : b)
#define SHOWMAT(mat,n,m){\
	unsigned int i,j;\
	for(i=0;i<n;i++){\
		for(j=0;j<m;j++){\
			printf("%f,",mat[i][j]);\
		}\
		printf("\n");\
	}\
	printf("\n");\
}

double viterbi(struct MarkovModels* mModels,int p,char* string,int lengthString,char* optimalPath){
	MarkovModel mModel = mModels->markovModels[p];
	double** dynamicMatrix = (double**)calloc((mModel.numStates-2),sizeof(double*));
	unsigned int i,j,z,maxNode=0;
	// Reservar memoria matriz //
	for(i=0;i<mModel.numStates-2;i++) dynamicMatrix[i] = (double*)calloc(lengthString,sizeof(double));
	// Inicializar con probabilidades iniciales //
	for(i=1;i<mModel.numStates-1;i++){
		if(mModel.A[0][i]!=0.0){ // Si hay un camino inicial al nodo i con probabilidad P(Nodo inicial a i)!= 0, se inicializa el valor del nodo i con P(Nodo inicial a i)*P(emitir el primer caracter de la cadena en el nodo i) //
			dynamicMatrix[i-1][0] = mModel.B[i-1][getCodewordIndex(mModels,p,string[0])] * mModel.A[0][i];
		}
	}
	// A partir de las probabilidades iniciales calcular la matriz de viterbi //
	for(i=0;i<lengthString-2;i++){
		for(j=1;j<mModel.numStates-1;j++){
			for(z=1;z<mModel.numStates-1;z++){
				// Por cada transición desde el estado inicial a alguno siguiente se suma al contenido con indice del nodo siguiente en dynamicMatrix
				// la probabilidad de transitar desde el estado actual al estado siguiente (se tienen los indices destino) * la probabilidad de 
				// emitir el siguiente codeword en el nodo destino * el contenido del nodo actual en dynamicMatrix.
				if(mModel.A[j][z] != 0.0){ // Si hay una transición del actual a un destino //
					if(VERBOSE){
						printf("\n ---------------------------------------------\n");
						printf("Probabilidad de ir de %d a %d : %f\n",j,z,mModel.A[j][z]);
						printf("Probabilidad de emitir %c en el nodo destino %d : %f\n",string[i+1],z,mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])]);
						printf("Valor de dynamicMatrix : %f\n",dynamicMatrix[j-1][i]);
						printf("Operacion: %f * %f * %f \n",mModel.A[j][z],mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])],dynamicMatrix[j-1][i]);
						printf("Resultado : %f\n", mModel.A[j][z] * mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])] * dynamicMatrix[j-1][i]);
						printf("Valor actual de dynamicMatrix destino : %f\n",dynamicMatrix[z-1][i+1]);
						printf("En destino se escribe : %f\n",dynamicMatrix[z-1][i+1] +mModel.A[j][z] * mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])] * dynamicMatrix[z-1][i]);
						printf("\n ---------------------------------------------\n");
					}
					if(FORWARD) dynamicMatrix[z-1][i+1] += mModel.A[j][z] * mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])] * dynamicMatrix[j-1][i];
					else if(VITERBI){
						double possibleValue = (mModel.A[j][z] * mModel.B[z-1][getCodewordIndex(mModels,p,string[i+1])] * dynamicMatrix[j-1][i]);
						double antValue      =  dynamicMatrix[z-1][i+1];
						dynamicMatrix[z-1][i+1] = MAX(possibleValue,antValue);
						if(MAX(possibleValue,antValue)==possibleValue) maxNode=j;
					}
				}
			}
		}
		optimalPath[i] = maxNode+0x30;
	}
	/** Calcular probabilidad final **/
	double pFinal = 0.0;
	for(j=1;j<mModel.numStates-1;j++){ // Por cada estado j en la matriz dinamica
		if(mModel.A[j][mModel.numStates-1]!=0.0){ // Si se puede transitar del estado j al estado final
			if(VERBOSE) printf("Transitar del nodo %d al nodo final con valor en matriz dinamica: %f y con probabilidad de transitar al nodo final : %f\n",j,dynamicMatrix[j-1][lengthString-2],mModel.A[j][mModel.numStates-1]);
			if(FORWARD) pFinal += dynamicMatrix[j-1][lengthString-2] * mModel.A[j][mModel.numStates-2];
			else if(VITERBI){
				double possibleValue = dynamicMatrix[j-1][lengthString-2] * mModel.A[j][mModel.numStates-2];
				double antValue      = pFinal;
				pFinal = MAX(antValue,possibleValue);
				if(MAX(antValue,possibleValue)==possibleValue) maxNode = j;
			}
		}
	}	
	optimalPath[i]   = maxNode+0x30;
	optimalPath[i+1] = 'F';
	optimalPath[i+2] = '\x00';
	if(VERBOSE) SHOWMAT(dynamicMatrix,mModel.numStates-2,lengthString);
	/** Liberar matriz de viterbi **/
	for(i=0;i<mModel.numStates-2;i++) free(dynamicMatrix[i]);
	free(dynamicMatrix);
	return pFinal;
}

int main(int argc, char **argv){
	printf("\t\t\t|--------------------------------|\n");
	printf("\t\t\t|         VITERBI-FORWARD        |\n");
	printf("\t\t\t|--------------------------------|\n\n");
	MarkovModels mModels;
	setMarkovModels(&mModels,1);
	char optimalPath[5];
	if((VITERBI && !FORWARD) || (FORWARD && !VITERBI)){
		double pFinal = viterbi(&mModels,0,"abc",4,optimalPath);
		printf("Liberando memoria reservada para los modelos de markov : %d (error)\n\n",freeMarkovModels(&mModels,1));
		if(VERBOSE){
			printf("\n-------------- Resultado --------------\n\n");
			printf("Probabilidad final : %f\n",pFinal);
			if(VITERBI) printf("Camino optimo : { %s }",optimalPath);
			printf("\n\n------------------------------------\n\n");
		}
	}
	return 0;
}
