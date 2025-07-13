use stm32f4::stm32f446;
use core::cell::RefCell;
use cortex_m::interrupt::Mutex;
use heapless::String;

// Global buffer for received data
static UART_RX_BUFFER: Mutex<RefCell<heapless::Vec<u8, 64>>> = 
    Mutex::new(RefCell::new(heapless::Vec::new()));

pub fn uart_init(dp: &stm32f446::Peripherals) {
    // Enable USART2 and GPIOA clocks
    dp.RCC.apb1enr.modify(|_, w| w.usart2en().set_bit());
    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    
    // Configure PA2 (TX) and PA3 (RX) as alternate function
    dp.GPIOA.moder.modify(|_, w| {
        w.moder2().alternate()
         .moder3().alternate()
    });
    
    // Set alternate function 7 for USART2
    dp.GPIOA.afrl.modify(|_, w| {
        w.afrl2().af7()
         .afrl3().af7()
    });
    
    // Configure USART2
    dp.USART2.cr1.modify(|_, w| {
        w.m().clear_bit()     // 8 data bits
         .pce().clear_bit()   // No parity
         .te().set_bit()      // Enable transmitter
         .re().set_bit()      // Enable receiver
         .rxneie().set_bit()  // Enable RX interrupt
    });

    dp.USART2.cr2.modify(|_, w| {
        w.stop().bits(0b00)   // 1 stop bit
    });

    // Baud rate for USART2 (APB1 clock is typically 45MHz)
    dp.USART2.brr.write(|w| unsafe { w.bits(4687) }); // 9600 baud

    dp.USART2.cr1.modify(|_, w| w.ue().set_bit()); // Enable USART
}

pub fn uart_send_byte(dp: &stm32f446::Peripherals, byte: u8) {
    while dp.USART2.sr.read().txe().bit_is_clear() {}
    dp.USART2.dr.write(|w| unsafe { w.bits(byte as u32) });
}

pub fn uart_send_string(dp: &stm32f446::Peripherals, s: &str) {
    for byte in s.bytes() {
        uart_send_byte(dp, byte);
    }
}

// Check if there's data in the buffer
pub fn uart_data_available() -> bool {
    cortex_m::interrupt::free(|cs| {
        !UART_RX_BUFFER.borrow(cs).borrow().is_empty()
    })
}

// Get one byte from buffer (non-blocking)
pub fn uart_get_byte() -> Option<u8> {
    cortex_m::interrupt::free(|cs| {
        UART_RX_BUFFER.borrow(cs).borrow_mut().pop()
    })
}

// Internal function to add byte to buffer (called from ISR)
pub fn uart_add_to_buffer(byte: u8) {
    cortex_m::interrupt::free(|cs| {
        let _ = UART_RX_BUFFER.borrow(cs).borrow_mut().push(byte);
    });
}

// Get complete string from buffer (reads until newline or buffer end)
pub fn uart_get_string() -> Option<String<64>> {
    cortex_m::interrupt::free(|cs| {
        let mut buffer = UART_RX_BUFFER.borrow(cs).borrow_mut();
        
        // Check if we have a complete line (ending with \r or \n)
        let mut line_end_pos = None;
        for (i, &byte) in buffer.iter().enumerate() {
            if byte == b'\r' || byte == b'\n' {
                line_end_pos = Some(i);
                break;
            }
        }
        
        if let Some(end_pos) = line_end_pos {
            // Extract the string up to the line ending
            let mut result: String<64> = String::<64>::new();
            
            // Pop bytes from the front of the buffer up to the line ending
            for _ in 0..=end_pos {
                if let Some(byte) = buffer.pop() {
                    if byte != b'\r' && byte != b'\n' {
                        let _ = result.push(byte as char);
                    }
                }
            }
            
            // Reverse the string since we popped from the end
            let reversed: String<64> = result.chars().rev().collect();
            Some(reversed)
        } else {
            None // No complete line yet
        }
    })
}

// Alternative: Get all available bytes as string (non-blocking)
pub fn uart_get_all_bytes() -> Option<String<64>> {
    cortex_m::interrupt::free(|cs| {
        let mut buffer = UART_RX_BUFFER.borrow(cs).borrow_mut();
        
        if buffer.is_empty() {
            return None;
        }
        
            let mut result: String<64> = String::<64>::new();
        
        // Read all bytes from buffer
        while let Some(byte) = buffer.pop() {
            if byte.is_ascii() && byte != b'\r' && byte != b'\n' {
                let _ = result.push(byte as char);
            }
        }
        
        if result.is_empty() {
            None
        } else {
            // Reverse since we popped from the end
            let reversed: String<64> = result.chars().rev().collect();
            Some(reversed)
        }
    })
}

// Check if we have a complete line (ending with \r or \n)
pub fn uart_line_available() -> bool {
    cortex_m::interrupt::free(|cs| {
        let buffer = UART_RX_BUFFER.borrow(cs).borrow();
        
        for &byte in buffer.iter() {
            if byte == b'\r' || byte == b'\n' {
                return true;
            }
        }
        false
    })
}