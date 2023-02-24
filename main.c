#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define DELTA_T 0.05 // time step

// structure to represent a 2D vector
typedef struct {
    double x;
    double y;
} vector_t;

// structure to represent a single body
typedef struct {
    double mass;
    vector_t position;
    vector_t velocity;
    vector_t acceleration;
} body_t;

// structure to represent the thread arguments
typedef struct {
    int start;
    int end;
    int num_bodies;
    body_t* bodies;
} thread_args_t;

double GravConstant;
int bodiesN, timeSteps;

// function to calculate the distance between two 2D vectors
double distance(vector_t v1, vector_t v2) {
    return sqrt((v1.x - v2.x) *(v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

// function to calculate the force between two bodies
vector_t force(body_t b1, body_t b2) {
    double r = distance(b1.position, b2.position);
    double f = GravConstant * b1.mass * b2.mass / r*r;
    vector_t direction = { (b2.position.x - b1.position.x) / r, (b2.position.y - b1.position.y) / r };
    vector_t force_vector = { f * direction.x, f * direction.y };
    return force_vector;
}

// function to update the acceleration of a single body
void update_acceleration(body_t* b, int num_bodies, body_t* bodies) {
    vector_t total_force = { 0, 0 };
    for (int i = 0; i < num_bodies; i++) {
        if (b != &bodies[i]) {
            total_force.x += force(*b, bodies[i]).x;
            total_force.y += force(*b, bodies[i]).y;
        }
    }
    b->acceleration.x = total_force.x / b->mass;
    b->acceleration.y = total_force.y / b->mass;
}

// function to update the position and velocity of a single body
void update_body(body_t* b) {
    b->velocity.x += b->acceleration.x * DELTA_T;
    b->velocity.y += b->acceleration.y * DELTA_T;
    b->position.x += b->velocity.x * DELTA_T;
    b->position.y += b->velocity.y * DELTA_T;
}


// function to update the positions and velocities of a subset of bodies
void* update_bodies(void* args) {
    thread_args_t* thread_args = (thread_args_t*) args;
    for (int i = thread_args->start; i < thread_args->end; i++) {
        update_acceleration(&thread_args->bodies[i], thread_args->num_bodies, thread_args->bodies);
        update_body(&thread_args->bodies[i]);
    }
    pthread_exit(NULL);
}

// function to resolve the collisions if two points have same x and y
void resolve_collisions(body_t *bodies)
{
    int i, j;

    for (i = 0; i < bodiesN - 1; i++)
        for (j = i + 1; j < bodiesN; j++)
        {
            if (bodies[i].position.x == bodies[j].position.x && bodies[i].position.y == bodies[j].position.y)
            {
                vector_t temp = bodies[i].velocity;
                bodies[i].velocity = bodies[j].velocity;
                bodies[j].velocity = temp;
            }
        }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s fileName\n", argv[0]);
        return EXIT_FAILURE;
    }

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    char* fileName = argv[1];
    int NUM_THREADS = atoi(argv[2]);

    // read body data from file
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }
    fscanf(fp, "%lf%d%d", &GravConstant, &bodiesN, &timeSteps);

    body_t *bodies = (body_t *) malloc(bodiesN * sizeof(body_t));

    if (bodies == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for bodies.\n");
        return EXIT_FAILURE;
    }


    for (int i = 0; i < bodiesN; i++)
    {
        fscanf(fp, "%lf", &bodies[i].mass);
        fscanf(fp, "%lf%lf",  &bodies[i].position.x, &bodies[i].position.y);
        fscanf(fp, "%lf%lf", &bodies[i].velocity.x, &bodies[i].velocity.y);
    }

    fclose(fp);
    // create threads to update the positions and velocities of the bodies
    int num_threads = NUM_THREADS < bodiesN ? NUM_THREADS : bodiesN; // use at most NUM_THREADS threads
    pthread_t *threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    thread_args_t *thread_args = (thread_args_t *) malloc(num_threads * sizeof(thread_args_t));
    if (threads == NULL || thread_args == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for threads and thread arguments.\n");
        return EXIT_FAILURE;
    }

    int bodies_per_thread = bodiesN / num_threads;

    FILE *f;
    memcpy(fileName,"out",2);
    f = fopen(fileName, "w+");

    for (int j = 0; j < timeSteps; j++)
    {
        for (int i = 0; i < num_threads; i++) {
            thread_args[i].start = i * bodies_per_thread;
            thread_args[i].end = i < num_threads - 1 ? (i + 1) * bodies_per_thread : bodiesN;
            thread_args[i].num_bodies = bodiesN;
            thread_args[i].bodies = bodies;
            if (pthread_create(&threads[i], NULL, update_bodies, &thread_args[i]) != 0) {
                fprintf(stderr, "Error: Failed to create thread %d.\n", i);
                return EXIT_FAILURE;
            }
        }

        // wait for all threads to finish
        for (int i = 0; i < num_threads; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                fprintf(stderr, "Error: Failed to join thread %d.\n", i);
                return EXIT_FAILURE;
            }
        }

        //resolve collisions after all done
        resolve_collisions(bodies);

        // print the final positions and velocities of the bodies to the output.csv file
        fprintf(f, "%d, ", j);
        for (int i = 0; i < bodiesN; i++)
            fprintf(f, "%lf, %lf, ", bodies[i].position.x, bodies[i].position.y);
        fprintf(f, "\n");
    }
    fclose(f);


    // free memory
    free(bodies);
    free(threads);
    free(thread_args);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", cpu_time_used);

    return EXIT_SUCCESS;
}
