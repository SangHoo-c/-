/*
21.10.13

ref). 
simple example : https://reakwon.tistory.com/46
how to siganaling myself : https://www.gnu.org/software/libc/manual/html_node/Signaling-Yourself.html
raise : https://www.techonthenet.com/c_language/standard_library_functions/signal_h/raise.php
*/


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int N;
void signalHandler(int sig){
    if(sig==SIGINT){
        N -= 1;
    }
    else if(sig == SIGQUIT){
        printf("SIGQUIT caught, will be ignored\n");
    }
    else if(sig == SIGTSTP){
        printf("SIGTSTP caught. This process will stop. \n");
        signal (SIGTSTP, SIG_DFL);      /// 세팅하기. signal 함수 본연의 모습 대로 처리.
        raise (SIGTSTP);                /// 자신의 signal 실행.
    }
    else if(sig == SIGCONT){
        printf("SIGCONT caught\n");
    }

}
int main(int argc, char * argv[]){
    char *c = argv[1];
    N = strtol(argv[1], &c, 10);

    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGTSTP, signalHandler);
    signal(SIGCONT, signalHandler);
    while(N > 0);
    exit(0);
}
