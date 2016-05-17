#include "lmdbInterface.h"
#include "lmdb.h"
#include <stdio.h>
#include <string.h>

void getDataFromDB(char* envName, char* dbName, char keyVal, char **outputData)
{
    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_val key, data;
    MDB_txn *txn;
    MDB_cursor *cursor;
    char kval[MAX_KEY_ALLOCATE_SIZE];

    /* Note: Most error checking omitted for simplicity */

    rc = mdb_env_create(&env);
    mdb_env_set_mapsize(env, MAX_DB_SIZE);
    mdb_env_set_maxdbs(env, (MDB_dbi)10);
    rc = mdb_env_open(env, envName, 0, 0664);
    rc = mdb_txn_begin(env, NULL, 0, &txn);
    rc = mdb_dbi_open(txn, dbName, 0, &dbi);
    
    // rc = mdb_put(txn, dbi, &key, &data, 0);
    rc = mdb_txn_commit(txn);
    if (rc) {
        fprintf(stderr, "mdb_txn_commit: (%d) %s\n", rc, mdb_strerror(rc));
        goto leave;
    }

    rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    rc = mdb_cursor_open(txn, dbi, &cursor);

    kval[0] = keyVal;
    key.mv_size = sizeof(kval);
    key.mv_data = kval;
    data.mv_size = sizeof(MAX_DATA_ALLOCATE_SIZE);
    data.mv_data = *outputData;

    fprintf(stderr, "\ngetDataFromDB: key: %s, data: %s\n",(char *) key.mv_data,(char *) data.mv_data);
    rc = mdb_cursor_get(cursor, &key, &data, MDB_SET);
    fprintf(stderr, "\ngetDataFromDB: key: %s, data: %s\n",(char *) key.mv_data,(char *) data.mv_data);

    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
leave:
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}
