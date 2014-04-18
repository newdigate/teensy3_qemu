/*
 * QEMU access to the auxiliary vector
 *
 * Copyright (C) 2013 Red Hat, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu-common.h"
#include "qemu/osdep.h"

#ifdef CONFIG_GETAUXVAL
/* Don't inline this in qemu/osdep.h, because pulling in <sys/auxv.h> for
   the system declaration of getauxval pulls in the system <elf.h>, which
   conflicts with qemu's version.  */

#include <sys/auxv.h>

unsigned long qemu_getauxval(unsigned long key)
{
    return getauxval(key);
}
#elif defined(__linux__)
#include "elf.h"

/* Our elf.h doesn't contain Elf32_auxv_t and Elf64_auxv_t, which is ok because
   that just makes it easier to define it properly for the host here.  */
typedef struct {
    unsigned long a_type;
    unsigned long a_val;
} ElfW_auxv_t;

static const ElfW_auxv_t *auxv;

void qemu_init_auxval(char **envp)
{
    /* The auxiliary vector is located just beyond the initial environment.  */
    while (*envp++ != NULL) {
        continue;
    }
    auxv = (const ElfW_auxv_t *)envp;
}

unsigned long qemu_getauxval(unsigned long type)
{
    /* If we were able to find the auxiliary vector, use it.  */
    if (auxv) {
        const ElfW_auxv_t *a;
        for (a = auxv; a->a_type != 0; a++) {
            if (a->a_type == type) {
                return a->a_val;
            }
        }
    }

    return 0;
}
#endif
