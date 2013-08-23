
    .file  "start.s"
    .text


/*---------------------------------------------------------------------------
 |  EXPORTS - Functions
 */
    .global  u_print
    .global  u_putch
    .global  u_getch
    .global  u_kbhit
    .global  _start


/*---------------------------------------------------------------------------
 |  IMPORTS - Functions
 */
    .extern  c_start
    .extern  p_asm_timerISR
    .extern  p_asm_extISR
    .extern  __STACK_TOP



/*---------------------------------------------------------------------------
 |  MACROS
 *-------------------------------------------------------------------------*/

#define ENDFUNC(fname)  .type fname,@function ; .size fname,.-fname



/*---------------------------------------------------------------------------
 |  DEFINITIONS
 *-------------------------------------------------------------------------*/

/* baud rate divisor */
/*#define UART_DIV_9600_LO    0x48  // at 11.0592 MHz  -> evaluation Board*/
#define UART_DIV_9600_LO    0x60  /* at 14.7456 MHz */
#define UART_DIV_9600_HI    0x00

#define PVR      0x11f   /* processor version register */
#define PVR_440GP            0x40120000
#define PVR_440GP_PASS_1_0   0x40120040  /* RevA */
#define PVR_440GP_PASS_1_1   0x40120440  /* RevA */
#define PVR_440GP_PASS_2_0   0x40120481  /* RevB */

#define SRR0    0x01a   /* save/restore register 0 */
#define SRR1    0x01b   /* save/restore register 1 */
#define CSRR0   0x03a   /* critical save/restore register 0 */
#define CSRR1   0x03b   /* critical save/restore register 1 */
#define DBSR    0x130   /* debug status register */
#define DBCR0   0x134   /* debug control register 0 */
#define DBCR1   0x135   /* debug control register 1 */
#define DBCR2   0x136   /* debug control register 2 */
#define IAC1    0x138   /* instruction address comparator 1 */
#define IAC2    0x139   /* instruction address comparator 2 */
#define IAC3    0x13a   /* instruction address comparator 3 */
#define IAC4    0x13b   /* instruction address comparator 4 */
#define DAC1    0x13c   /* data address comparator 1 */
#define DAC2    0x13d   /* data address comparator 2 */
#define DVC1    0x13e   /* data value compare register 1 */
#define DVC2    0x13f   /* data value compare register 2 */
#define XER     0x001   /* fixed point exception register */
#define LR      0x008   /* link register */
#define CTR     0x009   /* count register */
#define DEC     0x016   /* decrementer timer */
#define ESR     0x03e   /* execption syndrome register */
#define IVPR    0x03f   /* interrupt vector prefix register */
#define TBLW    0x11c   /* time base lower - privileged write */
#define TBUW    0x11d   /* time base upper - privileged write */
#define TSR     0x150   /* timer status register */
#define TCR     0x154   /* timer control register */
#define IVOR0   0x190   /* interrupt vector offset reg - critical input */
#define IVOR1   0x191   /* interrupt vector offset reg - machine check */
#define IVOR2   0x192   /* interrupt vector offset reg - data storage */
#define IVOR3   0x193   /* interrupt vector offset reg - instr storage */
#define IVOR4   0x194   /* interrupt vector offset reg - ext input */
#define IVOR5   0x195   /* interrupt vector offset reg - alignment */
#define IVOR6   0x196   /* interrupt vector offset reg - program */
#define IVOR7   0x197   /* interrupt vector offset reg - floating pt unavail */
#define IVOR8   0x198   /* interrupt vector offset reg - system call */
#define IVOR9   0x199   /* interrupt vector offset reg - aux proc unavail */
#define IVOR10  0x19a   /* interrupt vector offset reg - decrementer */
#define IVOR11  0x19b   /* interrupt vector offset reg - fixed int timer */
#define IVOR12  0x19c   /* interrupt vector offset reg - watchdog timer */
#define IVOR13  0x19d   /* interrupt vector offset reg - data tlb error */
#define IVOR14  0x19e   /* interrupt vector offset reg - instr tlb error */
#define IVOR15  0x19f   /* interrupt vector offset reg - debug */
#define CCR0    0x3b3   /* core configuration register */

