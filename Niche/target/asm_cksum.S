;/* 
; * FILENAME: asm_cksum_t.s 
; *
; * Copyright 2002-2006 by InterNiche Technologies Inc. All rights reserved.
; *
; * MODULE: ARM targets (4T and other Thumb-capable architectures)
; * 
; * ROUTINES: asm_cksum(ptr, word_count)
; *
; * DESCRIPTION: Make IP checksum from parms passed in r0 & r1, return
; *              sum in r0
; *              Assumes ptr is aligned to 16-bit boundary.
; *
; * Algorithm is loosely based on RFC1071, section 4.2
; *
; * PORTABLE: YES within ARM Thumb-capable CPU's
; *
; */    
      
      AREA    arm_cksum_t_s,CODE,READONLY, ALIGN=3
      EXPORT   asm_cksum

; register usage:
; r0 - checksum accumulator
; r1 - word count (parm 2)
; r2 - data pointer (parm 1)
; r3 - scratch
; r4-r7 - data buffer
; lr - return address

asm_cksum
      push     {r2-r7}           ; /* save local registers */

      mov      r2, r0            ; /* r2 = data pointer */
      mov      r0, #0            ; /* r0 = checksum */
      mov      r3, r0            ; /* CONST 0 */
      tst      r1, r1
      beq      done

      ;; /* process the first halfword if unaligned data pointer */

      mov      r4, #2
;RP_Modif      and      r4, r2            ; /* unaligned data pointer? */
      ands     r4, r2            ; /* unaligned data pointer? */
      beq      loop32x
      ldrh     r0, [r2, #0]      ; /* load first halfword */
      add      r2, #2
;RP_Modif      subs      r1, #1            ; /* decrement count */
      subs     r1, #1            ; /* decrement count */
      bne      loop32x
      b        done
      
      ;; /* compute checksum in 32-byte chunks */

loop32
      ldmia    r2!, {r4-r7}     ; /* load 32 bytes */
      add      r0, r0, r4
      adc      r0, r5
      adc      r0, r6
      adc      r0, r7
      ldmia    r2!, {r4-r7}     ; /* load 32 bytes */
      adc      r0, r4
      adc      r0, r5
      adc      r0, r6
      adc      r0, r7
      adc      r0, r3
loop32x
      subs     r1, #16
;RP_Modif	  cmp      r1, #0
;	  cmp      r1, #0
      bge      loop32

      ;; /* compute checksum over last partial chunk
      ;;  * -16 <= r1 <= -1
      ;;  * remaining words = r1 + 16
      ;;  * branch offset = 8 * (15 - (r1 + 16)) = 8 * (-1 - r1)
      ;;  */
      mvn      r5, r3            ; /* r5 = -1 */
      subs     r5, r5, r1        ; /* -16..-1 => 15..0 */
      lsr      r5, r5, #1
      lsls     r5, r5, #2        ; [also clears initial carry]
      add      pc, r5 
      nop
      ldmia    r2!, {r4}          ; /* 28 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /* 24 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /* 20 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /* 16 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /* 12 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /*  8 bytes */
      adc      r0, r4
      ldmia    r2!, {r4}          ; /*  4 bytes */
      adc      r0, r4
      adc      r0, r3            ; /*  0 bytes */

      ;; /* check for final halfword */

      lsrs     r1, r1, #1            ; /* odd number of words? */
      bcc      fold
      ldrh     r4, [r2, #0]
      add      r0, r0, r4
      adc      r0, r3

      ;; /* combine the upper and lower halfwords */
fold
      mov      r3, r0
      mov      r4, #16
      ror      r3, r4            ; r3 = r0 rol #16
      add      r0, r0, r3
      lsr      r0, r0, #16

done
      pop      {r2-r7}           ; /* restore regs, and return */
      bx       lr                ; /* return */

      END

