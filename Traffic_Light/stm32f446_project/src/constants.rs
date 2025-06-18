// GPIO Pin definitions
pub const RED_LEFT: u16 = 9;
pub const YELLOW_LEFT: u16 = 8;
pub const GREEN_LEFT: u16 = 10;
pub const GREEN_RIGHT: u16 = 6;
pub const YELLOW_RIGHT: u16 = 11;
pub const RED_RIGHT: u16 = 12;
pub const LEFT_TRAFFIC_INTENSITY: u16 = 4;
pub const RIGHT_TRAFFIC_INSTENSITY: u16 = 7;
pub const LEFT_TRAFFIC_INDICATOR: u16 = 5;
pub const RIGHT_TRAFFIC_INDICATOR: u16 = 15;

// State definitions
pub const ON: bool = true;
pub const OFF: bool = false;

// Traffic intensity levels
pub const NORMAL: u8 = 0;
pub const INTENSE: u8 = 1;
pub const HIGH_INTENSE: u8 = 2;

// Blink rates
pub const BLINK_OFF: u8 = 0;
pub const BLINK_SLOW: u8 = 1;
pub const BLINK_MEDIUM: u8 = 2;
pub const BLINK_FAST: u8 = 3;

// Timing constants
pub const TESTING_FACTOR: u16 = 5;
pub const DEBOUNCE_DELAY_MS: u32 = 500;