#define CPC0_CR0 0x0EB    /* Control Register 0  */
#define CPC0_ER  0x0B1    /* CPM Enable Register */
#define CPC0_FR  0x0B2    /* CPM Force  Register */
#define CPC0_CR0_U0EC     0x00400000
#define CPC0_CR0_U1EC     0x00200000
#define CPC0_CR0_CETE     0x40000000
#define CPC0_CR0_INIT     0x20600000

#define DBSR_MRR_MASK     0x30000000  /* Most recent reset */
#define DBSR_MRR_CORE 	  0x10000000  /* Core reset        */
#define DBSR_MRR_CHIP 	  0x20000000  /* Chip reset        */
#define DBSR_MRR_SYS  	  0x30000000  /* System reset      */

#define CCR0_DAPUIB       0x00100000  /* Disable APU instruction broadcast  */
#define CCR0_GICBT        0x00004000  /* Guaranteed instr cache block touch */
#define CCR0_GDCBT        0x00002000  /* Guaranteed data cache block touch  */
#define CCR0_BTAC_DISABLE 0x00000040

#define INV0    0x370      /* instruction cache normal victim 0 */
#define INV1    0x371      /* instruction cache normal victim 1 */
#define INV2    0x372      /* instruction cache normal victim 2 */
#define INV3    0x373      /* instruction cache normal victim 3 */
#define ITV0    0x374      /* instruction cache transient victim 0 */
#define ITV1    0x375      /* instruction cache transient victim 1 */
#define ITV2    0x376      /* instruction cache transient victim 2 */
#define ITV3    0x377      /* instruction cache transient victim 3 */
#define DNV0    0x390      /* data cache normal victim 0 */ 
#define DNV1    0x391      /* data cache normal victim 1 */
#define DNV2    0x392      /* data cache normal victim 2 */
#define DNV3    0x393      /* data cache normal victim 3 */
#define DTV0    0x394      /* data cache transient victim 0 */
#define DTV1    0x395      /* data cache transient victim 1 */
#define DTV2    0x396      /* data cache transient victim 2 */
#define DTV3    0x397      /* data cache transient victim 3 */
#define DVLIM   0x398      /* data cache victim limit */
#define IVLIM   0x399      /* instruction cache victim limit */
#define MMUCR   0x3b2      /* memory management control register */
#define MAX_TLB_ENTRIES 64 /* max. number of TLB entries */

#define IVLIM_TFLOOR_ENCODE(x)   (((x)&0x3F)<<22)
#define IVLIM_TCEILING_ENCODE(x) (((x)&0x3F)<<11)
#define IVLIM_NFLOOR_ENCODE(x)   (((x)&0x3F)<<0)
#define DVLIM_TFLOOR_ENCODE(x)   (((x)&0x3F)<<22)
#define DVLIM_TCEILING_ENCODE(x) (((x)&0x3F)<<11)
#define DVLIM_NFLOOR_ENCODE(x)   (((x)&0x3F)<<0)
#define MMUCR_STID_ENCODE(x)     (((x)&0xFF)<<0)
#define MMUCR_DEFAULT            0x00600000

#define UIC0_SR 0x0C0
#define UIC0_ER 0x0C2
#define UIC1_SR 0x0D0
#define UIC1_ER 0x0D2

#define TBLR    0x10C
#define TSR_PIT	0x08000000 /* PIT interrupt pending */

#define UART0_MMIO_BASE      0xE0000200
#define	UART_DATA_REG        0x00    
#define	UART_DL_LSB          0x00    
#define	UART_DL_MSB          0x01
#define	UART_INT_ENABLE      0x01
#define	UART_FIFO_CONTROL    0x02 
#define	UART_LINE_CONTROL    0x03
#define	UART_MODEM_CONTROL   0x04
#define	UART_LINE_STATUS     0x05
#define	UART_MODEM_STATUS    0x06
#define	UART_SCRATCH         0x07

