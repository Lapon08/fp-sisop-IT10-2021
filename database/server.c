#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

char SESSION_USERNAME[100] = {0};
char SESSION_DATABASE[100] = {0};
char column[1024] = {0};
char data_in_table[200][1024] = {0};
char selectColumnName[256][256] = {0};
int position_column[100];
int data_exist[100];
int count_column;
int count_row;
int count_data_exist;
char message[2048] = {0};

void reset()
{
    memset(data_exist, 0, sizeof(data_exist));
    memset(position_column, 0, sizeof(position_column));
    memset(selectColumnName, 0, sizeof(selectColumnName[0][0]) * 256 * 256);
}

void countColumn()
{
    char tmp[1024];
    int i = 0;
    strcpy(tmp, column);
    char *token = strtok(tmp, "\t");
    while (token != NULL)
    {
        //printf("%d -> %s\n", i, token);
        token = strtok(NULL, "\t");
        i++;
    }
    count_column = i;
}
void readDatabase(char *database, char *table)
{
    FILE *fptr;
    char fullpath[1024] = {0};
    sprintf(fullpath, "databases/%s/%s", database, table);
    //printf("\n%s", fullpath);
    fptr = fopen(fullpath, "r+");
    if (fptr == NULL)
    {
        strcpy(message, "Error\n");
        return;
    }
    char line[1024] = {0};
    int i = -1;
    while (fgets(line, sizeof(line) - 1, fptr))
    {
        if (i == -1)
        {
            strcpy(column, line);
        }
        else
        {
            strcpy(data_in_table[i], line);
        }
        i++;
    }
    count_row = i;

    fclose(fptr);
}

void selectColumn()
{
    char tmp[1024] = {0};
}
void writeTable(char *fullpath)
{
    FILE *fptr;
    int i = -1, j = 0;
    char commandRemove[200];
    sprintf(commandRemove, "rm %s", fullpath);
    system(commandRemove);
    fptr = fopen(fullpath, "a+");
    if (fptr == NULL)
    {
        strcpy(message, "Error\n");

        return;
    }
    int isexist = 0;
    for (int i = -1; i < count_row; i++)
    {
        if (i == -1)
        {
            fprintf(fptr, "%s", column);
        }
        else
        {

            if (i == data_exist[j])
            {
                j++;
                continue;
            }
            fprintf(fptr, "%s", data_in_table[i]);
        }
    }
    fclose(fptr);
}

int checkDataExist(char *data, char *path, int column)
{
    memset(data_exist, 0, sizeof(data_exist));
    int check = 0;
    FILE *fptr;
    fptr = fopen(path, "r+");
    if (fptr == NULL)
    {
        strcpy(message, "Error\n");
    }
    char line[500] = {0};
    char datatemp[1024] = {0};

    int i = 0;
    int j = 0;
    while (fgets(line, sizeof(line), fptr))
    {
        if (column != 69)
        {
            char tmp[1024] = {0};
            strcpy(tmp, line);
            char *token = strtok(tmp, "\t");
            // //printf("TOKEN ->%s\n", token);
            // //printf("LINE ->%s\n", line);
            for (int i = 0; i < column; i++)
            {
                token = strtok(NULL, "\t");
            }
            strcpy(datatemp, token);
        }
        else
        {
            strcpy(datatemp, line);
        }

        ////printf("%s -> %s\n", datatemp, data);
        ////printf("-----------------------\n");

        if (strcmp(datatemp, data) == 0)
        {
            ////printf("KETEMU\n");
            check = 1;
            // posisi data exist
            data_exist[j] = i - 1;
            j++;
        }
        i++;
    }
    // termasuk 0
    count_data_exist = j;
    ////printf("%d\n", count_data_exist);
    // for (int i = 0; i < count_data_exist; i++)
    // {
    //     //printf("data exist-> %d\n", data_exist[i]);
    // }

    fclose(fptr);
    return check;
}
int checkUser(char *username, char *password)
{
    if (getuid() == 0)
    {
        strcpy(SESSION_USERNAME, "root");
        return 1;
    }
    else
    {
        FILE *fptr;

        fptr = fopen("databases/init/users.table", "r+");
        if (fptr == NULL)
        {
            strcpy(message, "Error\n");
            return 0;
        }
        char line[500] = {0};
        char kredensial[1024] = {0};
        sprintf(kredensial, "%s\t%s\n", username, password);
        while (fgets(line, sizeof(line) - 1, fptr))
        {
            // //printf("%s%s",kredensial,line);
            // //printf("-----------------------\n");
            if (strcmp(kredensial, line) == 0)
            {
                strcpy(SESSION_USERNAME, username);
                return 1;
            }
        }
        fclose(fptr);
        return 0;
    }
}

