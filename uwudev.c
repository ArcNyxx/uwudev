/* uwudev - scrunklificator
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STRLIT(str) { str, sizeof(str) - 1 }
#define LENGTH(array) (sizeof(array) / sizeof(array[0]))

typedef struct choice {
	const int next, prob;
	const char ch;
} choice_t;

typedef struct ngram {
	const choice_t *choice;
	const int prob;
} ngram_t;

typedef struct str {
	const char *str;
	const int len;
} str_t;

typedef struct repeat {
	int times;
	char ch;
} repeat_t;

typedef enum opcode {
	OP_NULL, OP_NGRAM, OP_STR, OP_REPEAT
} opcode_t;

typedef struct oper {
	union {
		struct {
			const ngram_t *ngram;
			int prev;
		};
		const char *str;
		char ch;
	};

	opcode_t op;
	int len;
} oper_t;

typedef struct uwu {
	oper_t op[3];
	int total, last;
} uwu_t;

static int runops(oper_t *op, char *buf);
static void genops(uwu_t *uwu);

static const choice_t catnoise0[]   = { { 0,  2,  'a' }, { 1,  3,  'm' } };
static const choice_t catnoise1[]   = { { 7,  3,  'r' }, { 6,  4,  'e' } };
static const choice_t catnoise2[]   = { { 8,  1,  'y' } };
static const choice_t catnoise3[]   = { { 9,  1,  'w' } };
static const choice_t catnoise4[]   = { { 9,  1,  'w' } };
static const choice_t catnoise5[]   = { { 21, 6,  'm' }, { 22, 7,  'n' },
      { 23, 8,  'p' } };
static const choice_t catnoise6[]   = { { 4,  3,  'o' }, { 5,  5,  'w' } };
static const choice_t catnoise7[]   = { { 16, 15, 'o' }, { 12, 24, 'a' },
      { 18, 28, 'r' }, { 19, 29, 'w' }, { 13, 30, 'e' } };
static const choice_t catnoise8[]   = { { 26, 1,  'a' } };
static const choice_t catnoise9[]   = { { 21, 22, 'm' }, { 24, 32, 'r' },
      { 22, 36, 'n' }, { 25, 37, 'w' }, { 23, 38, 'p' } };
static const choice_t catnoise10[]  = { { 11, 1,  'u' } };
static const choice_t catnoise11[]  = { { 20, 1,  'r' } };
static const choice_t catnoise12[]  = { { 3,  1,  'o' } };
static const choice_t catnoise13[]  = { { 4,  1,  'o' } };
static const choice_t catnoise14[]  = { { 7,  1,  'r' }, { 6,  2,  'e' } };
static const choice_t catnoise15[]  = { { 8,  1,  'y' } };
static const choice_t catnoise16[]  = { { 9,  1,  'w' } };
static const choice_t catnoise17[]  = { { 10, 1,  'p' } };
static const choice_t catnoise18[]  = { { 18, 7,  'r' }, { 14, 10, 'm' },
      { 16, 13, 'o' }, { 17, 14, 'p' } };
static const choice_t catnoise19[]  = { { 21, 1,  'm' } };
static const choice_t catnoise20[]  = { { 18, 1,  'r' } };
static const choice_t catnoise21[]  = { { 7,  17, 'r' }, { 6,  30, 'e' } };
static const choice_t catnoise22[]  = { { 8,  1,  'y' } };
static const choice_t catnoise23[]  = { { 11, 1,  'u' } };
static const choice_t catnoise24[]  = { { 14, 7,  'm' }, { 15, 10, 'n' } };
static const choice_t catnoise25[]  = { { 25, 3,  'w' }, { 21, 4,  'm' } };
static const choice_t catnoise26[]  = { { 0,  4,  'a' }, { 2,  5,  'n' },
      { 1,  6,  'm' } };

static const ngram_t catnoise[] = {
        { catnoise0,  3  }, /* aa */ { catnoise1,  4  }, /* am */
        { catnoise2,  1  }, /* an */ { catnoise3,  1  }, /* ao */
        { catnoise4,  1  }, /* eo */ { catnoise5,  8  }, /* ew */
        { catnoise6,  5  }, /* me */ { catnoise7,  30 }, /* mr */
        { catnoise8,  1  }, /* ny */ { catnoise9,  38 }, /* ow */
        { catnoise10, 1  }, /* pp */ { catnoise11, 1  }, /* pu */
        { catnoise12, 1  }, /* ra */ { catnoise13, 1  }, /* re */
        { catnoise14, 2  }, /* rm */ { catnoise15, 1  }, /* rn */
        { catnoise16, 1  }, /* ro */ { catnoise17, 1  }, /* rp */
        { catnoise18, 14 }, /* rr */ { catnoise19, 1  }, /* rw */
        { catnoise20, 1  }, /* ur */ { catnoise21, 30 }, /* wm */
        { catnoise22, 1  }, /* wn */ { catnoise23, 1  }, /* wp */
        { catnoise24, 10 }, /* wr */ { catnoise25, 4  }, /* ww */
        { catnoise26, 6  }  /* ya */
};

static const choice_t keysmash0[]   = { { 1,  4,  'a' }, { 10, 5,  'k' },
      { 3,  6,  'd' }, { 11, 7,  'l' }, { 7,  8,  'h' } };
