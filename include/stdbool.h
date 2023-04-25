/*
 * ISO C Standard:  7.16  Boolean type and values  <stdbool.h>
 */
#pragma once

#if defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L
/* bool, true and false are keywords.  */
#else
#define bool _Bool
#define true 1
#define false 0
#endif