what is Process ? 

> Processor : H/W, CPU
>
> Program : S/W, a sequence of byte, a sequence of machine instruction stored in a file
>
> Process : S/W running on H/W, an instance of a program in execution, resident in **main memory**, 
> => disk 에 있는 program 이 cpu 에 의해 interpret 된다면, process 라고 한다. 

<br/>

**two key abstraction** 

-  Logical control flow : each program 은 자신 혼자서 CPU 를 사용하는 것처럼 생각한다. 
- Private address space : each program 은 자신 혼자서 main memory 를 사용하는 것 처럼 생각한다. 

How are these illusions provides? 

- Time sharing : OS interleaves Process executions
- Virtual address : OS manages logical address spaces 

<br/>

**Time sharing** 

- each process has its own logical control flow 

<br/>

**PCB** ( Process Control Block, metadata about Process )

- a data structure used by OS to store all the info for process management. 
- process id, **process state,** program counter, registers, ,,,



<br/>

**Context Switiching** 

- Control is passed from one process to another via context switch. 
- kernel code 에서 이루어지고, PCB 를 참고하여 Process 간 switch. 

d

- data : used to store global variables 
- code : machine instructions 
- heap : malloc, new ... allocated memory space 
- stack : local variables, function
- 프로그램이 0 ~ 0xffffffff 의 메모리 전체를 사용하는 것처럼 인식함. 

<br/>

<br/>

**Process State Transition** 

- new
- ready
- running 
- waiting 
- terminated 

<br/>

<br/>

**Creating a new Process** 

```C
pid_t fork(void)
// create a new process (childe process) that is identical to the calling process (parent process). 
// copy entire virtual address. 
// identical virtual address. 
// Fork is called once but returns twice. 

// both printf() are executed. 
if (fork() == 0){
  printf("hello from child");
}else{
  printf("hello from parent");
}  
```

<br/>

<br/>

**System call**

- fork 

```C
// parent and child both run the ** same code **. 
// distinguish parent from child using a return value from fork().
// start with same state, but each has private copy.
// child & parent 각각의 local varible 을 소유한다. 

void fork1(){
  int x = 1;
  pid_t pid = fork();
  if (pid == 0){
    printf("child has x = %d ", ++x);
  }else{
    printf("parent has x = %d ", --x);
  }
  printf("bye from process %d with x = %d ", getpid(), x);
}
```

```C
// both parent and child can keep forking. 

void fork2(){
  printf("L0 ");	// L0 1번 출력 
  fork();
  printf("L1 ");	// L1 2번 출력 
  fork();
  printf("bye ! "); // bye 4번 출력 
}
```

<br/>

- exit 

```C
// void exit (int status)
// normally returns with status 0 

void cleanup(void){
  printf("cleaning up ");
}

void fork_atexit(){
  atexit(cleanup);	// cleanup function 을 callback function 으로 등록
  fork();
  exit(0);	   
}
```

<br/>

- wait 

```C
pid_t wait(int *status);
// suspends curent process until one of its children terminates. 
// parent 프로세스가 blocked 되어 child process 가 terminate 되기를 기다린다. 
// return value is the pid of the child process that terminated. 

pid_t waitpid(pid_t pid, int *status, int options);
// can wait for specific process.  
```

```C
// ex1. 
void fork_wait(){
  int child_status;
  
  if(fork() == 0){
    printf("hello from Child. ");
  }else{
    printf("hello from parent. ");
    wait(&child_status);		// child process 가 exit 되기 전까지 기다린다. 
    printf("child has terminated. ");
  }
  printf("bye. ");
  exit(0);
}


// child 의 eixt code 는 PCB 에 저장되어 있는다.
```

```C
// ex2.
// multiple child process 
void fork_wait2(){
  pid_t pid[N];
  int i, child_status;
  for (i = 0; i < N; i ++)
  	if ((pid[i] = fork()) == 0) exit(100 + i);	// child 만들고 바로 exit. 
  for (i = 0; i < N; i ++){
    pid_t wpid = wait(&child_status);
  	if (WIFEXITED(child_status))
      printf("child %d terminated with exit status %d. ",
            wpid, WEXITSTATUS(child_status));
    else
      printf("child %d terminate abnormally. ", wpid);      
  }
}
```

<br/>

<br/>

**Zombies**

- when a process terminates, still consumes system resoruces. 
- Reaping 
- parent dies without reaping a child, then the child will be reaped by **init process**. 

```C
// $ ./a.out 7 &
// & : bash 에서 background 로 돌아가게 해준다. 
void fork7()  {
  if (fork() == 0){
    printf("child pid = %d ", getpid());
    exit(0);
  }else{
    printf("parent pid = %d ", getpid());
    while(1);
  }
}
// ps shows child processes as "defunct". => zombie process. 

// chid 는 먼저 exit 되었지만, parent 에서 wait 으로 child 를 불러올 수도 있기 때문에, 
// OS 에서 child 의 meta data 를 지우지 못한다. 
// child 프로세스의 PCB 는 main memory 에 그대로 남아있다. 

// killing parent allows child to be reaped. 
```

<br/>

- execl 

```C
/*
parent 와 child 가 identical code segment 를 가지고 있었다. 
ex. a.out 에서 실행되는 parent / child. 
=> run another program 을 돌리는 방법은 없을까???
=> execl, execv


[execl, execv] system call 을 호출하면
1. code 부분이 다른 프로그램으로 replaced 된다. 
2. stack, heap cleaned-up 된다. 

*/

// ex1. 
main(){
  if(fork() == 0){
    execl("/bin/ls", "ls", "/", 0);
  }
  wait(NULL);
  printf("completed");
  exit();
}

// ex2. 
main(){
  char *args[] = {"ls", "/", NULL};
  if(fork() == 0){
    execv("/bin/ls", args);
  }
  wait(NULL);
  printf("completed");
  exit();
}
```
