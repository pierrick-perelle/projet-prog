.global main
.text
main:
    ldr r0, =limite
    ldr r1, [r0]
    ldrb r2, [r0, #+2]
    ldrb r2, [r0, #+3]
    str r1, [r0, #+20]
    str r1, [r0, #+16]
    str r1, [r0, #+12]
    str r1, [r0, #+8]
    str r1, [r0, #+4]!
    ldmIA r0, {r2, r3, r4, r5, r6}
    swi 0x123456
.data
limite:
    .word 0x12345678
