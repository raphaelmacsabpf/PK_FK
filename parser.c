#include <stdio.h>
#include "parser.h"

int parser(char newTokens[][MAX_TOKEN_LENGTH]) {
    char inputCommand[MAX_COMMAND_LENGTH];
    char allTokens[MAX_COMMAND_LENGTH][MAX_TOKEN_LENGTH];
    char tokenBuffer[MAX_COMMAND_LENGTH];

    int inQuotes = 0;
    int pos = 0;
    tokenBuffer[0] = '\0';
    if (fgets(inputCommand, sizeof inputCommand, stdin)) {
        char *token;
        token = strtok(inputCommand,DELIMITER);
        while(token != NULL) {
            sprintf(allTokens[pos],"%s",token);
            pos++;
            token = strtok(NULL,DELIMITER);
        }           
    }
    int i, pos2 = 0, founded;
    for(i = 0; i < pos; i++, founded = 0) {
        if(allTokens[i][0] == '\'') {
            inQuotes = 1;
        }
        while(inQuotes == 1) {
            founded ++;
            if(tokenBuffer[0] == '\0') {
                sprintf(tokenBuffer,"%s",&allTokens[i][1]);
            }
            else {
                sprintf(tokenBuffer,"%s %s",tokenBuffer,allTokens[i]);
            }
            if(tokenBuffer[strlen(tokenBuffer)-1] == '\'') {
                tokenBuffer[strlen(tokenBuffer)-1] = '\0';
                inQuotes = 0;
                sprintf(newTokens[pos2], "%s",tokenBuffer);
                pos2++;
                tokenBuffer[0] = '\0';
                if(founded > 0)
                    i--;
            }
            i++;
        }
        sprintf(newTokens[pos2], "%s", allTokens[i]);
        pos2++;
    }
    int offset = 0;
    for(i = 0; i < pos2; i++) {
        if(newTokens[i][strlen(newTokens[i])-1] == '\'') {
            i++;
            offset++;
            sprintf(newTokens[i-offset],"%s",newTokens[i]);
        }
    }
    pos2-= offset;
    if(newTokens[pos2-1][strlen(newTokens[pos2-1])-1] == 10) {
        newTokens[pos2-1][strlen(newTokens[pos2-1])-1] = '\0';
    }
    return pos2;
}

int commandParser(char newTokens[][MAX_TOKEN_LENGTH]) {

    char inputCommand[MAX_COMMAND_LENGTH];
    char allTokens[MAX_COMMAND_LENGTH][MAX_TOKEN_LENGTH];
    char tokenBuffer[MAX_COMMAND_LENGTH];

    int inQuotes = 0;
    int pos = 0;
    tokenBuffer[0] = '\0';
    /*if (fgets(inputCommand, sizeof inputCommand, stdin)) {
        char *token;
        token = strtok(inputCommand,DELIMITER);
        while(token != NULL) {
            sprintf(allTokens[pos],"%s",token);
            pos++;
            token = strtok(NULL,DELIMITER);
        }           
    }*/
   
    int i, j, pos2 = 0, founded;
    if (fgets(inputCommand, sizeof(inputCommand), stdin)) {
        char tokenDelimiter[sizeof(DELIMITER)];
        sprintf(tokenDelimiter, "%s",DELIMITER);
        for(i = 0; i < strlen(inputCommand)+1; i++) {
            int delimiterFounded = 0;
            for(j = 0; j < strlen(tokenDelimiter); j++) {
                if(inputCommand[i] == tokenDelimiter[j] || inputCommand[i] == '\0') {
                    delimiterFounded = 1;
                }
            }
            if(delimiterFounded == 1) {
                if(strlen(tokenBuffer) > 0) {
                    sprintf(allTokens[pos],"%s",tokenBuffer);
                    delimiterFounded = 1;
                    tokenBuffer[0] = '\0';
                    pos++;
                }
            }
            else if(delimiterFounded == 0) {
                sprintf(tokenBuffer,"%s%c",tokenBuffer,inputCommand[i]);
            }

        }
    }
    tokenBuffer[0] = '\0';
    for(i = 0; i < pos; i++, founded = 0) {
        if(allTokens[i][0] == '\'') {
            inQuotes = 1;
        }
        while(inQuotes == 1) {
            founded ++;
            if(tokenBuffer[0] == '\0') {
                sprintf(tokenBuffer,"%s",&allTokens[i][1]);
            }
            else {
                sprintf(tokenBuffer,"%s %s",tokenBuffer,allTokens[i]);
            }
            if(tokenBuffer[strlen(tokenBuffer)-1] == '\'') {
                tokenBuffer[strlen(tokenBuffer)-1] = '\0';
                inQuotes = 0;
                sprintf(newTokens[pos2], "%s",tokenBuffer);
                pos2++;
                tokenBuffer[0] = '\0';
                if(founded > 0)
                    i--;
            }
            i++;
        }
        sprintf(newTokens[pos2], "%s", allTokens[i]);
        pos2++;
    }
    int offset = 0;
    for(i = 0; i < pos2; i++) {
        if(newTokens[i][strlen(newTokens[i])-1] == '\'') {
            i++;
            offset++;
            sprintf(newTokens[i-offset],"%s",newTokens[i]);
        }
    }
    pos2-= offset;
    if(newTokens[pos2-1][strlen(newTokens[pos2-1])-1] == 10) {
        newTokens[pos2-1][strlen(newTokens[pos2-1])-1] = '\0';
    }
    return pos2;
}