#define MSR_POW 0x00040000  /* Activates power management      */
#define MSR_WE  0x00040000  /* wait state enable               */
#define MSR_CE  0x00020000  /* critical interrupt enable       */
#define MSR_ILE 0x00010000  /* Interrupt little Endian         */
#define MSR_EE  0x00008000  /* external interrupt              */
#define MSR_PR  0x00004000  /* problem state                   */
#define MSR_FP  0x00002000  /* floating point available        */
#define MSR_ME  0x00001000  /* machine check                   */
#define MSR_FE0 0x00000800  /* floating point exception enable */
#define MSR_SE  0x00000400  /* single step trace enable        */
#define MSR_BE  0x00000200  /* branch trace enable             */
#define MSR_DE  0x00000200  /* debug enable                    */
#define MSR_FE1 0x00000100  /* floating point exception enable */
#define MSR_EP  0x00000040  /* prefix                          */
#define MSR_IP  0x00000040  /* prefix                          */
#define MSR_IR  0x00000020  /* instruction relocate            */
#define MSR_DR  0x00000010  /* data relocate                   */
#define MSR_RI  0x00000002  /* Recoverable interrupt           */
#define MSR_LE  0x00000001  /* Little Endian                   */
#define MSR_VALUE (MSR_FP | MSR_CE | MSR_ME | MSR_EE)

#define  SRAM_START     0xC0000000
#define  SRAM_SIZE      0x2000
#define  SRAM_INITSEG   0xFFFFD000

/* Interrupt Vectors */
.set  ..crit_vector,  0x0100
.set  ..mach_vector,  0x0200
.set  ..prot_vector,  0x0300
.set  ..isi_vector,   0x0400
.set  ..ext_vector,   0x0500
.set  ..align_vector, 0x0600
.set  ..prog_vector,  0x0700
.set  ..fpu_vector,   0x0800
.set  ..sys_vector,   0x0900
.set  ..apu_vector,   0x0A00
.set  ..dec_vector,   0x0B00
.set  ..fit_vector,   0x0C00
.set  ..wadt_vector,  0x0D00
.set  ..dtlb_vector,  0x0E00
.set  ..itlb_vector,  0x0F00
.set  ..debug_vector, 0x0F80



/*---------------------------------------------------------------------------
 |
 |  Start of chip initialization. This is the entry point.
 |
 *-------------------------------------------------------------------------*/

    .text
    .align   2
_start:

    /* invalidate the instruction and data cache */
    iccci %r0, %r0
    dccci %r0, %r0
    sync

    /* read processor version register */
    mfspr r9,PVR
    addis r10,  0, PVR_440GP_PASS_1_0@h
    ori   r10,r10, PVR_440GP_PASS_1_0@l
    addis r11,  0, PVR_440GP_PASS_1_1@h
    ori   r11,r11, PVR_440GP_PASS_1_1@l

    /*  clear SRR0, CSRR0, SRR1, CSRR1 */
    xor   r0,r0,r0
    mtspr SRR0,r0
    mtspr SRR1,r0
    mtspr CSRR0,r0
    mtspr CSRR1,r0

    /* Clear CPM Registers (enable all functional CPU units) */
    mtspr CPC0_ER,r0
    mtspr CPC0_FR,r0

/*
    Clear DBCR0 register (disable all debug events) 
    Reset all other debugging registers associated with DBCR0.
    Although the PPC440GP is defined to reset some of the debug event 
    enables during the reset operation, this is not required by the 
    architecture and hence the initialization software should not assume 
    this behavior. Software should disable all debug events in order to 
    prevent non-deterministic behavior on the trace interface to the core.
*/
/*
    mtspr DBCR0,r0
    mtspr DBCR1,r0
    mtspr DBCR2,r0
    mtspr IAC1,r0
    mtspr IAC2,r0
    mtspr IAC3,r0
    mtspr IAC4,r0
    mtspr DAC1,r0
    mtspr DAC2,r0
    mtspr DVC1,r0
    mtspr DVC2,r0
*/

    /* Clear DBSR register (initialize all debug event status) */
/*
    addi  r0, 0,-1  // r0 = 0xFFFFFFFF
    addis r1, 0,DBSR_MRR_MASK@h
    ori   r1,r1,DBSR_MRR_MASK@l
    andc  r1,r0,r1  // Keep the DBSR[MRR] bits
    mtspr DBSR,r1
*/

