#ifndef WOOL_TABLE_H
#define WOOL_TABLE_H

#include "common.h"
#include "value.h"

typedef struct {
    RefString* key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool setTable(Table* table, RefString* key, Value value);
bool getTable(Table* table, RefString* key, Value* value);
bool delTable(Table* table, RefString* key);
void copyTable(Table* src, Table* dest);
RefString* findStringTable(Table* table, const char* chars, int length, uint32_t hash);

#endif
