#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>



int main(int argc, char* argv[]){
    char *cmd= NULL;
    size_t size = 0;
    int cnt = 0;


    while(1){
        getline(&cmd, &size, stdin);
        cmd[strlen(cmd)-1] = '\0';
        // printf("loop : %d \n", cnt++);
        int child_status;

        if(strcmp("quit", cmd) == 0){
            printf("=== QUIT ===\n");
            break;
        }

        // 자식 프로세스
        if(fork() == 0){
            // printf("hi this is child \n");
            // printf("(%lu) %s\n", size, cmd);

            char * args[] = {};
            int i = 0;
            char *ptr= strtok(cmd, " ");
            while (ptr!= NULL) {
                args[i++] = ptr;
                ptr= strtok(NULL, " ");
            }
            args[i] = NULL;

            char path[100];
            sprintf(path, "/bin/%s", args[0]);
            printf("%s \n", path);

            execv(path, args);  // running new program.
            // printf("BYe %d \n", getpid());
            exit(0);    // child 프로세스 종료

        }else{ // 부모 프로세스
            // printf("hi this is parent \n");
            pid_t wpid = wait(&child_status);    // wait 는 부모가 자식을 기다릴때 쓰는것.
            if(WIFEXITED(child_status))
                continue;
                // printf("Child %d terminated with exit status %d\n",wpid, WEXITSTATUS(child_status));
            else
                printf("Child %d terminate abnormally\n", wpid);
        }

    }
    // printf("parent has terminated \n");
    // printf("BYe %d \n", getpid());
    exit(0);    //  부모 프로세스 종료
}
