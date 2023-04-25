#pragma once

#define xstr(s) str(s)
#define str(s) #s

#ifndef ZEPTOS_BANNER_EXTRA
#define ZEPTOS_BANNER_EXTRA ""
#endif
#define ZEPTOS_BANNER                                                          \
    ""                                                                         \
    ",---,         .    ,-.   ,-.  \n"                                         \
    "   /          |   /   \\ (   ` \n"                                        \
    "  /   ,-. ;-. |-  |   |  `-.  \n"                                         \
    " /    |-' | | |   \\   / .   ) \n"                                        \
    "'---' `-' |-' `-'  `-'   `-'  \n"                                         \
    "          '       " xstr(ZEPTOS_BANNER_EXTRA) "\n"
