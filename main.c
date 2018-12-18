#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <HashTable.h>

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

    BF_Init();

    /* Create hash index on id */
    if (HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3) < 0) {
        fprintf(stderr, "Error creating hash index.\n");
        exit(EXIT_FAILURE);
    }

    /* Open hash index based on id */
    if ((headerInfo = HT_OpenIndex(FILENAME)) == NULL) {
        fprintf(stderr, "Error opening hash index.\n");
        HT_CloseIndex(headerInfo);
        exit(EXIT_FAILURE);
    }

    printf("Insert Entries\n");
    for (int id = 0; id < 7; ++id) {

        /* Insert record in hash index based on id */
        if (HT_InsertEntry(*headerInfo, getRandomRecord(id)) < 0) {
            fprintf(stderr, "Error inserting entry in hash index\n");
            HT_CloseIndex(headerInfo);
            exit(EXIT_FAILURE);
        }
    }

    /* Close id hash index */
    if (HT_CloseIndex(headerInfo) < 0) {
        fprintf(stderr, "Error closing id hash index.\n");
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

    srand(12569874);
    record.id = id;
    r = rand() % 10;
    memcpy(record.name, names[r], strlen(names[r]) + 1);
    r = rand() % 10;
    memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
    r = rand() % 10;
    memcpy(record.address, address[r], strlen(address[r]) + 1);

    return record;
}

//int main(int argc, char** argv) {
//    int bfs [MAX_FILES];
//    int i, j;
//    char filename [5];
//    void* block;
//    int blkCnt;
//
//    BF_Init();
//    strcpy(filename, FILENAME);
//    for (i = 0; i < MAX_FILES; i++) {
//        printf("File %s\n", filename);
//        if (BF_CreateFile(filename) < 0) {
//            BF_PrintError("Error creating file");
//            exit(EXIT_FAILURE);
//        }
//        if ((bfs[i] = BF_OpenFile(filename)) < 0) {
//            BF_PrintError("Error opening file");
//            break;
//        }
//        for (j = 0; j < MAX_BLOCKS; j++) {
//
//            printf("Block %d\n", j);
//            if (BF_AllocateBlock(bfs[i]) < 0) {
//                BF_PrintError("Error allocating block");
//                break;
//            }
//
//            blkCnt = BF_GetBlockCounter(bfs[i]);
//            printf("File %d has %d blocks\n", bfs[i], blkCnt);
//
//            if (BF_ReadBlock(bfs[i], j, &block) < 0) {
//                BF_PrintError("Error getting block");
//                break;
//            }
//            strncpy(block, (char*)&j, sizeof(int));
//            if (BF_WriteBlock(bfs[i], j) < 0) {
//                BF_PrintError("Error writing block back");
//                break;
//            }
//        }
//        filename[0]++;
//
//        if (BF_ReadBlock(bfs[i], 0, &block) < 0) {
//            BF_PrintError("Error getting block");
//            break;
//        }
//
//        if (BF_CloseFile(bfs[i]) < 0) {
//            BF_PrintError("Error closing file");
//            break;
//        }
//
//    }
//    return 0;
//}
