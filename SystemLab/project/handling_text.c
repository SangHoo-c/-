#include <unistd.h> // open, read, lseek
#include <fcntl.h>  //  option
#include <errno.h>  // variable errno
#include <sys/types.h>  // lseek
#include <stdlib.h>

#define BSIZE 1024
#define ESIZE 16
#define S_SIZE 45

int _strlen(char*s);
void _strcopy(char*to, char*from);
void _memset(char * s, int fill);
int _strcmp(char *s1, char *s2);
char * _substr(int s, int e, char * str);
char * _strtok(char * str, const char * delim);
void _split(char * buf);

void func1(char ** user_inputs);
void func2(char ** user_inputs);
void func3(char * c_str);
void func4(char * user_buf);


typedef struct{
    char str[45];
    int l_num;
    int c_num;
}Word;

Word word[BSIZE];   /// input buf 에 모든 단어 저장
int widx = 0;   /// _split 에서 사용

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

        /// user_buf 입력값 처리
        int str_len = 1;
        char * c_str;

        for (int i = 0; i < BSIZE; ++i) {
            if(user_buf[i] == '\n') break;
            str_len += 1;
        }

        c_str = (char *)malloc(str_len+1);
        for (int i = 0; i < str_len; ++i) {
            c_str[i] = user_buf[i];
        }
        c_str[str_len-1] = '\0';    /// 개행문자 null 로 대체

        /// user buffer 체크
        /// user_buf 를 " " 기준으로 split 한후, ui 에 저장
        /// "Lorem Ipsum" ==> user_inputs = {"Lorem", "Ipsum"}

        /// func1, func2, func3 ,func4 에 user_buf 말고 user_inputs 을 넣어주는게 낫지 않을까???
        char * ui = _strtok(user_buf, " ");
        char * user_inputs[BSIZE] = {};
        int tmp_len = 0;
        while (ui != NULL){
            user_inputs[tmp_len++] = ui;
            ui = _strtok(NULL, " ");
        }

        int aster_ck = 0;
        int quata_ck = 0;
        int word_count = 0;

        /// func 1 & func 2
        for (int i = 0; i < BSIZE; ++i) {
            if (!user_inputs[i]) break;
            if (_strlen(user_inputs[i]) >= 1) word_count ++; /// 공백을 제거한 단어의 갯수 세기.
        }
        /// func 3
        if (user_inputs[0][0] == '"'){
            int wlen = _strlen(user_inputs[word_count-1]);
            quata_ck++;
            if (user_inputs[word_count-1][wlen-1] == '"')
                quata_ck++;
        }

        /// func 4
        if (word_count == 3)
            if (_strcmp(user_inputs[1], "*") == 0)
                aster_ck = 1;

        if (quata_ck == 2)
            func3(c_str);
        else if (aster_ck) func4(c_str);
        else if (word_count == 1) func1(user_inputs);
        else if (word_count > 1){
            if (quata_ck == 1){
                if (quata_ck == 1) write(STDOUT_FILENO, error, ESIZE);
            }
            else{
                func2(user_inputs);
            }
        }
        else write(STDOUT_FILENO, error, ESIZE);

        close(fd);
        free(c_str);
    }
    return 0;
}


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