static const choice_t keysmash1[]   = { { 7,  7,  'h' }, { 9,  13, 'j' },
      { 5,  18, 'f' }, { 11, 21, 'l' }, { 16, 24, 'u' }, { 6,  26, 'g' },
      { 3,  28, 'd' }, { 0,  29, ';' }, { 14, 30, 'r' } };
static const choice_t keysmash2[]   = { { 10, 1,  'k' }, { 1,  2, 'a' },
      { 7,  3,  'h' } };
static const choice_t keysmash3[]   = { { 5,  7,  'f' }, { 7,  12, 'h' },
      { 15, 13, 's' }, { 6,  14, 'g' }, { 10, 15, 'k' }, { 9,  16, 'j' },
      { 2,  17, 'b' } };
static const choice_t keysmash4[]   = { { 6,  1,  'g' } };
static const choice_t keysmash5[]   = { { 6,  12, 'g' }, { 10, 16, 'k' },
      { 9,  19, 'j' }, { 11, 21, 'l' }, { 7,  23, 'h' }, { 3,  25, 'd' },
      { 1,  26, 'a' } };
static const choice_t keysmash6[]   = { { 1,  8,  'a' }, { 7,  14, 'h' },
      { 0,  18, ';' }, { 9,  22, 'j' }, { 11, 25, 'l' }, { 2,  27, 'b' },
      { 5,  29, 'f' }, { 3,  30, 'd' }, { 15, 31, 's' }, { 10, 32, 'k' },
      { 16, 33, 'u' }, { 12, 34, 'n' } };
static const choice_t keysmash7[]   = { { 6,  7,  'g' }, { 9,  11, 'j' },
      { 5,  14, 'f' }, { 10, 17, 'k' }, { 1,  20, 'a' }, { 3,  23, 'd' },
      { 8,  25, 'i' }, { 14, 27, 'r' }, { 0,  28, ';' }, { 12, 29, 'n' },
      { 7,  30, 'h' }, { 16, 31, 'u' } };
static const choice_t keysmash8[]   = { { 16, 1,  'u' } };
static const choice_t keysmash9[]   = { { 7,  5,  'h' }, { 3,  9, 'd'} ,
      { 10, 12, 'k' }, { 5,  15, 'f' }, { 1,  17, 'a' }, { 14, 18, 'r' },
      { 4,  19, 'e' }, { 13, 20, 'o' }, { 11, 21, 'l' }, { 6,  22, 'g' } };
static const choice_t keysmash10[]  = { { 1,  6,  'a' }, { 5,  10, 'f' },
      { 6,  13, 'g' }, { 9,  16, 'j' }, { 15, 17, 's' }, { 3,  18, 'd' },
      { 11, 19, 'l' }, { 7,  20, 'h' } };
static const choice_t keysmash11[]  = { { 10, 4,  'k' }, { 0,  6,  ';' },
      { 15, 7,  's' }, { 6,  8,  'g' }, { 5,  9,  'f' }, { 3,  10, 'd' },
      { 1,  11, 'a' } };
static const choice_t keysmash12[]  = { { 6,  1,  'g' }, { 7,  2,  'h' } };
static const choice_t keysmash13[]  = { { 6,  1,  'g' } };
static const choice_t keysmash14[]  = { { 6,  4,  'g' }, { 1,  6,  'a' },
      { 7,  7,  'h' } };
static const choice_t keysmash15[]  = { { 3,  1,  'd' }, { 10, 2,  'k' } };
static const choice_t keysmash16[]  = { { 14, 3,  'r' }, { 1,  4,  'a' },
      { 5,  5,  'f' }, { 9,  6,  'j' }, { 4,  7,  'e' } };

static const ngram_t keysmash[] = {
	{ keysmash0,  8  }, /* ; */ { keysmash1,  30 }, /* a */
	{ keysmash2,  3  }, /* b */ { keysmash3,  17 }, /* d */
	{ keysmash4,  1  }, /* e */ { keysmash5,  26 }, /* f */
	{ keysmash6,  34 }, /* g */ { keysmash7,  31 }, /* h */
	{ keysmash8,  1  }, /* i */ { keysmash9,  22 }, /* j */
	{ keysmash10, 20 }, /* k */ { keysmash11, 11 }, /* l */
	{ keysmash12, 2  }, /* n */ { keysmash13, 1  }, /* o */
	{ keysmash14, 7  }, /* r */ { keysmash15, 2  }, /* s */
	{ keysmash16, 7  }  /* u */
};

static const choice_t scrunk0[]   = { { 34,  500,  'n' }, { 37,  867,  'w' },
                  { 31,  1067, 'd' } };
static const choice_t scrunk1[]   = { { 43,  1,    'o' } };
static const choice_t scrunk2[]   = { { 46,  60,   'u' }, { 45,  107,  'r' },
                  { 44,  147,  'a' } };
static const choice_t scrunk3[]   = { { 51,  1,    'o' } };
static const choice_t scrunk4[]   = { { 61,  1,    'l' } };
static const choice_t scrunk5[]   = { { 64,  1,    'a' } };
static const choice_t scrunk6[]   = { { 77,  200,  't' }, { 70,  300,  'c' },
                  { 68,  325,  ',' }, { 67,  349,  '!' }, { 69,  364,  '.' } };
