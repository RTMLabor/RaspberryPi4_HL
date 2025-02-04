#ifndef BDEBUG  /* prevent circular inclusions */
#define BDEBUG  /* by using protection macros */

#if defined(DEBUG) && !defined(NDEBUG)

#ifndef BDEBUG_WARNING
#define BDEBUG_WARNING
#warning DEBUG is enabled
#endif

int printf(const char *format, ...);

#define BDBG_DEBUG_ERROR             0x00000001U    /* error  condition messages */
#define BDBG_DEBUG_GENERAL           0x00000002U    /* general debug  messages */
#define BDBG_DEBUG_ALL               0xFFFFFFFFU    /* all debugging data */

#define bdbg_current_types (BDBG_DEBUG_GENERAL)

#define bdbg_stmnt(x)  x

#define bdbg_printf(type, ...) (((type) & bdbg_current_types) ? printf (__VA_ARGS__) : 0)


#else /* defined(DEBUG) && !defined(NDEBUG) */

#define bdbg_stmnt(x)

#define bdbg_printf(...)

#endif /* defined(DEBUG) && !defined(NDEBUG) */

#endif /* BDEBUG */
