use std::time::Duration;

use windows::Win32::Foundation::{LPARAM, WPARAM};
use windows::Win32::Media::Audio::Endpoints::IAudioEndpointVolume;
use windows::Win32::Media::Audio::{eConsole, eRender, IMMDeviceEnumerator, MMDeviceEnumerator};
use windows::Win32::System::Com::{CoCreateInstance, CoInitializeEx, CLSCTX_ALL, COINIT_MULTITHREADED};
use windows::Win32::UI::Input::KeyboardAndMouse::{GetKeyboardLayout, GetKeyboardLayoutList, HKL};
use windows::Win32::UI::WindowsAndMessaging::{GetForegroundWindow, GetWindowThreadProcessId, PostMessageW, WM_INPUTLANGCHANGEREQUEST};

use crate::{State, LAYOUT_OTHER, NO_VOLUME};

pub struct Host {
    languages: Vec<u16>,
    enumerator: IMMDeviceEnumerator,
}

fn language(layout: HKL) -> u16 {
    (layout.0 as usize & 0xFFFF) as u16
}

impl Host {
    pub fn new(layouts: &[String]) -> Result<Self, String> {
        let languages = layouts
            .iter()
            .map(|l| u16::from_str_radix(l, 16).map_err(|_| format!("layout '{l}': expected a hex language id like 0409")))
            .collect::<Result<_, _>>()?;
        let enumerator = unsafe {
            CoInitializeEx(None, COINIT_MULTITHREADED).ok().map_err(|e| format!("CoInitializeEx: {e}"))?;
            CoCreateInstance(&MMDeviceEnumerator, None, CLSCTX_ALL).map_err(|e| format!("MMDeviceEnumerator: {e}"))?
        };
        Ok(Host { languages, enumerator })
    }

    pub fn pump(&mut self, timeout: Duration) {
        std::thread::sleep(timeout);
    }

    pub fn state(&self) -> State {
        let (volume, muted) = self.volume().unwrap_or((NO_VOLUME, false));
        State { layout: self.layout(), volume, muted }
    }

    pub fn select(&mut self, index: u8) -> Result<(), String> {
        let wanted = *self.languages.get(index as usize).ok_or("index outside --layouts")?;
        let mut layouts = [HKL::default(); 32];
        let count = unsafe { GetKeyboardLayoutList(Some(&mut layouts)) } as usize;
        let layout = layouts[..count]
            .iter()
            .find(|l| language(**l) == wanted)
            .ok_or(format!("layout {wanted:04x} is not installed"))?;
        unsafe { PostMessageW(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, WPARAM(0), LPARAM(layout.0 as isize)) }
            .map_err(|e| format!("PostMessageW: {e}"))
    }

    fn layout(&self) -> u8 {
        let current = unsafe { language(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), None))) };
        self.languages
            .iter()
            .position(|l| *l == current)
            .map_or(LAYOUT_OTHER, |i| i as u8)
    }

    fn volume(&self) -> windows::core::Result<(u8, bool)> {
        unsafe {
            let device = self.enumerator.GetDefaultAudioEndpoint(eRender, eConsole)?;
            let endpoint: IAudioEndpointVolume = device.Activate(CLSCTX_ALL, None)?;
            let level = endpoint.GetMasterVolumeLevelScalar()?;
            let muted = endpoint.GetMute()?.as_bool();
            Ok(((level.clamp(0.0, 1.0) * 100.0).round() as u8, muted))
        }
    }
}