static const choice_t scrunk7[]   = { { 81,  2,    'i' }, { 79,  3,    'd' } };
static const choice_t scrunk8[]   = { { 88,  4,    'i' }, { 89,  5,    'o' } };
static const choice_t scrunk9[]   = { { 95,  1,    'i' }, { 94,  2,    'e' } };
static const choice_t scrunk10[]  = { { 107, 50,   'o' }, { 98,  81,   ' ' } };
static const choice_t scrunk11[]  = { { 135, 535,  'c' }, { 140, 835,  'p' },
                  { 139, 1100, 'o' }, { 138, 1200, 'm' }, { 136, 1271, 'h' } };
static const choice_t scrunk12[]  = { { 149, 262,  'h' }, { 152, 362,  'o' },
                  { 150, 453,  'i' }, { 147, 478,  'a' } };
static const choice_t scrunk13[]  = { { 165, 11,   'h' }, { 166, 19,   'i' } };
static const choice_t scrunk14[]  = { { 173, 1,    'a' } };
static const choice_t scrunk15[]  = { { 12,  190,  't' }, { 0,   326,  'a' },
                  { 11,  454,  's' }, { 8,   572,  'l' }, { 2,   646,  'c' },
		  { 14,  684,  'y' }, { 5,   722,  'h' }, { 13,  757,  'w' },
		  { 1,   790,  'b' }, { 10,  821,  'n' }, { 6,   849,  'i' } };
static const choice_t scrunk16[]  = { { 15,  31,   ' ' }, { 16,  60,   '!' } };
static const choice_t scrunk17[]  = { { 17,  777,  ',' }, { 26,  965,  't' },
                  { 18,  1098, 'a' }, { 25,  1227, 's' }, { 23,  1322, 'l' },
		  { 20,  1387, 'c' }, { 24,  1425, 'n' }, { 28,  1462, 'y' },
		  { 22,  1498, 'i' }, { 19,  1531, 'b' }, { 21,  1560, 'h' },
		  { 27,  1585, 'w' } };
static const choice_t scrunk18[]  = { { 37,  1,    'w' } };
static const choice_t scrunk19[]  = { { 43,  1,    'o' } };
static const choice_t scrunk20[]  = { { 45,  1,    'r' } };
static const choice_t scrunk21[]  = { { 64,  1,    'a' } };
static const choice_t scrunk22[]  = { { 67,  7,    '!' }, { 68,  13,   ',' },
		        	      { 69,  18,   '.' } };
static const choice_t scrunk23[]  = { { 89,  59,   'o' }, { 88,  95,   'i' } };
static const choice_t scrunk24[]  = { { 98,  1,    ' ' } };
static const choice_t scrunk25[]  = { { 135, 65,   'c' }, { 139, 100,  'o' },
	                              { 136, 129,  'h' } };
static const choice_t scrunk26[]  = { { 149, 38,   'h' }, { 150, 47,   'i' } };
static const choice_t scrunk27[]  = { { 165, 1,    'h' } };
static const choice_t scrunk28[]  = { { 173, 1,    'a' } };
static const choice_t scrunk29[]  = { { 12,  222,  't' }, { 11,  365,  's' },
		  { 0,   496,  'a' }, { 8,   583,  'l' }, { 2,   644,  'c' },
		  { 13,  684,  'w' }, { 6,   720,  'i' }, { 1,   754,  'b' },
		  { 5,   787,  'h' }, { 10,  818,  'n' }, { 14,  843,  'y' } };
static const choice_t scrunk30[]  = { { 29,  638,  ' ' }, { 30,  1271, '.' } };
static const choice_t scrunk31[]  = { { 51,  1,    'o' } };
static const choice_t scrunk32[]  = { { 87,  1,    'e' } };
static const choice_t scrunk33[]  = { { 97,  1,    't' } };
static const choice_t scrunk34[]  = { { 103, 5,    'd' }, { 98,  6,    ' ' } };
static const choice_t scrunk35[]  = { { 122, 1,    ' ' }, { 123, 2,    'p' } };
static const choice_t scrunk36[]  = { { 143, 1,    ' ' } };
static const choice_t scrunk37[]  = { { 168, 300,  'w' }, { 161, 400,  ' ' },
	          { 162, 443,  '!' }, { 164, 473,  '.' }, { 163, 500,  ',' } };
static const choice_t scrunk38[]  = { { 170, 17,   '!' }, { 172, 34,   '.' },
	          { 171, 50,   ',' } };
static const choice_t scrunk39[]  = { { 16,  29,   '!' }, { 15,  33,   ' ' } };
static const choice_t scrunk40[]  = { { 17,  1,    ',' } };
static const choice_t scrunk41[]  = { { 30,  7,    '.' }, { 29,  9,    ' ' } };
static const choice_t scrunk42[]  = { { 89,  2,    'o' }, { 87,  3,    'e' } };
static const choice_t scrunk43[]  = { { 113, 1,    'i' } };
static const choice_t scrunk44[]  = { { 34,  1,    'n' }, { 33,  2,    'm' },
	          { 36,  3,    't' } };
