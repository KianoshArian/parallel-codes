#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "../../lib/stb_image_write.h"
#include "../../lib/stb_image.h"
#include "stdio.h"
#include "time.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    int width;
    int height;
    int comp;
    unsigned char *data;
} image;

void serial_implementation(image *image_1, image *image_2, image *result)
{
    int k = 0;
    for (size_t i = 0; i < MIN(image_1->height, image_2->height); i++)
    {
        for (size_t j = 0; j < MIN(image_1->width, image_2->width); j++)
        {
            unsigned char a = image_1->data[i * image_1->width * image_1->comp + j * image_1->comp + k];
            unsigned char b = image_2->data[i * image_2->width * image_2->comp + j * image_2->comp + k] >> 2;

            result->data[i * result->width * result->comp + j * result->comp + k] = (a > 0xFF - b) ? 0xFF : a + b;
        }
    }
}

void simd_implementation(image *image_1, image *image_2, image *result)
{
    assert(result->comp == 1);

    __m128i a, b;
    __m128i mask = _mm_set1_epi8(0x3F);

    for (size_t i = 0; i < MIN(image_1->height, image_2->height); i++)
    {
        for (size_t j = 0; j < MIN(image_1->width - (image_1->width % 8), image_2->width - (image_2->width % 8)); j += 16)
        {
            a = _mm_loadu_si128((__m128i *)(image_2->data + i * image_2->width * image_2->comp + j * image_2->comp));
            a = _mm_srli_epi64(a, 2);
            a = _mm_and_si128(a, mask);
            b = _mm_loadu_si128((__m128i *)(image_1->data + i * image_1->width * image_1->comp + j * image_1->comp));
            a = _mm_adds_epu8(a, b);
            _mm_storeu_si128((__m128i *)(result->data + i * result->width * result->comp + j * result->comp), a);
        }
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
    image image_1 = load("../../images/CA#02__Image__01.png");
    image image_2 = load("../../images/CA#02__Image__02.png");

    clock_t start, end;
    start = clock();
    serial_implementation(&image_1, &image_2, &image_1);
    end = clock();
    save("serial.png", &image_1);

    clock_t serial_time = end - start;
    printf("serial time: %d\n", serial_time);

    image_1 = load("../../images/CA#02__Image__01.png");
    image_2 = load("../../images/CA#02__Image__02.png");

    start = clock();
    simd_implementation(&image_1, &image_2, &image_1);
    end = clock();
    save("simd.png", &image_1);

    clock_t simd_time = end - start;
    printf("simd time: %d\n", simd_time);

    printf("speed up: %f\n", (float)serial_time / simd_time);

    return 0;
}
