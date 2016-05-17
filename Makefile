all:
	#build open reader db
	gcc open-read-db.c -o open-read-db.out -llmdb
	#build migrateDB
	gcc -std=c99 migrateDB.c  -o migrateDB.out -llmdb
	#build client
	gcc client.c -o client.out  
	#build server
	gcc server.c lmdbInterface.c -o server.out -levent -llmdb