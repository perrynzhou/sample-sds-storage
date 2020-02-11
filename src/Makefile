all:
	gcc -g  -std=gnu99 -D_GNU_SOURCE  -DDEBUG_LOG -O0 slice.h slice.c utils.h utils.c \
		log.h log.c vector.h vector.c \
		hash.h hash.c list.h list.c bucket_store.h bucket_store.c  \
		bucket_object.h  conf.h conf.c  bucket_object.c bucket.h bucket.c bucketset.h bucketset.c netsocket.h netsocket.c sds_store.h sds_store.c \
		-o  test -lev -lpthread -ljansson
