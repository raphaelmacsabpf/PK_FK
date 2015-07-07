#include <stdlib.h>
#include <string.h>
#include "buffend.h"
#include "parser.h"
int main() {
    int i;
    int exitFlag = 0;
    int dbConnected = 0;
    int pos2 = 0;
    usql usql;
    usql.currentDatabase[0] = '\0';
    char newTokens[MAX_COMMAND_LENGTH][MAX_TOKEN_LENGTH];
    char SQLCommands[][MAX_TOKEN_LENGTH] = {
        "INSERT",
        "INTO",
        "VALUES",
        "SELECT",
        "FROM",
        "CREATE",
        "DATABASE",
        "TABLE",
        "PRIMARY",
        "FOREIGN",
        "KEY"
    };
    printf("\nUFFS SQL Database\nusql interface\nVersion 1.0\nCreated by students of Universidade Federal da Fronteira Sul - Brasil\nType \"\\?\" for help.\n");
    while(exitFlag == 0) {
        printf("%s=# ",usql.currentDatabase);
        pos2 = commandParser(newTokens);
        //INSERT
        if(strcasecmp(newTokens[0],SQLCommands[0]) == 0&& strcasecmp(newTokens[1],SQLCommands[1]) == 0) {
            if(dbConnected) {
                char insertTable[50];
                sprintf(insertTable,"%s", newTokens[2]); 
                if(strcasecmp(newTokens[3],SQLCommands[2]) == 0) {
                    sprintf(insertTable,"%s.%s.dat",usql.currentDatabase,newTokens[2]);
                    printf("Existe arquivo %s\n",insertTable);
                    if(existeArquivo(insertTable)) {
                        sprintf(insertTable,"%s",newTokens[2]);
                        struct fs_objects object;
                        tp_table *schema;
                        abreTabela(newTokens[2], &object, &schema, usql);
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
                                finalizaInsert(newTokens[2], columns, usql);
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
                printf("ERROR: Not Connected to any database\nTIP: Use \\c to connect!\n");
            }
        }
        //\c (Connect)
        else if(strcmp(newTokens[0],"\\c") == 0) {
            if(databaseExists(newTokens[1])) {
                printf("Connected to database %s\n", newTokens[1]);
                sprintf(usql.currentDatabase,"%s",newTokens[1]);
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
        //\? (Help)
        else if(strcmp(newTokens[0],"\\?") == 0) {
            if(pos2 == 1) {
                printf("----------Listing SQL commands----------\n");
                printf("INSERT - Insert an item in a table\n");
                printf("SELECT - Select an item from a table\n");
                printf("CREATE - Create an item\n");
                printf("----------Listing environment commands----------\n");
                printf("\\c - Connect to a datase\n");
                printf("\\q - Exit\n");
                printf("\\? - Show this help\n");
                printf("\nNOTE: Use \\? {COMMAND} for more help.\n");
            }
            else {
                if(strcasecmp(newTokens[1], SQLCommands[0]) == 0) {//INSERT
                    printf("INSERT - Insert an item in a table\n");
                    printf("Syntax:\nINSERT INTO {TABLE_NAME} VALUES([{A},{ATTRIBUTE_1},{ATTRIBUTE_3},...,{ATTRIBUTE_N}]);\n");
                    printf("Example:\nINSERT INTO Aluno VALUES(1112,'Raphael','Rua 27',91.3);\n");
                }
                else if(strcasecmp(newTokens[1], SQLCommands[3]) == 0) {//SELECT
                    printf("SELECT - Select items of a table\n");
                    printf("Syntax:\nSELECT * FROM {TABLE_NAME};\n");
                    printf("Example:\nSELECT * FROM Aluno;\n");
                }
                else {
                    printf("Help for command \"%s\" not found!\n", newTokens[1]);
                }
            }
            
        }
        //SELECT * FROM Tabela
        else if(strcasecmp(newTokens[0],SQLCommands[3]) == 0) {
            if(dbConnected) {
                if(strcasecmp(newTokens[2],SQLCommands[4]) == 0) {
                    imprime(newTokens[3], usql);
                }
                else {
                    printf("ERROR: Expecting token 'FROM', but found %s\nABORTING...\n", newTokens[2]);
                }
            }
            else {
                printf("ERROR: Not Connected to any database\nTIP: Use \\C to connect!\n");
            }
            
        }
        //CREATE
        else if(strcasecmp(newTokens[0],SQLCommands[5]) == 0) {
            if(strcasecmp(newTokens[1],SQLCommands[6]) == 0) {//DATABASE
                int result = tryCreateDatabase(newTokens[2]);
                if(result == SUCCESS) {
                    printf("CREATE DATABASE\n");
                }
                else if(result == ERROR_DATABASE_NAME) {
                    printf("ERROR: Database Name not found!\n");
                }
                else if(result == ERROR_DATABASE_FILE_NOT_FOUND) {
                    printf("ERROR: Database file not found!\n");
                }
                else if(result == ERROR_DATABASE_EXISTS) {
                    printf("ERROR: Database %s already exists!\n", newTokens[2]);
                }
            }//CREATE TABLE Inst (CodInst, I, PRIMARY KEY;Nome, S, 20;Endereco, S, 20;Reitor, S, 10);
            else if(strcasecmp(newTokens[1],SQLCommands[7]) == 0) {//TABLE
                if(dbConnected) {
                    int createError = 0;
                    char realFileName[100];
                    sprintf(realFileName, "%s.%s.dat", usql.currentDatabase,newTokens[2]);
                    if(existeArquivo(realFileName) == 0) {
                        if(strlen(newTokens[2]) > 0) {
                            int fieldStart = 3;
                            table  *tab = iniciaTabela(newTokens[2],usql);//Start Table
                            for(i = 0; fieldStart < pos2-1; i++) {
                                char fieldName[TAMANHO_NOME_CAMPO];
                                char fieldType[2];
                                int fieldLength;
                                int fieldOffset = 0;
                                int key = 0;
                                char fk1[TAMANHO_NOME_TABELA], fk2[TAMANHO_NOME_CAMPO];
                                sprintf(fieldName, "%s", newTokens[fieldStart]);
                                sprintf(fieldType, "%s", newTokens[fieldStart+1]);
                                if(strcasecmp(fieldType,"I") == 0) {//INTEGER
                                    fieldLength = sizeof(int);
                                    if(strcasecmp(newTokens[fieldStart+2],SQLCommands[8]) == 0) {//PRIMARY
                                        if(strcasecmp(newTokens[fieldStart+3], SQLCommands[10]) == 0) {//KEY
                                            key = 1;//SET PRIMARY KEY
                                            fieldOffset += 2;
                                        }
                                        else {
                                            createError = 1;
                                        }
                                    }
                                    else if(strcasecmp(newTokens[fieldStart+2], SQLCommands[9]) == 0) {//FOREIGN
                                        if(strcasecmp(newTokens[fieldStart+3], SQLCommands[10]) == 0) {//KEY
                                            key = 2;//SET FOREIGN KEY
                                            fieldOffset += 4;
                                            sprintf(fk1,"%s", newTokens[fieldStart+4]);
                                            sprintf(fk2,"%s", newTokens[fieldStart+5]);
                                        }
                                        else {
                                            createError = 2;
                                        }
                                    }
                                }
                                else if(strcasecmp(fieldType,"S") == 0) {//STRING
                                    fieldLength = sizeof(char) * atoi(newTokens[fieldStart+2]);
                                    if(strcasecmp(newTokens[fieldStart+3],SQLCommands[8]) == 0) {//PRIMARY
                                        if(strcasecmp(newTokens[fieldStart+4], SQLCommands[10]) == 0) {//KEY
                                            key = 1;//SET PRIMARY KEY
                                            fieldOffset += 2;
                                        }
                                        else {
                                            createError = 1;
                                        }
                                    }
                                    else if(strcasecmp(newTokens[fieldStart+3], SQLCommands[9]) == 0) {//FOREIGN
                                        if(strcasecmp(newTokens[fieldStart+4], SQLCommands[10]) == 0) {//KEY
                                            key = 2;//SET FOREIGN KEY
                                            fieldOffset += 4;
                                            sprintf(fk1,"%s", newTokens[fieldStart+5]);
                                            sprintf(fk2,"%s", newTokens[fieldStart+6]);
                                        }
                                        else {
                                            createError = 2;
                                        }
                                    }
                                    fieldOffset += 1;
                                }
                                if(strcasecmp(fieldType,"D") == 0) {//DOUBLE
                                    fieldLength = sizeof(double);
                                    if(strcasecmp(newTokens[fieldStart+2],SQLCommands[8]) == 0) {//PRIMARY
                                        if(strcasecmp(newTokens[fieldStart+3], SQLCommands[10]) == 0) {//KEY
                                            key = 1;//SET PRIMARY KEY
                                            fieldOffset += 2;
                                        }
                                        else {
                                            createError = 1;
                                        }
                                    }
                                    else if(strcasecmp(newTokens[fieldStart+2], SQLCommands[9]) == 0) {//FOREIGN
                                        if(strcasecmp(newTokens[fieldStart+3], SQLCommands[10]) == 0) {//KEY
                                            key = 2;//SET FOREIGN KEY
                                            fieldOffset += 4;
                                            sprintf(fk1,"%s", newTokens[fieldStart+4]);
                                            sprintf(fk2,"%s", newTokens[fieldStart+5]);
                                        }
                                        else {
                                            createError = 2;
                                        }
                                    }
                                }
                                fieldStart += fieldOffset+2;
                                tab = adicionaCampo(tab, fieldName, fieldType[0], fieldLength, key, fk1, fk2);
                                printf("CAMPO: %s, TAMANHO: %d, TIPO: %s, KEY: %d, FK1: %s, FK2: %s\n", fieldName, fieldLength, fieldType, key,fk1,fk2);
                            }
                            if(createError == 0) {
                                finalizaTabela(tab, usql);
                                FILE *ff = fopen("MEUFILE.dat","a");
                                fclose(ff);
                                printf("CREATE TABLE\n");
                            }
                            else {
                                printf("ERROR: Create table error!\n");
                            }
                            
                        }
                        else {
                            printf("ERROR: Expecting a table name, but not found!\n");
                        }
                    }
                    else {
                        printf("ERROR: Table %s already exists!\n", newTokens[2]);
                    }
                }
                else {
                    printf("ERROR: Not Connected to any database\nTIP: Use \\c to connect!\n");
                }
            }
            else {
                printf("ERROR: Expected a new token, but not found!\n");
            }
        }
        else {
            printf("ERROR: Token '%s', is not recognized\nUSE: \\? to help...\n",newTokens[0]);
        }
    }
    return 0;
}
//insert into Aluno VALUES(3391,'raphael','Santos',33.7);
