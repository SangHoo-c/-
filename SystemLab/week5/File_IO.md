## week4

I/O redirection

- stdin (keyboard) : file descriptor 0
- stdout (screen) : file descriptor 1
- stderr : file descriptor 2
- 모든 파일은 redirected 가능. 



> **Redirection** 이란, 
>
> **프로그램이나 파일의 output** 을 capturing 하고, 
> **another 파일의 input 또는 program 의 command **로 전송하는 것. 



input redirection 



```bash
$ command < file-name 
$ > overwrite
$ >> append
$ 2>filename : catch stderr
$ &>filename : catch both stdout and stderr 
$ M>N : M (file descriptor), N (file name)
```



pipe 

- standard output of one program used as standard input ot next program. 

```bash
$ cat /etc/passwd | grep mail 

# tq.txt 에서 / command 가 들어간 줄만 / tt.txt 에 overwrite 한다.  
$ cat ./tq.txt | grep command  > tt.txt

```



tee

- stdin 에서 읽은 내용을 stdout 에도 쓴다. 
- *tee* 는 shell script 에서 root 권한으로 특정 파일을 쓰거나 append 할때 주로 활용한다. 

```bash
ls -l | tee file.txt | less 

less : show command page by page 
```



What is file? 

- Linux file is a **sequence** of m bytes. 
- All I/O devices are represented as **files**.
  => **/dev/sda1** 
  => **/dev/tty2**
- the kernel is stored as a **file**
  => **/boot/vmlinux-435.35-gneric** (kernel image)
  => **/dev/mem** (kernel memory image)
  => **/proc** (kernel data structure)



Directory 

- dir consists of an array of links	( ex. /dir/a.txt )
