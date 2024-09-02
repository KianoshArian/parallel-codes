#include <stdio.h>
#include "iostream"
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

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
	int thread_count = omp_get_max_threads();
	float mins[thread_count];
	int indexes[thread_count];
	int i, t_num;

	for(i=0; i<thread_count; i++){
		mins[i]=arr[0];
		indexes[i]=0;
	}

	#pragma omp parallel shared(arr, i, mins, indexes) private(t_num) num_threads(thread_count)
	{
		t_num=omp_get_thread_num();
		#pragma omp for
			for (i=0; i<ARRAY_LENGTH; i++){
				if(arr[i] < mins[t_num]){
					mins[t_num] = arr[i];
					indexes[t_num] = i;
				}
			}
	}
	float min=mins[0];
	int index=indexes[0];
	for (i=1; i<4; i++){
		if(mins[i]<min){
			min = mins[i];
			index = indexes[i];
		}
	}
    cout<<"parallel:\nmin: "<< min << "\nindex: " << index << endl;
}

int main() {
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
    cout<<"serial time: "<<(s_end.tv_usec-s_start.tv_usec)<<endl;
    cout<<"parallel time: "<<(p_end.tv_usec-p_start.tv_usec)<<endl;
    cout<<"speedup: "<<float(s_end.tv_usec-s_start.tv_usec)/(p_end.tv_usec-p_start.tv_usec);
	return 0;
}
