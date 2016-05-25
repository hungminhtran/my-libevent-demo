all:
	#clear screen
	clear
	#build open reader db
	gcc open-read-db.c -o open-read-db.out -llmdb

	#build migrateDB
	gcc -std=c99 migrateDB.c  -o migrateDB.out -llmdb

	#build client
	gcc client.c -o client.out  

	#build server
	gcc server.c lmdbInterface.c -o server.out -levent -llmdb

	#build benchmark tool
	gcc -std=c99 benchmark.c -o benchmark.out -lpthread  

migrate:
	make all

	#run migrate data
	./migrateDB.out
test:
	make all

	#run migrate data
	./migrateDB.out

	#run server (lazy :">)
	./server.out 127.0.0.1
benchmark:
	make all
	./benchmark.out
