#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define _GNU_SOURCE
#include <getopt.h>

#define TRUE    1
#define FALSE   0

typedef struct _KEYLIST {
    char key;
    char value;
}KeyList;

typedef void (*CipherFunc)(char[], int);
typedef int (*FileCipherFunc)(char[], CipherFunc);

const KeyList list[] = {
    {'!','a'}, {'e','b'}, {'s','c'}, {'`','d'}, {'c','e'}, {'1','f'}, {'p','g'}, {'i','h'}, {'n','i'}, {'g','j'}, {'9','k'}, {'m','l'}, {'q','m'}, 
    {'w','n'}, {'x','o'}, {'&','p'}, {'t','q'}, {'%','r'}, {'5','s'}, {'y','t'}, {'v','u'}, {'u','v'}, {'b','w'}, {'o','x'}, {'8','y'}, {'j','z'},

    {'#','0'}, {'h','1'}, {'@','2'}, {'=','3'}, {'$','4'}, {'k','5'}, {'^','6'}, {'\\','7'}, {'d','8'}, {'l','9'},

    {'a','`'}, {'r','!'}, {'2','@'}, {' ','#'}, {'4','$'}, {'f','%'}, {'6','^'}, {'z','&'}, {'+','*'}, {'\'','('}, {'-',')'}, {')','-'}, 
    {'"','_'}, {'*','+'}, {'3','='}, {'.','['}, {'?','{'}, {'|',']'}, {']','}'}, {'7','\\'}, {'}','|'}, {'<',';'}, {'>',':'}, {'(','\''}, 
    {'_','"'}, {'/',','}, {';','<'}, {'[','.'}, {':','>'}, {',','/'}, {'{','?'}, {'0',' '}
};

int strlwr(char str[], int len) {
    for (int i = 0; i < len; i++) {
        if(str[i] >= 'A' && str[i] <= 'Z')
            *(str + i) = _tolower(*(str + i));
    }

    return 0;
}

void encode(char PlainText[], int len) {
    for (int n = 0; n <= len; n++) {
        for (int s = 0; s <= sizeof(list) / sizeof(list[0]); s++) {
            if (PlainText[n] == list[s].value) {
                PlainText[n] = list[s].key;
                break;
            }
        }
    }
}

void decode(char CipherText[], int len) {
    for (int n = 0; n <= len; n++) {
        for (int s = 0; s <= sizeof(list) / sizeof(list[0]); s++) {
            if (CipherText[n] == list[s].key) {
                CipherText[n] = list[s].value;
                break;
            }
        }
    }
}

#define LINE_LEN 1040
int FileCipher(char original_path[], CipherFunc cipher) {
    FILE* fp_read = NULL;
    FILE* fp_write = NULL;

    char line[LINE_LEN] = {0, };
    char cipher_path[LINE_LEN] = {0, };
    char* extension = strrchr(original_path,'.');

    fp_read = fopen(original_path, "rt");
    if (fp_read == NULL){
        printf("fopen error\n");
        return -1;
    }

    if(strcmp(extension, ".legrand")){
        encode(extension, strlen(extension));
        sprintf(line, "%s\n", extension);
        *extension = '\0';
        sprintf(cipher_path, "%s.legrand", original_path);
    }
    else{
        fgets(extension, LINE_LEN, fp_read);
        decode(extension, strlen(extension));
        original_path[strlen(original_path) - 1] = '\0';
        sprintf(cipher_path, "%s", original_path);
    }

    fp_write = fopen(cipher_path,"wt");
    if (fp_write == NULL){
        printf("fopen error\n");
        return -1;
    }
    fputs(line,fp_write);

    while(!feof(fp_read)){
        fgets(line, LINE_LEN, fp_read);
        strlwr(line, strlen(line));
        cipher(line, strlen(line));
        fputs(line, fp_write);
    }

    fclose(fp_read);
    fclose(fp_write);
    return 0;
}

int legrand(char text[], CipherFunc cipher, FileCipherFunc file){
    if(file == NULL){
        cipher(text, strlen(text));
        printf("%s\n", text);

    }else{
        file(text,cipher);
    }
        return 0;
}

void help(char* argv){
    printf(
           "Usage: Legrand [options] [PlainText] or [CipherText]\n\n"
           "-e, --encoding plain text to cipher text\n"
           "-d, --decoding  cipher text to plain text\n"
           "-h --help show this massage\n"
           "-f --file add option, -fe or -fd\n"
           "this program is use the pipe\n"
           "ex) echo \"PlainText\" | %s\n",argv);
}

#define PLAIN_TEXT_LEN 1040
int main(int argc, char* argv[]) {
    int opt = 0;
    FileCipherFunc file_cipher_func = NULL;

    char plain_text[PLAIN_TEXT_LEN]={0, };

    struct option longopts[] = {
	{"encoding", 1, NULL, 'e'},
	{"decoding", 1, NULL, 'd'},
	{"help", 0, NULL, 'h'},
    {"file", 0, NULL, 'f'},
	{0,0,0,0}};

    if(argc == 1){
        fgets(plain_text, PLAIN_TEXT_LEN,stdin);
        encode(plain_text, strlen(plain_text));
        printf("%s\n", plain_text);
        return 0;
    }

    while((opt = getopt_long(argc, argv, ":hfe:d:", longopts, NULL)) != -1) {
        switch(opt) {
        case 'e':
            legrand(optarg, encode, file_cipher_func);
            return 0;

        case 'd':
            legrand(optarg, decode, file_cipher_func);
            return 0;

        case 'h':
            help(argv[0]);
            return 0;

        case 'f':
            file_cipher_func = FileCipher;
            break;

        case '?':
            help(argv[0]);
            return 0;
        }
    }
    help(argv[0]);
    return 0;
}