/*
    Initialize CCR0 register 
    a. Enable store gathering
    b. Disable broadcast of instrs to aux processor (save power)
    c. Enable broadcast of trace information 
    d. Icbt is guaranteed to fill a cache line even during a stall
    e. Dcbt is guaranteed to fill a cache line even during a stall
    f. Disable exceptions for misaligned data accesses
    g. Instruction cache speculative line count = 0
    h. Instruction cache speculative line threshold = 0
*/
    addis r0, 0, CCR0_DAPUIB@h
    ori   r0,r0, CCR0_DAPUIB@l
    oris  r0,r0, CCR0_GICBT@h
    ori   r0,r0, CCR0_GICBT@l
    oris  r0,r0, CCR0_GDCBT@h
    ori   r0,r0, CCR0_GDCBT@l
    cmplw r10,r9
    beq   _440GP_PASS1_0
    cmplw r11,r9
    beq   _440GP_PASS1_0
    b     _440GP_PASS1_SKIP0
_440GP_PASS1_0:
    oris  r0,r0, CCR0_BTAC_DISABLE@h
    ori   r0,r0, CCR0_BTAC_DISABLE@l
_440GP_PASS1_SKIP0:
    mtspr CCR0,r0


/*
    Configure instruction and data cache regions 
    These steps must be performed prior to enabling the caches 
    a. Clear the instruction and data cache normal victim index registers 
    b. Clear the instruction and data cache transient victim index regs 
    c. Set the instruction and data cache victim limit registers 
*/
    xor   r0,r0,r0
    mtspr INV0,r0
    mtspr INV1,r0
    mtspr INV2,r0
    mtspr INV3,r0
    mtspr DNV0,r0
    mtspr DNV1,r0
    mtspr DNV2,r0
    mtspr DNV3,r0
    mtspr ITV0,r0
    mtspr ITV1,r0
    mtspr ITV2,r0
    mtspr ITV3,r0
    mtspr DTV0,r0
    mtspr DTV1,r0
    mtspr DTV2,r0
    mtspr DTV3,r0

    addis r0, 0, IVLIM_TFLOOR_ENCODE(0)@h
    ori   r0,r0, IVLIM_TFLOOR_ENCODE(0)@l
    oris  r0,r0, IVLIM_TCEILING_ENCODE(63)@h
    ori   r0,r0, IVLIM_TCEILING_ENCODE(63)@l
    oris  r0,r0, IVLIM_NFLOOR_ENCODE(0)@h
    ori   r0,r0, IVLIM_NFLOOR_ENCODE(0)@l 
    mtspr IVLIM,r0

    addis r0, 0, DVLIM_TFLOOR_ENCODE(0)@h
    ori   r0,r0, DVLIM_TFLOOR_ENCODE(0)@l
    oris  r0,r0, DVLIM_TCEILING_ENCODE(63)@h
    ori   r0,r0, DVLIM_TCEILING_ENCODE(63)@l
    oris  r0,r0, DVLIM_NFLOOR_ENCODE(0)@h
    ori   r0,r0, DVLIM_NFLOOR_ENCODE(0)@l 
    mtspr DVLIM,r0


    /* initialize MMUCR */
    addis r3, 0, MMUCR_DEFAULT@h
    ori   r3,r3, MMUCR_DEFAULT@l 
    mtspr MMUCR,r3

    /* Set TLB entry for DDR SDRAM:
       cached 0x00000000 -> 0x000000000  /  256MB
       Attributes: G/X/W/R */
    addi  r1, 0, 0x0000 /* Tlb entry #0 */
    addis r0, 0, 0x0000 /* EPN = 0x00000000 */
    ori   r0,r0, 0x0290 /* Valid, 256MB page size */
    tlbwe r0,r1, 0x0000 /* Write low part of entry */
    addis r0, 0, 0x0000 
    ori   r0,r0, 0x0000 /* RPN = 0x000000000 */
    tlbwe r0,r1, 0x0001 /* Write medium part of entry */
    addi  r0, 0, 0x013F /*    G, UX, UW, UR, SX, SW, SR = 1 */
    cmplw r10,r9
    beq   _440GP_PASSX_0
    cmplw r11,r9
    beq   _440GP_PASSX_0
    b     _440GP_PASSX_SKIP0