static const choice_t scrunk45[]  = { { 131, 5,    'u' }, { 128, 9,    'i' } };
static const choice_t scrunk46[]  = { { 159, 1,    't' } };
static const choice_t scrunk47[]  = { { 2,   2,    'c' }, { 3,   3,    'd' },
	          { 6,   4,    'i' }, { 11,  5,    's' } };
static const choice_t scrunk48[]  = { { 41,  36,   '.' }, { 39,  69,   '!' },
	          { 40,  100,  ',' } };
static const choice_t scrunk49[]  = { { 50,  1,    'l' } };
static const choice_t scrunk50[]  = { { 87,  1,    'e' } };
static const choice_t scrunk51[]  = { { 118, 2,    'r' }, { 117, 4,    'o' },
	          { 120, 5,    'u' } };
static const choice_t scrunk52[]  = { { 12,  6,    't' }, { 11,  10,   's' },
	          { 8,   13,   'l' }, { 2,   16,   'c' }, { 0,   18,   'a' },
		  { 13,  20,   'w' }, { 1,   22,   'b' }, { 7,   24,   'k' },
		  { 9,   25,   'm' } };
static const choice_t scrunk53[]  = { { 16,  157,  '!' }, { 15,  208,  ' ' } };
static const choice_t scrunk54[]  = { { 17,  1,    ',' } };
static const choice_t scrunk55[]  = { { 30,  80,   '.' }, { 29,  101,  ' ' } };
static const choice_t scrunk56[]  = { { 92,  1,    ' ' } };
static const choice_t scrunk57[]  = { { 108, 400,  'p' }, { 98,  500,  ' ' },
	          { 101, 536,  '.' }, { 99,  569,  '!' }, { 100, 600,  ',' } };
static const choice_t scrunk58[]  = { { 121, 1,    'w' } };
static const choice_t scrunk59[]  = { { 141, 1,    't' } };
static const choice_t scrunk60[]  = { { 0,   1,    'a' } };
static const choice_t scrunk61[]  = { { 90,  1,    'u' } };
static const choice_t scrunk62[]  = { { 53,  74,   '!' }, { 54,  139,  ',' },
	          { 55,  200,  '.' } };
static const choice_t scrunk63[]  = { { 87,  1,    'e' } };
static const choice_t scrunk64[]  = { { 35,  1,    'p' } };
static const choice_t scrunk65[]  = { { 52,  1200, ' ' }, { 57,  1400, 'n' },
	          { 56,  1500, 'm' }, { 53,  1537, '!' }, { 54,  1569, ',' },
		  { 55,  1600, '.' } };
static const choice_t scrunk66[]  = { { 130, 1,    'o' } };
static const choice_t scrunk67[]  = { { 16,  25,   '!' }, { 15,  38,   ' ' } };
static const choice_t scrunk68[]  = { { 17,  1,    ',' } };
static const choice_t scrunk69[]  = { { 30,  17,   '.' }, { 29,  25,   ' ' } };
static const choice_t scrunk70[]  = { { 44,  1,    'a' } };
static const choice_t scrunk71[]  = { { 49,  1,    'd' } };
static const choice_t scrunk72[]  = { { 57,  400,  'n' }, { 55,  443,  '.' },
	   	  { 54,  479,  ',' }, { 53,  500,  '!' } };
static const choice_t scrunk73[]  = { { 80,  1,    'e' } };
static const choice_t scrunk74[]  = { { 93,  1,    'b' }, { 97,  2,    't' } };
static const choice_t scrunk75[]  = { { 106, 6,    'k' }, { 105, 7,    'g' } };
static const choice_t scrunk76[]  = { { 127, 1,    'y' } };
static const choice_t scrunk77[]  = { { 154, 300,  't' }, { 143, 600,  ' ' },
	          { 146, 706,  '.' }, { 155, 806,  'y' }, { 145, 905,  ',' },
		  { 144, 1000, '!' } };
static const choice_t scrunk78[]  = { { 160, 1,    'e' } };
static const choice_t scrunk79[]  = { { 48,  1,    'b' } };
static const choice_t scrunk80[]  = { { 52,  1,    ' ' } };
static const choice_t scrunk81[]  = { { 77,  3,    't' }, { 74,  4,    'm' } };
static const choice_t scrunk82[]  = { { 91,  6,    'y' }, { 87,  7,    'e' } };
static const choice_t scrunk83[]  = { { 112, 39,   '.' }, { 110, 72,   '!' },
	          { 111, 100,  ',' } };
static const choice_t scrunk84[]  = { { 158, 1,    's' } };
static const choice_t scrunk85[]  = { { 169, 1,    ' ' } };
static const choice_t scrunk86[]  = { { 11,  1,    's' } };
static const choice_t scrunk87[]  = { { 52,  550,  ' ' }, { 53,  569,  '!' },
	          { 55,  586,  '.' }, { 54,  600,  ',' } };
static const choice_t scrunk88[]  = { { 77,  3,    't' }, { 73,  4,    'k' },
	          { 78,  5,    'v' }, { 74,  6,    'm' } };
static const choice_t scrunk89[]  = { { 117, 100,  'o' }, { 109, 150,  ' ' },
	          { 110, 169,  '!' }, { 111, 187,  ',' }, { 112, 200,  '.' } };
