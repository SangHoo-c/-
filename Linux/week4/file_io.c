/*
 * 1. 파일 포인터 선언
 * 2. 선언된 파일 포인터를 이용해 file open
 * 3. open 된 파일에서 내용 읽기
 * 4. 모든 작업을 마치면 open 된 파일을 닫아야 한다.
 * */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int recur(char * pos, char * key){

    DIR * directory = NULL;
    struct dirent * file = NULL;
    struct stat buf;    // file metadata 저장
    char filename[1024];
    char text[256];
    int count = 0;


    // create dir stream
    if (!(directory = opendir(pos))) {
        perror("Failed to open directory");
        return -1;
    }

    // 디렉토리의 파일/디렉토리명을 순서대로 한개씩 읽는다.
    // directory stream 의 end 까지 접근
    while ((file = readdir(directory)) != NULL){

        // "." 과 ".." file 은 continue
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
            continue;
        }

        // filename 에 [dir]/[file-name] 으로 저장하기
        sprintf(filename, "%s/%s", pos, file->d_name);

        // fills file metadata in buf pointed by path.
        if(stat(filename, &buf) == -1){
            continue;
        }

        // 해당 파일이 디렉토리라면, 재귀호출
        if(S_ISDIR(buf.st_mode)){
            count += recur(filename, key);
        }

        // regular 파일 접근
        else if (S_ISREG(buf.st_mode)){

            // file 오픈
            FILE *fp = fopen(filename, "r");
            if (fp == NULL){
                printf("cannot open file. file-name : %s \n", filename);
                return -1;
            }

            // 한줄씩 읽기 구현
            while (fgets(text, sizeof(text), fp) != NULL){
                char * ptr = strstr(text, key);
                while (ptr != NULL){
                    count ++;
                    ptr = strstr(ptr + 1, key);
                }
            }

            if(fclose(fp) != 0){
                printf("file %s not closed. \n", filename);
                return -1;
            }
        }
    }

    // close dir stream
    closedir(directory);
    return count;
}


int main(int argc, char **argv) {
    // argv[1] 아래에 있는 모든 파일에서 argv[2] 가 포함된 count 합니다.
    printf("%d \n", recur(argv[1], argv[2]));
    return 0;
}


/*
 * 참고한 사이트는 이와 같다.
 * opendir 의 활용 : https://www.it-note.kr/24
 * fgets 사용예시 : https://yk209.tistory.com/entry
 *
 * */
