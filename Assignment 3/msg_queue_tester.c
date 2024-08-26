/* Nicholas Coleman
 * Jacqueline Justice
 * CSCI 4100 Assignment 3
 * This program tests a message queue by creating multiple sending and
 * receiving threads
 */

#include "msg_queue.h"
#include <stdio.h>
#include <stdlib.h>

/* Structure to hold thread arguments */
struct thread_args {
  int t_id;
  int num_msgs;
  struct msg_queue *mq;
};

/* Function prototypes */
void thread_args_init(struct thread_args *args, int t_id, int num_msgs,
                      struct msg_queue *mq);
void *send_msgs(void *args);
void *receive_msgs(void *args);

/* Main function */
int main(int argc, char *argv[]) {
  // Declare a pthread_t variable for each of the sending and receiving threads
  pthread_t send_thread;
  pthread_t rec_thread;
  // Declare a msg_queue structure and initialize it using mq_init.
  struct msg_queue mq;
  mq_init(&mq);
  /*
  Declare a thread_args structure for each of the sending and receiving
  threads
  and initialize them using thread_args_init.
  */
  //for every message being received
  for(int i = 0; i < 4; i++){
     struct thread_args args_rec;
     thread_args_init(&args_rec, i, 6, &mq);
    
  }
  //for every message thats being sent
  for(int i = 0; i < 3; i++){
    struct thread_args args_send;
    thread_args_init(&args_send, i, 8, &mq);
   
  }
   /*
  Start each of the receiving threads using pthread_create, the receive_msgs
  function, and the appropriate thread_args structure.
  */
  struct thread_args args_rec;
  for(int i = 0; i < 4; i++){
   
    thread_args_init(&args_rec, i, 6, &mq);
     pthread_create(&rec_thread, NULL, receive_msgs, &args_rec);
  }
  //same with sending
  struct thread_args args_send;
  for(int i = 0; i < 3; i++){
    thread_args_init(&args_send, i, 8, &mq);
     pthread_create(&send_thread, NULL, send_msgs, &args_send);
  }
  // Wait for each of the sending and receiving threads to complete using
  // pthread_join
  for(int i = 0; i < 3; i++){
    pthread_join(rec_thread, NULL);
  }
  for(int i = 0; i < 4; i++){
    pthread_join(send_thread, NULL);
  }
}

/* Initialize thread_args structure */
void thread_args_init(struct thread_args *args, int t_id, int num_msgs,
                      struct msg_queue *mq) {

  // id number for the thread.
  args->t_id = t_id;
  // number of messages for the thread to send or receive.
  args->num_msgs = num_msgs;
  // queue is a pointer to the queue that the thread will use to send or
  // receive messages.
  args->mq = mq;
}

/* Function for sending thread to execute */
void *send_msgs(void *args) {
  // Cast the void pointer args to a pointer to a thread_args structure
  struct thread_args *real_args = (struct thread_args *)args;
  // Declare a mq_msg structure.
  struct mq_msg message;
  // for loop or while loop
  for (int i = 0; i < real_args->num_msgs; i++) {
    /*
    Initialize the mq_msg structure with the appropriate thread id and
    message id
    */
    message.t_id = real_args->t_id;
    message.m_id = i;
    // Send the message to the queue in the thread_args structure using 
     //mq_send
    mq_send(real_args->mq, &message);
  }
  // return null
  return NULL;
}
/* Function for receiving thread to execute */
void *receive_msgs(void *args) {
  // Cast the void pointer args to a pointer to a thread_args structure
  struct thread_args *real_args = (struct thread_args *)args;
  // Declare a mq_msg structure.
  struct mq_msg message;
  for (int i = 0; i < real_args->num_msgs; i++) {
    /*
    Receive the message from the queue in the thread_args structure using
    mq_receive
    */
    mq_receive(real_args->mq, &message);
    // Display the message along with the receiver thread id from the
    // thread_args structure.
    printf("[sender: %d, msg id: %d, receiver: %d]\n", message.t_id,
           message.m_id, real_args->t_id);
  }
  // return null
  return NULL;
}
