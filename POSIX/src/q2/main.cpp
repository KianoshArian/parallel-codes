#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>
#include <pthread.h>

using namespace std;

#define ARRAY_LENGTH 1048576
#define NUM_OF_THREADS 4

typedef struct{
    float mean;
    float* arr;
}sigma_params;

void serial_SD(float* arr){
    float mean, sum=0;
    for(int i=0; i<ARRAY_LENGTH; i++)
        sum+=arr[i];
    mean=sum/ARRAY_LENGTH;

    float sigma=0;
    for(int i=0; i<ARRAY_LENGTH; i++)
        sigma+=pow((arr[i]-mean), 2);
    float SD = sqrt(sigma/ARRAY_LENGTH);
    cout<<"serial:\nMean: "<<mean<<" Standard Deviation: "<<SD<<endl;


}

void *thread_sum(void *arg){
	float *arr=(float*) arg;
	float* sum=(float*) malloc(sizeof(int));
	*sum=0;
	for(int i=0; i<ARRAY_LENGTH/NUM_OF_THREADS; i++){
        *sum+=arr[i];
	}
	pthread_exit(sum);
}

void *thread_sigma(void *arg){
	float* sigma=(float*) malloc(sizeof(float));
    sigma_params* params=(sigma_params *)arg;
    float* arr=params->arr;
    float mean=params->mean;
	*sigma=0;
	for(int i=0; i<ARRAY_LENGTH/NUM_OF_THREADS; i++){
        *sigma+=pow((arr[i]-mean), 2);
	}
	pthread_exit(sigma);
    return 0;
}

void parallel_SD(float* arr){
	pthread_t t_sum[NUM_OF_THREADS];
	pthread_t t_sigma[NUM_OF_THREADS];
	void* result[4];
	float sum=0, mean, sigma=0;
	for(int i=0; i<NUM_OF_THREADS; i++){
		pthread_create(&t_sum[i], NULL, thread_sum, &arr[i*(ARRAY_LENGTH/NUM_OF_THREADS)]);
	}
	for(int i=0; i<NUM_OF_THREADS; i++){
		pthread_join(t_sum[i], &result[i]);
        sum+=*(float*)result[i];
	}
    mean=sum/ARRAY_LENGTH;
	for(int i=0; i<NUM_OF_THREADS; i++){
        sigma_params params={mean, &arr[i*(ARRAY_LENGTH/NUM_OF_THREADS)]};
		pthread_create(&t_sigma[i], NULL, thread_sigma, &params);
	}
	for(int i=0; i<NUM_OF_THREADS; i++){
		pthread_join(t_sigma[i], &result[i]);
        sigma+=*(float*)result[i];
	}
    float SD = sqrt(sigma/ARRAY_LENGTH);
    cout<<"parallel:\nMean: "<<mean<<" Standard Deviation: "<<SD<<endl;
}

int main(void){
	float *arr = new float [ARRAY_LENGTH];
    srand((unsigned)time(NULL));
    for (int i=0; i<ARRAY_LENGTH; i++)
        arr[i]=rand()%1000;
	struct timeval p_start, p_end, s_start, s_end;
	gettimeofday(&s_start, NULL);
    serial_SD(arr);
	gettimeofday(&s_end, NULL);
	gettimeofday(&p_start, NULL);
    parallel_SD(arr);
	gettimeofday(&p_end, NULL);
    cout<<"serial time: "<<(s_end.tv_usec-s_start.tv_usec)<<endl;
    cout<<"parallel time: "<<(p_end.tv_usec-p_start.tv_usec)<<endl;
    cout<<"speedup: "<<float(s_end.tv_usec-s_start.tv_usec)/(p_end.tv_usec-p_start.tv_usec);


	return 0;
}
