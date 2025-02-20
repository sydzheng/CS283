#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

extern void print_dragon();

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX);
    int rc = 0;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            exit(0);
        }

        if (strcasecmp(cmd_buff, "dragon") == 0) {
            print_dragon();
            continue;
        }

        if (cmd_buff[0] == '\0') {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }

        rc = build_cmd_buff(cmd_buff, &cmd);

        if (rc == ERR_CMD_OR_ARGS_TOO_BIG) {
            printf("error: 8 max args allowed\n");
        }

        if (rc == OK) {
            if (strcmp(cmd.argv[0], "cd") == 0) {
                if (cmd.argc > 2) {
                    printf("cd: too many arguments\n");
                    continue;
                } else {
                    exec_built_in_cmd(&cmd);
                }
            } else {
                exec_cmd(&cmd);
            }
        }

        free_cmd_buff(&cmd);
    }

    free(cmd_buff);
    return OK;
}

// Populates cmd_buff accordingly
// _cmd_buff contains the command line 
// argc contains the number of command line arguments
// argv contains an array of parsed command line arguments
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    const char *begPoint;
    const char *iteratePoint;
    int len;
    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);

    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    strcpy(cmd_buff->_cmd_buffer, cmd_line);

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }

    while (isspace((unsigned char)*cmd_line)) cmd_line++;

    begPoint = cmd_line;
    iteratePoint = cmd_line;

    while (isspace((unsigned char)*iteratePoint)) iteratePoint++;
    begPoint = iteratePoint;

    // If the command has arguments, this loop parses the arguments on spaces 
    // or quotes if applicable and uses two pointers to keep track of the beginning
    // and end of each argument
    while (*begPoint) {
        while (isspace((unsigned char)*begPoint)) begPoint++;
        if (*begPoint == '\0') break;

        if (*begPoint == '\"') {
            begPoint++;
            iteratePoint = begPoint;

            while (*iteratePoint && *iteratePoint != '\"') iteratePoint++;
        } else {
            iteratePoint = begPoint;

            while (*iteratePoint && !isspace((unsigned char)*iteratePoint)) iteratePoint++;           
        }

        len = iteratePoint - begPoint;
        if (len > 0) {
            cmd_buff->argv[cmd_buff->argc] = malloc(len + 1);
            if (cmd_buff->argv[cmd_buff->argc] == NULL) {
                return ERR_MEMORY;
            }

            strncpy(cmd_buff->argv[cmd_buff->argc], begPoint, len);
            cmd_buff->argv[cmd_buff->argc][len] = '\0';
            cmd_buff->argc++; 
        }

        if (cmd_buff->argc >= CMD_ARGV_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        if (*iteratePoint == '\"') iteratePoint++;
        begPoint = iteratePoint;
    }

    return OK;
}

// Executes built in commands, for now it only executes cd
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc < 2) {
        return BI_EXECUTED;
    } else {
        if (chdir(cmd->argv[1]) != 0) {
            perror("Error executing built in command");
        }
    }

    return BI_EXECUTED;
}

// Executes external commands using fork/exec pattern
int exec_cmd(cmd_buff_t *cmd) {
    int f_result, c_result;

    f_result = fork();
    if (f_result < 0) {
        perror("fork error");
        return ERR_EXEC_CMD;
    }

    if (f_result == 0) {
        if (execvp(cmd->argv[0], cmd->argv) < 0) {
            perror("fork error");
            exit(ERR_EXEC_CMD);
        }
    } else {
        wait(&c_result);
        WEXITSTATUS(c_result);
    }

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return OK;

    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
        cmd_buff->argv[i] = NULL;
    }
    
    free(cmd_buff->_cmd_buffer);
    cmd_buff->_cmd_buffer = 0;

    return OK;
}