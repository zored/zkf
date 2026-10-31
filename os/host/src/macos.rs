use std::ffi::{c_char, c_void, CStr};
use std::ptr::null;
use std::time::Duration;

use crate::{State, LAYOUT_OTHER, NO_VOLUME};

type CFTypeRef = *const c_void;
type CFStringRef = *const c_void;
type CFIndex = isize;

const UTF8: u32 = 0x0800_0100;
const RUN_LOOP_FINISHED: i32 = 1;
const DELIVER_IMMEDIATELY: isize = 4;
const SYSTEM_OBJECT: u32 = 1;

#[repr(C)]
struct PropertyAddress {
    selector: u32,
    scope: u32,
    element: u32,
}

const fn fourcc(code: &[u8; 4]) -> u32 {
    u32::from_be_bytes(*code)
}

const DEFAULT_OUTPUT: PropertyAddress = PropertyAddress { selector: fourcc(b"dOut"), scope: fourcc(b"glob"), element: 0 };
const MAIN_VOLUME: PropertyAddress = PropertyAddress { selector: fourcc(b"vmvc"), scope: fourcc(b"outp"), element: 0 };
const MUTE: PropertyAddress = PropertyAddress { selector: fourcc(b"mute"), scope: fourcc(b"outp"), element: 0 };

#[link(name = "CoreFoundation", kind = "framework")]
extern "C" {
    static kCFRunLoopDefaultMode: CFStringRef;
    static kCFTypeDictionaryKeyCallBacks: c_void;
    static kCFTypeDictionaryValueCallBacks: c_void;
    fn CFRelease(cf: CFTypeRef);
    fn CFStringCreateWithBytes(alloc: CFTypeRef, bytes: *const u8, len: CFIndex, encoding: u32, external: u8) -> CFStringRef;
    fn CFStringGetCString(s: CFStringRef, buf: *mut c_char, size: CFIndex, encoding: u32) -> u8;
    fn CFDictionaryCreate(alloc: CFTypeRef, keys: *const CFTypeRef, values: *const CFTypeRef, count: CFIndex, key_callbacks: *const c_void, value_callbacks: *const c_void) -> CFTypeRef;
    fn CFArrayGetCount(array: CFTypeRef) -> CFIndex;
    fn CFArrayGetValueAtIndex(array: CFTypeRef, index: CFIndex) -> CFTypeRef;
    fn CFRunLoopRunInMode(mode: CFStringRef, seconds: f64, return_after_source: u8) -> i32;
    fn CFNotificationCenterGetDistributedCenter() -> CFTypeRef;
    fn CFNotificationCenterAddObserver(center: CFTypeRef, observer: *const c_void, callback: extern "C" fn(CFTypeRef, *mut c_void, CFStringRef, CFTypeRef, CFTypeRef), name: CFStringRef, object: CFTypeRef, behavior: isize);
}

#[link(name = "Carbon", kind = "framework")]
extern "C" {
    static kTISPropertyInputSourceID: CFStringRef;
    static kTISNotifySelectedKeyboardInputSourceChanged: CFStringRef;
    fn TISCopyCurrentKeyboardInputSource() -> CFTypeRef;
    fn TISGetInputSourceProperty(source: CFTypeRef, key: CFStringRef) -> CFTypeRef;
    fn TISCreateInputSourceList(properties: CFTypeRef, include_all: u8) -> CFTypeRef;
    fn TISSelectInputSource(source: CFTypeRef) -> i32;
}

#[link(name = "CoreAudio", kind = "framework")]
extern "C" {
    fn AudioObjectHasProperty(id: u32, address: *const PropertyAddress) -> u8;
    fn AudioObjectGetPropertyData(id: u32, address: *const PropertyAddress, qualifier_size: u32, qualifier: *const c_void, size: *mut u32, data: *mut c_void) -> i32;
}

#[link(name = "AudioToolbox", kind = "framework")]
extern "C" {
    fn AudioHardwareServiceHasProperty(id: u32, address: *const PropertyAddress) -> u8;
    fn AudioHardwareServiceGetPropertyData(id: u32, address: *const PropertyAddress, qualifier_size: u32, qualifier: *const c_void, size: *mut u32, data: *mut c_void) -> i32;
}

extern "C" fn on_input_source_changed(_: CFTypeRef, _: *mut c_void, _: CFStringRef, _: CFTypeRef, _: CFTypeRef) {}

pub struct Host {
    layouts: Vec<String>,
}

