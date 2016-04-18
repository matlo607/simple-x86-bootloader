#ifndef _x86_REGS_H
#define _x86_REGS_H

#include <arch/x86/checkCCversion.h>

#include <stdint.h>

typedef union x86_general_purpose_reg_u {
    struct {
        uint8_t _Rl;
        uint8_t _Rh;
    };
    uint16_t _Rx;
    uint32_t _eRx;
} x86_general_purpose_reg_t;

typedef uint16_t x86_segment_reg_t;

union x86_index_pointer_reg_u {
    uint16_t _R;
    uint32_t _eR;
};

typedef union x86_index_pointer_reg_u x86_index_reg_t;
typedef union x86_index_pointer_reg_u x86_pointer_reg_t;

typedef struct x86_flags_s {
    uint32_t CF : 1; // Carry flag
    uint32_t reserved_bit1 : 1;
    uint32_t PF : 1; // Parity flag
    uint32_t reserved_bit3 : 1;
    uint32_t AF : 1; // Auxiliary carry flag
    uint32_t reserved_bit5 : 1;
    uint32_t ZF : 1; // Zero flag
    uint32_t SF : 1; // Sign flag
    uint32_t TF : 1; // Trap flag
    uint32_t IF : 1; // Interrupt enable flag
    uint32_t DF : 1; // Direction flag
    uint32_t OF : 1; // Overflow flag
    uint32_t IOPL : 2; // I/O Priviledge level
    uint32_t NT : 1; // Nested task flag
    uint32_t reserved_bit15 : 1;
    uint32_t RF : 1; // Resume flag
    uint32_t VM : 1; // Virtual 8086 mode flag
    uint32_t AC : 1; // Alignment check flag (486+)
    uint32_t VIF : 1; // Virtual interrupt flag
    uint32_t VIP : 1; // Virtual interrupt pending flag
    uint32_t ID : 1; // ID flag;
    uint32_t reserved_bits_22_to_31 : 10;
} __attribute__((packed)) x86_flags_t;


#define x86_FLAGS_CF    1
#define x86_FLAGS_PF    (1 << 2)
#define x86_FLAGS_AF    (1 << 4)
#define x86_FLAGS_ZF    (1 << 6)
#define x86_FLAGS_SF    (1 << 7)
#define x86_FLAGS_TF    (1 << 8)
#define x86_FLAGS_IF    (1 << 9)
#define x86_FLAGS_DF    (1 << 10)
#define x86_FLAGS_OF    (1 << 11)
#define x86_FLAGS_IOPL  (3 << 12)
#define x86_FLAGS_NT    (1 << 14)
#define x86_FLAGS_RF    (1 << 16)
#define x86_FLAGS_VM    (1 << 17)
#define x86_FLAGS_AC    (1 << 18)
#define x86_FLAGS_VIF   (1 << 19)
#define x86_FLAGS_VIP   (1 << 20)
#define x86_FLAGS_ID    (1 << 21)


typedef struct x86_regs_s {
    x86_segment_reg_t DS; // data segment
    x86_segment_reg_t ES; // extended data segment
    x86_segment_reg_t FS; // extended data segment
    x86_segment_reg_t GS; // extended data segment

    x86_pointer_reg_t BP; // base pointer

    // general purpose registers
    x86_general_purpose_reg_t A; // accumulator register
    x86_general_purpose_reg_t B; // base register
    x86_general_purpose_reg_t C; // counter register
    x86_general_purpose_reg_t D; // data register

    x86_flags_t eflags; // flags register

    x86_index_reg_t DI; // destination index register
    x86_index_reg_t SI; // source index register
} __attribute__((packed)) x86_regs_t;


/*
 * \brief Initialize a x86_regs_t data structure.
 * \param[in] p_regs : pointer on the data structure to initialize
 *
 * Firstly the data structure is zeroed and then, %ds, %es segment
 * registers are initialized with the current values.
 * */
extern void x86_regs_init(x86_regs_t* p_regs);

#ifdef DEBUG
extern void x86_regs_debug_print(const x86_regs_t* p_regs);
#endif

#endif
