#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

#define THREADCOUNT 8

void *arr[4000];

void *mymalloc(void*);
void *myfree(void*);

static long n = 4000;
pthread_t t[THREADCOUNT];
pthread_mutex_t m;

int main(void) {

	printf("%ld\n", time(NULL));
	int i;
	pthread_mutex_init(&m, NULL);

	for(i=0;i<THREADCOUNT;++i)
		pthread_create(&t[i], NULL, mymalloc, &i);

	for(i=0;i<THREADCOUNT;++i)
		pthread_join(t[i], NULL);

	n=4000;

        for(i=0;i<THREADCOUNT;++i)
                pthread_create(&t[i], NULL, myfree, &i);

        for(i=0;i<THREADCOUNT;++i)
                pthread_join(t[i], NULL);

	pthread_mutex_destroy(&m);
	printf("%ld\n", time(NULL));
	return 0;
}

void *mymalloc(void* a) {

	int i = *(int*)a;

	while(n > 0) {

		pthread_mutex_lock(&m);

			if(n > 0) {
				arr[n] = malloc(4096);
				n--;
			}

		pthread_mutex_unlock(&m);
	}

}


void *myfree(void* a) {

        int i = *(int*)a;

        while(n > 0) {

                pthread_mutex_lock(&m);

                        if(n > 0) {
                                free(arr[n]);
                                n--;
                        }

                pthread_mutex_unlock(&m);
        }


}


