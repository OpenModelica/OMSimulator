/* This header is used to generate the FMU test DLL and in the C API test that uses the DLL */
#ifndef FMU2_MODEL_DEFINES_H_

/*#define STRINGIFY(a)			#a
#define STRINGIFY2(a)			STRINGIFY(a)
#define MODEL_IDENTIFIER_STR	STRINGIFY2(MODEL_IDENTIFIER) */

#define N_STATES				0
#define N_EVENT_INDICATORS		0
#define N_REAL					2
#define N_INTEGER				0
#define N_BOOLEAN				0
#define N_STRING				0

#define N_INPUT_REAL			1 /* CS only */
#define N_INPUT_REAL_MAX_ORDER	1 /* CS only */
#define N_OUTPUT_REAL			1 /* CS only */
#define N_OUTPUT_REAL_MAX_ORDER	1 /* CS only */

#define FMI_VERSION			"2.0"

#define FMI_GUID			"{0b2f3b44-2334-44ab-ba6a-53bcd986cd34}"

#define BUFFER					1024
#define MAGIC_TEST_VALUE		13.0	/* A test value for some functions  */

/* Sizes */



#endif /* End of header FMU2_MODEL_DEFINES_H_ */
