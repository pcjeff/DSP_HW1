#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hmm.h"
#define Seq_len 50
#define Sample_count 10000

HMM* hmm;  
double _alpha[MAX_SEQ][MAX_STATE];
double _beta[MAX_SEQ][MAX_STATE];
double _gamma[MAX_SEQ][MAX_STATE];
double _epslion[MAX_SEQ][MAX_STATE][MAX_STATE];
double acc_epslion[MAX_STATE][MAX_STATE];
double acc_gamma[MAX_SEQ][MAX_STATE];
double stat_gamma[MAX_STATE];
double stat2_gamma[MAX_STATE];
double ob_gamma[MAX_OBSERV][MAX_STATE];

void load_observation(FILE* fptr,char* observation)
{	 
	 fscanf(fptr, "%s", observation);
}
void DumpModel(FILE* fptr)
{
	dumpHMM(fptr, hmm);
}
int ob2index(char ob)
{
	 return ob-65;
}
void forward(char* observation)
{
	int t=0, j=0, i=0;
	for(i=0 ; i<hmm->state_num ; i++)
	{
		_alpha[0][i] = hmm->initial[i] * hmm->observation[ ob2index(observation[0]) ][i];
	}
	for(t=1 ; t < Seq_len ; t++)
	{
		for(i=0 ; i<hmm->state_num ; i++)
		{
			 _alpha[t][i] = 0.;
			 for(j=0 ; j<hmm->state_num ; j++)
			 {
	 	 	 	 _alpha[t][i] += _alpha[t-1][j] * hmm->transition[j][i]; 
			 }
			 _alpha[t][i] *= hmm->observation[ ob2index(observation[t]) ][i];
		}
	}
}
void backward(char* observation)
{
	int i=0, j=0, t=0;
	for(i=0 ; i<hmm->state_num ; i++)
		_beta[Seq_len-1][i] = 1;
	for(t=Seq_len-2 ; t>=0 ; t--)
	{
		for(i=0 ; i<hmm->state_num ; i++)
		{
			_beta[t][i] = 0.;
			for(j=0 ; j<hmm->state_num ; j++)
			{
				_beta[t][i] += _beta[t+1][j] * hmm->observation[ ob2index(observation[t+1]) ][j] * hmm->transition[i][j];
			}
		}
	}

}
void Getgamma(char* observation)
{
	 int i=0, t=0;
	 double sum_alpha_beta = 0.;
	 for(t=0 ; t<Seq_len ; t++)
	 {
		 sum_alpha_beta = 0.;
		 for(i=0  ; i<hmm->state_num ; i++)
	 	 {
			 sum_alpha_beta += _alpha[t][i] * _beta[t][i];
		 //printf("alpha: %lf, beta: %lf\n", _alpha[t][i], _beta[t][i]);
		 }
		 _gamma[t][i] = 0;
	 	 for(i=0 ; i<hmm->state_num ; i++)
		 {
	 	 	 _gamma[t][i] = ( _alpha[t][i] * _beta[t][i] ) / sum_alpha_beta ;
			 if(t != Seq_len-1)
			 {
	 	 	 	 stat_gamma[i] += _gamma[t][i];
			 }
		 	 acc_gamma[t][i] += _gamma[t][i];
	 	 	 ob_gamma[ ob2index(observation[t]) ][i] += _gamma[t][i];
			 stat2_gamma[i] += _gamma[t][i];
		 }
	 }

}
void Getepsilon(char* observation)
{
	int t=0, i=0., j=0;
	double nor = 0.;
	for(t=0 ; t<Seq_len-1 ; t++)
	{
		nor = 0.;
		for(i=0 ; i<hmm->state_num ; i++)
		{
			 for(j=0 ; j<hmm->state_num ; j++)
			 {
	 	 	 	 nor += _alpha[t][i] * hmm->transition[i][j] * 
					 hmm->observation[ ob2index(observation[t+1]) ][j] * _beta[t+1][j];
			 }
		}
		for(i=0 ; i<hmm->state_num ; i++)
		{
			for(j=0 ; j<hmm->state_num ; j++)
			{
	 	 	 	 _epslion[t][i][j] = ( _alpha[t][i] * hmm->transition[i][j] * 
					 hmm->observation[ ob2index(observation[t+1]) ][j] * _beta[t+1][j] ) / nor ;
				 acc_epslion[i][j] += _epslion[t][i][j];
			}
		}
	}
}
void UpdateHMM()
{
	 int i=0, j=0, k=0;
	 for(i=0 ; i<hmm->state_num ; i++)
	 {
		 hmm->initial[i] = acc_gamma[0][i] / Sample_count;
	 }
	 for(i=0 ; i<hmm->state_num ; i++)
	 {
		 for(j=0 ; j<hmm->state_num ; j++)
		 {
			 hmm->transition[i][j] = acc_epslion[i][j] / stat_gamma[i];
		 }
	 }
	 for(k=0 ; k<hmm->observ_num ; k++)
	 {
		 for(i=0 ; i<hmm->state_num ; i++)
		 {
			 hmm->observation[k][i] = ob_gamma[k][i] / stat2_gamma[i];
		 }
	 }
}
int main(int argc, char* argv[])
{
	 int i=0, j=0;
	 int iteration = atoi(argv[1]);
	 char* HMM_init_FileName = argv[2];
	 char* Seq_model_FileName = argv[3];
	 char* Output_model_FileName = argv[4];//params
	 FILE* fin = fopen(Seq_model_FileName, "r");//data file
	 FILE* fout = fopen(Output_model_FileName, "w");
	 char observation[Seq_len] = "";//observation sequence
	 
	 hmm = (HMM*)malloc(sizeof(HMM));//HMM model
	 
	 loadHMM(hmm, HMM_init_FileName);
	 bzero( _alpha, sizeof(_alpha));
	 bzero( _beta, sizeof(_beta));
	 bzero( _gamma, sizeof(_gamma));
	 bzero( _epslion, sizeof(_epslion));

	 for(i=0 ; i<iteration ; i++)
	 {
	 	 bzero( acc_epslion, sizeof(acc_epslion));
	 	 bzero( acc_gamma, sizeof(acc_gamma));
		 bzero( stat_gamma, sizeof(stat_gamma));
	 	 bzero( stat2_gamma, sizeof(stat2_gamma));
	 	 bzero( ob_gamma, sizeof(ob_gamma));
	 	 for(j=0 ; j<Sample_count ; j++)
	 	 {
	 	 	 load_observation(fin, observation);
	 	     forward(observation);
	 	     backward(observation);
	 	     Getgamma(observation);
	 	     Getepsilon(observation);
	 	 }
	 	 UpdateHMM(); 
		 fseek(fin, 0, SEEK_SET);
	 }
	 DumpModel(fout);
	 return 0;	 
}

