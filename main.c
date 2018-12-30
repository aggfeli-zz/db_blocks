#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <HashTable.h>
#include "SecondaryHashTable.h"
#include <time.h>

#include "include/BF.h"

#define FILENAME "file"
#define SECONDARYFILENAME "secondaryfile"

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

SecondaryRecord getSecondaryRecord(Record record, int index);

int main(int argc, char **argv) {
    HT_info *headerInfo;
    SHT_info *sht_info;
    Record record;
    SecondaryRecord secondaryRecord;
    int hashIndex;
    BF_Init();

/**********************************************HashTable***************************************************************/
    /* Create hash index on id */
    if (HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3) < 0) {
        fprintf(stderr, "Error creating file.\n");
        exit(EXIT_FAILURE);
    }
/**********************************************************************************************************************/



/***************************************SecondaryHashTable*************************************************************/
    /* Create hash index on id */
    if (SHT_CreateSecondaryIndex(SECONDARYFILENAME, "name", sizeof(char*), 3) < 0) {
        fprintf(stderr, "Error creating secondary  file.\n");
        exit(EXIT_FAILURE);
    }
/**********************************************************************************************************************/

    printf("Insert Entries\n");
    for (int id = 0; id < 10; ++id) {

    /************************Insert HashTable**************************************************************************/
        if ((headerInfo = HT_OpenIndex(FILENAME)) == NULL) {
            fprintf(stderr, "Error opening file.\n");
            HT_CloseIndex(headerInfo);
            exit(EXIT_FAILURE);
        }

        /* Insert record in hash index based on id */
        record = getRandomRecord(id);
        hashIndex = HT_InsertEntry(*headerInfo, record);
        if ( hashIndex < 0) {
            fprintf(stderr, "Error inserting entry in file\n");
            HT_CloseIndex(headerInfo);
            exit(EXIT_FAILURE);
        }

        /* Close id hash index */
        if (HT_CloseIndex(headerInfo) < 0) {
            fprintf(stderr, "Error closing file.\n");
            exit(EXIT_FAILURE);
        }
    /******************************************************************************************************************/


    /**********************Insert Secondary HashTable******************************************************************/
        if ((sht_info = SHT_OpenSecondaryIndex(SECONDARYFILENAME)) == NULL) {
            fprintf(stderr, "Error opening secondary file.\n");
            SHT_CloseSecondaryIndex(sht_info);
            exit(EXIT_FAILURE);
        }

        /* Insert record in hash index based on id */
        secondaryRecord = getSecondaryRecord(record, hashIndex);
        hashIndex = SHT_SecondaryInsertEntry(*sht_info, secondaryRecord);
        if ( hashIndex < 0) {
            fprintf(stderr, "Error inserting entry in sec file\n");
            SHT_CloseSecondaryIndex(sht_info);
            exit(EXIT_FAILURE);
        }

        /* Close id hash index */
        if (SHT_CloseSecondaryIndex(sht_info) < 0) {
            fprintf(stderr, "Error closing sec file.\n");
            exit(EXIT_FAILURE);
        }
    /******************************************************************************************************************/

    }

/*******************Get and Delete from HashTable**********************************************************************/

    if ((headerInfo = HT_OpenIndex(FILENAME)) == NULL) {
        fprintf(stderr, "Error opening file.\n");
        HT_CloseIndex(headerInfo);
        exit(EXIT_FAILURE);
    }

    int value = 0;
    int numOfBlocksRead = HT_GetAllEntries(*headerInfo, &value);

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
/**********************************************************************************************************************/


/*******************Get and Delete from Secondary HashTable************************************************************/

    if ((sht_info = SHT_OpenSecondaryIndex(SECONDARYFILENAME)) == NULL) {
        fprintf(stderr, "Error opening secondary file.\n");
        SHT_CloseSecondaryIndex(sht_info);
        exit(EXIT_FAILURE);
    }

    int numOfBlocksReadInSecHashtable = SHT_SecondaryGetAllEntries(*sht_info, *headerInfo, "Vagelis");

    /* Close id hash index */
    if (SHT_CloseSecondaryIndex(sht_info) < 0) {
        fprintf(stderr, "Error closing secondary file.\n");
        exit(EXIT_FAILURE);
    }
/**********************************************************************************************************************/

    return EXIT_SUCCESS;
}

int HashStatistics(char *filename) {
    return 0;
};

SecondaryRecord getSecondaryRecord(Record record, int index) {
    SecondaryRecord secondaryRecord;
    secondaryRecord.record = record;
    secondaryRecord.blockId = index;
    return secondaryRecord;
}

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