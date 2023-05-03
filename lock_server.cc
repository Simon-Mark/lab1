// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <map>

// implement lock
struct lock {
  enum State {FREE, LOCKED};
  lock_protocol::lockid_t lid;
  State state;
  pthread_mutex_t mutex;
  pthread_cond_t cv;

  lock(lock_protocol::lockid_t lid, State st) : lid(lid), state(st) {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv, NULL);
  }
};


lock_server::lock_server(): nacquire (0) {
  pthread_mutex_init(&mutex, NULL);
}

lock_protocol::status lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r) {
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}



// Request to release a lock with a specified ID
lock_protocol::status lock_server::release(int clt, lock_protocol::lockid_t lid, int &r) {
  // Initialization to acquire the lock is successful
  lock_protocol::status ret = lock_protocol::OK;

  pthread_mutex_lock(&mutex);

  auto iter = lock_map.find(lid);
  if (iter != lock_map.end()) {
    iter -> second -> state = lock::FREE;
    pthread_cond_broadcast(&iter -> second -> cv);
  } else {
    ret = lock_protocol::IOERR;
  }

  pthread_mutex_unlock(&mutex);

  return ret;
}

// Request to acquire a lock with a specified ID
lock_protocol::status lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r) {

  lock_protocol::status ret = lock_protocol::OK;

  pthread_mutex_lock(&mutex);

  auto iter = lock_map.find(lid);
  if(iter != lock_map.end()){
    while(iter -> second -> state != lock::FREE) {
      pthread_cond_wait(&iter -> second -> cv, &iter -> second -> mutex);
    }
    iter -> second -> state = lock::LOCKED;
  } else {  // if lock not exist
    auto new_mutex = new lock(lid, lock::LOCKED);
    lock_map.insert(std::make_pair(lid, new_mutex)); 
  }

  pthread_mutex_unlock(&mutex);

  return ret;
}


