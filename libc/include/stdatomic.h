#pragma once

#define __AS_ATOMIC__(x) ((__typeof_unqual__(*(x))*)x)

#define atomic_fetch_add(x, y) __atomic_fetch_add(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_fetch_and(x, y) __atomic_fetch_and(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_fetch_or(x, y) __atomic_fetch_or(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_fetch_sub(x, y) __atomic_fetch_sub(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_fetch_xor(x, y) __atomic_fetch_xor(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_exchange(x, y) __atomic_exchange_n(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
#define atomic_load(x) __atomic_load_n(__AS_ATOMIC__(x), __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_strong(x, y, z) __atomic_compare_exchange_n(__AS_ATOMIC__(x), y, z, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak(x, y, z) __atomic_compare_exchange_n(__AS_ATOMIC__(x), y, z, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_store(x, y) __atomic_store_n(__AS_ATOMIC__(x), y, __ATOMIC_SEQ_CST)