static const choice_t scrunk90[]  = { { 157, 1,    'n' } };
static const choice_t scrunk91[]  = { { 169, 300,  ' ' }, { 170, 373,  '!' },
	          { 172, 439,  '.' }, { 171, 500,  ',' } };
static const choice_t scrunk92[]  = { { 12,  1,    't' } };
static const choice_t scrunk93[]  = { { 42,  1,    'l' } };
static const choice_t scrunk94[]  = { { 58,  1,    'o' } };
static const choice_t scrunk95[]  = { { 76,  1,    'p' } };
static const choice_t scrunk96[]  = { { 115, 1,    'l' } };
static const choice_t scrunk97[]  = { { 151, 100,  'l' }, { 155, 200,  'y' },
	          { 146, 238,  '.' }, { 145, 273,  ',' }, { 144, 300,  '!' } };
static const choice_t scrunk98[]  = { { 12,  2,    't' }, { 4,   3,    'f' } };
static const choice_t scrunk99[]  = { { 16,  26,   '!' }, { 15,  33,   ' ' } };
static const choice_t scrunk100[] = { { 17,  1,    ',' } };
static const choice_t scrunk101[] = { { 30,  25,   '.' }, { 29,  36,   ' ' } };
static const choice_t scrunk102[] = { { 32,  1,    'l' } };
static const choice_t scrunk103[] = { { 47,  1,    ' ' } };
static const choice_t scrunk104[] = { { 60,  1,    ' ' } };
static const choice_t scrunk105[] = { { 62,  1,    'e' }, { 63,  2,    'l' } };
static const choice_t scrunk106[] = { { 82,  7,    'l' }, { 85,  9,    'y' },
	          { 84,  10,   'u' }, { 83,  11,   'o' } };
static const choice_t scrunk107[] = { { 121, 1,    'w' } };
static const choice_t scrunk108[] = { { 125, 1,    's' } };
static const choice_t scrunk109[] = { { 0,   1,    'a' }, { 6,   2,    'i' },
	          { 8,   3,    'l' }, { 1,   4,    'b' }, { 9,   5,    'm' } };
static const choice_t scrunk110[] = { { 16,  105,  '!' }, { 15,  136,  ' ' } };
static const choice_t scrunk111[] = { { 17,  1,    ',' } };
static const choice_t scrunk112[] = { { 30,  104,  '.' }, { 29,  141,  ' ' } };
static const choice_t scrunk113[] = { { 75,  1,    'n' } };
static const choice_t scrunk114[] = { { 81,  1,    'i' } };
static const choice_t scrunk115[] = { { 86,  1,    ' ' } };
static const choice_t scrunk116[] = { { 106, 1,    'k' } };
static const choice_t scrunk117[] = { { 119, 300,  't' }, { 114, 500,  'k' },
	          { 117, 600,  'o' }, { 109, 700,  ' ' }, { 112, 776,  '.' },
		  { 110, 841,  '!' }, { 111, 900,  ',' } };
static const choice_t scrunk118[] = { { 129, 1,    'n' } };
static const choice_t scrunk119[] = { { 153, 1,    's' } };
static const choice_t scrunk120[] = { { 156, 1,    'b' } };
static const choice_t scrunk121[] = { { 161, 2,    ' ' }, { 167,  3,   'm' } };
static const choice_t scrunk122[] = { { 10,  1,    'n' } };
static const choice_t scrunk123[] = { { 127, 1,    'y' } };
static const choice_t scrunk124[] = { { 130, 1,    'o' } };
static const choice_t scrunk125[] = { { 142, 1,    'y' } };
static const choice_t scrunk126[] = { { 157, 1,    'n' } };
static const choice_t scrunk127[] = { { 169, 100,  ' ' }, { 170, 141,  '!' },
	          { 171, 177,  ',' }, { 172, 200,  '.' } };
static const choice_t scrunk128[] = { { 75,  1,    'n' }, { 74,  2,    'm' } };
static const choice_t scrunk129[] = { { 102, 1,    'a' } };
static const choice_t scrunk130[] = { { 116, 1,    'n' }, { 113, 2,    'i' } };
static const choice_t scrunk131[] = { { 157, 1,    'n' } };
static const choice_t scrunk132[] = { { 16,  8,    '!' }, { 15,  11,   ' ' } };
static const choice_t scrunk133[] = { { 17,  1,    ',' } };
static const choice_t scrunk134[] = { { 30,  13,   '.' }, { 29,  17,   ' ' } };
static const choice_t scrunk135[] = { { 45,  1,    'r' } };
static const choice_t scrunk136[] = { { 66,  1,    'r' } };
static const choice_t scrunk137[] = { { 77,  2,    't' }, { 72,  3,    'e' } };
static const choice_t scrunk138[] = { { 96,  1,    'o' } };
static const choice_t scrunk139[] = { { 109, 2,    ' ' }, { 117, 3,    'o' } };
static const choice_t scrunk140[] = { { 126, 2,    'u' }, { 124, 3,    'r' } };
static const choice_t scrunk141[] = { { 143, 1,    ' ' } };
static const choice_t scrunk142[] = { { 169, 300,  ' ' }, { 172, 341,  '.' },
	          { 171, 376,  ',' }, { 170, 400,  '!' } };
static const choice_t scrunk143[] = { { 12,  2,    't' }, { 0,   4,    'a' },
	          { 2,   5,    'c' }, { 11,  6,    's' } };
