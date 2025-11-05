# Syscall Implementation Explanation

## Overview

This document explains how syscalls work in this STM32 embedded system, focusing on everything except the `read` syscall.

---

## 1. SVC_CALL Macro - The Magic Bridge

**Location:** `userland/utils/unistd.c`

The `SVC_CALL` macro is the key to invoking syscalls from user code:

```c
#define SVC_CALL(num, a0, a1, a2, a3)                                          \
  ({                                                                           \
    register uint32_t r0 __asm("r0") = (uint32_t)(a0);   /* Arg 1 */         \
    register uint32_t r1 __asm("r1") = (uint32_t)(a1);   /* Arg 2 */         \
    register uint32_t r2 __asm("r2") = (uint32_t)(a2);   /* Arg 3 */         \
    register uint32_t r3 __asm("r3") = (uint32_t)(a3);   /* Arg 4 */         \
    __asm volatile("svc %[imm]"                           /* Execute SVC */   \
                   : "+r"(r0)                             /* Output: r0 */    \
                   : [imm] "I"(num), "r"(r1), "r"(r2), "r"(r3)  /* Input */  \
                   : "memory");                           /* Clobber */       \
    r0;                                                   /* Return r0 */     \
  })
```

### How It Works:

1. **Loads arguments into ARM registers:**
   - `r0` = first argument (a0) - also used for return value
   - `r1` = second argument (a1)
   - `r2` = third argument (a2)
   - `r3` = fourth argument (a3)

2. **Executes SVC instruction:**
   - `svc %[imm]` - The `%[imm]` is replaced with the syscall number
   - Example: `svc #55` for `SYS_write`

3. **Returns value from r0:**
   - After the syscall completes, r0 contains the return value
   - The macro returns this value

### Example Usage:

```c
write(1, "Hello", 5);
```

Expands to:
```c
r0 = 1        // fd (STDOUT_FILENO)
r1 = "Hello"  // buffer address
r2 = 5        // count
r3 = 0        // unused
svc #55       // Execute SVC with number 55 (SYS_write)
return r0;    // Return value (bytes written)
```

---

## 2. SVCall_Handler - The Exception Entry Point

**Location:** `kern/arch/stm32f446re/sys_lib/stm32_startup.c`

When the `svc` instruction executes, ARM Cortex-M4 hardware automatically:

1. **Saves CPU state** to the stack (r0-r3, r12, LR, PC, xPSR)
2. **Switches to Handler mode** (privileged)
3. **Jumps to SVCall_Handler**

### Assembly Handler (Naked Function):

```c
__attribute__((naked)) void SVCall_Handler(void) {
  __asm volatile(
    "tst lr, #4\n"        /* Test bit 2 of LR (EXC_RETURN) */
    "ite eq\n"            /* If-Then-Else */
    "mrseq r0, msp\n"     /* If EQ: r0 = MSP (Main Stack Pointer) */
    "mrsne r0, psp\n"     /* If NE: r0 = PSP (Process Stack Pointer) */
    "b SVC_Handler_C\n"   /* Branch to C handler */
  );
}
```

### Why "naked"?

- **Normal C functions** automatically create a stack frame (push/pop registers)
- **Naked functions** = pure assembly, no compiler-generated code
- We need this because the exception stack frame is already set up by hardware
- Adding our own stack frame would corrupt the saved registers

### What It Does:

1. **Tests LR bit 2:**
   - `LR` (Link Register) contains `EXC_RETURN` value after exception
   - Bit 2 = 0 → Use MSP (Main Stack Pointer) - kernel mode
   - Bit 2 = 1 → Use PSP (Process Stack Pointer) - user mode

2. **Loads correct stack pointer into r0:**
   - `mrseq r0, msp` - if bit 2 = 0, r0 = MSP
   - `mrsne r0, psp` - if bit 2 = 1, r0 = PSP

3. **Branches to C handler:**
   - Passes stack pointer in r0 to `SVC_Handler_C()`

### EXC_RETURN Values:

