#include "adjson.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef struct {
	const char *json;
}ad_context;

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json ++;} while(0)

static void ad_parse_whitespace(ad_context* c){
	const char *p = c->json;
	while( *p == ' ' || *p == '\t' || *p == 'r' || *p == '\n')
		p++;
	c->json = p;	
}

int 
ad_parse_next_sep(ad_context *c){
	if(*c->json == '\0' || *c->json ==',' || *c->json ==':'){
		return AD_PARSE_OK;
	}
	return AD_PARSE_ROOT_NOT_SINGULAR;
}

int 
clean_up(ad_context *c){
	ad_parse_whitespace(c);
	return ad_parse_next_sep(c);
} 

int 
ad_parse_null(ad_context * c, ad_value *v){
	int rc = 0;
	EXPECT(c, 'n');
	rc = strncmp(c->json, "ull", 3);
	if (rc != 0){
                return AD_PARSE_INVALID_VALUE;
        }
	c->json += 3;
	v->type = AD_NULL;
	return clean_up(c);
}

int 
ad_parse_true(ad_context *c, ad_value *v){
	
	int rc = 0;
	EXPECT(c, 't');
	rc = strncmp(c->json, "rue", 3);
	if (rc != 0){
                return AD_PARSE_INVALID_VALUE;
        }
        c->json += 3;
        v->type = AD_TRUE;
	return clean_up(c);
}

int
ad_parse_false(ad_context *c, ad_value *v){
	
	int rc = 0;
	EXPECT(c, 'f');
	rc =  strncmp(c->json, "alse", 4);
	if (rc != 0){
                return AD_PARSE_INVALID_VALUE;
        }
        c->json += 4;
        v->type = AD_FALSE;

	return clean_up(c);
}

int 
ad_parse_value(ad_context * c, ad_value *v){
	switch(*c->json){
		case 'n' : return ad_parse_null(c, v);
		case 't' : return ad_parse_true(c, v);
		case 'f' : return ad_parse_false(c, v);

		case '\0' : return AD_PARSE_EXPECT_VALUE;
		default : return AD_PARSE_INVALID_VALUE;

	}
}



int
ad_parse(ad_value *v, const char* json){
        ad_context c;
        assert( v != NULL);
        c.json = json;
        v->type = AD_NULL;
        ad_parse_whitespace(&c);
        return ad_parse_value(&c, v);
}

ad_type
ad_get_type(const ad_value* v){
        return v->type;
}

