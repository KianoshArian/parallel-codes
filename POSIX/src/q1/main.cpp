#include <stdio.h>
#include "iostream"
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

#define ARRAY_LENGTH 1048576
#define NUM_OF_THREADS 4

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

void *thread_func(void *arg){
	float *arr=(float*) arg;
	int* index=(int*) malloc(sizeof(int));
	index[0]=0;
	for(int i=1; i<ARRAY_LENGTH/NUM_OF_THREADS; i++){
		if(arr[i]<arr[index[0]])
			index[0]=i;
	}
	pthread_exit(index);
}

void parallel_min(float* arr){
	pthread_t t[NUM_OF_THREADS];
	void* result[4];
	int indexes[4];
	for(int i=0; i<NUM_OF_THREADS; i++){
		pthread_create(&t[i], NULL, thread_func, &arr[i*(ARRAY_LENGTH/NUM_OF_THREADS)]);
	}
	for(int i=0; i<NUM_OF_THREADS; i++){
		pthread_join(t[i], &result[i]);
		indexes[i]=*(int*)result[i]+i*(ARRAY_LENGTH/NUM_OF_THREADS);
	}
	int min_index=0;
	float min=arr[min_index];
	for(int i=0; i<4; i++){
		if(arr[indexes[i]]<min){
			min=arr[indexes[i]];
			min_index=indexes[i];
		}
	}
    cout<<"parallel:\nmin: "<< arr[min_index] << "\nindex: " << min_index << endl;
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
