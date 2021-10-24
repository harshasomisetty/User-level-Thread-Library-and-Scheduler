// File:	mypthread.c
// List all group member's name: Harsha Somisetty
// username of iLab: hs884
// iLab Server: ilab3.cs.rutgers.edu

#include "mypthread.h"

int thread_count = 0; // start id at 0,  main thread id 0, other at 1

pthread_node * head = NULL; // Queue of all threads
pthread_node * currentN = NULL; // Currently running thread

static ucontext_t * schedulerC = NULL; // context of scheduler thred
static tcb * mainT;

static struct sigaction sa; // action handler
static struct itimerval timer; // timer to switch back to scheduler

static void schedule();
pthread_node* newNode(tcb* thread);
pthread_node* pop();
void push(pthread_node * node);

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)



int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {

    thread_count++;
    printf("creating thread %d\n", thread_count);
    
    tcb* thread_new = (tcb*) malloc(sizeof(tcb)); // data
    

    thread_new->id = thread_count;
    thread_new->status = SCHEDULED;
    thread_new->elapsed = 1;

    thread_new->context =  (ucontext_t*) malloc(sizeof(ucontext_t)); // context

    getcontext(thread_new->context);
    
    thread_new->context->uc_stack.ss_sp = malloc(STACKSIZE); // stack
    thread_new->context->uc_stack.ss_size = STACKSIZE;
    thread_new->context->uc_link = schedulerC;    

    makecontext( thread_new->context, (void (*)()) function, 1, arg);

    if (schedulerC == NULL){
        makeScheduler();
    }
        
    push(newNode(thread_new)); // add thread as a node to queue 
    *thread = thread_new->id; // returning thread id basically

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {
    
    swapcontext(currentN->data->context, schedulerC);

    return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
    // Deallocated any dynamic memory created when starting  thread

    //    setcontext(schedulerC);
    printf("exiting?");
    
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

    // wait for a specific thread to terminate
    // de-allocate any dynamic memory created by the joining thread
    printf("trying to join\n");
    setcontext(currentN->data->context);
    return 0;
};





/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	return 0;
};



/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
    currentN->data->elapsed++; //increment time elapsed

    printf("thread %d has elapsed %d\n", currentN->data->id, currentN->data->elapsed);

    push(currentN); // if ended, need to exit
    currentN = pop(); // gets the next best thread to execute

    setitimer(ITIMER_REAL, &timer, NULL);    
    setcontext(currentN->data->context);
    
}

static void schedule() {
    // context switch here every timer interrupt

    printf("got into schedulerrrrr \n");
    sched_stcf();

}

static void switchScheduler(int signo, siginfo_t *info, void *context){

    if (swapcontext(currentN->data->context, schedulerC) == -1)
        handle_error("Failed Scheduler Context Switch");
}

void makeScheduler(){
    schedulerC = (ucontext_t*) malloc(sizeof(ucontext_t));

    if(getcontext(schedulerC) == -1)
        handle_error("Schedule Context Creation Error");
        
    schedulerC->uc_stack.ss_sp = malloc(STACKSIZE);
    schedulerC->uc_stack.ss_size = STACKSIZE;

    makecontext(schedulerC, &schedule, 0);

    // add main thread to queue so rest of the main can execute

    mainT = (tcb *) malloc(sizeof(tcb));
    mainT->id = 0;
    mainT->status = SCHEDULED;
    mainT->elapsed = 0;
    mainT->context = (ucontext_t*) malloc(sizeof(ucontext_t));
    getcontext(mainT->context); //store current thread (main) context into this thread block
    
    currentN = newNode(mainT);
    
    // initing timer, every 25ms switches back to scheduler 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = sa.sa_flags | SA_SIGINFO;
    sa.sa_handler = NULL;
    sa.sa_sigaction = switchScheduler;

    sigaction(SIGALRM, &sa, NULL);
    
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 2500;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}



























pthread_node* newNode(tcb* thread){
    pthread_node *node = (struct pthread_node*) malloc(sizeof(pthread_node)); // node or listitem
    (*node).data = thread;
    (*node).data->elapsed = 0;
    (*node).next = NULL;
    return node;
}

// circular queue of thread
void push(pthread_node * node){
    if (node != NULL){
        pthread_node * ptr = head;
    
        if (head == NULL){
            head = node;
        } else{

            while(ptr->next != NULL && ptr->data->elapsed <= node->data->elapsed){
                ptr = ptr->next;
            }
        
            if (ptr == head) { // insert node at head of queue
                node->next = ptr->next;
                ptr->next = node;
            }else{
                node->next = ptr->next;
                ptr->next = node;
            }
        }
    }
}

pthread_node* pop(){
    pthread_node * next = head;
    head = head->next;
    return next;
}
