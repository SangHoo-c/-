#include <stdio.h>

#include <unistd.h> // open, read, lseek
#include <fcntl.h>  //  option
#include <errno.h>  // variable errno
#include <sys/types.h>  // lseek
#include <stdlib.h>

#define BSIZE 1024
#define ESIZE 16
#define S_SIZE 45


int _strlen(char*s){
    int i=0;
    while(s[i]) ++i;
    return i;
}

void _strcopy(char*to, char*from){
    while((*to++ = *from++));
}

void _memset(char * s, int fill){
    for (int i = 0; i < S_SIZE - 1; ++i) {
        if (s[i] == '\0') break;
        s[i] = fill;
    }
}


int _strcmp(char *s1, char *s2){
    /// 두문자열이 같으면 0 을 return
    char c1, c2;
    while (1){
        c1 = *s1++;
        c2 = *s2++;
        if (c1 != c2)
            return 1;
        if (!c1 || !c2)     /// 더 이상 비교할게 없으면, break 0 을 return
            break;
    }
    return 0;
}

char * _substr(int s, int e, char * str){
    char *new = (char *) malloc(e-s+2);
    _strcopy(new, str + s);
    new[e-s+1] = '\0';      /// 끝자리 NULL
    return new;
}

char * _strtok(char * str, const char * delim){
    static char * p_cur;
    char * p_delim;

    if( str != NULL) p_cur = str;
    else str = p_cur;

    if (*p_cur == NULL) return NULL;

    while (*p_cur){
        p_delim = (char*) delim;
        while (*p_delim){
            if (*p_cur == *p_delim){    /// delim 과 같다면, 자른다.
                *p_cur = NULL;
                ++p_cur;
                return str;             /// 자른 결과 반환
            }
            ++p_delim;      /// delim 을 끝까지 돌려본다.
        }
        ++p_cur;        /// delim 과 같지 않다면, 다음으로 넘어간다.
    }
    return str;     /// 더 이상 자를 수 없다면, str 을 반환
}



void _split(char * buf);
void func1(char * user_buf);
void func2(char * user_buf);
void func3(char * user_buf);
void func4(char * user_buf);

typedef struct{
    char str[45];
    int l_num;
    int c_num;
}Word;

Word word[BSIZE];
int widx = 0;

int main(int argc, char * argv[]){
    int fd = 0;
    char buf[BSIZE] = {0, };
    char error[ESIZE] = "errno : 0\n";
    char ins[32] = "Enter Keyword(s): ";
    char user_buf[BSIZE] = {0, };
    char * fn = argv[1];

    if((fd = open(fn, O_RDWR)) == -1){
        error[8] = errno + '0';
        write(STDOUT_FILENO, error, ESIZE);
        close(fd);
    }else{
        read(fd, buf, BSIZE);
        write(STDOUT_FILENO, ins, 32);
        read(STDOUT_FILENO, user_buf, BSIZE);


        /// input buffer 단어별 쪼개서 저장
        _split(buf);
        for (int i = 0; i < BSIZE; ++i) {
            if (!word[i].c_num) break;
            printf("%d %d %s \n", word[i].l_num, word[i].c_num, word[i].str);
        }


        /// user buffer 체크
        int space_cnt = 0;
        int aster_ck = 0;
        int quate_ck = 0;

        for (int i = 0; i < BSIZE; ++i) {
            if(user_buf[i] == '\n') break;
            else if(user_buf[i] == ' ') space_cnt ++;
            else if(user_buf[i] == '*') aster_ck = 1;
            else if(user_buf[i] =='"') quate_ck += 1;
        }

        if(space_cnt == 0) printf("func1 \n");
        else if(aster_ck == 1) printf("func4 \n");
        else if(quate_ck == 2) printf("func3 \n");
        else if(space_cnt > 0 && quate_ck < 2) printf("func2 \n");
        else printf("wrong input \n");


        /// user_buf 입력값 처리
        int str_len = 0;
        char * c_str;

        for (int i = 0; i < BSIZE; ++i) {
            if(user_buf[i] == '\n') break;
            str_len += 1;
        }

        c_str = (char *)malloc(str_len+1);
        for (int i = 0; i < str_len; ++i) {
            c_str[i] = user_buf[i];
        }

//        func1(user_buf);
//        func2(user_buf);
//        func3(user_buf);
        func4(user_buf);

        close(fd);
        free(c_str);
    }

    return 0;
}

void _split(char * buf){
    char tmp[S_SIZE]= "";
    int idx = 0;
    int line_num = 1;
    int char_num = 1;
    for (int i = 0; i < BSIZE; ++i) {
        if (buf[i] == ' '){
            idx = 0;
            _strcopy(word[widx].str, tmp);
            word[widx].l_num = line_num;
            word[widx].c_num = char_num;
            char_num++;
            widx++;
            _memset(tmp, 0);
        }
        else if(buf[i] == '\n'){
            _strcopy(word[widx].str, tmp);
            word[widx].l_num = line_num;
            word[widx].c_num = char_num;
            widx++;
            _memset(tmp, 0);
            line_num ++;
            char_num = 1;
            idx = 0;
        }
        else if(buf[i] == '\0'){
            _strcopy(word[widx].str, tmp);
            word[widx].l_num = line_num;
            word[widx].c_num = char_num;
            _memset(tmp, 0);
            break;
        }
        else tmp[idx++] = buf[i];
    }
}


