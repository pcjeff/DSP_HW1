#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hmm.h"
#define Seq_len 50

double veteribi_log[6][50] = {0.};

void load_observation(FILE* fptr,char* observation)
{	 
	 fscanf(fptr, "%s", observation);
}
int ob2index(char ob)
{
	 return ob-65;
}
void forward(HMM* hmm, char* observation)
{
	 int i=0, j=0, k=0, m=0;
	 double max_prop = 0., prop = 0.;
	 for(i=0 ; i<hmm->state_num  ; i++ )
		 veteribi_log[i][0] = log(hmm->initial[i]) + log(hmm->observation[ ob2index(observation[0]) ][i]);
	 //fill the initial prop to log
	 for(i = 0; i < Seq_len-1 ; i++)
	 {
	 	 for(j = 0 ; j < hmm->state_num ; j++)
		 {
			 max_prop = -10000;
			 prop = -10000;
			 for(k = 0 ; k < hmm->state_num ; k++)
			 {
				 prop = veteribi_log[k][i] + log(hmm->transition[k][j]);
				 //Use log prop, preventing prop being too small.
	 	 	 	 if(prop > max_prop)
					 max_prop = prop;
			 }
		 	 veteribi_log[j][ i+1 ] = max_prop + log( hmm->observation[ ob2index(observation[i+1]) ][j]);
		 }
	 }
}
int findMax(double log[6][50], int col, int size)
{
	 int i=0, ind=-1;
	 double max = -1000000000;
	 for(i=0 ; i<size ; i++)
	 {
	 	 if(log[i][col] > max)
		 {
			 max = log[i][col];
		 	 ind = i;
	 	 }
	 }
	 return ind;
}
int* backward(HMM* hmm, double veteribi_log[6][50])
{
	 int i=0, j=0, m=0, n=0;
	 int* veteribi_ind = (int*)malloc(sizeof(int)* Seq_len );
	 double max = -10000, prop = -10000, max_ind = -1;
	 veteribi_ind[Seq_len-1] = findMax(veteribi_log, Seq_len-1, hmm->state_num);
	 for(i=Seq_len-2 ; i>=0 ; i--)
	 {
		 max = -10000;
		 prop = -10000;
		 max_ind = -1;
	 	 for(j=0 ; j<hmm->state_num ; j++)
		 {
			 prop = veteribi_log[j][i] + log(hmm->transition[j][ veteribi_ind[i+1] ]);
		 	 if( prop > max )		
			 {
				 printf("max: %lf, prop: %lf, j: %d, i: %d\n", max, prop, j, i);
				 max = prop;
				 max_ind = j;
		 	 }
		 }
		 veteribi_ind[i] = max_ind;
	 }
	 return veteribi_ind;
}
int main(int argc, char* argv[])
{
	 int i=0, j=0;
	 int iteration = atoi(argv[1]);
	 char* HMM_init_FileName = argv[2];
	 char* Seq_model_FileName = argv[3];
	 char* Output_model_FileName = argv[4];//params
	 HMM* hmm = (HMM*)malloc(sizeof(HMM));//HMM model
	 FILE* fptr = fopen(Seq_model_FileName, "r");//data file
	 char observation[Seq_len] = "";//observation sequence
	 int* veteribi_ind = (int*)malloc(sizeof(int));

	 loadHMM(hmm, HMM_init_FileName);
	 for(i=0 ; i<1 ; i++)
	 {
	 	 load_observation(fptr, observation);
	 	 forward(hmm, observation);
	 	 veteribi_ind = backward(hmm, veteribi_log);
	 	 for(i=0 ; i<Seq_len ; i++)
	 	 {
			 printf("%d: ", i);
	 	     for(j=0 ; j<hmm->state_num ; j++)
	 	 	 	 printf("%f ", veteribi_log[j][i]);			 
	 	     printf("\n");
	 	 }
	 	 for(j=0 ; j<Seq_len ; j++)
	 	 {
	 	     printf("%d ", veteribi_ind[j]);
	 	 }
		 printf("\n");
	 }
	 return 0;	 
}

