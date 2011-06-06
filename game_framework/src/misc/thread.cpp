#include <pthread.h>
#include<semaphore.h>
#include "../define.h"
#include <cstdatomic>
#include "../atomic_pod.h"

// volatile int cnt = 0;

// volatile std::atomic<int> cnt(0);

volatile atomic_pod<int> cnt(0);

sem_t mutex;

void *thread(void *argp);

int main(int argc, char *argv[])
{

    int niters = 100000000;

    sem_init(&mutex, 0, 1);

    
    printf("%d\n", _GLIBCXX_ATOMIC_BUILTINS_1);
    printf("%d\n", _GLIBCXX_ATOMIC_BUILTINS_2);
    printf("%d\n", _GLIBCXX_ATOMIC_BUILTINS_4);
    printf("%d\n", _GLIBCXX_ATOMIC_BUILTINS_8);
    
    
    pthread_t tid1, tid2;
    if (pthread_create(&tid1, 0, thread, &niters) < 0) {
        handle_error("pthread_create");
    }
    if (pthread_create(&tid2, 0, thread, &niters) < 0) {
        handle_error("pthread_create");
    }
    pthread_join(tid1, 0);
    pthread_join(tid2, 0);

    if (cnt != (2 * niters)) {
        printf("BOOM! cnt = %d\n", cnt.load());
    }
    else {
        printf("OK! cnt = %d\n", cnt.load());
    }
    exit(0);
}

void *thread(void *argp)
{
    int niters = *((int *)argp);
    for (int i = 0; i < niters; i++) {
        // sem_wait(&mutex);
        //cnt++;
        ++cnt;
        // __sync_fetch_and_add(&cnt, 1);
        // sem_post(&mutex);
    }
        
    return 0;
}
