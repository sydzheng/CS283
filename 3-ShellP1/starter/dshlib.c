#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char* saveptrCmd;
    char* saveptrSubCmd;
    char* command;

    memset(clist, 0, sizeof(command_list_t));

    command = strtok_r(cmd_line, PIPE_STRING, &saveptrCmd);

    // This loop iterates through each of the pipe separated commands (if pipes are present)
    // and removes leading and trailing spaces before parsing the command further into 
    // executable and arguments. If the command is empty (just spaces), it's ignored and the next
    // command after the pipe is parsed instead. 
    while (command != NULL) {
        char *end;
        char *parsedCmd;

        while (isspace((unsigned char)*command)) command++;

        end = command + strlen(command) - 1;
        while (isspace((unsigned char)*end) && end > command) end--;
        end[1] = '\0';

        if (*command == '\0') {
            command = strtok_r(NULL, PIPE_STRING, &saveptrCmd);
            continue;
        }

        parsedCmd = strtok_r(command, " ", &saveptrSubCmd);
        command_t *accessCmd = &clist->commands[clist->num];

        if (strlen(parsedCmd) > EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        accessCmd->exe[0] = '\0';
        accessCmd->args[0] = '\0';

        strncpy(accessCmd->exe, parsedCmd, EXE_MAX);
        accessCmd->exe[EXE_MAX-1] = '\0';

        parsedCmd = strtok_r(NULL, " ", &saveptrSubCmd);
        bool firstArg = false;

        while (parsedCmd != NULL) {
            if (strlen(parsedCmd) + strlen(accessCmd->args) > ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            if (firstArg) {
                strcat(accessCmd->args, " ");
            }

            strcat(accessCmd->args, parsedCmd);
            firstArg = true;
            parsedCmd = strtok_r(NULL, " ", &saveptrSubCmd);
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }       
        clist->num++;

        command = strtok_r(NULL, PIPE_STRING, &saveptrCmd);
    }

    return OK;
}