void func1(char * user_buf){
    int str_len = 0;
    char * c_str;

    // 입력 받은 문자열 처리
    for (int i = 0; i < BSIZE; ++i) {
        if(user_buf[i] == '\n') break;
        str_len += 1;
    }

    c_str = (char *)malloc(str_len+1);
    for (int i = 0; i < str_len; ++i) {
        c_str[i] = user_buf[i];
    }

    // 텍스트 전체 순회
    for (int i = 0; i < BSIZE; ++i) {
        if (!word[i].c_num) break;
        if (_strcmp(c_str, word[i].str) == 0)
            printf("%d %d %s \n", word[i].l_num, word[i].c_num, word[i].str);
    }
    free(c_str);
}

void func2(char * user_buf){
    char c_str[45] = {0};
    int idx = 0;
    int check[BSIZE] = {0, };
    int word_cnt = 0;
    int prev_lnum = 0;      /// 중복제거를 위한 flag
    for (int i = 0; i < BSIZE; ++i) {
        if (user_buf[i] == '\n'){
            if (_strlen(c_str) > 0) word_cnt ++;
            for (int i = 0; i < BSIZE; ++i) {
                if (!word[i].c_num) break;
                if (_strcmp(c_str, word[i].str) == 0){
                    if (word[i].l_num != prev_lnum){
                        check[word[i].l_num] ++;
                        prev_lnum = word[i].l_num;
                    }
                }
            }
            break;
        }
        else if(user_buf[i] == ' '){
            if (_strlen(c_str) > 0) word_cnt ++;        /// 공백제거 counting
            for (int i = 0; i < BSIZE; ++i) {
                if (!word[i].c_num) break;              /// word 안에 아무것도 없으면 break
                if (_strcmp(c_str, word[i].str) == 0){
                    if (word[i].l_num != prev_lnum){
                        check[word[i].l_num] ++;
                        prev_lnum = word[i].l_num;
                    }
                }
            }
            _memset(c_str, 0);
            idx = 0;
        }
        else{
            c_str[idx++] = user_buf[i];
        }
    }
//    for (int i = 0; i < 10; ++i) {
//        printf("# %d %d \n", i, check[i]);
//    }
//    printf("\n word-count : %d \n", word_cnt);
    for (int i = 0; i < BSIZE; ++i) {
        if(check[i] == word_cnt)
            printf("%d \n", i);
    }
}

void func3(char * user_buf){
    int str_len = 0;
    char * c_str;
    char * n_str;

    // 입력 받은 문자열 처리
    for (int i = 0; i < BSIZE; ++i) {
        if(user_buf[i] == '\n') break;
        str_len += 1;
    }

    c_str = (char *)malloc(str_len+1);
    for (int i = 0; i < str_len; ++i) {
        c_str[i] = user_buf[i];
    }

    /// 문자열 split 한후, tmp_buf 에 저장
    n_str = _substr(1, str_len-2, c_str);
    char * tmp = _strtok(n_str, " ");
    char * tmp_buf[BSIZE] = {};
    int tmp_len = 0;
    while (tmp != NULL){
        tmp_buf[tmp_len++] = tmp;
        tmp = _strtok(NULL, " ");
    }


    for (int i = 0; i < tmp_len; ++i) {
        printf("@@ %s\n", tmp_buf[i]);
    }

    for (int i = 0; i < BSIZE; ++i) {
        if (!word[i].c_num) break;
        int iidx = i;
        int tidx = 0;
        while (1){
            if (!tmp_buf[tidx]) break;
            else if (_strcmp(tmp_buf[tidx], word[iidx].str) == 0){
                tidx ++;
                iidx ++;
            }
            else break;
        }
        if (tidx == tmp_len)
            printf("%d : %d \n", word[i].l_num, word[i].c_num);
    }
    free(c_str);
    free(n_str);
}

void func4(char * user_buf){
    int str_len = 0;
    char * c_str;

    // 입력 받은 문자열 처리
    for (int i = 0; i < BSIZE; ++i) {
        if(user_buf[i] == '\n') break;
        str_len += 1;
    }

    c_str = (char *)malloc(str_len+1);
    for (int i = 0; i < str_len; ++i) {
        c_str[i] = user_buf[i];
    }

    /// 문자열 split 한후, tmp_buf 에 저장
    char * tmp = _strtok(c_str, " ");
    char * tmp_buf[BSIZE] = {};
    int tmp_len = 0;
    while (tmp != NULL){
        tmp_buf[tmp_len++] = tmp;
        tmp = _strtok(NULL, " ");
    }

    for (int i = 0; i < tmp_len; ++i) {
        printf("@@ %s\n", tmp_buf[i]);
    }

    for (int i = 0; i < BSIZE; ++i) {
        if (!word[i].c_num) break;
        int iidx = i;
        int tidx = 0;
        while (1){
            if (!tmp_buf[tidx]) break;
            else if (tidx == 1){
                tidx ++;
                iidx ++;
            }
            else if (_strcmp(tmp_buf[tidx], word[iidx].str) == 0){
                tidx ++;
                iidx ++;
            }
            else break;
        }
        if (tidx == tmp_len)
            printf("%d : %d \n", word[i].l_num, word[i].c_num);
    }
    free(c_str);
}