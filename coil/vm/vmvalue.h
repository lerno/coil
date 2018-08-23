#pragma once


#include "common.h"

typedef enum
{
	VAL_BOOL,
	VAL_NIL,
	VAL_FLOAT,
	VAL_INT
} ValueType;

typedef struct _Value
{
  ValueType type;
  union
  {
    bool boolean;
    double f64;
    int64_t i64;
  } as;
} Value;


#define BOOL_VAL(_value) ((Value){ VAL_BOOL, { .boolean = _value } })
#define NIL_VAL ((Value){ VAL_NIL, { .i64 = 0 } })
#define INT_VAL(_value) ((Value){ VAL_INT, { .i64 = _value } })
#define FLOAT_VAL(_value) ((Value){ VAL_FLOAT, { .f64= _value } })

#define FLOAT_CAST(_value) do { \
  switch (_value.type) { \
    case VAL_FLOAT: break; \
    case VAL_NIL: \
    case VAL_INT: _value.as.f64 = _value.as.i64; break; \
    case VAL_BOOL: _value.as.f64 = _value.as.boolean ? 1. : 0.; break; \
  } \
  _value.type = VAL_FLOAT; } while (0);

#define INT_CAST(_value) do { \
  switch (_value.type) { \
    case VAL_INT: break; \
    case VAL_FLOAT: _value.as.i64 = (int64_t)_value.as.f64; break; \
    case VAL_BOOL: _value.as.i64 = _value.as.boolean; break; \
    case VAL_NIL: break; \
  } \
  _value.type = VAL_INT; } while (0)

#define BOOL_CAST(_value) do { \
  switch (_value.type) { \
    case VAL_BOOL: break; \
    case VAL_INT: _value.as.boolean = _value.as.i64 != 0; \
    case VAL_FLOAT: _value.as.boolean = _value.as.f64 != 0.; break; \
    case VAL_NIL: _value.as.boolean = false; \
  } \
  _value.type = VAL_BOOL; } while (0)

#define AS_BOOL(__value) (__value).as.boolean
#define AS_FLOAT(__value) (__value).as.f64
#define AS_INT(__value) (__value).as.i64

#define IS_BOOL(_value) ((_value).type == VAL_BOOL)
#define IS_NIL(_value) ((_value).type == VAL_NIL)
#define IS_FLOAT(_value) ((_value).type == VAL_FLOAT)
#define IS_INT(_value) ((_value).type == VAL_INT)

typedef struct
{
	int size;
	int capacity;
	Value *values;
} VmValueArray;

void vm_value_array_init(VmValueArray *value_array);
void vm_value_write(VmValueArray *value_array, Value value);
void vm_value_free(VmValueArray *value_array);
void vm_value_print(Value value);
