#ifndef BASEIS_HASHTABLE_H
#define BASEIS_HASHTABLE_H

typedef struct{
    int id;
    char name[15];
    char surname[20];
    char address[40];
} Record;

typedef struct {
    int fileDesc;           /* αναγνωριστικός αριθμός ανοίγματος αρχείου από το επίπεδο block */
    char attrType;          /* ο τύπος του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο, 'c' ή'i' */
    char* attrName;         /* το όνομα του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο */
    int attrLength;         /* το μέγεθος του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο */
    long int numBuckets;    /* το πλήθος των “κάδων” του αρχείου κατακερματισμού */
} HT_info;


int HT_CreateIndex(  char *fileName,    /* όνομα αρχείου */
                    char attrType,      /* τύπος πεδίου-κλειδιού: 'c', 'i' */
                    char* attrName,     /* όνομα πεδίου-κλειδιού */
                    int attrLength,     /* μήκος πεδίου-κλειδιού */
                    int buckets         /* αριθμός κάδων κατακερματισμού*/
                    );

HT_info* HT_OpenIndex(char *fileName);

int HT_CloseIndex(HT_info* header_info );

int HT_InsertEntry(HT_info header_info, Record record);

int HT_DeleteEntry(HT_info header_info, void *value);

int HT_GetAllEntries(HT_info header_info,void *value);

#endif //BASEIS_HASHTABLE_H
