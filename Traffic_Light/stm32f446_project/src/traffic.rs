use core::sync::atomic::{AtomicBool, AtomicU8};

pub static LEFT_TRAFFIC_INTENSITY_LEVEL: AtomicU8 = AtomicU8::new(0);
pub static RIGHT_TRAFFIC_INTENSITY_LEVEL: AtomicU8 = AtomicU8::new(0);

pub static LEFT_INDICATOR_RATE: AtomicU8 = AtomicU8::new(0);
pub static RIGHT_INDICATOR_RATE: AtomicU8 = AtomicU8::new(0);

pub static LEFT_BLINK_STATE: AtomicBool = AtomicBool::new(false);
pub static RIGHT_BLINK_STATE: AtomicBool = AtomicBool::new(false);
pub static LEFT_BLINK_COUNTER: AtomicU8 = AtomicU8::new(0);
pub static RIGHT_BLINK_COUNTER: AtomicU8 = AtomicU8::new(0);



use crate::constants::*;

pub fn get_traffic_delays(left_intensity: u8, right_intensity: u8) -> [u16; 4] {
    let delay_normal: [u16; 4] = [15, 5, 15, 5];
    let delay_left_intense: [u16; 4] = [10, 5, 30, 5];
    let delay_left_high_intense: [u16; 4] = [10, 5, 50, 5];
    let delay_right_intense: [u16; 4] = [30, 5, 10, 5];
    let delay_right_high_intense: [u16; 4] = [50, 5, 10, 5];

    if left_intensity == right_intensity {
        delay_normal
    } else if left_intensity == NORMAL {
        if right_intensity == INTENSE {
            delay_right_intense
        } else if right_intensity == HIGH_INTENSE {
            delay_right_high_intense
        } else {
            delay_normal
        }
    } else if left_intensity == INTENSE {
        if right_intensity == NORMAL {
            delay_left_intense
        } else if right_intensity == HIGH_INTENSE {
            delay_right_intense
        } else {
            delay_normal
        }
    } else if left_intensity == HIGH_INTENSE {
        if right_intensity == NORMAL {
            delay_left_high_intense
        } else if right_intensity == INTENSE {
            delay_left_intense
        } else {
            delay_normal
        }
    } else {
        delay_normal
    }
}