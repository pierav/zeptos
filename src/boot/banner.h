#pragma once

#define xstr(s) str(s)
#define str(s) #s

#define ZEPTOS_BANNER                                                          \
    ""                                                                         \
    ",---,         .    ,-.   ,-.  \n"                                         \
    "   /          |   /   \\ (   ` \n"                                        \
    "  /   ,-. ;-. |-  |   |  `-.  \n"                                         \
    " /    |-' | | |   \\   / .   ) \n"                                        \
    "'---' `-' |-' `-'  `-'   `-'  \n"                                         \
    "          '       " xstr(CONFIG_ZEPTOS_BANNER_EXTRA) "\n"

#define PRINT_BANNER() puts(ZEPTOS_BANNER);