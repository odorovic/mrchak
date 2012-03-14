#include <stdio.h>

#include "json.h"

void identLevel (int level)
{
	putchar ('\n');
	while (level--)
		putchar ('\t');
}

void recurseValue (value *node, int level);

void recurseObject (members *node, int level)
{
	if (!node)
		return ;

	identLevel (level);
	printf ("%s: ", node->pair->str);
	recurseValue (node->pair->val, level);

	if (node->next){
		printf (", ");
		recurseObject (node->next, level);
	}
}

void recurseArray (elements *node, int level)
{
	if (!node)
		return ;

	identLevel (level);
	recurseValue (node->val, level);

	if (node->next){
		printf (", ");
		recurseArray (node->next, level);
	}
}


void recurseValue (value *node, int level)
{
	if (!node)
		return ;
	
	switch (node->type){
		case VAL_STRING:
			printf ("%s",node->val.str);
			break;
		case VAL_NUMBER:
			printf ("%lld",(long long int) node->val.num);
			break;
		case VAL_OBJECT:
			putchar ('{');
			recurseObject (node->val.obj, level+1);
			identLevel (level); putchar ('}');
			break;
		case VAL_ARRAY:
			printf ("[");
			recurseArray (node->val.arr, level+1);
			identLevel (level); printf ("]");
			break;
		case VAL_TRUE:
			printf ("TRUE");
			break;
		case VAL_FALSE:
			printf ("FALSE");
			break;
		case VAL_NULL:	
			printf ("NULL");
			break;
	}
}

int work (value *jsonTree)
{
	recurseValue (jsonTree, 0);	
	return 0;
}