char * _strtok(char * str, const char * delim){ /// 개행문자 또는 delim 을 만나면 자른다.
    static char * p_cur;
    if( str != NULL) p_cur = str;
    else str = p_cur;

    if (*p_cur == NULL) return NULL;

    while (*p_cur){
        if (*p_cur == *delim || *p_cur == '\n'){    /// delim 과 같다면, 자른다.
            *p_cur = NULL;
            ++p_cur;
            return str;             /// 자른 결과 반환
        }
        ++p_cur;        /// delim 과 같지 않다면, 다음으로 넘어간다.
    }
    return str;     /// 더 이상 자를 수 없다면, str 을 반환
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

int _pow(int n, int p){
    int res = 1;
    for (int i = 0; i < p; ++i) {
        res *= n;
    }
    return res;
}

int _itoa(int n, char * s, int idx){
    /// 자릿수 계산
    int tmp = n;
    int pos =1;
    while (1){
        if ((tmp /= 10) == 0)
            break;
        pos ++;
    }

    /// 1462 => "1462"
    char c;
    int div;
    for (int i = pos - 1; i >= 0; --i) {
        div = _pow(10, i);
        c = '0' + (n / div);
        s[idx++] = c;
        n %= div;
    }
    s[idx] = '\0';  /// 문자열 끝 널 문자.
    return idx;
}

void func1(char ** user_inputs){
    /// 텍스트 전체 순회
    for (int i = 0; i < BSIZE; ++i) {
        if (!word[i].c_num) break;
        if (_strcmp(user_inputs[0], word[i].str) == 0){
            /// printf("%d %d %s \n", word[i].l_num, word[i].c_num, word[i].str);
            char ar[45] = {};
            int gix = 0;
            gix = _itoa(word[i].l_num, ar, 0);
            ar[gix++] = ':';
            gix = _itoa(word[i].c_num, ar, gix);
            ar[gix++] = '\n';
            ar[gix] = '\0';
            write(STDOUT_FILENO, ar, _strlen(ar));
        }
    }
}

void func2(char ** user_inputs){
    int prev_lnum = 0;
    int check[BSIZE] = {0, };
    int word_cnt = 0;   /// user input 갯수
    for (int i = 0; i < BSIZE; ++i) {
        if (!user_inputs[i]) break;
        word_cnt ++;
    }

    /// user input 에 있는 문자 한개씩 참조하여
    /// word 배열 확인
    for (int i = 0; i < word_cnt; ++i) {
        for (int j = 0; j < BSIZE; ++j) {
            if (!word[j].c_num) break;
            if (_strcmp(user_inputs[i], word[j].str) == 0){
                if (word[j].l_num != prev_lnum){
                    check[word[j].l_num] ++;
                    prev_lnum = word[j].l_num;
                }
            }
        }
    }

    for (int i = 0; i < BSIZE; ++i) {
        if(check[i] == word_cnt){
            /// printf("%d \n", i);
            char ar[45] = {};
            int gix = 0;
            gix = _itoa(i, ar, 0);
            ar[gix++] = '\n';
            ar[gix] = '\0';
            write(STDOUT_FILENO, ar, _strlen(ar));
        }
    }
}

void func3(char * c_str){
    int str_len = 0;
    char * n_str;

    str_len = _strlen(c_str);
    /// 인용표 제거. "Lorem Ipsum" => Lorem Ipsum
    n_str = _substr(1, str_len-2, c_str);

    /// delim 을 기준으로 쪼개서 tmp_buf 에 저장
    /// char ** tmp_buf = { "Lorem", "Ipsum"}
    char * tmp = _strtok(n_str, " ");
    char * tmp_buf[BSIZE] = {};
    int tmp_len = 0;
    while (tmp != NULL){
        tmp_buf[tmp_len++] = tmp;
        tmp = _strtok(NULL, " ");
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
        if (tidx == tmp_len){
            /// printf("%d : %d \n", word[i].l_num, word[i].c_num);
            char ar[45] = {};
            int gix = 0;
            gix = _itoa(word[i].l_num, ar, 0);
            ar[gix++] = ':';
            gix = _itoa(word[i].c_num, ar, gix);
            ar[gix++] = '\n';
            ar[gix] = '\0';
            write(STDOUT_FILENO, ar, _strlen(ar));
        }
    }
    free(n_str);
}

void func4(char * c_str){
    /// 문자열 split 한후, tmp_buf 에 저장
    char * tmp = _strtok(c_str, " ");
    char * tmp_buf[BSIZE] = {};
    int tmp_len = 0;
    while (tmp != NULL){
        tmp_buf[tmp_len++] = tmp;
        tmp = _strtok(NULL, " ");
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
        if (tidx == tmp_len){
            /// printf("%d : %d \n", word[i].l_num, word[i].c_num);
            char ar[45] = {};
            int gix = 0;
            gix = _itoa(word[i].l_num, ar, 0);
            ar[gix++] = ':';
            gix = _itoa(word[i].c_num, ar, gix);
            ar[gix++] = '\n';
            ar[gix] = '\0';
            write(STDOUT_FILENO, ar, _strlen(ar));
        }
    }
}

