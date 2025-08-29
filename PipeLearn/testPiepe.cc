#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstring>

void ChildWrite(int wfd)
{
    char buffer[1024];
    int cnt;
    while (true)
    {
        snprintf(buffer, sizeof(buffer), "I am child , pid: %d, cnt: %d", getpid(), cnt++);
        write(wfd, buffer, strlen(buffer));
        sleep(1);
        if (cnt == 5)
            break;
    }
}

void FatherRead(int rfd)
{
    char buffer[1024];
    while (true)
    {
        buffer[0] = 0;
        ssize_t n = read(rfd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "child say: " << buffer << std::endl;
        }
    }
}
int main()
{
    int fds[2] = {0}; // read 0 , write 1
    int n = pipe(fds);
    if (n < 0)
    {
        std::cerr << "pipe error" << std::endl;
        return 1;
    }
    std::cout << "fds[0]: " << fds[0] << std::endl;
    std::cout << "fds[1]: " << fds[1] << std::endl;

    pid_t id = fork();
    if (id == 0)
    {
        close(fds[0]); // close read
        ChildWrite(fds[1]);
        close(fds[1]);
        exit(0);
    }

    close(fds[1]); // close parent write
    FatherRead(fds[0]);
    waitpid(id, nullptr, 0);
    return 0;
}