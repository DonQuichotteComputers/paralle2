/*
 * DonQuichotteComputers (at) gmail (dot) com: 2016/05/04 port for Epiphany CPU from different sources:
 *
 * Bit Twiddling Hacks -- Count the consecutive zero bits (trailing) on the right with multiply and lookup
 * "Using de Bruijn Sequences to Index 1 in a Computer Word" by Charles E. Leiserson, Harald Prokof, and Keith H. Randall.
 * 
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 *
 * 
 * Replacing slow gcc 4.8.2 __builtin_ctz(ll) & popcount routines under Epiphany 
 * The original gcc __builtin_ctz contains about 34 instructions and a branch after 19 instructions
 *   => speedy branch-free and LUT-based solutions
 *      data: 350 bytes
 *      code: inlined
 */

static inline unsigned char Epiphany_ctz(unsigned int);
static inline unsigned char Epiphany_ctzll(unsigned long long);
static inline unsigned char Epiphany_popcount(unsigned int);
static inline unsigned char Epiphany_popcountll(unsigned long long);


const unsigned char tcountbit[256]={
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

const unsigned char MultiplyDeBruijn64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

const unsigned char MultiplyDeBruijn32[32]={
   0,  1, 28,  2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17,  4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18,  6, 11,  5, 10, 9
};

/* you may add +1... or anything you want
static const unsigned char tbsf64[64]={
    1,  2, 49,  3, 58, 50, 29,  4,
   62, 59, 51, 43, 39, 30, 18,  5,
   63, 56, 60, 37, 54, 52, 44, 23,
   46, 40, 34, 31, 25, 19, 13,  6,
   64, 48, 57, 28, 61, 42, 38, 17,
   55, 36, 53, 22, 45, 33, 24, 12,
   47, 27, 41, 16, 35, 21, 32, 11,
   26, 15, 20, 10, 14,  9,  8,  7
};
*/

// find the number of trailing zeros in 32-bit v 
// The constant 0x077CB531UL is a de Bruijn sequence, which produces a unique pattern of bits into the high 5 bits for each possible bit position that it is multiplied against. 
// <!> When there are no bits set, it returns 0. 
static inline unsigned char Epiphany_ctz(unsigned int v) {
  return MultiplyDeBruijn32[((unsigned int)((v & -v) * 0x077CB531U)) >> 27];
}

// The expression (v & -v) extracts the least significant 1 bit from v. 
// <!> When there are no bits set, it returns 0. 
static inline unsigned char Epiphany_ctzll(unsigned long long v) {
  return MultiplyDeBruijn64[((v & -v) * 0x03f79d71b4cb0a89ULL) >> 58];
}

static inline unsigned char Epiphany_popcount(unsigned int v) {
  unsigned char *P=(unsigned char *)&v;
  return tcountbit[P[0]] + tcountbit[P[1]] + tcountbit[P[2]] + tcountbit[P[3]];
}

static inline unsigned char Epiphany_popcountll(unsigned long long v) {
  unsigned char *P=(unsigned char *)&v;
  return tcountbit[P[0]] + tcountbit[P[1]] + tcountbit[P[2]] + tcountbit[P[3]] + tcountbit[P[4]] + tcountbit[P[5]] + tcountbit[P[6]] + tcountbit[P[7]] ;
}

#define getbitN32 Epiphany_popcount
#define getbitN64 Epiphany_popcountll

//from notzed on the forum, "...gcc extended inline asm, 'cc' clobber_php.htm"
//volatile needed, the compiler may mix code without taking care of the condition flags :/ 
unsigned int bitrev(unsigned int val) {
  unsigned int res;

  __asm__ volatile ("bitr %[res],%[val]"
    : [res] "=r" (res)
    : [val] "r" (val)
    : "cc");

  return res;
}
