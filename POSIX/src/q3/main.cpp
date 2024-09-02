#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include <sys/time.h>
#include "../../lib/stb_image_write.h"
#include "../../lib/stb_image.h"
#include <pthread.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define THREAD_NUM 6

using namespace std;

typedef struct
{
    int width;
    int height;
    int comp;
    unsigned char *data;
} image;

typedef struct
{
    size_t i;
    image *image_1;
    image *image_2;
    image *result;
} thread_params;

void serial_implementation(image *image_1, image *image_2, image *result)
{
    for (size_t i = 0; i < MIN(image_1->height, image_2->height); i++)
    {
        for (size_t j = 0; j < MIN(image_1->width, image_2->width); j++)
        {
            for (size_t k = 0; k < result->comp; k++)
            {
                result->data[i * result->width * result->comp + j * result->comp + k] = abs(
                    image_2->data[i * image_2->width * image_2->comp + j * image_2->comp + k] -
                    image_1->data[i * image_1->width * image_1->comp + j * image_1->comp + k]);
            }
        }
    }
}

void *thread_func(void *arg){
    thread_params* params=(thread_params *)arg;
    image *image_1 = params->image_1;
    image *image_2 = params->image_2;
    image *result = params->result;
    size_t thread_i = params->i;
    size_t start_index = thread_i*MIN(image_1->height, image_2->height)/THREAD_NUM;
    size_t finish_index = start_index+MIN(image_1->height, image_2->height)/THREAD_NUM;

    for (size_t i = start_index; i < finish_index; i++)
    {
        for (size_t j = 0; j < MIN(image_1->width, image_2->width); j++)
        {
            for (size_t k = 0; k < result->comp; k++)
            {
                result->data[i * result->width * result->comp + j * result->comp + k] = abs(
                    image_2->data[i * image_2->width * image_2->comp + j * image_2->comp + k] -
                    image_1->data[i * image_1->width * image_1->comp + j * image_1->comp + k]);
            }
        }
    }
	pthread_exit(NULL);
}

void pthread_implementation(image *image_1, image *image_2, image *result)
{
	pthread_t t[THREAD_NUM];
    thread_params params[THREAD_NUM];
	for(size_t i=0; i<THREAD_NUM; i++){
        params[i]={i, image_1, image_2, result};
		pthread_create(&t[i], NULL, thread_func, &params[i]);
	}
	for(int i=0; i<THREAD_NUM; i++){
		pthread_join(t[i], NULL);
	}
}

image load(const char *path)
{
    image image;
    image.data = stbi_load(path, &image.width, &image.height, &image.comp, 1);
    image.comp = 1;

    return image;
}

void save(const char *path, image *image)
{
    stbi_write_png(path, image->width, image->height, image->comp, image->data, image->width * image->comp);
}

int main(int argc, char const *argv[])
{
	struct timeval p_start, p_end, s_start, s_end;
    image image_1 = load("../../images/CA#02__Image__01.png");
    image image_2 = load("../../images/CA#02__Image__02.png");

	gettimeofday(&s_start, NULL);
    serial_implementation(&image_1, &image_2, &image_1);
	gettimeofday(&s_end, NULL);
    save("serial.png", &image_1);
    cout<<"serial time: "<<(s_end.tv_usec-s_start.tv_usec)<<endl;


    image_1 = load("../../images/CA#02__Image__01.png");
    image_2 = load("../../images/CA#02__Image__02.png");
	gettimeofday(&p_start, NULL);
    pthread_implementation(&image_1, &image_2, &image_1);
	gettimeofday(&p_end, NULL);
    save("pthread.png", &image_1);
    cout<<"parallel time: "<<(p_end.tv_usec-p_start.tv_usec)<<endl;


    cout<<"speedup: "<<float(s_end.tv_usec-s_start.tv_usec)/(p_end.tv_usec-p_start.tv_usec);

    return 0;
}
