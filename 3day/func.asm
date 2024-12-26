[BITS 32]

global io_hlt

[SECTION .test]
io_hlt:
    hlt 
    ret 