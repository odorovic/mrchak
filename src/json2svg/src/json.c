#include "json.h"

#include <stdlib.h>

struct objCallbackLst_t {
	objectCallback callback;
	struct objCallbackLst_t *next;
}; 

typedef struct objCallbackLst_t objCallbackLst;

static objCallbackLst *callbacks = NULL;

void addCallback (objectCallback callback)
{
	objCallbackLst *lst = (objCallbackLst *)  calloc (sizeof (objCallbackLst), 1);
	lst->next = callbacks;
	callbacks = lst;
	lst->callback = callback;
}

static void testCallback (char *str, members *node)
{
	objCallbackLst *lst = callbacks;
	while (lst){
		if (!strcmp (str, lst->callback.event))
			lst->callback.fn(node->pair->val);	
		lst = lst->next;
	}
}

static void recurseValue (value *node, int level);

static void recurseObject (members *node, int level)
{
	if (!node)
		return ;

	testCallback (node->pair->str, node);

	recurseValue (node->pair->val, level);

	if (node->next){
		recurseObject (node->next, level);
	}
}

static void recurseArray (elements *node, int level)
{
	if (!node)
		return ;

	recurseValue (node->val, level);

	if (node->next){
		recurseArray (node->next, level);
	}
}


static void recurseValue (value *node, int level)
{
	if (!node)
		return ;
	
	switch (node->type){
		case VAL_STRING: break;
		case VAL_NUMBER: break;
		case VAL_OBJECT:
			recurseObject (node->val.obj, level+1);
			break;
		case VAL_ARRAY:
			recurseArray (node->val.arr, level+1);
			break;
		case VAL_TRUE: break;
		case VAL_FALSE:	break;
		case VAL_NULL: break;
	}
}

void parseTree (value *tree)
{
	recurseValue (tree, 0);
}
