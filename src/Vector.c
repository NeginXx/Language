#include "../include/StackTrace.h"
#include "../include/Vector.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

enum Error {
	kNoError,
	kNotEnoughMemory,
	kArrInitFailed,
	kArrReallocFailed,
	kBufOverflow
};

int NewNamesBuf(NamesBuf** vec) {
	$;
	NamesBuf names = {};
	names.buf = (char*)calloc(names.capacity, sizeof(char));
	if (names.buf == NULL) {
		RETURN kNotEnoughMemory;
	}

	*vec = (NamesBuf*)calloc(1, sizeof(NamesBuf));
	memcpy(*vec, &names, sizeof(names));

	RETURN 0;
}

int NamesBufPushback(NamesBuf* vec, const char* str, size_t str_len) {
	$;
	assert(vec != NULL);
	assert(str != NULL);

	// str_len + 1 because we also gonna write \0 in buffer
	while (vec->sz + str_len + 1 > vec->capacity) {
		RETURN kBufOverflow;
	}
	vec->last_str = vec->buf + vec->sz;
	memcpy(vec->buf + vec->sz, str, str_len);
	*(vec->buf + vec->sz + str_len) = '\0';
	vec->sz += str_len + 1;

	RETURN 0;
}

int NewVector(Vector** vec, size_t type_sz) {
	$;
	assert(vec != NULL);
	assert(*vec == NULL);

	Vector vector;
	vector.arr = calloc(vector.mincapacity, type_sz);
	if (vector.arr == NULL) {
		RETURN kArrInitFailed;
	}
	vector.capacity = vector.mincapacity;

	*vec = (Vector*)calloc(1, sizeof(Vector));
	memcpy(*vec, &vector, sizeof(vector));

	RETURN 0;
}

int VectorConstruct(Vector* vec, size_t type_sz) {
	$;
	assert(vec != NULL);

	vec->arr = calloc(vec->mincapacity, type_sz);
	if (vec->arr == NULL) {
		RETURN kArrInitFailed;
	}
	vec->capacity = vec->mincapacity;

	RETURN 0;
}

int VectorRealloc(Vector* vec, size_t type_sz) {
	$;
	assert(vec != NULL);

	void* oldptr = vec->arr;
	vec->capacity *= vec->increase_capacity;
	vec->arr = realloc(vec->arr, vec->capacity * type_sz);
	if (vec->arr == NULL) {
		vec->arr = oldptr;
		RETURN kArrReallocFailed;
	}

	RETURN 0;
}

int VectorPushback(Vector* vec, const void* val, size_t type_sz) {
	$;
	assert(vec != NULL);
	assert(val != NULL);

	if (vec->sz >= vec->capacity) {
		int error = VectorRealloc(vec, type_sz);
		if (error != 0) {
			RETURN error;
		}
	}
	memcpy((char*)vec->arr + vec->sz * type_sz, val, type_sz);
	++vec->sz;

	RETURN 0;
}

void* VectorGet(Vector* vec, size_t idx, size_t type_sz) {
	assert(vec != NULL);
	return (char*)vec->arr + idx * type_sz;
}

void* VectorBack(Vector* vec, size_t type_sz) {
	assert(vec != NULL);
	assert(vec->sz >= 1);
	return (char*)vec->arr + (vec->sz - 1) * type_sz;
}

void VectorDestroy(Vector* vec) {
	$;
	free(vec->arr);
	memset(vec, 0, sizeof(vec));
	$$;
}