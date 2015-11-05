#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hmm.h"
#define Seq_len 50
#define Sample_count 2500

HMM hmm[5];


void load_observation(FILE* fptr,char* observation)
{	 
	 fscanf(fptr, "%s", observation);
}
int ob2index(char ob)
{
	 return ob-65;
}
double veteribi(int model_ind, char* observation)
{
	int t=0, i=0, j=0;
	double likelihood = 1, max_prop = 0., prop = 0.;
	double veter_log[Seq_len][MAX_STATE];
	int veter_ind[Seq_len] = {-1}, max_ind=-1;

	for(i=0 ; i<hmm[model_ind].state_num ; i++)
	{
		veter_log[0][i] = hmm[model_ind].initial[i] * hmm[model_ind].observation[ ob2index(observation[0]) ][i];
	}
	//pi(i) * b_i(o_1)
	for(t=1 ; t<Seq_len ; t++)
	{
		for(i=0 ; i<hmm[model_ind].state_num ; i++)
		{
			 max_prop = -10000;
			 prop = -10000;
			 for(j=0 ; j<hmm[model_ind].state_num ; j++)
			 {
				 prop = veter_log[t-1][j] * hmm[model_ind].transition[j][i];
				 if(prop > max_prop)
				 {
	 	 	 	 	 max_prop = prop;
				 }
			 }
			 veter_log[t][i] = max_prop * hmm[model_ind].observation[ ob2index(observation[t]) ][i];
	 		
		}
	}

	max_prop = -1000;
	max_ind = -1;
	for(i=0 ; i<hmm[model_ind].state_num ; i++)
	{
		prop = veter_log[Seq_len-1][i];
		if(prop > max_prop)
		{
			max_prop = prop;
			max_ind = i;
		}
	}
	//get P* = max_1<=i<=N[delta_T(i)]
	/*
	for(i=0 ; i<Seq_len ; i++)
	{
		for(j=0 ; j<6 ; j++)
			printf("%E ", veter_log[i][j]);
		printf("\n");
	}
	*/
	return max_prop;
}
//return likelihood

int main(int argc, char* argv[])
{
	 const char* ModelList = argv[1];
	 char* TestDataFileName = argv[2];
	 char* OutputFileName = argv[3];
	 char observation[Seq_len] = "";
	 double max_prop=-10000, prop=-10000;
	 int i=0, j=0, max_ind = -1;
	 FILE* fin = fopen(TestDataFileName, "r");
	 FILE* fout = fopen(OutputFileName, "w");
	 if(fin == NULL)
		 printf("fopen failed!!: %s\n", TestDataFileName);
	 
	 load_models(ModelList, hmm, 5);
	 //model index 0~4
	 for(i=0 ; i<Sample_count ; i++)
	 {
	 	 fscanf(fin, "%s", observation);
		 max_prop = -10000;
		 prop = -10000;
		 max_ind = -1;
		 for(j=0 ; j<5 ; j++)
		 {
	 	 	 prop = veteribi(j, observation);
			 if(prop > max_prop)
			 {
	 	 	 	 max_prop = prop;
				 max_ind = j;
			 }
		 }
		 fprintf(fout, "model_0%d.txt\n", max_ind+1, max_prop);
	 }
	 //printf("prop: %E\n", prop);
	 return 0;
}