_440GP_PASSX_0:
    addi  r0, 0, 0x093F /* W, G, UX, UW, UR, SX, SW, SR = 1 */
_440GP_PASSX_SKIP0:
    tlbwe r0,r1, 0x0002 /* Write high part of entry */

    /* Set TLB entry for peripherals:
       0xE0000000 -> 0x140000000  /  256MB
       Attributes: I/G/W/R */
    addi  r1,  0, 0x0001 /* Tlb entry #1 */
    addis r0,  0, 0xE000 /* EPN = 0xE0000000 */
    ori   r0, r0, 0x0290 /* Valid, 256MB page size */
    tlbwe r0, r1, 0x0000 /* Write low part of entry */
    addis r0,  0, 0x4000
    ori   r0, r0, 0x0001 /* RPN = 0x140000000 */
    tlbwe r0, r1, 0x0001 /* Write medium part of entry */
    addi  r0,  0, 0x051B /* I, G, UW, UR, SW, SR = 1 */
    tlbwe r0, r1, 0x0002 /* Write high part of entry */
 
    /* Setup for subsequent change in instruction fetch address
       (Do a context switch to activate new TLB entries.) */
    mfmsr   r3
    mtspr   SRR1,r3
    addis   r0, 0, ..jumpover@h
    ori     r0,r0, ..jumpover@l
    mtspr   SRR0,r0
    rfi

    /* execution continues at label ..jumpover */


    /*-----------------------------------------------------------------------
     |  Interrupt Vectors - default handlers (entry points)
     *---------------------------------------------------------------------*/

    /* Critical vector */
    .align 2
..Critical_vector:
    addis   r3, 0,..crit_string@h
    ori     r3,r3,..crit_string@l
    bl      u_print
..crit_spin:
    b       ..crit_spin
..crit_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Critical exception detected"
    .byte   0x0d,0x0a,0x00,0x00,0x00,0x00

    /* Machine check exception */
    .align 2
..Machine_check_vector:
    addis   r3, 0,..mach_string@h 
    ori     r3,r3,..mach_string@l
    bl      u_print
..mach_spin:
    b       ..mach_spin
..mach_string:
    .4byte  0x0d0a0d0a
    .byte   "Machine check detected"
    .byte   0x0d,0x0a,0x00,0x00,0x00,0x00

    /* Data access protection exception */
    .align 2
..Data_protection_vector:
    addis   r3, 0,..prot_string@h
    ori     r3,r3,..prot_string@l
    bl      u_print
..prot_spin:
    b       ..prot_spin
..prot_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Data protection violation detected" 
    .byte   0x0d,0x0a,0x00

    /* Instruction Storage exception */
    .align 2
..Instruction_protection_vector:
    addis   r3, 0,..inst_prot_string@h
    ori     r3,r3,..inst_prot_string@l
    bl      u_print
..inst_prot_spin:
    b       ..inst_prot_spin
..inst_prot_string:
    .4byte 0x0d0a0d0a
    .byte "Unexpected Instruction storage protection violation detected"
    .byte  0x0d,0x0a,0x00,0x00

    /* External interrupt */
    .align 2
..External_vector:
    addis   r3, 0,..inst_ext_string@h
    ori     r3,r3,..inst_ext_string@l
    bl      u_print
..inst_ext_spin:
    b       ..inst_ext_spin
..inst_ext_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected external interrupt occurred"
    .byte   0x0d,0x0a,0x00,0x00,0x00,0x00

    /* Alignment exception */
    .align 2
..Alignment_vector:
    addis   r3, 0,..algn_string@h
    ori     r3,r3,..algn_string@l
    bl      u_print
..algn_spin:
    b       ..algn_spin
..algn_string:
    .4byte  0x0d0a0d0a
    .byte   "Alignment exception detected"
    .byte   0x0d,0x0a,0x00,0x00

    /* FPU exception */
    .align 2
