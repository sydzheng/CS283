#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
    if (!cmd_buff) return ERR_MEMORY;

    int rc = 0;
    command_list_t *cmd_list = malloc(sizeof(command_list_t));
    if (!cmd_list) {
        free(cmd_buff);
        return ERR_MEMORY;
    }
    memset(cmd_list, 0, sizeof(command_list_t));

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            printf("exiting...\n");
            free(cmd_buff);
            free(cmd_list);
            exit(0);
        }

        if (cmd_buff[0] == '\0') {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }

        rc = build_cmd_list(cmd_buff, cmd_list);

        if (rc == ERR_CMD_OR_ARGS_TOO_BIG) {
            printf("error: argc 9 max\n");
            free_cmd_list(cmd_list);
            continue;
        }  

        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            free_cmd_list(cmd_list);
            continue;
        }  
        
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            free_cmd_list(cmd_list);
            continue;
        }

        if (rc == OK) {
            if (strcmp(cmd_list->commands[0].argv[0], "cd") == 0) {
                if (cmd_list->commands[0].argc > 2) {
                    printf("cd: too many arguments\n");
                    free_cmd_list(cmd_list);
                    continue;
                } else {
                    exec_built_in_cmd(&cmd_list->commands[0]);
                }   
            } else {
                if (cmd_list->num == 1) {
                    exec_cmd(&cmd_list->commands[0]);
                } else {
                    execute_pipeline(cmd_list);
                }
            }

            free_cmd_list(cmd_list);
        }
    }

    free(cmd_list);
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
    int len = 0;
    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX*sizeof(*cmd_line));

    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX);
    cmd_buff->_cmd_buffer[SH_CMD_MAX-1] = '\0';

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

        if (cmd_buff->argc > CMD_ARGV_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        if (*iteratePoint == '\"') iteratePoint++;
        begPoint = iteratePoint;
    }

    return OK;
}

// builds the command_list_t struct which contains a list of cmd_buff_t
// separates each command by pipe and uses the build_cmd_buff to parse each indivual command
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char* saveptrCmd;
    char* command;
    int cmd_buff_build;
    clist->num = 0;

    memset(clist, 0, sizeof(command_list_t));

    command = strtok_r(cmd_line, PIPE_STRING, &saveptrCmd);

    if (command == NULL) {
        return WARN_NO_CMDS;
    }
    // This loop iterates through each of the pipe separated commands (if pipes are present)
    // and removes leading and trailing spaces before parsing the command further into 
    // executable and arguments. If the command is empty (just spaces), it's ignored and the next
    // command after the pipe is parsed instead. 
    while (command != NULL) {
        char *end;

        while (isspace((unsigned char)*command)) command++;

        end = command + strlen(command) - 1;
        while (isspace((unsigned char)*end) && end > command) end--;
        end[1] = '\0';

        if (*command == '\0') {
            command = strtok_r(NULL, PIPE_STRING, &saveptrCmd);
            continue;
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd_buff_build = build_cmd_buff(command, cmd);

        if (cmd_buff_build == ERR_CMD_OR_ARGS_TOO_BIG) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        if (cmd_buff_build == OK) {
            clist->num++;
        }

        command = strtok_r(NULL, PIPE_STRING, &saveptrCmd);
    }
    
    if (clist->num == 0) {
        return WARN_NO_CMDS;
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

// Executes multiple external commands using pipes
int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            break;
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(ERR_EXEC_CMD);
        }

        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }

    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return OK;

    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
        cmd_buff->argv[i] = NULL;
    }
    
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }

    cmd_buff->argc = 0;
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    if (!cmd_lst) return OK;

    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }

    memset(cmd_lst, 0, sizeof(command_list_t));
    return OK;
}