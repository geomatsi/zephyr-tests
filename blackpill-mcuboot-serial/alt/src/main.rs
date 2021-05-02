#![no_main]
#![no_std]

use panic_rtt_target as _;
use rtt_target::{rprintln, rtt_init_print};

use stm32f4xx_hal as hal;

use hal::{
    gpio::{gpioc, Output, PushPull},
    prelude::*,
    stm32::{interrupt, Interrupt, Peripherals, TIM2},
    timer::{Event, Timer},
};

use core::cell::RefCell;
use core::ops::DerefMut;
use cortex_m::{asm::nop, interrupt::Mutex};
use cortex_m_rt::entry;
use embedded_hal::digital::v2::OutputPin;
use embedded_hal::timer::CountDown;

type LEDPIN = gpioc::PC13<Output<PushPull>>;

static G_LED: Mutex<RefCell<Option<LEDPIN>>> = Mutex::new(RefCell::new(None));
static G_TIM: Mutex<RefCell<Option<Timer<TIM2>>>> = Mutex::new(RefCell::new(None));

#[entry]
fn main() -> ! {
    let mut cp = cortex_m::Peripherals::take().unwrap();
    let nvic = &mut cp.NVIC;

    let dp = Peripherals::take().unwrap();
    let rcc = dp.RCC.constrain();

    rtt_init_print!();

    let clocks = rcc
        .cfgr
        .use_hse(25.mhz())
        .sysclk(84.mhz())
        .hclk(84.mhz())
        .pclk1(42.mhz())
        .pclk2(84.mhz())
        .freeze();

    let gpioc = dp.GPIOC.split();
    let mut led = gpioc.pc13.into_push_pull_output();
    led.set_high().unwrap();

    let mut timer = Timer::tim2(dp.TIM2, 1.hz(), clocks);
    timer.listen(Event::TimeOut);

    cortex_m::interrupt::free(|cs| {
        G_TIM.borrow(cs).replace(Some(timer));
        G_LED.borrow(cs).replace(Some(led));
    });

    unsafe {
        nvic.set_priority(Interrupt::TIM2, 2);
        cortex_m::peripheral::NVIC::unmask(Interrupt::TIM2);
    }

    cortex_m::peripheral::NVIC::unpend(Interrupt::TIM2);

    let mut cycle: u32 = 0;

    loop {
        rprintln!("idle loop: {}", cycle);
        // NOP since RTT does not work well with WFI
        delay(5000000);
        cycle += 1;
    }
}

fn delay(count: u32) {
    for _ in 0..count {
        nop();
    }
}

#[interrupt]
fn TIM2() {
    cortex_m::interrupt::free(|cs| {
        if let (Some(ref mut tim), Some(ref mut led)) = (
            G_TIM.borrow(cs).borrow_mut().deref_mut(),
            G_LED.borrow(cs).borrow_mut().deref_mut(),
        ) {
            rprintln!("tim2 irq");
            led.toggle().unwrap();
            tim.wait().unwrap();
        }
    });
}