| Value | Bit 2 | Meaning |
|-------|-------|---------|
| 0xFFFFFFF1 | 0 | Return to Handler mode, use MSP |
| 0xFFFFFFF9 | 0 | Return to Thread mode, use MSP |
| 0xFFFFFFFD | 1 | Return to Thread mode, use PSP |

---

## 3. SVC_Handler_C - Extract Info and Dispatch

**Location:** `kern/arch/stm32f446re/sys_lib/stm32_startup.c`

This C function receives the stack pointer and extracts all necessary information:

```c
void SVC_Handler_C(uint32_t *stack) {
  /* Step 1: Get return address (PC) from stack frame */
  uint32_t pc = stack[6];
  
  /* Step 2: Extract SVC number from instruction */
  uint8_t svc_no = ((const uint8_t *)(pc - 2U))[0];
  
  /* Step 3: Extract arguments from stack */
  uint32_t a0 = stack[0];  /* r0 - Argument 1 / Return value */
  uint32_t a1 = stack[1];  /* r1 - Argument 2 */
  uint32_t a2 = stack[2];  /* r2 - Argument 3 */
  uint32_t a3 = stack[3];  /* r3 - Argument 4 */
  
  /* Step 4: Call kernel dispatcher */
  uint32_t rc = syscall_dispatch((uint16_t)svc_no, a0, a1, a2, a3);
  
  /* Step 5: Store return value back to r0 in stack */
  stack[0] = rc;
}
```

### Stack Frame Layout (Hardware-Pushed):

```
Higher Memory
┌─────────────┐
│   xPSR      │  stack[7]  (Program Status Register)
├─────────────┤
│   PC        │  stack[6]  (Return address) ← Used to find SVC instruction
├─────────────┤
│   LR        │  stack[5]  (Link Register)
├─────────────┤
│   R12       │  stack[4]
├─────────────┤
│   R3        │  stack[3]  (Argument 4)
├─────────────┤
│   R2        │  stack[2]  (Argument 3)
├─────────────┤
│   R1        │  stack[1]  (Argument 2)
├─────────────┤
│   R0        │  stack[0]  (Argument 1 / Return value) ← Modified here
└─────────────┘
Lower Memory (Stack grows down)
```

### Why PC-2?

- PC points to the instruction **after** the `svc` instruction
- In ARM Thumb mode, PC = current instruction + 4 bytes
- `svc` instruction is 2 bytes
- So: PC - 2 = address of `svc` instruction
- The SVC number is encoded in the immediate field of the instruction

### SVC Instruction Encoding:

```
ARM Thumb-2 Instruction: svc #55
┌─────────────┬────────────┐
│  1101 1111  │  0011 0111 │  (16-bit instruction)
└─────────────┴────────────┘
     Opcode    #55 (0x37)
```

---

## 4. syscall_dispatch - The Kernel Dispatcher

**Location:** `kern/syscall/syscall.c`

This is the main switch statement that routes syscalls to their implementations:

### SYS_write (55) - Write to UART

```c
case SYS_write: {
  uint32_t fd = a0;
  const uint8_t *buf = (const uint8_t *)a1;
  uint32_t len = a2;
  
  if (fd != STDOUT_FILENO || buf == 0 || len == 0U) {
    return 0U;
  }
  
  /* Write to UART */
  for (uint32_t i = 0; i < len; i++) {
    Uart_write(buf[i], __CONSOLE);
  }
  
  return len;  /* Return bytes written */
}
```

**Flow:**
1. Validates file descriptor (must be STDOUT_FILENO = 1)
2. Validates buffer pointer and length
3. Loops through each byte and writes to UART
4. Returns number of bytes written

**Example:**
```c
write(1, "Hi", 2);
```
- a0 = 1 (STDOUT_FILENO)
- a1 = address of "Hi"
- a2 = 2
- Result: "Hi" appears on serial console, returns 2

---

### SYS_getpid (5) - Get Process ID

```c
case SYS_getpid: {
  return (uint32_t)g_current_task_id;
}
```

