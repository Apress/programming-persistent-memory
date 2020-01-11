/*
 * Copyright 2015-2020, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * tx.c - An example using the transaction API
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libpmemobj.h>

#define die(...) do {fprintf(stderr, __VA_ARGS__); exit(1);} while(0)
#define POOL "/mnt/pmem/balance"

static PMEMobjpool *pool;


struct account {
    PMEMoid name;
    uint64_t balance;
};

POBJ_LAYOUT_BEGIN(a);
POBJ_LAYOUT_TOID(a, struct account);
POBJ_LAYOUT_END(a);

/*
 * Even though we return the oid in a volatile register, there's no
 * persistent leak as all "struct account" (type 1) allocations are
 * reachable via POBJ_FOREACH_TYPE().
 */
static PMEMoid new_account(const char *name, int deposit)
{
    int len = strlen(name) + 1;

    struct pobj_action act[2];
    PMEMoid str = pmemobj_reserve(pool, act + 0, len, 0);
    if (OID_IS_NULL(str))
        die("Can't allocate string: %m\n");
    /*
     * memcpy below must flush, but doesn't need to drain -- even just a
     * single drain after all flushes is enough.
     */
    pmemobj_memcpy(pool, pmemobj_direct(str), name, len, PMEMOBJ_F_MEM_NODRAIN);
    TOID(struct account) acc;
    PMEMoid acc_oid = pmemobj_reserve(pool, act + 1, sizeof(struct account), 1);
    TOID_ASSIGN(acc, acc_oid);
    if (TOID_IS_NULL(acc))
        die("Can't allocate account: %m\n");
    D_RW(acc)->name = str;
    D_RW(acc)->balance = deposit;
    pmemobj_persist(pool, D_RW(acc), sizeof(struct account));
    pmemobj_publish(pool, act, 2);
    return acc_oid;
}

int main()
{
    if (!(pool = pmemobj_create(POOL, "", PMEMOBJ_MIN_POOL, 0600)))
        die("Can't create pool “%s”: %m\n", POOL);

    TOID(struct account) account_a, account_b;
    TOID_ASSIGN(account_a, new_account("Julius Caesar", 100));
    TOID_ASSIGN(account_b, new_account("Mark Anthony", 50));

    int price = 42;
    TX_BEGIN(pool) {
        TX_ADD_DIRECT(&D_RW(account_a)->balance);
        TX_ADD_DIRECT(&D_RW(account_b)->balance);
        D_RW(account_a)->balance -= price;
        D_RW(account_b)->balance += price;
    } TX_END

    pmemobj_close(pool);
    return 0;
}
