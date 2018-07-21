
#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H

#ifdef NDEBUG
#define static_assert(T_CONDITION, T_ERROR) ((void)0)
#else
#define static_assert(T_CONDITION, T_ERROR) typedef char static_assertion_ ## T_ERROR[(T_CONDITION) 1 : -1]
#endif

#endif