..FPU_vector:
    addis   r3, 0,..fpu_string@h
    ori     r3,r3,..fpu_string@l
    bl      u_print
..fpu_spin:
    b       ..fpu_spin
..fpu_string:
    .4byte  0x0d0a0d0a
    .byte   "FPU exception detected"
    .byte   0x0d,0x0a,0x00,0x00,0x00,0x00

    /* Program exception */
    .align 2
..Program_vector:
    addis   r3, 0,..prog_string@h
    ori     r3,r3,..prog_string@l
    bl      u_print
..prog_spin:
    b       ..prog_spin
..prog_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Program exception detected"
    .byte   0x0d,0x0a,0x00

    /* System call */
    .align 2
..Sys_call_vector:
    addis   r3, 0,..sysc_string@h
    ori     r3,r3,..sysc_string@l
    bl      u_print
..sysc_spin:
    b       ..sysc_spin
..sysc_string:
    .4byte  0x0d0a0d0a
    .byte   "System call detected"
    .byte   0x0d,0x0a,0x00,0x00

    /* Auxiliary Processor Unavailable */
    .align 2
..AUX_vector:
    addis   r3, 0,..aux_string@h
    ori     r3,r3,..aux_string@l
    bl      u_print
..aux_spin:
    b       ..aux_spin
..aux_string:
    .4byte  0x0d0a0d0a
    .byte   "auxiliary processor unavailable detected"
    .byte   0x0d,0x0a,0x00,0x00

    /* Programmable interval timer */
    .align 2
..Pg_timer_vector:
    addis   r3, 0,..inst_timer_string@h
    ori     r3,r3,..inst_timer_string@l
    bl      u_print
..inst_timer_spin:
    b       ..inst_timer_spin
..inst_timer_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected timer interrupt occurred"
    .byte   0x0d,0x0a,0x00,0x00,0x00

    /* Fixed interval timer exception */
    .align 2
..Fx_timer_vector:
    addis   r3, 0,..fint_string@h
    ori     r3,r3,..fint_string@l 
    bl      u_print
..fint_spin:
    b       ..fint_spin
..fint_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Fixed interval timer expiration detected"
    .byte   0x0d,0x0a,0x00,0x00,0x00

    /* Watchdog timer exception */
    .align 2
..Watchdog_vector:
    addis   r3, 0,..wchd_string@h 
    ori     r3,r3,..wchd_string@l          
    bl      u_print
..wchd_spin:
    b       ..wchd_spin
..wchd_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Watchdog timer expiration detected"
    .byte   0x0d,0x0a,0x00

    /* Data TLB Miss exception */
    .align 2
..Data_TLB_miss_vector:
    addis   r3, 0,..dtlb_string@h
    ori     r3,r3,..dtlb_string@l
    bl      u_print
..dtlb_spin:
    b       ..dtlb_spin
..dtlb_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Data TLB Miss detected"
    .byte   0x0d,0x0a,0x00

    /* Instruction TLB Miss exception */
    .align 2
..Inst_TLB_miss_vector:
    addis   r3, 0,..itlb_string@h
    ori     r3,r3,..itlb_string@l
    bl      u_print
..itlb_spin:
    b       ..itlb_spin
..itlb_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Instruction TLB Miss detected"
    .byte   0x0d,0x0a,0x00,0x00

    /* Debug exception */
    .align 2
..Debug_vector:
    addis   r3, 0,..debg_string@h 
    ori     r3,r3,..debg_string@l          
    bl      u_print
..debg_spin:
    b       ..debg_spin
..debg_string:
    .4byte  0x0d0a0d0a
    .byte   "Unexpected Debug exception detected"
    .byte   0x0d,0x0a,0x00,0x00,0x00


        .align 2