static const choice_t scrunk144[] = { { 16,  85,   '!' }, { 15,  122,  ' ' } };
static const choice_t scrunk145[] = { { 17,  1,    ',' } };
static const choice_t scrunk146[] = { { 30,  101,  '.' }, { 29,  144,  ' ' } };
static const choice_t scrunk147[] = { { 35,  1,    'p' } };
static const choice_t scrunk148[] = { { 59,  200,  's' }, { 53,  238,  '!' },
	          { 55,  271,  '.' }, { 54,  300,  ',' } };
static const choice_t scrunk149[] = { { 65,  1,    'e' } };
static const choice_t scrunk150[] = { { 72,  1,    'e' } };
static const choice_t scrunk151[] = { { 87,  1,    'e' } };
static const choice_t scrunk152[] = { { 117, 3,    'o' }, { 109, 4,    ' ' } };
static const choice_t scrunk153[] = { { 137, 1,    'i' } };
static const choice_t scrunk154[] = { { 151, 1,    'l' } };
static const choice_t scrunk155[] = { { 169, 100,  ' ' }, { 171, 138,  ',' },
	          { 170, 169,  '!' }, { 172, 200,  '.' } };
static const choice_t scrunk156[] = { { 42,  1,    'l' } };
static const choice_t scrunk157[] = { { 106, 4,    'k' }, { 105, 7,    'g' },
		  { 104, 8,    'f' } };
static const choice_t scrunk158[] = { { 134, 34,   '.' }, { 132, 67,   '!' },
	          { 133, 100,  ',' } };
static const choice_t scrunk159[] = { { 148, 1,    'e' } };
static const choice_t scrunk160[] = { { 52,  1,    ' ' } };
static const choice_t scrunk161[] = { { 12,  2,    't' }, { 8,   3,    'l' },
	          { 11,  4,    's' }, { 13,  5,    'w' }, { 6,   6,    'i' } };
static const choice_t scrunk162[] = { { 16,  24,   '!' }, { 15,  43,   ' ' } };
static const choice_t scrunk163[] = { { 17,  1,    ',' } };
static const choice_t scrunk164[] = { { 30,  4,    '.' }, { 29,  5,    ' ' } };
static const choice_t scrunk165[] = { { 65,  1,    'e' } };
static const choice_t scrunk166[] = { { 71,  1,    'd' } };
static const choice_t scrunk167[] = { { 94,  1,    'e' } };
static const choice_t scrunk168[] = { { 161, 3,    ' ' }, { 168, 5,    'w' } };
static const choice_t scrunk169[] = { { 12,  2,    't' }, { 11,  4,    's' },
	          { 3,   6,    'd' }, { 0,   8,    'a' }, { 7,   9,    'k' },
		  { 13,  10,   'w' } };
static const choice_t scrunk170[] = { { 16,  5,    '!' }, { 15,  7,    ' ' } };
static const choice_t scrunk171[] = { { 17,  1,    ',' } };
static const choice_t scrunk172[] = { { 30,  51,   '.' }, { 29,  65,   ' ' } };
static const choice_t scrunk173[] = { { 38,  1,    'y' } };

