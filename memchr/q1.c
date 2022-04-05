#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdio.h> 
#include <stdbool.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary */
#define UNALIGNED(X) ((long) X & (sizeof(long) - 1))

/* How many bytes are loaded each iteration of the word copy loop */
#define LBLOCKSIZE (sizeof(long))

/* Threshhold for punting to the bytewise iterator */
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

#if LONG_MAX == 2147483647L
#define DETECT_NULL(X) (((X) -0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
/* Nonzero if X (a long int) contains a NULL byte. */
#define DETECT_NULL(X) (((X) -0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* @return nonzero if (long)X contains the byte used to fill MASK. */
#define DETECT_CHAR(X, MASK) (DETECT_NULL(X ^ MASK))


// @src_void: The memory area
// @c: The byte to search for
// @length: The size of the area.
void *memchr_opt(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned char d = c;
    printf("%x\n", d);
    //檢查是否小於8byte
    while (UNALIGNED(src)) {
        if (!length--)
            return NULL;
        if (*src == d)
            return (void *) src;
        src++;
    }

    if (!TOO_SMALL(length)) {
        /* If we get this far, we know that length is large and
         * src is word-aligned.
         */

        /* The fast code reads the source one word at a time and only performs
         * the bytewise search on word-sized segments if they contain the search
         * character, which is detected by XORing the word-sized segment with a
         * word-sized block of the search character and then detecting for the
         * presence of NULL in the result.
         */

        // 存src的記憶體位址
        unsigned long *asrc = (unsigned long *) src;
        unsigned long mask = d << 8 | d;
        mask = mask << 16 | mask;
        for (unsigned int i = 32; i < LBLOCKSIZE * 8; i <<= 1) {
            mask = (mask << i) | mask;
        }

        printf("%lx\n", LBLOCKSIZE);
        printf("%lx\n", length);
        while (length >= LBLOCKSIZE) {
            /* XXXXX: Your implementation should appear here */
            //如果有找到
            if (DETECT_CHAR(*asrc, mask)) {
                src = (unsigned char *) asrc;
                printf("src : %s\n", src);
                while (length-- != 0) {
                    //find the char
                    if (*src++ == d)
                        return (void *) (src - 1);
                }
            }
            else {
                length -= LBLOCKSIZE;
                printf("%lx\n", asrc);
                printf("%s\n", src);
                // 這邊宣告的是double的型態，當指標+1 等於一次前進8 byte
                asrc += 1;
                printf("%lx\n", asrc);
            }
        }

        /* If there are fewer than LBLOCKSIZE characters left, then we resort to
         * the bytewise loop.
         */
        src = (unsigned char *) asrc;
    }

    while (length--) {
        if (*src == d)
            return (void *) src;
        src++;
    }

    return NULL;
}


int main()
{
    const char str[] = "http://wiki.csie.ncku.edu.tw";
    const char ch = 'n';
    const char ch1 = '/';
    const char ch2 = '.';


    char *ret = memchr_opt(str, ch, strlen(str));
    printf("String after |%c| is - |%s|\n", ch, ret);

    char *ret1 = memchr_opt(str, ch1, strlen(str));
    printf("String after |%c| is - |%s|\n", ch1, ret1);

    char *ret2 = memchr_opt(str, ch2, strlen(str));
    printf("String after |%c| is - |%s|\n", ch2, ret2);
    return 0;
}