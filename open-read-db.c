/* sample-mdb.txt - MDB toy/sample
 *
 * Do a line-by-line comparison of this and sample-bdb.txt
 */
/*
 * Copyright 2012-2016 Howard Chu, Symas Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
#include <stdio.h>
#include <string.h>
#include "lmdb.h"

int main(int argc,char * argv[])
{
    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_val key, data;
    MDB_txn *txn;
    MDB_cursor *cursor;
    char sval[32];

    /* Note: Most error checking omitted for simplicity */

    rc = mdb_env_create(&env);
    mdb_env_set_maxdbs(env, (MDB_dbi)10);
    rc = mdb_env_open(env, "./demoDB", 0, 0664);
    rc = mdb_txn_begin(env, NULL, 0, &txn);
    rc = mdb_dbi_open(txn, NULL, 0, &dbi);
    if (rc) {
        fprintf(stderr, "fail to open db: (%d) %s\n", rc, mdb_strerror(rc));
        goto leave;
    }
    key.mv_size = sizeof(int);
    int keyVal = 1;
    key.mv_data = &keyVal;

    data.mv_size = sizeof(sval);
    memset(sval, 0, 32);
    data.mv_data = sval;
    mdb_get(txn, dbi, &key, &data);
    fprintf(stderr, "demo open&read: %d %s",*(int*)key.mv_data, (char*)sval);
    mdb_txn_abort(txn);
leave:
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
    return 0;
}
