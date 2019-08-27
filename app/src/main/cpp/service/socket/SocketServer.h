//
// Created by yuanshuai on 2019/3/28.
//

#ifndef FACERECOGNITION_SOCKETSERVER_H
#define FACERECOGNITION_SOCKETSERVER_H
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/in.h>
#include <endian.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../server_common.h"

//http://123.206.96.155:7651/info_management/oneTomany/
#define SERVER_URL ""
#define SERVER_PORT  7651
#define  BIND_ERROR -1
#define  CONNECT_ERROR -2
#define HERT_JUMP 30
extern int server_sockfd;

int bindListenSocket();

int writeToServer(const void *buf, size_t count);

int readFromServer(void *buf, size_t count);

int closeSocket();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //FACERECOGNITION_SOCKETSERVER_H
