     .data
dir:    .byte 1              // Direction: 1 (right), -1 (left)
pos:    .byte 0              // Current LED position

    .text
    .global blinker

blinker:
    push {r0, r1, r2}        // Save registers

    // Load direction and position
    LDR R0, =dir
    LDRSB R1, [R0]           // dir: signed byte
    LDR R0, =pos
    LDRB R2, [R0]            // pos: current LED position

    // Update position
    ADD R2, R2, R1           // pos = pos + dir
    CMP R2, #3               // Check upper boundary
    BGT reverse_direction
    CMP R2, #0               // Check lower boundary
    BLT reverse_direction

    // Update LED
update_led:
    LDR R0, =0x41200000      // Replace with actual LED hardware address
    MOV R1, #1               // Base LED pattern
    LSL R1, R1, R2           // Shift LED bit
    STR R1, [R0]
                // Write to LED register


    // Delay loop
delay_loop:
    LDR R0, =0x1000000
delay_loop_inner:
    SUBS R0, R0, #1
    BNE delay_loop_inner

    B end_blinker

reverse_direction:
    LDR R0, =dir
    LDRSB R1, [R0]
    NEG R1, R1               // Reverse direction
    STRB R1, [R0]

    CMP R2, #3
    BGT set_to_three
    CMP R2, #0
    BLT set_to_zero
    B update_led

set_to_three:
    MOV R2, #3
    B update_led

set_to_zero:
    MOV R2, #0

end_blinker:
    LDR R0, =pos
    STRB R2, [R0]            // Save updated position
    pop {r0, r1, r2}
    BX LR