**Flow:**
1. Simply returns the global `g_current_task_id` variable
2. No validation needed
3. Very fast - just a memory read

**Example:**
```c
int pid = getpid();
```
- Returns: 1000 (or whatever `g_current_task_id` is set to)

---

### SYS___time (113) - Get System Time

```c
case SYS___time: {
  return __getTime();
}
```

**Flow:**
1. Calls `__getTime()` which returns SysTick counter in milliseconds
2. SysTick is incremented every 1ms by `SysTick_Handler`
3. Returns elapsed time since system boot

**Example:**
```c
uint32_t time = getSysTickTime();
```
- Returns: milliseconds since boot (e.g., 120)

---

### SYS_reboot (119) - System Reset

```c
case SYS_reboot: {
  __NVIC_SystemReset();
  return 0U;  // Never reached
}
```

**Flow:**
1. Calls `__NVIC_SystemReset()` which:
   - Sets SYSRESETREQ bit in SCB->AIRCR
   - Triggers a full system reset
   - CPU restarts from Reset_Handler
2. Never returns (system resets)

**Example:**
```c
reboot();  // System immediately resets
```

---

### SYS_yield (120) - Voluntary Context Switch

```c
case SYS_yield: {
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
  return 0U;
}
```

**Flow:**
1. Sets PENDSVSET bit in SCB->ICSR register
2. This "pends" (schedules) a PendSV exception
3. PendSV runs at lowest priority, so it executes after higher priority interrupts
4. Currently, PendSV_Handler just returns (stub for future context switching)

