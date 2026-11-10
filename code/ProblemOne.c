/*
* Problem 1: Chef-Waiter Restaurant System
 *
 * This program simulates a restaurant where chefs
 * prepare meals and waiters serve them.
 *
 * Chefs and waiters share a buffer with limited space.
 * A mutex protects the buffer, and condition variables
 * make threads wait when the buffer is full or empty.
 *
 * CPS 356 - Operating Systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Maximum values allowed by the assignment
#define MAX_CHEFS 5
#define MAX_WAITERS 5
#define MAX_BUFFER 20

typedef struct {
    int id;
} ThreadArg;

/*
 * Chefs and waiters use the same buffer to exchange meals.
 * The buffer can hold up to 20 meals, but the actual capacity
 * depends on the user's input.
 */
static int buffer[MAX_BUFFER];

static int chefs_count;
static int waiters_count;
static int capacity;
static int meals_per_chef;

/*
 * The buffer works like a circular queue.
 * head points to the next meal a waiter will take.
 * tail points to the position where a chef will add a meal.
 * buffer_count keeps track of how many meals are waiting.
 */
static int head = 0;
static int tail = 0;
static int buffer_count = 0;

// Track how many meals have been prepared and served, along with the highest buffer usage.
static int next_meal_id = 1;
static int prepared = 0;
static int served = 0;
static int max_buffer_usage = 0;
static int chefs_finished = 0;

/*
 * The mutex prevents chefs and waiters from changing the
 * shared buffer at the same time. Without it, the counters
 * and buffer positions could become incorrect.
 */
static pthread_mutex_t mutex =
    PTHREAD_MUTEX_INITIALIZER;
/*
 * These condition variables help threads wait instead of
 * repeatedly checking the buffer.
 *
 * not_full: chefs wait here when there is no space.
 * not_empty: waiters wait here when there are no meals.
 */
static pthread_cond_t not_full =
    PTHREAD_COND_INITIALIZER;

static pthread_cond_t not_empty =
    PTHREAD_COND_INITIALIZER;

// Each chef prepares M meals
static void *chef(void *arg) {
    int id = ((ThreadArg *)arg)->id;

    for (int i = 0; i < meals_per_chef; ++i) {
        pthread_mutex_lock(&mutex);

        // A chef must wait until a waiter removes a meal and frees up space.
        while (buffer_count == capacity) {
            pthread_cond_wait(&not_full, &mutex);
        }

        int meal_id = next_meal_id++;

        // Place meal into circular buffer
        buffer[tail] = meal_id;
        tail = (tail + 1) % capacity;

        ++buffer_count;
        ++prepared;

        if (buffer_count > max_buffer_usage) {
            max_buffer_usage = buffer_count;
        }

        printf("Chef %d prepared meal %d "
               "(buffer size = %d)\n",
               id, meal_id, buffer_count);

        // A new meal is available, so wake up a waiter that may be waiting.
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    // Record this chef's completion
    pthread_mutex_lock(&mutex);

    ++chefs_finished;

    if (chefs_finished == chefs_count) {
        pthread_cond_broadcast(&not_empty);
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

// Waiters repeatedly take meals from the shared buffer until all chefs finish and no meals remain.
static void *waiter(void *arg) {
    int id = ((ThreadArg *)arg)->id;

    while (1) {
        pthread_mutex_lock(&mutex);
        // A chef cannot add a meal if the buffer is full.
        // The condition variable releases the mutex while waiting,
        // allowing a waiter to remove a meal.
        while (buffer_count == 0 &&
               chefs_finished < chefs_count) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        // Exit when no meals remain
        if (buffer_count == 0 &&
            chefs_finished == chefs_count) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Remove one meal from buffer
        int meal_id = buffer[head];
        head = (head + 1) % capacity;

        --buffer_count;
        ++served;

        printf("Waiter %d served meal %d "
               "(buffer size = %d)\n",
               id, meal_id, buffer_count);

        // Notify a waiting chef
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(void) {
    printf("Enter C W B M: ");

    if (scanf("%d %d %d %d",
              &chefs_count,
              &waiters_count,
              &capacity,
              &meals_per_chef) != 4 ||
        chefs_count < 1 || chefs_count > MAX_CHEFS ||
        waiters_count < 1 || waiters_count > MAX_WAITERS ||
        capacity < 5 || capacity > MAX_BUFFER ||
        meals_per_chef < 1 || meals_per_chef > 20) {

        fprintf(stderr,
                "Invalid input. Use: "
                "1<=C<=5, 1<=W<=5, "
                "5<=B<=20, 1<=M<=20.\n");

        return EXIT_FAILURE;
    }

    pthread_t chefs[MAX_CHEFS];
    pthread_t waiters[MAX_WAITERS];

    ThreadArg chef_args[MAX_CHEFS];
    ThreadArg waiter_args[MAX_WAITERS];

    // Start waiter threads
    for (int i = 0; i < waiters_count; ++i) {
        waiter_args[i].id = i + 1;

        if (pthread_create(&waiters[i], NULL,
                           waiter, &waiter_args[i]) != 0) {
            fprintf(stderr,
                    "Failed to create waiter thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Start chef threads
    for (int i = 0; i < chefs_count; ++i) {
        chef_args[i].id = i + 1;

        if (pthread_create(&chefs[i], NULL,
                           chef, &chef_args[i]) != 0) {
            fprintf(stderr,
                    "Failed to create chef thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Make sure all chefs have finished preparing meals before the program ends.
    for (int i = 0; i < chefs_count; ++i) {
        pthread_join(chefs[i], NULL);
    }

    // Wait for every waiter to finish
    for (int i = 0; i < waiters_count; ++i) {
        pthread_join(waiters[i], NULL);
    }

    printf("\nExecution Summary\n");
    printf("Total meals prepared: %d\n", prepared);
    printf("Total meals served: %d\n", served);
    printf("Maximum buffer usage: %d\n",
           max_buffer_usage);

    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