void addUser(char *namaUser, char *passwordUser)
{
    if (getuid() == 0)
    {
        int check = checkDataExist(namaUser, "databases/init/users.table", 0);
        if (!check)
        {
            FILE *fptr;
            fptr = fopen("databases/init/users.table", "a+");

            if (fptr == NULL)
            {
                strcpy(message, "Error\n");

                return;
            }

            fprintf(fptr, "%s\t%s\n", namaUser, passwordUser);
            fclose(fptr);
        }
        else
        {
            strcpy(message, "User Already Used\n");
        }
    }
    else
    {
        strcpy(message, "You Are Not root\n");
    }
    strcpy(message, "Success\n");
    memset(data_exist, 0, sizeof(data_exist));
}
int checkCommandAddUser(char *command, char *username, char *password)
{
    // CREATE USER [nama_user] IDENTIFIED BY [password_user];
    char *token = strtok(command, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    token = strtok(NULL, " ");
    // [nama_user]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }

    strcpy(username, token);
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    if (strcmp(token, "IDENTIFIED") != 0)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    if (strcmp(token, "BY") != 0)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    // [password_user]
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    if (strcmp(&token[strlen(token) - 1], ";") == 0)
    {
        strncpy(password, token, strlen(token) - 1);
    }
    else
    {
        strcpy(message, "Invalid Syntax\n");
        return 0;
    }
    // DEBBUG
    // //printf("%s\t%s", username, password);
    return 1;
}
void createDatabase(char *database)
{
    char fullpath[200];
    sprintf(fullpath, "databases/%s", database);
    int check = mkdir(fullpath, 0777);

    // check if directory is created or not
    if (!check)
    {
        FILE *fptr;
        fptr = fopen("databases/init/permission.table", "a+");

        if (fptr == NULL)
        {
            strcpy(message, "Error\n");
            return;
        }
        if (strcmp(SESSION_USERNAME, "root") == 0)
        {
            fprintf(fptr, "%s\t%s\n", SESSION_USERNAME, database);
            fclose(fptr);
        }
        else
        {
            fprintf(fptr, "%s\t%s\nroot\t%s\n", SESSION_USERNAME, database, database);
            fclose(fptr);
        }
        strcpy(message, "Success\n");
    }
    else
    {
        strcpy(message, "Unable to create directory\n");
        return;
    }
}
int checkCommandCreateDatabase(char *command, char *database)
{
    // CREATE DATABASE [nama_database];
    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(command, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    // [nama_user]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(database, token, strlen(token) - 1);
    // //printf("DEBUG ->%s ini database\n", database);
    return 1;
}

void grantDatabase(char *username, char *database)
{
    if (getuid() == 0)
    {
        int check = 0;
        char data[1024] = {0};
        sprintf(data, "%s\t%s\n", username, database);
        check = checkDataExist(data, "databases/init/permission.table", 69);
        if (check == 0)
        {
            FILE *fptr;
            fptr = fopen("databases/init/permission.table", "a+");
            if (fptr == NULL)
            {
                strcpy(message, "Error\n");

                return;
            }
            fprintf(fptr, "%s\t%s\n", username, database);
            memset(data_exist, 0, sizeof(data_exist));
            fclose(fptr);
            strcpy(message, "Success\n");
        }
        else
        {
            strcpy(message, "the user already has access to the database\n");
            memset(data_exist, 0, sizeof(data_exist));
            return;
        }
    }
    else
    {
        strcpy(message, "Sorry, You are not root\n");
    }
}

int checkCommandGrantDatabase(char *command, char *username, char *database)
{
    // GRANT PERMISSION [nama_database] INTO [nama_user];
    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(command, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    // [nama_database]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strcpy(database, token);
    token = strtok(NULL, " ");
    // INTO
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    if (strcmp(token, "INTO") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }

    token = strtok(NULL, " ");
    // [nama_user]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(username, token, strlen(token) - 1);
    // //printf("DEBUG -> %s\t%s\n", username, database);
    return 1;
}

int checkCommandUseDatabase(char *command, char *database)
{
    // USE [nama_database];
    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(command, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    token = strtok(NULL, " ");
    // [nama_database]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(database, token, strlen(token) - 1);
    return 1;
}
void useDatabase(char *database)
{
    char data[300] = {0};
    sprintf(data, "%s\t%s\n", SESSION_USERNAME, database);
    int check = checkDataExist(data, "databases/init/permission.table", 69);
    // //printf("data -> %s", data);
    if (check)
    {
        strcpy(SESSION_DATABASE, database);
        strcpy(message, "Success\n");
    }
    else
    {
        strcpy(message, "Database Not Found / You Not have Permission\n");
    }
    memset(data_exist, 0, sizeof(data_exist));
}
int checkCommandCreateTable(char *input, char *table, char *column_name)
{
    char hasil[1024] = {0};

    // Returns first token
    if (strcmp(&input[strlen(input) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(input, " ");
    //printf("1 %s\n", token);
    token = strtok(NULL, " ");
    //printf("2 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    token = strtok(NULL, " ");
    strcpy(table, token);
    //printf("3 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    token = strtok(NULL, "(");
    //printf("4 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    else if (strcmp(token, ");") == 0)
    {
        strcpy(message, "Your Column Is Empty\n");
        return 0;
    }
    else
    {
        strcpy(column, " ");
        strncat(column, token, strlen(token) - 2);
        //printf("%s\n", column);
        int j = 0;
        char *token2 = strtok(column, " ");
        j++;
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        //printf("%s\n", token2);
        strcat(column_name, token2);
        strcat(column_name, "|");
        token2 = strtok(NULL, " ");
        j++;
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        while (token2 != NULL)
        {

            if (strstr(token2, ","))
            {
                strncat(column_name, token2, strlen(token2) - 1);
                strcat(column_name, "\t");
            }
            else
            {
                strcat(column_name, token2);
                strcat(column_name, "|");
            }

            //printf("%s\n", token2);
            token2 = strtok(NULL, " ");
            j++;
        }
        if (j % 2 == 0)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }

        column_name[strlen(column_name) - 1] = 0;
        // //printf("\n%s-----%d", column_name, j);
    }

    // kolom1|string\tkolom2|int\tkolom3|string\tkolom4|int
    return 1;
}
void createTable(char *table, char *column_name)
{
    FILE *fptr;
    char fullpath[500];
    sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
    // //printf("%s-----------", fullpath);
    fptr = fopen(fullpath, "a+");
    if (fptr == NULL)
    {
        strcpy(message, "Error\n");

        return;
    }
    fprintf(fptr, "%s\n", column_name);
    fclose(fptr);
    strcpy(message, "Success\n");
}

int checkCommandDropDatabase(char *command, char *database)
{
    // DROP DATABASE database1;

    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(command, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    token = strtok(NULL, " ");
    // [nama_database]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(database, token, strlen(token) - 1);
    return 1;
}

void dropDatabase(char *data, char *database)
{
    char fullpathCommand[1024] = {0};
    sprintf(fullpathCommand, "rm -r databases/%s", database);
    system(fullpathCommand);
    readDatabase("init", "permission.table");
    writeTable("databases/init/permission.table");
    strcpy(message, "Success\n");
}
int checkCommandDropTable(char *command, char *table)
{
    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    char *token = strtok(command, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    token = strtok(NULL, " ");
    // [nama_table]
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(table, token, strlen(token) - 1);
    return 1;
}
int readTableOfDatabase(char *fullpath, char *table)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(fullpath)) != NULL)
    {

        while ((ent = readdir(dir)) != NULL)
        {
            // //printf("%s ->%s", ent->d_name, table);
            if (strcmp(ent->d_name, table) == 0)
            {
                // //printf("%s ->%s", ent->d_name, table);
                closedir(dir);
                return 1;
            }
        }
        closedir(dir);
        return 0;
    }
}
void dropTable(char *table)
{
    char fullpath[500] = {0};
    sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
    // //printf("%s\n",fullpath);
    if (remove(fullpath) == 0)
        strcpy(message, "Success\n");

    else
        strcpy(message, "Error\n");
}
int checkCommandDropColumn(char *command, char *column, char *table)
{
    // DROP COLUMN kolom1 FROM table1;
    if (strcmp(&command[strlen(command) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // DROP
    char *token = strtok(command, " ");
    // COLUMN
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // kolom1
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(column, token, strlen(token));
    // FROM
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // table
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strncpy(table, token, strlen(token) - 1);
    return 1;
}
int positionCloumn(char *column_name)
{
    char temp[100] = {0};
    strcpy(temp, column);
    int i = 0;
    char *token = strtok(temp, "\t");
    while (token != NULL)
    {
        if (strstr(token, column_name))
        {
            break;
        }
        token = strtok(NULL, "\t");
        i++;
    }
    return i;
}
void dropColumn(int position)
{
    char temp[200][1024] = {0};
    char hasil[200][1024] = {0};
    for (int i = 0; i < count_row; i++)
    {
        strcpy(temp[i], data_in_table[i]);
        // //printf("TEMP = > %s\n", temp[i]);
    }

    for (int i = 0; i < count_row; i++)
    {

        char *token = strtok(temp[i], "\t");
        int count = 0;
        while (count < count_column)
        {
            // //printf("count %d ; position %d\n", count, position);
            if (count == position)
            {
                // //printf("token -> %s\n", token);
                token = strtok(NULL, "\t");
                count++;
                continue;
            }
            else
            {
                // sidoarjo\n\t
                strcat(hasil[i], token);
                if (!strstr(hasil[i], "\n"))
                {
                    strcat(hasil[i], "\t");
                }
                // //printf("token -> %s\n", token);
                token = strtok(NULL, "\t");
                count++;
            }
        }

        if (!strstr(hasil[i], "\n"))
        {
            //printf("masuk");
            strncpy(hasil[i], hasil[i], strlen(hasil[i]) - 1);
            strcat(hasil[i], "\n");
        }
        //printf("Hasil -> %s ---- %d", hasil[i], i);
    }
    for (int i = 0; i < count_row; i++)
    {
        strcpy(data_in_table[i], hasil[i]);
    }
    for (int i = 0; i < 100; i++)
    {
        data_exist[i] = -2;
    }
}
void dropColumn2(int position)
{
    char tmp[1024] = {0};
    char hasil[1024] = {0};
    strcpy(tmp, column);
    char *token = strtok(tmp, "\t");
    int count = 0;
    while (count < count_column)
    {
        // //printf("count %d ; position %d\n", count, position);
        if (count == position)
        {
            // //printf("token -> %s\n", token);
            token = strtok(NULL, "\t");
            count++;
            continue;
        }
        else
        {
            // sidoarjo\n\t
            strcat(hasil, token);
            if (!strstr(hasil, "\n"))
            {
                strcat(hasil, "\t");
            }
            // //printf("token -> %s\n", token);
            token = strtok(NULL, "\t");
            count++;
        }
    }

    if (!strstr(hasil, "\n"))
    {
        //printf("masuk");
        strncpy(hasil, hasil, strlen(hasil) - 1);
        strcat(hasil, "\n");
    }
    strcpy(column, hasil);
}
int checkCommandInsert(char *input, char *column_name, char *table, int *total)
{
    char hasil[1024] = {0};
    char kolom[1024] = {0};
    int j = 0;
    // Returns first token
    if (strcmp(&input[strlen(input) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // INSERT
    char *token = strtok(input, " ");
    //printf("1 %s\n", token);
    // INTO
    token = strtok(NULL, " ");
    //printf("2 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // [TABLE]
    token = strtok(NULL, " ");
    strcpy(table, token);
    //printf("3 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    if (strstr(token, "\'"))
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }

    token = strtok(NULL, "(");
    //printf("4 %s\n", token);
    if (strcmp(token, ");") == 0)
    {
        //printf("Your Column Is Empty");
        return 0;
    }
    strcpy(kolom, " ");
    strncat(kolom, token, strlen(token) - 2);
    //printf("KOLOM - >%s\n", kolom);
    // [Kolom]
    char *token2 = strtok(kolom, " ");
    j++;
    while (token2 != NULL)
    {
        if (strstr(token2, ","))
        {
            if (strstr(token2, "\'"))
            {
                char tmp[200] = {0};
                strcpy(tmp, token2);
                memmove(tmp, tmp + 1, strlen(tmp));
                strncat(column_name, tmp, strlen(tmp) - 2);
                strcat(column_name, "\t");
            }
            else
            {
                strncat(column_name, token2, strlen(token2) - 1);
                strcat(column_name, "\t");
            }
        }
        else
        {
            if (strstr(token2, "\'"))
            {
                char tmp[200] = {0};
                strcpy(tmp, token2);
                memmove(tmp, tmp + 1, strlen(tmp));
                strncat(column_name, tmp, strlen(tmp) - 1);
            }
            else
            {
                strncat(column_name, token2, strlen(token2));
            }
        }
        token2 = strtok(NULL, " ");
        j++;
    }
    *total = j;
    if (strstr(column_name, ","))
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    ////printf("----------%s----------%d----------", hasil, j);
    return 1;
}

void insert(char *data, char *table)
{
    FILE *fptr;
    char fullpath[200];
    sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
    fptr = fopen(fullpath, "a+");
    if (fptr == NULL)
    {
        strcpy(message, "Error\n");

        return;
    }
    fprintf(fptr, "%s\n", data);
    fclose(fptr);
    strcpy(message, "Success\n");
}

int checkCommandDelete(char *input, char *table, char *column_name, char *data_column, int *mode)
{
    int j = 0;
    // Returns first token
    if (strcmp(&input[strlen(input) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // DELETE
    char *token = strtok(input, " ");
    ////printf("1 %s\n", token);
    // FROM
    token = strtok(NULL, " ");
    ////printf("2 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // [TABLE]
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strcpy(table, token);
    ////printf("3 %s\n", token);

    token = strtok(NULL, " ");
    ////printf("4 %s\n", token);
    if (token == NULL)
    {
        table[strlen(table) - 1] = '\0';
        *mode = 1;
    }
    else
    {
        *mode = 2;
        // WHERE
        // DELETE FROM table1 WHERE kolom1=’value1’;
        token = strtok(NULL, " ");
        ////printf("5 %s\n", token);
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        // KOLOM
        char tmp[200];
        strcpy(tmp, token);
        char *token2 = strtok(tmp, "=");
        ////printf("6 %s\n", token2);
        if (token2 == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        strcpy(column_name, token2);
        token2 = strtok(NULL, "=");
        if (strstr(token2, "\'"))
        {
            if (token2[0] == '\'')
            {
                memmove(token2, token2 + 1, strlen(token2));
            }

            strncpy(data_column, token2, strlen(token2) - 2);
            ////printf("7 %s\n", data_column);
        }
        else
        {
            strncpy(data_column, token2, strlen(token2) - 1);
            ////printf("7 %s\n", data_column);
        }
    }

    return 1;
}
void deleteData(int mode, int position, char *fullpath)
{
    if (mode == 1)
    {
        FILE *fptr;
        fptr = fopen(fullpath, "w+");
        if (fptr == NULL)
        {
            strcpy(message, "Error\n");
            return;
        }
        fclose(fptr);
    }
    else if (mode == 2)
    {
        FILE *fptr;
        int i = -1, j = 0;
        fptr = fopen(fullpath, "w+");
        if (fptr == NULL)
        {
            strcpy(message, "Error\n");
            return;
        }
        for (int i = -1; i < count_row; i++)
        {
            if (i == -1)
            {
                fprintf(fptr, "%s", column);
            }
            else
            {

                if (i == data_exist[j])
                {
                    j++;
                    continue;
                }
                fprintf(fptr, "%s", data_in_table[i]);
            }
        }
        strcpy(message, "Success\n");
        fclose(fptr);
    }
}

int checkCommandUpdate(char *input, char *table, char *data_column, char *column_name, char *search_column, char *search_data, int *mode)
{

    int j = 0;
    // Returns first token
    if (strcmp(&input[strlen(input) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    // UPDATE
    char *token = strtok(input, " ");
    ////printf("1 %s\n", token);
    // [TABLE]
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strcpy(table, token);
    ////printf("2 %s\n", token);
    // SET
    token = strtok(NULL, " ");
    ////printf("3 %s\n", token);
    if (token == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }

    token = strtok(NULL, " ");
    char temp[1024] = {0};
    char temp2[1024] = {0};
    strcpy(temp, token);

    //printf("4 %s\n", token);
    // WHERE
    token = strtok(NULL, " ");
    ////printf("5 %s\n", token);
    if (token == NULL)
    {
        *mode = 1;
        // WHERE
        // KOLOM
        char *token2 = strtok(temp, "=");
        ////printf("6 %s\n", token2);
        if (token2 == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        strcpy(column_name, token2);
        token2 = strtok(NULL, "=");
        if (strstr(token2, "\'"))
        {
            if (token2[0] == '\'')
            {
                memmove(token2, token2 + 1, strlen(token2));
            }

            strncpy(data_column, token2, strlen(token2) - 2);
            ////printf("7 %s\n", data_column);
        }
        else
        {
            strncpy(data_column, token2, strlen(token2) - 1);
            ////printf("7 %s\n", data_column);
        }
        return 1;
    }
    *mode = 2;
    token = strtok(NULL, " ");
    strcpy(temp2, token);
    ////printf("6 %s\n", token);
    char *token2 = strtok(temp, "=");
    ////printf("6 %s\n", token2);
    if (token2 == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strcpy(column_name, token2);
    token2 = strtok(NULL, "=");
    if (strstr(token2, "\'"))
    {
        if (token2[0] == '\'')
        {
            memmove(token2, token2 + 1, strlen(token2));
        }

        strncpy(data_column, token2, strlen(token2) - 1);
        ////printf("7 %s\n", data_column);
    }
    else
    {
        strncpy(data_column, token2, strlen(token2));
        ////printf("7 %s\n", data_column);
    }
    char tmp[200];
    strcpy(tmp, token);
    char *token3 = strtok(tmp, "=");
    ////printf("6 %s\n", token3);
    if (token3 == NULL)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    strcpy(search_column, token3);
    token3 = strtok(NULL, "=");
    if (strstr(token3, "\'"))
    {
        if (token3[0] == '\'')
        {
            memmove(token3, token3 + 1, strlen(token3));
        }

        strncpy(search_data, token3, strlen(token3) - 2);
        ////printf("7 %s\n", search_data);
    }
    else
    {
        strncpy(search_data, token3, strlen(token3) - 1);
        ////printf("7 %s\n", search_data);
    }
    // WHERE
    //UPDATE table1 SET kolom1='new_value1' WHERE;’;
    // KOLOM

    return 1;
}
void updateData(int mode, int position, char *data_column, char *search_column, char *search_data)
{
    if (mode == 1)
    {
        char tmp[200][1024] = {0};
        char hasil[200][1024] = {0};
        for (int i = 0; i < count_row; i++)
        {
            strncpy(tmp[i], data_in_table[i], strlen(data_in_table[i]) - 1);
            ////printf("%s\n", tmp[i]);
        }
        for (int i = 0; i < count_row; i++)
        {
            char *token = strtok(tmp[i], "\t");
            int j = 0;

            while (token != NULL)
            {
                if (j == position)
                {
                    strcat(hasil[i], data_column);
                    strcat(hasil[i], "\t");
                }
                else
                {
                    strcat(hasil[i], token);
                    strcat(hasil[i], "\t");
                }
                token = strtok(NULL, "\t");
                j++;
            }
            hasil[i][strlen(hasil[i]) - 1] = '\0';
            if (!strstr(hasil[i], "\n"))
            {
                strcat(hasil[i], "\n");
                /* code */
            }
        }
        for (int i = 0; i < count_row; i++)
        {
            strcpy(data_in_table[i], hasil[i]);
            ////printf("HASIL -> %s", data_in_table[i]);
        }
        strcpy(message, "Success\n");
    }
    else if (mode == 2)
    {
        char tmp[200][1024] = {0};
        char hasil[200][1024] = {0};
        for (int i = 0; i < count_row; i++)
        {
            strncpy(tmp[i], data_in_table[i], strlen(data_in_table[i]));
            // //printf("%s\n", tmp[i]);
        }
        int j = 0;
        for (int i = 0; i < count_row; i++)
        {
            if (i == data_exist[j])
            {
                char *token = strtok(tmp[i], "\t");
                int k = 0;

                while (token != NULL)
                {
                    if (k == position)
                    {
                        strcat(hasil[i], data_column);
                        strcat(hasil[i], "\t");
                    }
                    else
                    {
                        strcat(hasil[i], token);
                        strcat(hasil[i], "\t");
                    }
                    token = strtok(NULL, "\t");
                    k++;
                }
                hasil[i][strlen(hasil[i]) - 1] = '\0';

                j++;
            }
            else
            {

                strcat(hasil[i], tmp[i]);
            }
            if (!strstr(hasil[i], "\n"))
            {
                strcat(hasil[i], "\n");
                /* code */
            }
        }
        for (int i = 0; i < count_row; i++)
        {
            strcpy(data_in_table[i], hasil[i]);
            // //printf("HASIL -> %s", data_in_table[i]);
        }
    }
}
int checkCommandSelect(char *input, char *table, char *search_column, char *search_data, int *mode, int *countColumnSelect)
{
    int j = 0;

    if (strcmp(&input[strlen(input) - 1], ";") != 0)
    {
        strcpy(message, "Invalid Syntax\n");

        return 0;
    }
    if (strstr(input, "*"))
    {
        // SELECT
        char *token = strtok(input, " ");
        ////printf("1 %s\n", token);

        token = strtok(NULL, " ");
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        if (strcmp(token, "*") != 0)
        {
            return 0;
        }
        ////printf("2 %s\n", token);
        // FROM
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        // [table]
        ////printf("3 %s\n", token);
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        if (strstr(token, ";"))
        {
            strncpy(table, token, strlen(token) - 1);
        }
        else
        {
            strncpy(table, token, strlen(token));
        }

        ////printf("4 %s\n", table);
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            *mode = 1;
            return 1;
        }
        // WHERE [nama_kolom]=[value];
        if (strcmp(token, "WHERE") == 0)
        {
            ////printf("5 %s\n", token);
            token = strtok(NULL, " ");
            ////printf("6 %s\n", token);
            char tmp[100];
            strcpy(tmp, token);
            char *token2 = strtok(tmp, "=");
            strcpy(search_column, token2);
            ////printf("7 %s\n", search_column);
            token2 = strtok(NULL, " ");

            strcpy(search_data, token2);
            if (search_data[0] == '\'')
            {
                memmove(search_data, search_data + 1, strlen(search_data));
                search_data[strlen(search_data) - 1] = '\0';
                search_data[strlen(search_data) - 1] = '\0';
            }
            else
            {
                search_data[strlen(search_data) - 1] = '\0';
            }
        }
        else
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        *mode = 2;
        return 1;
    }
    else
    {
        // SELECT
        char *token = strtok(input, " ");
        ////printf("1 %s\n", token);
        // // SELECT [nama_kolom, … | *] FROM [nama_tabel];

        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        token = strtok(NULL, " ");
        if (strcmp(token, "FROM") == 0)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        int i = 0;
        while (strcmp(token, "FROM") != 0)
        {

            strcat(selectColumnName[i], token);
            if (strstr(selectColumnName[i], ","))
            {
                selectColumnName[i][strlen(selectColumnName[i]) - 1] = '\0';
            }
            ////printf("--> %s\n", selectColumnName[i]);
            token = strtok(NULL, " ");
            *countColumnSelect = *countColumnSelect + 1;
            i++;
        }
        // FROM
        if (token == NULL)
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        // [table]
        ////printf("3 %s\n", token);
        token = strtok(NULL, " ");

        if (strstr(token, ";"))
        {
            strncpy(table, token, strlen(token) - 1);
        }
        else
        {
            strncpy(table, token, strlen(token));
        }
        ////printf("4 %s\n", table);
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            *mode = 3;
            return 1;
        }
        // WHERE [nama_kolom]=[value];
        if (strcmp(token, "WHERE") == 0)
        {
            ////printf("5 %s\n", token);
            token = strtok(NULL, " ");
            ////printf("6 %s\n", token);
            char tmp[100];
            strcpy(tmp, token);
            char *token2 = strtok(tmp, "=");
            strcpy(search_column, token2);
            ////printf("7 %s\n", search_column);
            token2 = strtok(NULL, " ");

            strcpy(search_data, token2);
            if (search_data[0] == '\'')
            {
                memmove(search_data, search_data + 1, strlen(search_data));
                search_data[strlen(search_data) - 1] = '\0';
                search_data[strlen(search_data) - 1] = '\0';
            }
            else
            {
                search_data[strlen(search_data) - 1] = '\0';
            }

            ////printf("8 %s\n", search_data);
        }
        else
        {
            strcpy(message, "Invalid Syntax\n");

            return 0;
        }
        *mode = 4;
        return 1;
    }
}

void dropColumnForSelect(char *column_name, int position)
{
    char temp[200][1024] = {0};
    char hasil[200][1024] = {0};
    for (int i = 0; i < count_row; i++)
    {
        strcpy(temp[i], data_in_table[i]);
        ////printf("TEMP = > %s\n", temp[i]);
    }

    for (int i = 0; i < count_row; i++)
    {

        char *token = strtok(temp[i], "\t");
        int count = 0;
        while (count < count_column)
        {
            ////printf("count %d ; position %d\n", count, position);
            if (count == position)
            {
                ////printf("token -> %s\n", token);
                token = strtok(NULL, "\t");
                count++;
                continue;
            }
            else
            {
                // sidoarjo\n\t
                strcat(hasil[i], token);
                if (!strstr(hasil[i], "\n"))
                {
                    strcat(hasil[i], "\t");
                }
                ////printf("token -> %s\n", token);
                token = strtok(NULL, "\t");
                count++;
            }
        }

        if (!strstr(hasil[i], "\n"))
        {
            //printf("masuk");
            strncpy(hasil[i], hasil[i], strlen(hasil[i]) - 1);
            strcat(hasil[i], "\n");
        }
        //rintf("Hasil -> %s ---- %d", hasil[i], i);
    }
    for (int i = 0; i < count_row; i++)
    {
        strcpy(data_in_table[i], hasil[i]);
    }
    for (int i = 0; i < 100; i++)
    {
        data_exist[i] = -2;
    }
}

void stdoutSelect(int mode)
{
    char output[2048] = {0};
    int j = 0;
    int l = 0;
    char tmp[200][1024] = {0};
    char tmpcolumn[1024] = {0};
    char hasilcolumn[1024] = {0};
    char hasil[200][1024] = {0};
    for (int i = 0; i < count_row; i++)
    {
        strcpy(tmp[i], data_in_table[i]);
    }
    for (int i = -1; i < count_row; i++)
    {
        if (i == -1)
        {

            if (mode == 3 || mode == 4)
            {
                strcpy(tmpcolumn, column);
                char *token = strtok(tmpcolumn, "\t");
                int j = 0;
                int k = 0;
                while (token != NULL)
                {
                    if (j != position_column[k])
                    {
                        j++;
                        token = strtok(NULL, "\t");
                        continue;
                    }
                    else
                    {
                        strcat(hasilcolumn, token);
                        if (!strstr(hasilcolumn, "\n"))
                        {
                            strcat(hasilcolumn, "\t");
                        }
                        j++;
                        k++;
                        token = strtok(NULL, "\t");
                    }
                }
                if (!strstr(hasilcolumn, "\n"))
                {
                    strcat(hasilcolumn, "\n");
                }

                strcat(output, hasilcolumn);
            }
            else
            {
                strcat(output, column);
            }
        }
        else
        {
            if (mode == 1)
            {
                strcat(output, data_in_table[i]);
            }
            if (mode == 2)
            {
                if (i == data_exist[j])
                {
                    strcat(output, data_in_table[i]);
                    j++;
                }
                else
                {
                    continue;
                }
            }
            if (mode == 3)
            {
                char *token = strtok(tmp[i], "\t");
                int j = 0;
                int k = 0;
                while (token != NULL)
                {
                    if (j != position_column[k])
                    {
                        j++;
                        token = strtok(NULL, "\t");
                        continue;
                    }
                    else
                    {
                        strcat(hasil[i], token);
                        if (!strstr(hasil[i], "\n"))
                        {
                            strcat(hasil[i], "\t");
                        }
                        j++;
                        k++;
                        token = strtok(NULL, "\t");
                    }
                }
                if (!strstr(hasil[i], "\n"))
                {
                    strcat(hasil[i], "\n");
                }
                strcat(output, hasil[i]);
            }
            if (mode == 4)
            {
                char *token = strtok(tmp[i], "\t");
                int j = 0;
                int k = 0;

                while (token != NULL)
                {
                    if (j != position_column[k])
                    {
                        j++;
                        token = strtok(NULL, "\t");
                        continue;
                    }
                    else
                    {
                        strcat(hasil[i], token);
                        if (!strstr(hasil[i], "\n"))
                        {
                            strcat(hasil[i], "\t");
                        }
                        j++;
                        k++;
                        token = strtok(NULL, "\t");
                    }
                }
                if (!strstr(hasil[i], "\n"))
                {
                    strcat(hasil[i], "\n");
                }
                if (i == data_exist[l])
                {
                    strcat(output, hasil[i]);
                    l++;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    strcpy(message, output);
}

void selectCommand(int mode)
{
    if (mode == 1)
    {
        stdoutSelect(mode);
    }
    else if (mode == 2)
    {
        stdoutSelect(mode);
    }
    else if (mode == 3)
    {
        stdoutSelect(mode);
    }
    else if (mode == 4)
    {
        stdoutSelect(mode);
    }
}

void logging(char *input)
{
    // timestamp(yyyy-mm-dd hh:mm:ss):username:command
    //2021-05-19 02:05:15:jack:SELECT FROM table1
    FILE *f = fopen("database.log", "a+");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d:%s:%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, SESSION_USERNAME, input);
    fclose(f);
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
void selectionSort(int arr[], int n)
{
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++)
    {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}

int main()
{
    // Check User
    while (1)
    {
        checkUser("user", "password");
        printf(">> ");
        // Add User
        char input[1000] = {0};
        gets(input);
        input[strcspn(input, "\n")] = 0;
        logging(input);
        if (strncmp(input, "CREATE USER", 11) == 0)
        {
            char username[100] = {0};
            char password[100] = {0};
            if (checkCommandAddUser(input, username, password))
            {
                addUser(username, password);
            }
        }
        if (strncmp(input, "CREATE DATABASE", 15) == 0)
        {
            char database[100] = {0};
            if (checkCommandCreateDatabase(input, database))
            {
                createDatabase(database);
            }
        }
        if (strncmp(input, "GRANT PERMISSION", 16) == 0)
        {
            char username[100] = {0};
            char database[100] = {0};
            if (checkCommandGrantDatabase(input, username, database))
            {
                grantDatabase(username, database);
            }
        }
        if (strncmp(input, "USE", 3) == 0)
        {
            ////printf("masuk 4");
            char username[100] = {0};
            char database[100] = {0};
            if (checkCommandUseDatabase(input, database))
            {
                useDatabase(database);
            }
        }
        // CREATE TABLE [nama_tabel] ([nama_kolom] [tipe_data], ...);
        if (strncmp(input, "CREATE TABLE", 12) == 0)
        {
            char username[100] = {0};
            char column_name[100] = {0};
            char table[100];
            if (strcmp(SESSION_DATABASE, "") != 0)
            {

                if (checkCommandCreateTable(input, table, column_name))
                {
                    char fullpath[500] = {0};
                    sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                    // //printf("%s-----------", fullpath);
                    int check = readTableOfDatabase(fullpath, table);
                    if (check == 0)
                    {
                        createTable(table, column_name);
                    }
                    else
                    {
                        strcpy(message, "Table Already Used\n");
                    }
                }
            }
            else
            {
                strcpy(message, "Please Spesify Your Database\n");
            }
        }
        if (strncmp(input, "DROP DATABASE", 13) == 0)
        {
            char username[100] = {0};
            char database[100] = {0};
            if (checkCommandDropDatabase(input, database))
            {
                char data[2048] = {0};
                sprintf(data, "%s\t%s\n", SESSION_USERNAME, database);
                strcat(database, "\n");
                int check = checkDataExist(database, "databases/init/permission.table", 1);
                if (check)
                {
                    strtok(database, "\n");
                    dropDatabase(data, database);
                }
                else
                {
                    strcpy(message, "Database Not Found / You Not have Permission\n");
                }
            }
        }
        if (strncmp(input, "DROP TABLE", 10) == 0)
        {
            int check = 0;
            char table[100];
            check = checkCommandDropTable(input, table);
            if (check)
            {
                if (strcmp(SESSION_DATABASE, "") != 0)
                {
                    char fullpath[200];
                    sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                    if (readTableOfDatabase(fullpath, table))
                    {
                        dropTable(table);
                    }
                    else
                    {
                        strcpy(message, "Table Not Found\n");
                    }
                }
                else
                {
                    strcpy(message, "Please Spesify Your Database\n");
                }
            }
        }
        if (strncmp(input, "DROP COLUMN", 11) == 0)
        {
            int check = 0;
            char column_name[100] = {0};
            char table[100] = {0};
            check = checkCommandDropColumn(input, column_name, table);
            if (check)
            {
                if (strcmp(SESSION_DATABASE, "") != 0)
                {

                    // readDatabase
                    readDatabase(SESSION_DATABASE, table);
                    countColumn();
                    // //printf("DEBUG -> %d %d ", count_column, count_row);
                    char fullpath[400];
                    sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                    if (strstr(column, column_name))
                    {

                        if (readTableOfDatabase(fullpath, table))
                        {
                            // menemukan posisi kolom

                            int position = positionCloumn(column_name);
                            // //printf("DEBUG -> %d %d %d", count_column, count_row, position);
                            // melakukan write
                            dropColumn(position);
                            dropColumn2(position);
                            // char fullpath[400];
                            sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
                            // //printf("fullpath -> %s", fullpath);
                            writeTable(fullpath);
                            strcpy(message, "Success\n");

                            // //printf("tahap pengembangan -> %d", position);
                        }
                        else
                        {
                            strcpy(message, "Table Not Found\n");
                        }
                    }
                    else
                    {
                        strcpy(message, "Column Not Found\n");
                    }
                }
                else
                {
                    strcpy(message, "Please Spesify Your Database\n");
                }
            }
        }
        if (strncmp(input, "INSERT INTO", 11) == 0)
        {

            char column_name[100] = {0};
            char table[100];
            int total = 0;
            int check = checkCommandInsert(input, column_name, table, &total);
            char fullpath[200];
            sprintf(fullpath, "databases/%s", SESSION_DATABASE);
            if (strcmp(SESSION_DATABASE, "") != 0)
            {
                char fullpath[200];
                sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                if (readTableOfDatabase(fullpath, table))
                {
                    if (check)
                    {
                        readDatabase(SESSION_DATABASE, table);
                        countColumn();
                        // //printf("masuk %d %d", total - 1, count_column);
                        if (count_column == total - 1)
                        {
                            // //printf("masuk");

                            insert(column_name, table);
                        }
                        else
                        {
                            strcpy(message, "too many or too few columns\n");
                        }
                    }
                }
                else
                {
                    strcpy(message, "Table Not Found\n");
                }
            }
            else
            {
                strcpy(message, "Please Spesify Your Database\n");
            }
        }
        if (strncmp(input, "DELETE FROM", 11) == 0)
        {
            char table[100] = {0};
            char column_name[100] = {0};
            char data_column[100] = {0};
            // mode 1 = DELETE semua, mode 2 Delete sesuai WHERE
            int mode;

            if (strcmp(SESSION_DATABASE, "") != 0)
            {
                int check = checkCommandDelete(input, table, column_name, data_column, &mode);
                if (check)
                {
                    char fullpath[400] = {0};
                    sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);

                    char fullpath2[400] = {0};
                    sprintf(fullpath2, "databases/%s", SESSION_DATABASE);
                    readDatabase(SESSION_DATABASE, table);
                    countColumn();
                    int position = positionCloumn(column_name);
                    ////printf("\nDEBUG -> %d %d %d\n", count_column, count_row, position);
                    if (strstr(column, column_name))
                    {
                        if (readTableOfDatabase(fullpath2, table))
                        {
                            if (count_column == position + 1)
                            {
                                strcat(data_column, "\n");
                            }

                            checkDataExist(data_column, fullpath, position);
                            ////printf("data exist %d ", count_data_exist);
                            if (count_data_exist != 0)
                            {
                                deleteData(mode, position, fullpath);
                            }
                            else
                            {
                                strcpy(message, "Data Not Exist\n");
                            }
                        }
                        else
                        {
                            strcpy(message, "Table Not Found\n");
                        }
                    }
                    else
                    {
                        strcpy(message, "Column Not Found\n");
                    }
                }
            }
            else
            {
                strcpy(message, "Please Spesify Your Database\n");
            }
        }
        // UPDATE [nama_tabel] SET [nama_kolom]=[value];
        if (strncmp(input, "UPDATE", 6) == 0)
        {
            char column_name[100] = {0};
            char data_column[100] = {0};
            char search_column[100] = {0};
            char search_data[100] = {0};
            int mode;
            char table[100];
            int total = 0;

            if (strcmp(SESSION_DATABASE, "") != 0)
            {
                int check = checkCommandUpdate(input, table, data_column, column_name, search_column, search_data, &mode);
                if (check)
                {
                    readDatabase(SESSION_DATABASE, table);
                    countColumn();
                    if (strstr(column, column_name))
                    {
                        char fullpath[200];
                        sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                        if (readTableOfDatabase(fullpath, table))
                        {
                            ////printf("masuk %d %d", total - 1, count_column);
                            ////printf("masuk");
                            int position = positionCloumn(column_name);
                            int position_search = positionCloumn(search_column);
                            char fullpath[400];
                            sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
                            if (position_search + 1 == count_column)
                            {
                                strcat(search_data, "\n");
                            }

                            checkDataExist(search_data, fullpath, position_search);
                            ////printf("\nDEBUG -> %d %d %d %d\n", count_column, count_row, position, count_data_exist);
                            ////printf("\n DEBUG %d", data_exist[0]);
                            updateData(mode, position, data_column, search_column, search_data);
                            memset(data_exist, -2, sizeof(data_exist));
                            writeTable(fullpath);
                        }
                        else
                        {
                            strcpy(message, "Table Not Found\n");
                        }
                    }
                    else
                    {
                        strcpy(message, "Column Not Found\n");
                    }
                }
            }
            else
            {
                strcpy(message, "Please Spesify Your Database\n");
            }
        }
        if (strncmp(input, "SELECT", 6) == 0)
        {
            char search_column[100] = {0};
            char search_data[100] = {0};
            int mode;
            int countColumnSelect = 0;
            char table[100];
            if (strcmp(SESSION_DATABASE, "") != 0)
            {
                int check = checkCommandSelect(input, table, search_column, search_data, &mode, &countColumnSelect);
                if (check)
                {
                    readDatabase(SESSION_DATABASE, table);
                    countColumn();
                    int check = 1;
                    for (int i = 0; i < countColumnSelect; i++)
                    {
                        if (!strstr(column, selectColumnName[i]))
                        {
                            check = 0;
                        }
                    }
                    if (check)
                    {
                        int check2 = 1;
                        if (mode == 2 || mode == 4)
                        {
                            if (!strstr(column, search_column))
                            {
                                check2 = 0;
                            }
                        }
                        if (check2)
                        {
                            char fullpath[200];
                            sprintf(fullpath, "databases/%s", SESSION_DATABASE);
                            ////printf("-------%s-----", fullpath);
                            if (readTableOfDatabase(fullpath, table))
                            {
                                ////printf("Count -> %d\n", countColumnSelect);
                                int position_search = positionCloumn(search_column);

                                for (int i = 0; i < countColumnSelect; i++)
                                {
                                    position_column[i] = positionCloumn(selectColumnName[i]);
                                    ////printf("\n Tes %s -   > %d\n", selectColumnName[i], position_column[i]);
                                }

                                selectionSort(position_column, countColumnSelect);
                                for (int i = 0; i < countColumnSelect; i++)
                                {
                                    ////printf("\n Tes %s -   > %d\n", selectColumnName[i], position_column[i]);
                                }
                                char fullpath[400];
                                ////printf("FULLPATH %s", fullpath);
                                sprintf(fullpath, "databases/%s/%s", SESSION_DATABASE, table);
                                if (position_search + 1 == count_column)
                                {
                                    strcat(search_data, "\n");
                                }

                                checkDataExist(search_data, fullpath, position_search);
                                selectCommand(mode);
                            }
                            else
                            {
                                strcpy(message, "Table Not Found\n");
                            }
                        }
                        else
                        {
                            strcpy(message, "Column Not Found\n");
                        }
                    }

                    else
                    {
                        strcpy(message, "Column Not Found\n");
                    }
                }
            }
            else
            {
                strcpy(message, "Please Spesify Your Database\n");
            }
        }
        reset();
        printf("%s", message);
        strcpy(message, "");
    }
    return 0;
}