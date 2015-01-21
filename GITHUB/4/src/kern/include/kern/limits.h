#ifndef _KERN_LIMITS_H_
#define _KERN_LIMITS_H_



/* Longest filename (without directory) not including null terminator */
#define NAME_MAX  255

#include <kern/limits.h>


/* Longest full path name */
#define PATH_MAX        1024
#define PID_MIN         0
#define PID_MAX         256


#endif /* _KERN_LIMITS_H_ */
