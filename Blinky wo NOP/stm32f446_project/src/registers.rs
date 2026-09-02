use core::ops::{BitOrAssign, BitAndAssign, Not};

#[repr(C)]
pub struct Register {
    pub value: u32,
}

impl Register {
    pub const fn new() -> Self {
        Register { value: 0 }
    }
}

impl BitOrAssign<u32> for Register {
    fn bitor_assign(&mut self, rhs: u32) {
        self.value |= rhs;
    }
}

impl BitAndAssign<u32> for Register {
    fn bitand_assign(&mut self, rhs: u32) {
        self.value &= rhs;
    }
}

#[repr(C)]
pub struct RCC {
    pub cr: Register,
    pub pllcfgr: Register,
    pub cfgr: Register,
    pub cir: Register,
    pub ahb1rstr: Register,
    pub ahb2rstr: Register,
    pub ahb3rstr: Register,
    _reserved0: [u32; 1],
    pub apb1rstr: Register,
    pub apb2rstr: Register,
    _reserved1: [u32; 2],
    pub ahb1enr: Register,
    pub ahb2enr: Register,
    pub ahb3enr: Register,
    _reserved2: [u32; 1],
    pub apb1enr: Register,
    pub apb2enr: Register,
}

#[repr(C)]
pub struct GPIO {
    pub moder: Register,
    pub otyper: Register,
    pub ospeedr: Register,
    pub pupdr: Register,
    pub idr: Register,
    pub odr: Register,
    pub bsrr: Register,
    pub lckr: Register,
    pub afr: [Register; 2],
}

#[repr(C)]
pub struct TIMER {
    pub cr1: Register,
    pub cr2: Register,
    pub smcr: Register,
    pub dier: Register,
    pub sr: Register,
    pub egr: Register,
    pub ccmr1: Register,
    pub ccmr2: Register,
    pub ccer: Register,
    pub cnt: Register,
    pub psc: Register,
    pub arr: Register,
}

pub const RCC: *mut RCC = 0x40023800 as *mut RCC;
pub const GPIOA: *mut GPIO = 0x40020000 as *mut GPIO;
pub const TIM6: *mut TIMER = 0x40001000 as *mut TIMER; 