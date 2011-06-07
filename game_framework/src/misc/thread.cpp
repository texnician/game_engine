#include <pthread.h>
#include<semaphore.h>
#include "../define.h"
#include "../atomic_pod.h"
#include "../log.h"

// volatile int cnt = 0;

// volatile std::atomic<int> cnt(0);
volatile atomic_pod<bool> flag(false);

volatile atomic_pod<int> cnt(0);

sem_t mutex;

void *thread(void *argp);

int main(int argc, char *argv[])
{
    file_log::reporting_level() = L_DEBUG;

    int niters = 30;

    sem_init(&mutex, 0, 1);
    
    pthread_t tid1, tid2, tid3;
    if (pthread_create(&tid1, 0, thread, &niters) < 0) {
        handle_error("pthread_create");
    }
    if (pthread_create(&tid2, 0, thread, &niters) < 0) {
        handle_error("pthread_create");
    }
    if (pthread_create(&tid3, 0, thread, &niters) < 0) {
        handle_error("pthread_create");
    }
    pthread_join(tid1, 0);
    pthread_join(tid2, 0);
    pthread_join(tid3, 0);

    if (cnt != (3 * niters) || flag) {
        printf("BOOM! cnt = %d --- %d\n", cnt.load(), flag.load());
    }
    else {
        printf("OK! cnt = %d --- %d\n", cnt.load(), flag.load());
    }
    exit(0);
}

void *thread(void *argp)
{
    int niters = *((int *)argp);
    for (int i = 0; i < niters; i++) {
        // sem_wait(&mutex);
        //flag.exchange(!flag.load());
        int ret = ++cnt;
        RT_LOG(L_DEBUG, "1st runtime log %d", ret); RT_LOG(L_DEBUG, "2nd runtime log %d", -ret);
        if (ret > 5) {
            rt_log::set(__FILE__, 56, rt_log::DISABLED);
        }
        // __sync_fetch_and_add(&cnt, 1);
        // sem_post(&mutex);
    }
        
    return 0;
}
