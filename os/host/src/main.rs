use std::ffi::CString;
use std::sync::mpsc::{self, Receiver, Sender};
use std::thread;
use std::time::{Duration, Instant};

use hidapi::{HidApi, HidDevice};

#[cfg_attr(target_os = "macos", path = "macos.rs")]
#[cfg_attr(windows, path = "windows.rs")]
mod platform;

const USAGE_PAGE: u16 = 0xFF60;
const USAGE: u16 = 0x61;
const REPORT_SIZE: usize = 32;
const VERSION: u8 = 1;
const REPORT_LAYOUT: u8 = 0x01;
const REPORT_VOLUME: u8 = 0x02;
const REPORT_HELLO: u8 = 0x80;
const REPORT_SELECT_LAYOUT: u8 = 0x81;
pub const LAYOUT_OTHER: u8 = 0xFE;
pub const NO_VOLUME: u8 = 0xFF;
const REPEAT: Duration = Duration::from_secs(5);
const POLL: Duration = Duration::from_millis(100);
const RECONNECT: Duration = Duration::from_secs(1);

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub struct State {
    pub layout: u8,
    pub volume: u8,
    pub muted: bool,
}

#[derive(Debug)]
enum Event {
    Hello(u64),
    SelectLayout(u64, u8),
    Gone(u64, String),
}

struct Link {
    writer: HidDevice,
    generation: u64,
}

struct Args {
    layouts: Vec<String>,
    verbose: bool,
}

fn main() {
    if let Err(error) = run() {
        eprintln!("zkf-host: {error}");
        std::process::exit(1);
    }
}

fn run() -> Result<(), String> {
    let args = parse_args()?;
    let mut host = platform::Host::new(&args.layouts)?;
    let mut api = HidApi::new().map_err(|e| format!("hidapi: {e}"))?;
    #[cfg(target_os = "macos")]
    api.set_open_exclusive(false);
    let (tx, rx) = mpsc::channel();
    let mut link: Option<Link> = None;
    let mut generation = 0;
    let mut sent: Option<(State, Instant)> = None;
    let mut tried_at: Option<Instant> = None;
    let mut logged: Option<State> = None;
    log(&format!("layouts: {}", args.layouts.join(", ")));

    loop {
        host.pump(POLL);
        if link.is_none() && tried_at.map_or(true, |t| t.elapsed() >= RECONNECT) {
            tried_at = Some(Instant::now());
            generation += 1;
            link = connect(&mut api, generation, &tx);
            sent = None;
        }
        handle_events(&rx, &mut link, &mut sent, &mut host, args.verbose);
        let state = host.state();
        if args.verbose && logged != Some(state) {
            logged = Some(state);
            log(&format!("state: {state:?}"));
        }
        let due = sent.map_or(true, |(last, at)| last != state || at.elapsed() >= REPEAT);
        if let (Some(current), true) = (&link, due) {
            match send_state(&current.writer, state) {
                Ok(()) => sent = Some((state, Instant::now())),
                Err(e) => {
                    log(&format!("disconnected: {e}"));
                    link = None;
                }
            }
        }
    }
}

fn parse_args() -> Result<Args, String> {
    let usage = "usage: zkf-host --layouts <EN source>,<RU source>[,...] [--verbose]";
    let mut layouts = None;
    let mut verbose = false;
    let mut args = std::env::args().skip(1);
    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--layouts" => layouts = args.next(),
            "--verbose" => verbose = true,
            _ => return Err(format!("unknown argument '{arg}'\n{usage}")),
        }
    }
    let layouts: Vec<String> = layouts
        .ok_or(usage)?
        .split(',')
        .map(|s| s.trim().to_string())
        .filter(|s| !s.is_empty())
        .collect();
    if layouts.is_empty() || layouts.len() >= LAYOUT_OTHER as usize {
        return Err(usage.to_string());
    }
    Ok(Args { layouts, verbose })
}

fn handle_events(rx: &Receiver<Event>, link: &mut Option<Link>, sent: &mut Option<(State, Instant)>, host: &mut platform::Host, verbose: bool) {
    while let Ok(event) = rx.try_recv() {
        if verbose {
            log(&format!("event: {event:?}"));
        }
        let current = link.as_ref().map(|l| l.generation);
        match event {
            Event::Gone(g, e) if current == Some(g) => {
                log(&format!("disconnected: {e}"));
                *link = None;
            }
            Event::Hello(g) if current == Some(g) => *sent = None,
            Event::SelectLayout(g, index) if current == Some(g) => {
                if let Err(e) = host.select(index) {
                    log(&format!("select layout {index}: {e}"));
                }
            }
            _ => {}
        }
    }
}

fn connect(api: &mut HidApi, generation: u64, tx: &Sender<Event>) -> Option<Link> {
    if let Err(e) = api.refresh_devices() {
        log(&format!("enumerate: {e}"));
        return None;
    }
    let info = api
        .device_list()
        .find(|d| d.usage_page() == USAGE_PAGE && d.usage() == USAGE)?;
    let name = format!(
        "{} ({:04x}:{:04x})",
        info.product_string().unwrap_or("?"),
        info.vendor_id(),
        info.product_id()
    );
    let path: CString = info.path().into();
    let open = |api: &HidApi| api.open_path(&path).map_err(|e| log(&format!("open {name}: {e}"))).ok();
    let reader = open(api)?;
    let writer = open(api)?;
    let tx = tx.clone();
    thread::spawn(move || read_loop(reader, generation, tx));
    log(&format!("connected: {name}"));
    Some(Link { writer, generation })
}

fn read_loop(reader: HidDevice, generation: u64, tx: Sender<Event>) {
    let mut buf = [0u8; REPORT_SIZE];
    loop {
        let event = match reader.read(&mut buf) {
            Err(e) => {
                let _ = tx.send(Event::Gone(generation, e.to_string()));
                return;
            }
            Ok(n) if n < 3 || buf[1] != VERSION => {
                log(&format!("ignored report: {:02x?}", &buf[..n.min(4)]));
                continue;
            }
            Ok(_) => match buf[0] {
                REPORT_HELLO => Event::Hello(generation),
                REPORT_SELECT_LAYOUT => Event::SelectLayout(generation, buf[2]),
                _ => continue,
            },
        };
        if tx.send(event).is_err() {
            return;
        }
    }
}

fn send_state(writer: &HidDevice, state: State) -> Result<(), String> {
    send(writer, &[REPORT_LAYOUT, VERSION, state.layout])?;
    send(writer, &[REPORT_VOLUME, VERSION, state.volume, state.muted as u8])
}

fn send(writer: &HidDevice, payload: &[u8]) -> Result<(), String> {
    let mut report = [0u8; REPORT_SIZE + 1];
    report[1..=payload.len()].copy_from_slice(payload);
    writer.write(&report).map(|_| ()).map_err(|e| e.to_string())
}

pub fn log(message: &str) {
    eprintln!("zkf-host: {message}");
}
