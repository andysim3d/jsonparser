#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
	do {\
		test_count ++;\
		if (equality) \
			test_pass ++;\
		else{\
			fprintf(stderr, "%s:%d: except: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
		main_ret = 1;\
		}\
	}while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
static void test_parse_null(){
	ad_value v;
	v.type = AD_TRUE;
	EXPECT_EQ_INT(AD_PARSE_OK, ad_parse(&v, "null"));
	EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));

}

static void test_parse_true(){
	ad_value v;
	v.type = AD_TRUE;
	EXPECT_EQ_INT(AD_PARSE_OK, ad_parse(&v, "true"));
	EXPECT_EQ_INT(AD_TRUE, ad_get_type(&v));
}

static void test_parse_false(){
	ad_value v;
	v.type = AD_TRUE;
	EXPECT_EQ_INT(AD_PARSE_OK, ad_parse(&v, "false "));
	EXPECT_EQ_INT(AD_FALSE, ad_get_type(&v));
}


static void test_parse_expect_value() {
	ad_value v;
	v.type = AD_FALSE;
	EXPECT_EQ_INT(AD_PARSE_INVALID_VALUE, ad_parse(&v, "nul "));
	EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));

	v.type = AD_FALSE;
	EXPECT_EQ_INT(AD_PARSE_INVALID_VALUE, ad_parse(&v, "nul"));
	EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));

}

static void test_parse_invalid_value() {

}

static void test_parse_root_not_singular() {

        ad_value v;
        v.type = AD_FALSE;
        EXPECT_EQ_INT(AD_PARSE_ROOT_NOT_SINGULAR, ad_parse(&v, "null x"));
        EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));

}

static void test_parse(){
	test_parse_null();
	test_parse_false();
	test_parse_true();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
}

int main(){
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}

