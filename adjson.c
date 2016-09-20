#include "adjson.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

typedef struct {
	const char *json;
}ad_context;

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json ++;} while(0)
#define ISDIGIT(ch) ((ch) >= '0' &&(ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' &&(ch) <= '9')
#define ISDOT(ch) ((ch) == '.')
#define ISE(ch) ((ch) == 'E' || (ch) == 'e')
#define ISPOSITIVE(ch) ((ch) == '+')
#define ISNEGATIVE(ch) ((ch) == '-')

static void ad_parse_whitespace(ad_context* c){
	const char *p = c->json;
	while( *p == ' ' || *p == '\t' || *p == 'r' || *p == '\n')
		p++;
	c->json = p;	
}


/*
	state-->0 init
		
		1 number(without e, dot)
		2 dot
		3 number with dot
		4 e 
		5 number with e
		6 dot after e
		7 number after e after dot
		8 start with 0
		10 negitave after e
		11 positave after e
*/
int 
ad_parse_literal(ad_context *c, ad_value *v, const char* com, int length, int match_value){
	int rc = 0;

	rc = strncmp(c->json, com, length);
	if (rc != 0){
		return AD_PARSE_INVALID_VALUE;
	}
	c->json += length;
	v->type = match_value;
	return AD_PARSE_OK;

}


static int ad_parse_number(ad_context *c, ad_value *v){
	const char *p = c->json;
	if (*p == '-') p++;
	if (*p == '0') p++;
	else{
		if (!ISDIGIT1TO9(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}
	if (*p == '.'){
		p ++;
		if (!ISDIGIT(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}
	if (*p == 'e' || *p == 'E' ){
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!ISDIGIT(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}

	errno = 0;
	v->n = strtod(c->json, NULL);
	if (errno == ERANGE && v->n == HUGE_VAL){
		return AD_PARSE_NUMBER_TOO_BIG;
	}
	
	c->json = p;
	v->type = AD_NUMBER;
	return AD_PARSE_OK;
}

int 
ad_parse_null(ad_context * c, ad_value *v){
	return ad_parse_literal(c, v, "null", 4, AD_NULL);
}

int 
ad_parse_true(ad_context *c, ad_value *v){
	return ad_parse_literal(c, v, "true", 4, AD_TRUE);
}

int
ad_parse_false(ad_context *c, ad_value *v){
	return ad_parse_literal(c, v, "false", 5, AD_FALSE);
}

int 
ad_parse_value(ad_context * c, ad_value *v){
	switch(*c->json){
		case 'n' : return ad_parse_literal(c,v,"null", 4, AD_NULL);
		case 't' : return ad_parse_literal(c,v,"true", 4, AD_TRUE);
		case 'f' : return ad_parse_literal(c,v,"false", 5, AD_FALSE);
		default : return ad_parse_number(c, v);
		case '\0' : return AD_PARSE_EXPECT_VALUE;

	}
}


int
ad_parse(ad_value *v, const char* json){
        ad_context c;
	int ret = 0;
        assert( v != NULL);
        c.json = json;
        v->type = AD_NULL;
        ad_parse_whitespace(&c);
        if( (ret = ad_parse_value(&c, v)) == AD_PARSE_OK){
		ad_parse_whitespace(&c);
		if (*c.json != '\0'){
			v->type = AD_NULL;
			ret = AD_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	return ret;
}

ad_type
ad_get_type(const ad_value* v){
	assert(v != NULL);
        return v->type;
}

double 
ad_get_number(const ad_value *v){
	assert(v != NULL && v->type == AD_NUMBER);
	return v->n;
}
