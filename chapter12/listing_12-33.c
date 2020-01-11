/*
Copyright (c) 2020, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * listing_12-33.c -- example of two threads adding the same persistent memory
 *                   location to their respective transactions simultaneously
 */

#include <libpmemobj.h>
#include <pthread.h>

struct my_root {
    int value;
    int is_odd;
};

POBJ_LAYOUT_BEGIN(example);
POBJ_LAYOUT_ROOT(example, struct my_root);
POBJ_LAYOUT_END(example);

pthread_mutex_t lock;

// function to be run by extra thread
void *func(void *args) {
    PMEMobjpool *pop = (PMEMobjpool *) args;

    TX_BEGIN(pop) {
        pthread_mutex_lock(&lock);
        TOID(struct my_root) root 
            = POBJ_ROOT(pop, struct my_root);
        TX_ADD(root);
        D_RW(root)->value = D_RO(root)->value + 3;
        pthread_mutex_unlock(&lock);
    } TX_END
}

int main(int argc, char *argv[]) {
    PMEMobjpool *pop= pmemobj_create("/mnt/pmem/pool",
                      POBJ_LAYOUT_NAME(example),
                      (1024 * 1024 * 10), 0666);

    pthread_t thread;
    pthread_mutex_init(&lock, NULL);

    TX_BEGIN(pop) {
        pthread_mutex_lock(&lock);
        TOID(struct my_root) root 
            = POBJ_ROOT(pop, struct my_root);
        TX_ADD(root);
        pthread_create(&thread, NULL, 
                       func, (void *) pop);
        D_RW(root)->value = D_RO(root)->value + 4;
        D_RW(root)->is_odd = D_RO(root)->value % 2;
        pthread_mutex_unlock(&lock);
        // wait to make sure other thread finishes 1st
        pthread_join(thread, NULL);
    } TX_END

    pthread_mutex_destroy(&lock);
    return 0;
}
