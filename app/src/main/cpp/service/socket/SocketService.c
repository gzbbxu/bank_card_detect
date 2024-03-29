//
// Created by zkk on 2019/3/28.
//


#include "SocketServer.h"
#include "Alog_pri.h"

#define LOG_TAG "LOG_TAG_socket"

int server_sockfd = -1;

ssize_t _readn(int fd, void *buf, size_t count) {
    size_t nleft = count; //剩下需要读取的数据的个数
    ssize_t nread; //成功读取的个数
    char *bufp = (char *) buf;
    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (nread == 0)
            return count - nleft;

        bufp += nread;
        nleft -= nread;
    }

    return count;
}

ssize_t _writen(int fd, const void *buf, size_t count) {
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char *) buf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (nwritten == 0)
            continue;

        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;
}


int bindListenSocket() {
//     sock = -1;
    if (server_sockfd < 0) {
        ALOGI("socket 建立>>");
        if ((server_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            ALOGI("socket 失败>> %d ", server_sockfd);
            return BIND_ERROR;

        }

    }
//    ALOGI("socket bindListenSocket >>");
    // 定义结构体
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_URL);
    int ret;
    if ((ret = connect(server_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        perror("connect");
        ALOGI("connect error >> %d", errno);
        server_sockfd = -1;
        return CONNECT_ERROR;
    }

    return 0;
}

int writeToServer(const void *buf, size_t count) {
    return _writen(server_sockfd, buf, count);
}

int readFromServer(void *buf, size_t count) {
    return _readn(server_sockfd, buf, count);
}

int closeSocket() {
    if (server_sockfd == -1) {
        ALOGE("socket 已经关闭");
        return -2;
    }
    if (close(server_sockfd) < 0) {
//    if (shutdown(server_sockfd,SHUT_RDWR) < 0) {
        ALOGI("close error >> %d", errno);
        server_sockfd = -1;
        return 0;
    } else {
        ALOGE("close success");
        return -1;
    }
}