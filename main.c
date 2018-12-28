#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <HashTable.h>
#include <time.h>

#include "include/BF.h"

#define FILENAME "file"

const char *names[] = {
        "Yannis",
        "Christofos",
        "Sofia",
        "Marianna",
        "Vagelis",
        "Maria",
        "Iosif",
        "Dionisis",
        "Konstantina",
        "Theofilos"
};

const char *surnames[] = {
        "Ioannidis",
        "Svingos",
        "Karvounari",
        "Rezkalla",
        "Nikolopoulos",
        "Berreta",
        "Koronis",
        "Gaitanis",
        "Oikonomou",
        "Mailis"
};

const char *address[] = {
        "Athens",
        "San Francisco",
        "Los Angeles",
        "Amsterdam",
        "London",
        "New York",
        "Tokyo",
        "Hong Kong",
        "Munich",
        "Miami"
};

Record getRandomRecord(int id);

int main(int argc, char **argv) {
    HT_info *headerInfo;
    int hashIndex;
    BF_Init();

    /* Create hash index on id */
    if (HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3) < 0) {
        fprintf(stderr, "Error creating file.\n");
        exit(EXIT_FAILURE);
    }

    /* Open hash index based on id */
    if ((headerInfo = HT_OpenIndex(FILENAME)) == NULL) {
        fprintf(stderr, "Error opening file.\n");
        HT_CloseIndex(headerInfo);
        exit(EXIT_FAILURE);
    }

    printf("Insert Entries\n");
    for (int id = 0; id < 10; ++id) {

        /* Insert record in hash index based on id */
        hashIndex = HT_InsertEntry(*headerInfo, getRandomRecord(id));
        if ( hashIndex < 0) {
            fprintf(stderr, "Error inserting entry in file\n");
            HT_CloseIndex(headerInfo);
            exit(EXIT_FAILURE);
        }
    }

    int value = 0;
    int num = HT_GetAllEntries(*headerInfo, &value);

    if (HT_DeleteEntry(*headerInfo, &value) < 0) {
        fprintf(stderr, "Error delete entry.\n");
        exit(EXIT_FAILURE);
    }

//    HT_GetAllEntries(*headerInfo, &value);
//
//    hashIndex = HT_InsertEntry(*headerInfo, getRandomRecord(0));
//    if ( hashIndex < 0) {
//        fprintf(stderr, "Error inserting entry in file\n");
//        HT_CloseIndex(headerInfo);
//        exit(EXIT_FAILURE);
//    }
//
//    HT_GetAllEntries(*headerInfo, &value);

    /* Close id hash index */
    if (HT_CloseIndex(headerInfo) < 0) {
        fprintf(stderr, "Error closing file.\n");
        exit(EXIT_FAILURE);
    }

    /* ** Print blocks to see content */

    return EXIT_SUCCESS;
}

int HashStatistics(char *filename) {
    return 0;
};

Record getRandomRecord(int id) {
    Record record;
    int r;

    srand ((unsigned int) clock());
    record.id = id;
    r = rand() % 10;
    memcpy(record.name, names[r], strlen(names[r]) + 1);
    r = rand() % 10;
    memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
    r = rand() % 10;
    memcpy(record.address, address[r], strlen(address[r]) + 1);

    return record;
}