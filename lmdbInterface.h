#ifndef MAX_DATA_ALLOCATE_SIZE

#define MAX_DATA_ALLOCATE_SIZE 1024*4096
// #define MAX_DATA_ALLOCATE_SIZE 32 //for debug
#define MAX_KEY_ALLOCATE_SIZE 1
#define MAX_DB_SIZE 104857600000
#endif

void getDataFromDB(char* envName, char* dbName, char keyVal, char **outputData);

