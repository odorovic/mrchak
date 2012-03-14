#ifndef JSON_H__
#define JSON_H__

#include <stdint.h>
#include <stdbool.h>

//Only integers are supported as numbers. XXX: no floating points
enum valueTypes {VAL_STRING, VAL_NUMBER, VAL_OBJECT, VAL_ARRAY, VAL_TRUE, VAL_FALSE, VAL_NULL};

typedef struct value_t value;
typedef struct elements_t elements;
typedef struct elements_t *array; //Array is just a pointer to first element
typedef struct pair_t pair;
typedef struct members_t members;
typedef struct members_t *object; //Object is just a pointer to first member

struct value_t {
	enum valueTypes type;
	union {
		char *str;
		int64_t num;
		object obj;
		array arr;
	} val;
};

struct elements_t {
	elements *prev, *next;
	value *val;
};

struct pair_t {
	char *str;
	value *val;
};
	
struct members_t {
	members *prev, *next;
	pair *pair;		
};

// Use this one to set up callbacks
struct objectCallback_t {
	char *event;
	void (*fn)(value *node);
} ;

typedef struct objectCallback_t objectCallback;

void addCallback (objectCallback callback);

//Invoke this one at the end of parsing
void parseTree (value *jsonTree);

//XXX: ALL MODULES MUST IMPLEMENT THIS FUNCTION
int work(value *jsonTree);

//SOME DEBUG INFO
#define YACC_FILE "yacc.log"

#endif
