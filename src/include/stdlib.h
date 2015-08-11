#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
unsigned int atoi(const unsigned char *s);
#ifndef NULL
#define NULL                ((void *)0)
#endif
#endif /* _STDLIB_H_ */

