#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include "x86intrin.h"
#include <sys/time.h>

using namespace std;

#define ARRAY_LENGTH 1048576

void serial_min(float* arr){
    float min = arr[0];
    int index = 0;
    for(int i=1; i<ARRAY_LENGTH; i++){
        if(arr[i] < min){
            min = arr[i];
            index = i;
        }
    }
    cout<<"serial:\nmin: "<< min << "\nindex: " << index << endl;
}

void parallel_min(float* arr){
	__m128 vec, min, comp;
    int in[4] = {0, 1, 2, 3};
    min = _mm_loadu_ps (arr);
	for (long i = 4; i < ARRAY_LENGTH; i+=4) {
		vec = _mm_loadu_ps (&arr[i]);
        comp = _mm_cmplt_ps(min, vec);
        float comp_res[4];
        _mm_storeu_ps(comp_res, comp);
        if(comp_res[0]==0)
            in[0] = i;
        if(comp_res[1]==0)
            in[1] = i+1;
        if(comp_res[2]==0)
            in[2] = i+2;
        if(comp_res[3]==0)
            in[3] = i+3;
        min = _mm_min_ps (min, vec);
	}
    float result[4];
    _mm_storeu_ps(result, min);
    int min_index=0;
    for(int i=1; i<4; i++)
        if(result[i] < result[min_index])
            min_index=i;

    cout<<"parallel:\nmin: "<< result[min_index] << "\nindex: "<<in[min_index]<<endl;
}

int main(void){
	float *arr = new float [ARRAY_LENGTH];
    srand((unsigned)time(NULL));
    for (int i=0; i<ARRAY_LENGTH; i++)
        arr[i]=rand();
	struct timeval p_start, p_end, s_start, s_end;
	gettimeofday(&s_start, NULL);
    serial_min(arr);
	gettimeofday(&s_end, NULL);
	gettimeofday(&p_start, NULL);
    parallel_min(arr);
	gettimeofday(&p_end, NULL);
    cout<<"serial time: "<<(s_end.tv_usec-s_start.tv_usec) << endl;
    cout<<"parallel time: "<<(p_end.tv_usec-p_start.tv_usec) << endl;
    cout<<"speed up: "<<(float)(s_end.tv_usec-s_start.tv_usec)/(p_end.tv_usec-p_start.tv_usec) << endl;


	return 0;
}
