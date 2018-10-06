#pragma once

#include "common.h"
#include "vmvalue.h"

typedef enum
{
	OBJ_STRING
} ObjType;


struct sObj
{
	ObjType type;
	struct sObj *next;
};

struct sObjString
{
	Obj obj;
	int length;
	char *chars;
};


void print_object(Value value);

ObjString *take_string(char *chars, int length);
ObjString *copy_string(const char* chars, int length);

static inline bool isObjType(Value value, ObjType type)
{
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#define OBJ_TYPE(_value) (AS_OBJ(_value)->type)
#define IS_STRING(_value) isObjType(_value, OBJ_STRING)
#define AS_STRING(_value) ((ObjString *)AS_OBJ(_value))
#define AS_CSTRING(_value) (((ObjString *)AS_OBJ(_value))->chars)
