#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <cmath>

using namespace std;

#define ARRAY_LENGTH 1048576

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

void parallel_SD(float* arr){
    float sum=0, sigma=0;
	int i, thread_count = omp_get_max_threads();

    #pragma omp parallel for reduction(+:sum) num_threads(thread_count)
        for(i=0; i<ARRAY_LENGTH; i++)
            sum+=arr[i];

    float mean = sum/ARRAY_LENGTH;

    #pragma omp parallel for reduction(+:sigma) num_threads(thread_count)
        for(i=0; i<ARRAY_LENGTH; i++)
            sigma+=pow((arr[i]-mean), 2);

    float SD = sqrt(sigma/ARRAY_LENGTH);
    cout<<"serial:\nMean: "<<mean<<" Standard Deviation: "<<SD<<endl;
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
