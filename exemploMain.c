#include <stdlib.h>
#include <string.h>
#include "buffend.h"
#define DELIMITER " (),;"
int main() {
    int exitFlag = 0;
    int dbConnected = 0;
    char currentDatabase[50] = "";
    char inputCommand[128];
    char allTokens[128][128];
    char newTokens[128][128];
    char tokenBuffer[128];
    char SQLCommands[][50] = {
        "INSERT",
        "INTO",
        "VALUES",
        "SELECT",
        "FROM"
    };
    while(exitFlag == 0) {
        int inQuotes = 0;
        int pos = 0;
        tokenBuffer[0] = '\0';
        printf("%s=# ",currentDatabase);
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
        //INSERT
        if(strcasecmp(newTokens[0],SQLCommands[0]) == 0&& strcasecmp(newTokens[1],SQLCommands[1]) == 0) {
            if(dbConnected) {
                char insertTable[50];
                sprintf(insertTable,"%s", newTokens[2]); 
                if(strcasecmp(newTokens[3],SQLCommands[2]) == 0) {
                    sprintf(insertTable,"%s.dat",insertTable);
                    if(existeArquivo(insertTable)) {
                        struct fs_objects object;
                        tp_table *schema;
                        abreTabela(newTokens[2], &object, &schema);
                        if(object.qtdCampos == pos2-5) {
                            column *columns = NULL;
                            int j, success = 1;
                            for(i = 4, j = 0; i < pos2-1; i++, j++) {
                                if(schema[j].tipo == 'S') {
                                    if(strlen(newTokens[i]) >= schema[j].tam) {
                                        printf("ERROR: Field length out of bounds on field '%s'\n", schema[j].nome);
                                        success = 0;
                                    }
                                }
                                columns = insereValor(columns, schema[j].nome, newTokens[i]);
                            }
                            if(success) {
                                finalizaInsert(newTokens[2], columns);
                                printf("INSERT 1\n");
                            }
                        }
                        else {
                            printf("ERROR: Field count does not match, expected reading %d fields but found %d fields!\nABORTING...\n",object.qtdCampos,pos2-5);
                            for(i = 4; i < pos2-1; i++) {
                                printf("VALOR = %s\n",newTokens[i]);
                            }
                        }
                    }
                    else {
                        printf("ERROR: Table %s not exists!\nABORTING...\n",newTokens[2]);
                    }
                    
                }
                else {
                    printf("ERROR: Expecting token 'VALUES', but found %s\nABORTING...\n", newTokens[3]);
                }
            }
            else {
                printf("ERROR: Not Connected to any database\nTIP: Use \\C to connect!\n");
            }
        }
        //\c (Connect)
        else if(strcmp(newTokens[0],"\\c") == 0) {
            char tryBD[50];
            sprintf(tryBD,"fs_schema_%s.dat",newTokens[1]);
            if(fopen(tryBD,"r") != NULL) {
                printf("Connected to database %s\n", newTokens[1]);
                sprintf(currentDatabase,"%s",newTokens[1]);
                dbConnected = 1;
            }
            else {
                printf("ERROR: Database %s not exists!\n",newTokens[1]);
            }
        }
        //\q (Sair)
        else if(strcmp(newTokens[0],"\\q") == 0) {
            printf("LEAVING...\n");
            exitFlag = 1;
        }
        //SELECT * FROM Tabela
        else if(strcasecmp(newTokens[0],SQLCommands[3]) == 0) {
            if(dbConnected) {
                if(strcasecmp(newTokens[2],SQLCommands[4]) == 0) {
                    imprime(newTokens[3]);
                }
                else {
                    printf("ERROR: Expecting token 'FROM', but found %s\nABORTING...\n", newTokens[2]);
                }
            }
            else {
                printf("ERROR: Not Connected to any database\nTIP: Use \\C to connect!\n");
            }
            
        }
        else {
            printf("ERROR: Token '%s', is not recognized\nABORTING...\n",newTokens[0]);
        }

    }
    return 0;
}
//insert into Aluno VALUES(3391,'raphael','Santos',33.7);