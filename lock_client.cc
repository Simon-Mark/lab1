// RPC stubs for clients to talk to lock_server

#include "lock_client.h"
#include "rpc.h"
#include <arpa/inet.h>

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>

lock_client::lock_client(std::string dst)
{

  pthread_mutex_init(&m_mutex, NULL);

  sockaddr_in dstsock;
  make_sockaddr(dst.c_str(), &dstsock);
  cl = new rpcc(dstsock);
  if (cl->bind() < 0) {
    printf("lock_client: call bind\n");
  }
}

int lock_client::stat(lock_protocol::lockid_t lid) {
  pthread_mutex_lock(&m_mutex);
  int r;  // Store the results returned by the server
  int ret = cl->call(lock_protocol::stat, cl->id(), lid, r);
      //cl->call() :A RPC function used to send a request to the server.
  // printf("%n\n", &ret);
  assert (ret == lock_protocol::OK);

  pthread_mutex_unlock(&m_mutex);
  return r;
}

lock_protocol::status lock_client::acquire(lock_protocol::lockid_t lid) {
  pthread_mutex_lock(&m_mutex);
  int r;    
  int ret = cl->call(lock_protocol::acquire, cl->id(), lid, r);
  // assert (ret == lock_protocol::OK);
  pthread_mutex_unlock(&m_mutex);
  return ret;
}

lock_protocol::status lock_client::release(lock_protocol::lockid_t lid) {
  pthread_mutex_lock(&m_mutex);
  int r;
  int ret = cl->call(lock_protocol::release, cl->id(), lid, r);
  // assert (ret == lock_protocol::OK);
  pthread_mutex_unlock(&m_mutex);
  return ret;
}