..jumpover:

    /*  Initialize interrupt vector jump table */
    xor     r0,r0,r0   
    mtspr   IVPR,r0
    ori     r4,r0,..crit_vector
    mtspr   IVOR0,r4
    ori     r4,r0,..mach_vector
    mtspr   IVOR1,r4
    ori     r4,r0,..prot_vector
    mtspr   IVOR2,r4
    ori     r4,r0,..isi_vector
    mtspr   IVOR3,r4
    ori     r4,r0,..ext_vector
    mtspr   IVOR4,r4
    ori     r4,r0,..align_vector
    mtspr   IVOR5,r4
    ori     r4,r0,..prog_vector
    mtspr   IVOR6,r4
    ori     r4,r0,..fpu_vector
    mtspr   IVOR7,r4
    ori     r4,r0,..sys_vector
    mtspr   IVOR8,r4
    ori     r4,r0,..apu_vector
    mtspr   IVOR9,r4
    ori     r4,r0,..dec_vector
    mtspr   IVOR10,r4
    ori     r4,r0,..fit_vector
    mtspr   IVOR11,r4
    ori     r4,r0,..wadt_vector
    mtspr   IVOR12,r4
    ori     r4,r0,..dtlb_vector
    mtspr   IVOR13,r4
    ori     r4,r0,..itlb_vector
    mtspr   IVOR14,r4
    ori     r4,r0,..debug_vector
    mtspr   IVOR15,r4

    mflr    r1   /* save link reg in r1 */
    bl      ..end_list
    .long   ..crit_vector
    ba      ..Critical_vector
    .long   ..mach_vector
    ba      ..Machine_check_vector
    .long   ..prot_vector
    ba      ..Data_protection_vector
    .long   ..isi_vector
    ba      ..Instruction_protection_vector
    .long   ..ext_vector
    /* ba      ..External_vector */
    ba      p_asm_extISR
    .long   ..align_vector
    ba      ..Alignment_vector
    .long   ..prog_vector
    ba      ..Program_vector
    .long   ..fpu_vector
    ba      ..FPU_vector
    .long   ..sys_vector
    ba      ..Sys_call_vector
    .long   ..apu_vector
    ba      ..AUX_vector
    .long   ..dec_vector
    /* ba      ..Pg_timer_vector */
    ba      p_asm_timerISR
    .long   ..fit_vector
    ba      ..Fx_timer_vector
    .long   ..wadt_vector
    ba      ..Watchdog_vector
    .long   ..dtlb_vector 
    ba      ..Data_TLB_miss_vector
    .long   ..itlb_vector
    ba      ..Inst_TLB_miss_vector
    .long   ..debug_vector 
    ba      ..Debug_vector
    .long 0
    .long 0
..end_list:
    mflr    r4
    addi    r4,r4,-8   /* adjust for loop load */
..vec_loop:
    lwzu    r5,8(r4)
    lwz     r6,4(r4)
    cmpw    0,r5,0
    beq     ..vec_done
    stw     r6,0(r5)    /* store branch */
    dcbf    r0,r5       /* flush to storage */
    b       ..vec_loop
..vec_done:
    isync
    mtlr    r1


    /* initialize CPC0_CR0
       (set ext. clock for UART0) */
    addis   r3, 0,CPC0_CR0_INIT@h
    ori     r3,r3,CPC0_CR0_INIT@l
    mtdcr   CPC0_CR0,r3

    /* initialize UART0 */
    addi    r3, 0,(UART_DIV_9600_LO+(UART_DIV_9600_HI<<8))
    addis   r7, 0,UART0_MMIO_BASE@h
    ori     r7,r7,UART0_MMIO_BASE@l
    addi    r4, 0,0x80                /* get access to baudrate registers */
    stb     r4,UART_LINE_CONTROL(r7)
    stb     r3,UART_DL_LSB(r7)
    rlwinm  r4,r3,24,0x000000FF
    stb     r4,UART_DL_MSB(r7)
    addi    r4, 0,0x03                /* lcr: 8 bits, no parity */
    stb     r4,UART_LINE_CONTROL(r7)
    addi    r4, 0,0x00                /* disable FIFO */
    stb     r4,UART_FIFO_CONTROL(r7)
    addi    r4, 0,0x03                /* modem control: DTR RTS */
    stb     r4,UART_MODEM_CONTROL(r7)
    lbz     r3,UART_LINE_STATUS(r7)   /* clear line status */
    lbz     r3,UART_DATA_REG(r7)      /* read receive buffer */
    addi    r4, 0,0x00                /* set interrupt enable reg */
    stb     r4,UART_INT_ENABLE(r7)

    /* clear timer registers */
    xor     r0,r0,r0  /* r0 = 0x00000000 */
    addi    r1, 0,-1  /* r1 = 0xFFFFFFFF */
    mtspr   DEC,r0
    mtspr   TBLW,r0
    mtspr   TBUW,r0
    mtspr   TSR,r1
    mtspr   TCR,r0

    /* reset interrupt controllers */
    mtdcr   UIC1_ER,r0
    mtdcr   UIC1_SR,r1
    mtdcr   UIC0_ER,r0
    mtdcr   UIC0_SR,r1

    /* clear misc registers */
    mtspr   ESR,r0
    mtspr   XER,r0

    /* initialize MSR */
    addis   r3, 0,MSR_VALUE@h
    ori     r3,r3,MSR_VALUE@l
    mtmsr   r3

    /* set up stackpointer, write zero stack frame */
    addis   r1, 0,__STACK_TOP@h
    ori     r1,r1,__STACK_TOP@l
    rlwinm  r1,r1, 0, 0xFFFFFFE0 /* align stack */
    addi    r3, 0,32/4
    mtctr   r3
