use core::ptr::{read_volatile, write_volatile};
use crate::registers::RCC;

// Register addresses
const RCC_BASE: u32 = 0x40023800;
const RCC_CR: u32 = RCC_BASE;
const RCC_CFGR: u32 = RCC_BASE + 0x08;
const RCC_PLLCFGR: u32 = RCC_BASE + 0x84;
const RCC_APB1ENR: u32 = RCC_BASE + 0x40;

const FLASH_BASE: u32 = 0x40023C00;
const FLASH_ACR: u32 = FLASH_BASE;

const PWR_BASE: u32 = 0x40007000;
const PWR_CR: u32 = PWR_BASE;

pub fn configure_system_clock() {
    unsafe {
        // 1. Enable HSE and wait for it to become ready
        RCC.as_mut().unwrap().cr.value |= (1 << 16);  // HSEON
        while (RCC.as_ref().unwrap().cr.value & (1 << 17)) == 0 {}  // HSERDY

        // 2. Enable PWR clock and set voltage regulator
        RCC.as_mut().unwrap().apb1enr.value |= (1 << 28);  // PWREN
        
        let pwr_cr = read_volatile(PWR_CR as *const u32);
        write_volatile(PWR_CR as *mut u32, pwr_cr | (1 << 14)); // VOS

        // 3. Configure Flash prefetch and latency
        let flash_acr = 0x40023C00 as *mut u32;
        *flash_acr |= (1 << 8) | (1 << 9) | (1 << 10) | (5 << 0);  // ICEN | DCEN | PRFTEN | LATENCY_5WS

        // 4. Configure prescalers
        // Clear and set prescalers
        RCC.as_mut().unwrap().cfgr.value &= !(0xF << 4);  // Clear HPRE
        RCC.as_mut().unwrap().cfgr.value &= !(0x7 << 10);  // Clear PPRE1
        RCC.as_mut().unwrap().cfgr.value &= !(0x7 << 13);  // Clear PPRE2
        
        // Set prescalers
        RCC.as_mut().unwrap().cfgr.value |= (0x0 << 4);   // HPRE = DIV1
        RCC.as_mut().unwrap().cfgr.value |= (0x5 << 10);  // PPRE1 = DIV4
        RCC.as_mut().unwrap().cfgr.value |= (0x4 << 13);  // PPRE2 = DIV2

        // 5. Configure PLL
        RCC.as_mut().unwrap().pllcfgr.value = (4 << 0) |    // PLLM = 4
                              (180 << 6) |    // PLLN = 180
                              (0 << 16) |     // PLLP = 2
                              (1 << 22);      // PLLSRC = HSE

        // 6. Enable PLL and wait for it to become ready
        RCC.as_mut().unwrap().cr.value |= (1 << 24);  // PLLON
        while (RCC.as_ref().unwrap().cr.value & (1 << 25)) == 0 {}  // PLLRDY

        // 7. Select PLL as system clock
        RCC.as_mut().unwrap().cfgr.value &= !0x3;  // Clear SW
        RCC.as_mut().unwrap().cfgr.value |= 0x2;   // SW = PLL
        while ((RCC.as_ref().unwrap().cfgr.value >> 2) & 0x3) != 0x2 {}  // SWS = PLL
    }
} 