impl Host {
    pub fn new(layouts: &[String]) -> Result<Self, String> {
        // The observer wakes the run loop on a switch; the source is read in state().
        unsafe {
            CFNotificationCenterAddObserver(
                CFNotificationCenterGetDistributedCenter(),
                on_input_source_changed as *const c_void,
                on_input_source_changed,
                kTISNotifySelectedKeyboardInputSourceChanged,
                null(),
                DELIVER_IMMEDIATELY,
            );
        }
        let host = Host { layouts: layouts.to_vec() };
        for (index, id) in layouts.iter().enumerate() {
            host.find_source(id).map_err(|e| format!("layout {index}: {e}"))?;
        }
        Ok(host)
    }

    pub fn pump(&mut self, timeout: Duration) {
        if unsafe { CFRunLoopRunInMode(kCFRunLoopDefaultMode, timeout.as_secs_f64(), 1) } == RUN_LOOP_FINISHED {
            std::thread::sleep(timeout);
        }
    }

    pub fn state(&self) -> State {
        let (volume, muted) = volume();
        State { layout: self.layout(), volume, muted }
    }

    pub fn select(&mut self, index: u8) -> Result<(), String> {
        let id = self.layouts.get(index as usize).ok_or("index outside --layouts")?;
        let list = self.find_source(id)?;
        let status = unsafe { TISSelectInputSource(CFArrayGetValueAtIndex(list, 0)) };
        unsafe { CFRelease(list) };
        match status {
            0 => Ok(()),
            _ => Err(format!("TISSelectInputSource({id}) = {status}")),
        }
    }

    fn layout(&self) -> u8 {
        let id = unsafe {
            let source = TISCopyCurrentKeyboardInputSource();
            let id = string(TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
            CFRelease(source);
            id
        };
        self.layouts
            .iter()
            .position(|l| Some(l) == id.as_ref())
            .map_or(LAYOUT_OTHER, |i| i as u8)
    }

    fn find_source(&self, id: &str) -> Result<CFTypeRef, String> {
        unsafe {
            let value = CFStringCreateWithBytes(null(), id.as_ptr(), id.len() as CFIndex, UTF8, 0);
            let key = kTISPropertyInputSourceID;
            let filter = CFDictionaryCreate(null(), &key, &value, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            let list = TISCreateInputSourceList(filter, 0);
            CFRelease(filter);
            CFRelease(value);
            if list.is_null() || CFArrayGetCount(list) == 0 {
                if !list.is_null() {
                    CFRelease(list);
                }
                return Err(format!("input source '{id}' is not enabled"));
            }
            Ok(list)
        }
    }
}

unsafe fn string(s: CFStringRef) -> Option<String> {
    if s.is_null() {
        return None;
    }
    let mut buf = [0 as c_char; 256];
    if CFStringGetCString(s, buf.as_mut_ptr(), buf.len() as CFIndex, UTF8) == 0 {
        return None;
    }
    Some(CStr::from_ptr(buf.as_ptr()).to_string_lossy().into_owned())
}

fn volume() -> (u8, bool) {
    let mut device = 0u32;
    if get(AudioObjectGetPropertyData, SYSTEM_OBJECT, &DEFAULT_OUTPUT, &mut device).is_none() || device == 0 {
        return (NO_VOLUME, false);
    }
    let mut muted = 0u32;
    let muted = unsafe { AudioObjectHasProperty(device, &MUTE) } != 0
        && get(AudioObjectGetPropertyData, device, &MUTE, &mut muted).is_some()
        && muted != 0;
    let mut level = 0f32;
    if unsafe { AudioHardwareServiceHasProperty(device, &MAIN_VOLUME) } == 0
        || get(AudioHardwareServiceGetPropertyData, device, &MAIN_VOLUME, &mut level).is_none()
    {
        return (NO_VOLUME, muted);
    }
    ((level.clamp(0.0, 1.0) * 100.0).round() as u8, muted)
}

type GetProperty = unsafe extern "C" fn(u32, *const PropertyAddress, u32, *const c_void, *mut u32, *mut c_void) -> i32;

fn get<T>(getter: GetProperty, id: u32, address: &PropertyAddress, out: &mut T) -> Option<()> {
    let mut size = std::mem::size_of::<T>() as u32;
    let status = unsafe { getter(id, address, 0, null(), &mut size, out as *mut T as *mut c_void) };
    (status == 0).then_some(())
}
