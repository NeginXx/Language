#pragma once
struct Vector {
	void* arr = NULL;
	size_t sz = 0;
	size_t capacity = 0;
	size_t mincapacity = 5;
	size_t increase_capacity = 2;
};
struct NamesBuf {
  char* buf = NULL;
  char* last_str = NULL;
  size_t sz = 0;
  size_t capacity = 1e6;
};
int NewNamesBuf(NamesBuf** vec);
int NamesBufPushback(NamesBuf* vec, const char* str, size_t str_len);

int NewVector(Vector** vec, size_t type_sz);
int VectorConstruct(Vector* vec, size_t type_sz);
int VectorPushback(Vector* vec, const void* val, size_t type_sz);
void VectorDestroy(Vector* vec);
void* VectorGet(Vector* vec, size_t idx, size_t type_sz);