**Why PendSV?**
- PendSV is designed for context switching
- Runs at lowest priority (doesn't interrupt critical code)
- Can be safely used for task switching

**Example:**
```c
yield();  // Triggers PendSV, allows other tasks to run
```

---

### SYS__exit (3) - Terminate Process

```c
case SYS__exit: {
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
  return 0U;
}
```

**Flow:**
1. Same as yield - triggers PendSV
2. In a full OS, PendSV would:
   - Save current task state
   - Mark task as terminated
   - Switch to next ready task
3. Currently just triggers PendSV (stub)

**Example:**
```c
exit(0);  // Terminates current process
```

---

## 5. Return Value Handling

After `syscall_dispatch` returns:

```c
/* Step 5: Store return value back to r0 in stack */
stack[0] = rc;
```

**What happens:**
1. Return value is stored in `stack[0]` (where r0 was saved)
2. When exception returns, hardware pops stack
3. r0 is restored with the return value
4. User code receives the return value

**Example:**
```c
int bytes = write(1, "Hi", 2);
// bytes = 2 (return value from syscall)
```

---

## 6. PendSV_Handler - Context Switching Stub

**Location:** `kern/arch/stm32f446re/sys_lib/stm32_startup.c`

```c
void PendSV_Handler(void) {
  /* PendSV stub - currently just returns immediately */
  /* In a full implementation, this would:
   *   1. Save current task context (registers, stack pointer)
   *   2. Select next task (scheduler)
   *   3. Restore next task context
   *   4. Return (which switches to next task)
   */
  return;
}
```

**Current Status:**
- Just returns immediately
- Allows `yield()` and `exit()` to work without hanging
- Placeholder for future full context switching implementation

**Future Implementation Would:**
1. Save all registers (r0-r12, LR, PC, xPSR) to current task's TCB
2. Save stack pointer (PSP) to current task's TCB
3. Call scheduler to select next task
4. Load next task's stack pointer
5. Restore all registers from next task's TCB
6. Return (which switches to next task)

---

## 7. Complete Flow Example: write()

Let's trace a complete `write()` syscall:

```
User Code:
  write(1, "Hi", 2);
         ↓
SVC_CALL Macro:
  r0 = 1
  r1 = address of "Hi"
  r2 = 2
  r3 = 0
  svc #55  ← Hardware exception triggered
         ↓
Hardware (ARM Cortex-M4):
  - Push r0-r3, r12, LR, PC, xPSR to PSP
  - Set LR = 0xFFFFFFFD (return to PSP)
  - Switch to Handler mode
  - Jump to SVCall_Handler
         ↓
SVCall_Handler (Assembly):
  tst lr, #4  → bit 2 = 1 (PSP mode)
  mrsne r0, psp  → r0 = PSP address
  b SVC_Handler_C
         ↓
SVC_Handler_C:
  pc = stack[6]  → Get return address
  svc_no = *(pc-2)  → Extract 55 from instruction
  a0 = stack[0] = 1
  a1 = stack[1] = address of "Hi"
  a2 = stack[2] = 2
  a3 = stack[3] = 0
  rc = syscall_dispatch(55, 1, "Hi", 2, 0)
         ↓
syscall_dispatch:
  case SYS_write:
    for (i=0; i<2; i++)
      Uart_write("Hi"[i], __CONSOLE)
    return 2
         ↓
SVC_Handler_C:
  stack[0] = 2  ← Store return value
         ↓
Hardware (Exception Return):
  - Pop r0-r3, r12, LR, PC, xPSR from PSP
  - r0 = 2 (return value)
  - Switch to Thread mode
  - Continue after svc instruction
         ↓
User Code:
  int bytes = write(...);  // bytes = 2
```

---

## 8. Key Design Decisions

### Why Naked Function for SVCall_Handler?

**Problem:** Normal C functions create stack frames:
```c
void normal_function() {
  // Compiler generates:
  push {r4-r7, lr}  // Save registers
  // ... function body ...
  pop {r4-r7, pc}   // Restore and return
}
```

**Issue:** This would corrupt the exception stack frame!

**Solution:** Naked function = pure assembly, no compiler code:
```c
__attribute__((naked)) void SVCall_Handler(void) {
  // Only our assembly code, no stack frame manipulation
}
```

### Why Extract SVC Number from Instruction?

**Alternative approaches:**
- ❌ Pass in register: Wastes a register, not standard
- ❌ Fixed syscall table: Inflexible
- ✅ Extract from instruction: Standard ARM practice, efficient

### Why Two Stack Pointers (MSP/PSP)?

- **MSP (Main Stack Pointer):** Kernel/exception handlers use this
- **PSP (Process Stack Pointer):** User tasks use this
- **Separation:** Prevents user code from corrupting kernel stack
- **Security:** Kernel can access both, user can only access PSP

### Why PendSV for Context Switching?

- **Lowest priority:** Doesn't interrupt critical code
- **Pendable:** Can be scheduled, doesn't execute immediately
- **Designed for this:** ARM specifically designed PendSV for context switching

---

## 9. Testing the Syscalls

### Example Test Code:

```c
void kmain(void) {
  __sys_init();
  g_current_task_id = 1000;
  
  // Test write
  write(1, "Hello\n", 6);
  
  // Test getpid
  int pid = getpid();
  kprintf("PID: %d\n", pid);
  
  // Test getSysTickTime
  uint32_t time = getSysTickTime();
  kprintf("Time: %d ms\n", time);
  
  // Test yield
  yield();
  kprintf("After yield\n");
  
  while(1) {}
}
```

---

## Summary

The syscall mechanism provides:

1. **User/Kernel Separation:** User code uses SVC to request kernel services
2. **Controlled Entry:** Only way to enter kernel mode is via SVC
3. **Argument Passing:** Efficient via registers (r0-r3)
4. **Return Values:** Via r0 register
5. **Stack Safety:** Separate stacks for user and kernel
6. **Extensibility:** Easy to add new syscalls

All syscalls follow the same pattern:
1. User calls wrapper function (e.g., `write()`)
2. Wrapper uses SVC_CALL macro
3. Hardware triggers exception
4. SVCall_Handler extracts info
5. syscall_dispatch routes to implementation
6. Return value passed back via stack
7. Hardware restores context
8. User receives return value

This is a complete, working syscall implementation for ARM Cortex-M4!