static const ngram_t scrunk[] = {
        { scrunk0,   1067 }, /* a  */ { scrunk1,   1    }, /* b  */
        { scrunk2,   147  }, /* c  */ { scrunk3,   1    }, /* d  */
        { scrunk4,   1    }, /* f  */ { scrunk5,   1    }, /* h  */
        { scrunk6,   364  }, /* i  */ { scrunk7,   3    }, /* k  */
        { scrunk8,   5    }, /* l  */ { scrunk9,   2    }, /* m  */
        { scrunk10,  81   }, /* n  */ { scrunk11,  1271 }, /* s  */
        { scrunk12,  478  }, /* t  */ { scrunk13,  19   }, /* w  */
        { scrunk14,  1    }, /* y  */ { scrunk15,  849  }, /* !  */
        { scrunk16,  60   }, /* !! */ { scrunk17,  1585 }, /* ,, */
        { scrunk18,  1    }, /* ,a */ { scrunk19,  1    }, /* ,b */
        { scrunk20,  1    }, /* ,c */ { scrunk21,  1    }, /* ,h */
        { scrunk22,  18   }, /* ,i */ { scrunk23,  95   }, /* ,l */
        { scrunk24,  1    }, /* ,n */ { scrunk25,  129  }, /* ,s */
        { scrunk26,  47   }, /* ,t */ { scrunk27,  1    }, /* ,w */
        { scrunk28,  1    }, /* ,y */ { scrunk29,  843  }, /* .  */
        { scrunk30,  1271 }, /* .. */ { scrunk31,  1    }, /* ad */
        { scrunk32,  1    }, /* al */ { scrunk33,  1    }, /* am */
        { scrunk34,  6    }, /* an */ { scrunk35,  2    }, /* ap */
        { scrunk36,  1    }, /* at */ { scrunk37,  500  }, /* aw */
        { scrunk38,  50   }, /* ay */ { scrunk39,  33   }, /* b! */
        { scrunk40,  1    }, /* b, */ { scrunk41,  9    }, /* b. */
        { scrunk42,  3    }, /* bl */ { scrunk43,  1    }, /* bo */
        { scrunk44,  3    }, /* ca */ { scrunk45,  9    }, /* cr */
        { scrunk46,  1    }, /* cu */ { scrunk47,  5    }, /* d  */
        { scrunk48,  100  }, /* db */ { scrunk49,  1    }, /* dd */
        { scrunk50,  1    }, /* dl */ { scrunk51,  5    }, /* do */
        { scrunk52,  25   }, /* e  */ { scrunk53,  208  }, /* e! */
        { scrunk54,  1    }, /* e, */ { scrunk55,  101  }, /* e. */
        { scrunk56,  1    }, /* em */ { scrunk57,  600  }, /* en */
        { scrunk58,  1    }, /* eo */ { scrunk59,  1    }, /* es */
        { scrunk60,  1    }, /* f  */ { scrunk61,  1    }, /* fl */
        { scrunk62,  200  }, /* ge */ { scrunk63,  1    }, /* gl */
        { scrunk64,  1    }, /* ha */ { scrunk65,  1600 }, /* he */
        { scrunk66,  1    }, /* hr */ { scrunk67,  38   }, /* i! */
        { scrunk68,  1    }, /* i, */ { scrunk69,  25   }, /* i. */
        { scrunk70,  1    }, /* ic */ { scrunk71,  1    }, /* id */
        { scrunk72,  500  }, /* ie */ { scrunk73,  1    }, /* ik */
        { scrunk74,  2    }, /* im */ { scrunk75,  7    }, /* in */
        { scrunk76,  1    }, /* ip */ { scrunk77,  1000 }, /* it */
        { scrunk78,  1    }, /* iv */ { scrunk79,  1    }, /* kd */
        { scrunk80,  1    }, /* ke */ { scrunk81,  4    }, /* ki */
        { scrunk82,  7    }, /* kl */ { scrunk83,  100  }, /* ko */
        { scrunk84,  1    }, /* ku */ { scrunk85,  1    }, /* ky */
        { scrunk86,  1    }, /* l  */ { scrunk87,  600  }, /* le */
        { scrunk88,  6    }, /* li */ { scrunk89,  200  }, /* lo */
        { scrunk90,  1    }, /* lu */ { scrunk91,  500  }, /* ly */
        { scrunk92,  1    }, /* m  */ { scrunk93,  1    }, /* mb */
        { scrunk94,  1    }, /* me */ { scrunk95,  1    }, /* mi */
        { scrunk96,  1    }, /* mo */ { scrunk97,  300  }, /* mt */
        { scrunk98,  3    }, /* n  */ { scrunk99,  33   }, /* n! */
        { scrunk100, 1    }, /* n, */ { scrunk101, 36   }, /* n. */
        { scrunk102, 1    }, /* na */ { scrunk103, 1    }, /* nd */
        { scrunk104, 1    }, /* nf */ { scrunk105, 2    }, /* ng */
        { scrunk106, 11   }, /* nk */ { scrunk107, 1    }, /* no */
        { scrunk108, 1    }, /* np */ { scrunk109, 5    }, /* o  */
        { scrunk110, 136  }, /* o! */ { scrunk111, 1    }, /* o, */
        { scrunk112, 141  }, /* o. */ { scrunk113, 1    }, /* oi */
        { scrunk114, 1    }, /* ok */ { scrunk115, 1    }, /* ol */
        { scrunk116, 1    }, /* on */ { scrunk117, 900  }, /* oo */
        { scrunk118, 1    }, /* or */ { scrunk119, 1    }, /* ot */
        { scrunk120, 1    }, /* ou */ { scrunk121, 3    }, /* ow */
        { scrunk122, 1    }, /* p  */ { scrunk123, 1    }, /* pp */
        { scrunk124, 1    }, /* pr */ { scrunk125, 1    }, /* ps */
        { scrunk126, 1    }, /* pu */ { scrunk127, 200  }, /* py */
        { scrunk128, 2    }, /* ri */ { scrunk129, 1    }, /* rn */
        { scrunk130, 2    }, /* ro */ { scrunk131, 1    }, /* ru */
        { scrunk132, 11   }, /* s! */ { scrunk133, 1    }, /* s, */
        { scrunk134, 17   }, /* s. */ { scrunk135, 1    }, /* sc */
        { scrunk136, 1    }, /* sh */ { scrunk137, 3    }, /* si */
        { scrunk138, 1    }, /* sm */ { scrunk139, 3    }, /* so */
        { scrunk140, 3    }, /* sp */ { scrunk141, 1    }, /* st */
        { scrunk142, 400  }, /* sy */ { scrunk143, 6    }, /* t  */
        { scrunk144, 122  }, /* t! */ { scrunk145, 1    }, /* t, */
        { scrunk146, 144  }, /* t. */ { scrunk147, 1    }, /* ta */
        { scrunk148, 300  }, /* te */ { scrunk149, 1    }, /* th */
        { scrunk150, 1    }, /* ti */ { scrunk151, 1    }, /* tl */
        { scrunk152, 4    }, /* to */ { scrunk153, 1    }, /* ts */
        { scrunk154, 1    }, /* tt */ { scrunk155, 200  }, /* ty */
        { scrunk156, 1    }, /* ub */ { scrunk157, 8    }, /* un */
        { scrunk158, 100  }, /* us */ { scrunk159, 1    }, /* ut */
        { scrunk160, 1    }, /* ve */ { scrunk161, 6    }, /* w  */
        { scrunk162, 43   }, /* w! */ { scrunk163, 1    }, /* w, */
        { scrunk164, 5    }, /* w. */ { scrunk165, 1    }, /* wh */
        { scrunk166, 1    }, /* wi */ { scrunk167, 1    }, /* wm */
        { scrunk168, 5    }, /* ww */ { scrunk169, 10   }, /* y  */
        { scrunk170, 7    }, /* y! */ { scrunk171, 1    }, /* y, */
        { scrunk172, 65   }, /* y. */ { scrunk173, 1    }  /* ya */
};

