
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len) {
    //TODO: #4:  Implement the setup buff as per the directions
    int buffInd = 0;
    int curr = 0;
    bool inWord = false;

    memset(buff, '.', len*sizeof(char));
    
    // The following loop traverses through the user string input
    // until a null terminator is reached and adds the string to the
    // buffer without duplicate white spaces. The statements check
    // whether each character in the string is a space and sets the 
    // value of inWord accordingly to keep track of where all the words
    // are in between the spaces. 
    while (user_str[curr] != '\0') {
        if (user_str[curr] == ' ') {
            if (inWord) {
                buff[buffInd++] = ' ';
                inWord = false;
            }
        } else {
            buff[buffInd++] = user_str[curr];
            inWord = true;
        }
        curr++;
    }
    
    // The previous loop will end the buffer with a space if the user string
    // ends with spaces. Since we don't want trailing spaces, this check ensures
    // that after the last word is added into buffer, the next character is a '.'.
    if (buff[buffInd-1] == ' ') {
        buff[buffInd-1] = '.';
    }

    // buffInd is also used to track the length of the string without the extra spaces.
    // This ensures that buffInd does not count a space at the end to be part of the string
    // length. 
    if (user_str[curr-1] == ' ') {
        buffInd--;
    }

    if (buffInd > len) {
        return -1;
    }

    // Error condition: User input string is empty
    if (user_str[0] == '\0') {
        return -2;
    }

    // If the user inputs spaces for a string, the buffInd will become negative, this
    // resets the buffInd to 0 since the length of the string is 0 without any words. 
    if (buffInd < 0) {
        buffInd = 0;
    }

    return buffInd; 
}

void print_buff(char *buff, int len) {
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    bool wordStart = false;
    int wc = 0;

    // This loop traverses the buffer and uses a bool to keep
    // track of when we are at the start of a word. The bool is 
    // initialized to false and changes to true when a character in the
    // buffer is not a space. When we are at the start of a word, the counter
    // that keeps track of the number of words is incremented. Whenever there
    // is a space, the bool changes back to false until another word is encountered.
    for (int i=0; i<str_len; i++) {
        if (!wordStart) {
            if (buff[i] == ' ') {
                continue;
            } else {
                wc++;
                wordStart = true;
            }
        } else {
            if (buff[i] == ' ') {
                wordStart = false;
            }
        }
    }
    return wc;
}

void reverse_string(char *buff, int len, int str_len) {
    int endInd = str_len-1;
    int begInd = 0;
    char tmpChar;

    // This loop traverses the buffer and uses two pointers (one initialized
    // at the beginning and one initialzied at the end of the characters to
    // be reversed). A tmpChar is also initialized to store the value of the
    // character that is about to be swapped. The characters at the beg and end
    // pointers are swapped and the pointers are advanced towards each other. When
    // the pointers intersect each other, the loop ends. 
    while (endInd > begInd) {
        tmpChar = buff[begInd];
        buff[begInd] = buff[endInd];
        buff[endInd] = tmpChar;
        begInd++;
        endInd--;
    }
}

void word_print(char *buff, int len, int str_len) {
    int wc = 0;
    int wlen = 0;
    bool wordStart = true;

    // This loop traverses the buffer and keeps track of when a word begins
    // through an initialized bool value. Since the buffer always starts with
    // a word, assuming that the user did not input an empty string, the wordStart
    // is initialized to true. If we are at the start of the word, word count is 
    // incremented and the wordStart is set to false to indicate that we are 
    // processing a word. If there is a space, wordStart is set back to true because
    // that means in the buffer there is another word. 
    for (int i=0; i<str_len; i++) {
        if (wordStart) {
            wc++;
            wordStart = false;
            printf("%d. ", wc);
        }

        if (buff[i] == ' ') {
            printf(" (%d)\n", wlen);
            wlen = 0;
            wordStart = true; 
        } else {
            printf("%c", buff[i]);
            wlen++;
        }
    }
    printf(" (%d)\n", wlen);
    printf("\n");
    printf("Number of words returned: %d\n", wc);
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    //      This is safe because it checks if there are less than 2 
    //      arguments OR if the argv[1] != '-'. It is fine that argv[1]
    //      does not exist because it would not pass the first check. 
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    //      The purpose is to check if the number of arguments is less
    //      than 3. This ensures that the user provides at least three arguments 
    //      (which should be: first being the program, the second being the option flag, and 
    //      the third being the string). 
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*)malloc(BUFFER_SZ*sizeof(char));

    if (buff == NULL) {
        printf("Error allocating memory.");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            reverse_string(buff, BUFFER_SZ, user_str_len);
            printf("Reversed String: %.*s\n", user_str_len, buff);
            break;
        case 'w':
            printf("Word Print\n----------\n");
            word_print(buff, BUFFER_SZ, user_str_len);
        
            break;
        case 'x':
            if (argc < 5) {
                printf("-x usage: ./stringfun -x \"string\" [word to replace] [word to replace with]\n");
                exit(1);
            }
            printf("Not Implemented!\n");
            exit(0);

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          It self-contains the buffer and its size to a small unit. Since there's
//          only a certain amount of memory allocated towards the buffer, it's 
//          good to pass the size of the pointer as well. 
