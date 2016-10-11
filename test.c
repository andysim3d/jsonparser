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

#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%f")


#define EXPECT_EQ_STRING(expect, actual, alength)\
	EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")

#define TEST_ERROR(error, json)\
	do {\
		ad_value v;\
		ad_init(&v);\
		v.type = AD_FALSE;\
		EXPECT_EQ_INT(error, ad_parse(&v, json));\
		EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));\
		ad_free(&v);\
	} while(0)


#define TEST_NUMBER(expect, json)\
	do {\
		ad_value v;\
		ad_init(&v);\
		EXPECT_EQ_INT(AD_PARSE_OK, ad_parse(&v, json));\
		EXPECT_EQ_INT(AD_NUMBER, ad_get_type(&v));\
		EXPECT_EQ_DOUBLE(expect, ad_get_number(&v));\
		ad_free(&v);\
	} while(0)
		
#define TEST_STRING(expect, json)\
	do {\
		ad_value v;\
		ad_init(&v);\
		EXPECT_EQ_INT(AD_PARSE_OK, ad_parse(&v,json));\
		EXPECT_EQ_INT(AD_STRING, ad_get_type(&v));\
		EXPECT_EQ_STRING(expect, ad_get_string(&v), ad_get_string_length(&v));\
		ad_free(&v);\
	}while(0)

static void test_parse_number(){
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(-1e+10, "-1e+10");
	TEST_NUMBER(-1e+10, "-1e+10");
	TEST_NUMBER(1e-10, "1e-10");
	TEST_NUMBER(1e-10, "1e-10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-100000000");
}
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
	TEST_ERROR(AD_PARSE_EXPECT_VALUE, "");
	TEST_ERROR(AD_PARSE_EXPECT_VALUE, " ");

/*	ad_value v;

	v.type = AD_FALSE;
	EXPECT_EQ_INT(AD_PARSE_INVALID_VALUE, ad_parse(&v, "nul "));
	EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));

	v.type = AD_FALSE;
	EXPECT_EQ_INT(AD_PARSE_INVALID_VALUE, ad_parse(&v, "nul"));
	EXPECT_EQ_INT(AD_NULL, ad_get_type(&v));
*/
}

static void test_parse_invalid_value() {
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "?");

#if 1
    /* invalid number */
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(AD_PARSE_INVALID_VALUE, "nan");
#endif
}
static void test_parse_number_too_big() {
#if 1 
    TEST_ERROR(AD_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(AD_PARSE_NUMBER_TOO_BIG, "-1e309");
#endif
}




static void test_parse_root_not_singular() {
	TEST_ERROR(AD_PARSE_ROOT_NOT_SINGULAR, "null x");
#if 1 
    /* invalid number */
    TEST_ERROR(AD_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(AD_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(AD_PARSE_ROOT_NOT_SINGULAR, "0x123");
#endif

}

static void test_access_boolean() {
    ad_value v;
    ad_init(&v);
    ad_set_string(&v, "a", 1);
    ad_set_boolean(&v, 1);
    EXPECT_TRUE(ad_get_boolean(&v));
    ad_set_boolean(&v, 0);
    EXPECT_FALSE(ad_get_boolean(&v));
    ad_free(&v);
}

static void test_access_number() {
    ad_value v;
    ad_init(&v);
    ad_set_string(&v, "a", 1);
    ad_set_number(&v, 1234.5);
    EXPECT_EQ_DOUBLE(1234.5, ad_get_number(&v));
    ad_free(&v);
}


static void test_parse_string(){
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}
static void test_parse(){
	test_parse_null();
	test_parse_false();
	test_parse_true();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number();
	test_parse_number_too_big();
	test_parse_string();
	test_access_number();
	test_access_boolean();
}


int main(){
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}

