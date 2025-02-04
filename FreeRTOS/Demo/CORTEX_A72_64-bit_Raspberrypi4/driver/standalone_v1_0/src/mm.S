.globl memzero
memzero:

    stp xzr, xzr, [x0], #16
    stp xzr, xzr, [x0], #16
    subs x1, x1, #32
    b.gt memzero
    ret
