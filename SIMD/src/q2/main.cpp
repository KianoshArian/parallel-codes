#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include "x86intrin.h"
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
	__m128 vec, sum, means, sub, pow;
    float mean;
    sum = _mm_loadu_ps (arr);
	for (long i = 4; i < ARRAY_LENGTH; i+=4) {
		vec = _mm_loadu_ps (&arr[i]);
        sum = _mm_add_ps(sum, vec);
	}
    float result[4];
    _mm_storeu_ps(result, sum);
    mean=(result[0]+result[1]+result[2]+result[3])/ARRAY_LENGTH;
    float meanArr[4]={mean, mean, mean, mean};
    means = _mm_loadu_ps (meanArr);

    vec = _mm_loadu_ps (arr);
    sub = _mm_sub_ps (vec, means);
    sum = _mm_mul_ps (sub, sub);
	for (long i = 4; i < ARRAY_LENGTH; i+=4) {
        vec = _mm_loadu_ps (&arr[i]);
        sub = _mm_sub_ps (vec, means);
        pow = _mm_mul_ps (sub, sub);
        sum = _mm_add_ps(sum, pow);
	}
    _mm_storeu_ps(result, sum);
    float SD = sqrt((result[0]+result[1]+result[2]+result[3])/ARRAY_LENGTH);
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
    cout<<"parallel time: "<<(p_end.tv_usec-p_start.tv_usec) << endl;
    cout<<"speed up: "<<(float)(s_end.tv_usec-s_start.tv_usec)/(p_end.tv_usec-p_start.tv_usec) << endl;


	return 0;
}
