#include <stdio.h>
#include "lmdb.h"
#include <string.h>
#include "lmdbInterface.h"

#define DB_ENV "./demoDB"
#define DB_NAME "what"

int main(int argc,char * argv[])
{
    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_val key, data;
    MDB_txn *txn;
    MDB_cursor *cursor;
    char sval[MAX_DATA_ALLOCATE_SIZE];

    /* Note: Most error checking omitted for simplicity */

    rc = mdb_env_create(&env);
    mdb_env_set_mapsize(env, MAX_DB_SIZE);
    mdb_env_set_maxdbs(env, (MDB_dbi)10);
    rc = mdb_env_open(env, DB_ENV, 0, 0664);
    rc = mdb_txn_begin(env, NULL, 0, &txn);
    rc = mdb_dbi_open(txn, DB_NAME, MDB_CREATE, &dbi);

    
    // key.mv_size = sizeof(int);
    // key.mv_data = sval;
    // data.mv_size = sizeof(sval);
    // data.mv_data = sval;
    // memset(sval, 65, sizeof(sval));
    // rc = mdb_put(txn, dbi, &key, &data, 0);
    // rc = mdb_txn_commit(txn);
    // if (rc) {
    //     fprintf(stderr, "mdb_txn_commit: (%d) %s\n", rc, mdb_strerror(rc));
    //     goto leave;
    // }

    for (int i = 65; i < 65+29; i++) {
        memset(sval, i, sizeof(sval));

        key.mv_size = MAX_KEY_ALLOCATE_SIZE;
        key.mv_data = sval;
        data.mv_size = sizeof(sval);
        data.mv_data = sval;
        memset(sval, i, sizeof(sval));
        rc = mdb_put(txn, dbi, &key, &data, 0);
        // rc = mdb_txn_commit(txn);
    }
     rc = mdb_txn_commit(txn);
    if (rc) {
        fprintf(stderr, "mdb_txn_commit: (%d) %s\n", rc, mdb_strerror(rc));
        goto leave;
    }
    fprintf(stderr, "print out data:\n");
    rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    rc = mdb_cursor_open(txn, dbi, &cursor);
    while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
        // fprintf(stderr, "key: %s, data: %s\n",(char *) key.mv_data,(char *) data.mv_data);
        fprintf(stderr, "data: %s\n",(char *) data.mv_data);
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
leave:
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
    return 0;
}