const str_t strs[] = {
	STRLIT("*tilts head*"),             STRLIT("*twitches ears slightly*"),
	STRLIT("*purrs*"),                  STRLIT("*falls asleep*"),
	STRLIT("*sits on ur keyboard*"),    STRLIT("*nuzzles*"),
	STRLIT("*stares at u*"),            STRLIT("*sneezes*"),
	STRLIT("*plays with yarn*"),        STRLIT("*eats all ur doritos*"),
	STRLIT("*lies down on a surface*"), STRLIT("*claws the air*"),
	STRLIT("*points towards case of monster zero ultra*")
};

static void
genops(uwu_t *uwu)
{
	int ran = random() % 9;
	ran += ran >= uwu->last, uwu->last = ran;

	switch (ran) {
	case 0:
		uwu->op[0] = (oper_t){ .str = "uwu", .len = 3, .op = OP_STR };
		uwu->total = 1;
		break;
	case 1:
		uwu->op[0] = (oper_t){ .str = "owo", .len = 3, .op = OP_STR };
		uwu->total = 1;
		break;
	case 2:
		uwu->op[0] = (oper_t){ .str = ":3", .len = 2, .op = OP_STR };
		uwu->total = 1;
		break;
	case 3: ;
		const str_t *str = &strs[random() % LENGTH(strs)];
		uwu->op[0] = (oper_t){ .str = str->str,
				.len = str->len, .op = OP_STR };
		uwu->total = 1;
		break;
	case 4:
		uwu->op[0] = (oper_t){ .str = "ny", .len = 2, .op = OP_STR };
		uwu->op[1] = (oper_t){ .ch = 'a',
				.len = random() % 7 + 1, .op = OP_REPEAT };
		uwu->total = 2;
		break;
	case 5:
		uwu->op[0] = (oper_t){ .str = ">", .len = 1, .op = OP_STR };
		uwu->op[1] = (oper_t){ .ch = '/',
				.len = random() % 4 + 3, .op = OP_REPEAT };
		uwu->op[2] = (oper_t){ .str = "<", .len = 1, .op = OP_STR };
		uwu->total = 3;
		break;
	case 6:
		uwu->op[0] = (oper_t){ .ch = 'A',
				.len = random() % 12 + 5, .op = OP_REPEAT };
		uwu->total = 1;
		break;
	case 7:
		uwu->op[0] = (oper_t){ .str = "mr", .len = 2, .op = OP_STR };
		uwu->op[1] = (oper_t){ .ngram = catnoise, .prev = 7,
				.len = random() % 125 + 25, .op = OP_NGRAM };
		uwu->total = 2;
		break;
	case 8:
		uwu->op[0] = (oper_t){ .ngram = keysmash,
				.prev = random() % LENGTH(keysmash),
				.len = random() % 125 + 25, .op = OP_NGRAM };
		uwu->total = 1;
		break;
	case 9:
		uwu->op[0] = (oper_t){ .str = "aw", .len = 2, .op = OP_STR };
		uwu->op[1] = (oper_t){ .ngram = scrunk, .prev = 37,
				.len = random() % 75 + 25, .op = OP_NGRAM };
		uwu->total = 2;
		break;
	default: ;
	}
}

static int
runops(oper_t *op, char *buf)
{
	switch (op->op) {
	case OP_NGRAM:
		for (int i = 0, j; i < op->len; ++i) {
			int ran = random() % op->ngram[op->prev].prob;
			for (j = 0; op->ngram[op->prev].choice[j].prob <= ran;
					++j);
			buf[i] = op->ngram[op->prev].choice[j].ch;
			op->prev = op->ngram[op->prev].choice[j].next;
		}
		break;
	case OP_STR:
		memcpy(buf, op->str, op->len);
		break;
	case OP_REPEAT:
		memset(buf, op->ch, op->len);
		break;
	default: ;
	}
	return op->len;
}

int
main(int argc, char **argv)
{
	int len = 0; char *buf;
	if (posix_memalign((void **)&buf, 65536, 65536 + 256) != 0)
		return 1;
	srandom(time(NULL));
	uwu_t uwu = { .last = random() % 10 };

	for (;;) {
		genops(&uwu);
		for (int i = 0; i < uwu.total; ++i) {
			len += runops(&uwu.op[i], &buf[len]);
			if (len >= 65536) {
				write(STDOUT_FILENO, buf, 65536);
				memcpy(buf, &buf[65536], len - 65536);
				len -= 65536;
			}
		}
		buf[len++] = ' ';
	}
}