..wzero:
    stwu    r0,-4(r1)
    bdnz    ..wzero

    /* jump to C init function */
    bl      c_start
..halt:
    b       ..halt



/*---------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void u_print(char *str);
 |
 |    Terminal output to UART0:
 |      write a character string to the terminal
 |
 *-------------------------------------------------------------------------*/

    .align  2
u_print:
    mflr    r6
    addi    r7,r3,0
..uploop:
    lbz     r3,0x0(r7)   /* get next char   */
    cmpi    0,0,r3,0x00  /* end of string?  */
    beq     ..upend
    bl      u_putch      /* print character */
    addi    r7,r7,1
    b       ..uploop
..upend:
    mtlr    r6
    blr
    ENDFUNC(u_print)



/*---------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void u_putch(char c);
 |
 |    Terminal output to UART0:
 |      write a character to the terminal
 |
 *-------------------------------------------------------------------------*/

    .align  2
u_putch:
    addis   r4, 0,UART0_MMIO_BASE@h
    ori     r4,r4,UART0_MMIO_BASE@l
    cmpi    0,0,r3,10  /* line feed? */
    bne     ..u_pwait2
    xor     r0,r0,r0
    ori     r0,r0,13
..u_pwait1:
    lbz     r5,UART_LINE_STATUS(r4)
    eieio
    andi.   r5,r5,0x20  /* test THRE bit */
    beq     ..u_pwait1
    stb     r0,UART_DATA_REG(r4)
    eieio
..u_pwait2:
    lbz     r5,UART_LINE_STATUS(r4)
    eieio
    andi.   r5,r5,0x20  /* test THRE bit */
    beq     ..u_pwait2
    stb     r3,UART_DATA_REG(r4)
    eieio
    blr
    ENDFUNC(u_putch)



/*---------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    int u_getch(void);
 |
 |    Terminal input from UART0:
 |      read a character from the terminal
 |
 *-------------------------------------------------------------------------*/

    .align  2
u_getch:
    addis   r4, 0,UART0_MMIO_BASE@h
    ori     r4,r4,UART0_MMIO_BASE@l
..u_gwait:
    lbz     r3,UART_LINE_STATUS(r4)
    eieio
    andi.   r3,r3,0x01  /* test DR bit */
    beq     ..u_gwait 
    lbz     r3,UART_DATA_REG(r4)
    blr
    ENDFUNC(u_getch)



/*---------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    int u_kbhit(void);
 |
 |    Terminal input from UART0:
 |      returns nonzero when a key was hit on the terminal
 |
 *-------------------------------------------------------------------------*/

    .align  2
u_kbhit:
    addis   r4, 0,UART0_MMIO_BASE@h
    ori     r4,r4,UART0_MMIO_BASE@l
    lbz     r3,UART_LINE_STATUS(r4)
    eieio
    andi.   r3,r3,0x01  /* get DR bit */
    blr
    ENDFUNC(u_kbhit)


