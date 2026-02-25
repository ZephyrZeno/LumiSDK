//! lumiapp — Rust FFI bindings for LumiOS Application SDK
//! Copyright 2026 Lumi Team. Apache-2.0

#![allow(non_camel_case_types)]

use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};
use std::ptr;

// ── FFI declarations ────────────────────────────────────────────

pub type lumi_result_t = c_int;
pub const LUMI_OK: lumi_result_t = 0;
pub const LUMI_ERR_NOMEM: lumi_result_t = -1;
pub const LUMI_ERR_INVALID: lumi_result_t = -2;
pub const LUMI_ERR_NOT_FOUND: lumi_result_t = -3;
pub const LUMI_ERR_IO: lumi_result_t = -4;

pub type lumi_log_level_t = c_int;
pub const LUMI_LOG_VERBOSE: lumi_log_level_t = 0;
pub const LUMI_LOG_DEBUG: lumi_log_level_t = 1;
pub const LUMI_LOG_INFO: lumi_log_level_t = 2;
pub const LUMI_LOG_WARN: lumi_log_level_t = 3;
pub const LUMI_LOG_ERROR: lumi_log_level_t = 4;

#[repr(C)]
pub struct lumi_manifest_t {
    pub app_id: *const c_char,
    pub name: *const c_char,
    pub version: *const c_char,
    pub icon: *const c_char,
}

pub type lumi_on_lifecycle = Option<unsafe extern "C" fn(*mut c_void, *mut c_void)>;

#[repr(C)]
pub struct lumi_lifecycle_t {
    pub on_create: lumi_on_lifecycle,
    pub on_start: lumi_on_lifecycle,
    pub on_resume: lumi_on_lifecycle,
    pub on_pause: lumi_on_lifecycle,
    pub on_stop: lumi_on_lifecycle,
    pub on_destroy: lumi_on_lifecycle,
}

pub enum lumi_app_t {}
pub enum lumi_view_t {}

extern "C" {
    pub fn lumi_result_str(code: lumi_result_t) -> *const c_char;
    pub fn lumi_log(level: lumi_log_level_t, tag: *const c_char, fmt: *const c_char, ...);
    pub fn lumi_log_set_level(min_level: lumi_log_level_t);

    pub fn lumi_app_create(manifest: *const lumi_manifest_t, lifecycle: *const lumi_lifecycle_t, userdata: *mut c_void) -> *mut lumi_app_t;
    pub fn lumi_app_destroy(app: *mut lumi_app_t);
    pub fn lumi_app_run(app: *mut lumi_app_t) -> c_int;
    pub fn lumi_app_quit(app: *mut lumi_app_t);

    pub fn lumi_column() -> *mut lumi_view_t;
    pub fn lumi_row() -> *mut lumi_view_t;
    pub fn lumi_text(content: *const c_char) -> *mut lumi_view_t;
    pub fn lumi_button(label: *const c_char) -> *mut lumi_view_t;
    pub fn lumi_image(source: *const c_char) -> *mut lumi_view_t;
    pub fn lumi_spacer() -> *mut lumi_view_t;
    pub fn lumi_view_add_child(parent: *mut lumi_view_t, child: *mut lumi_view_t);
    pub fn lumi_view_destroy(view: *mut lumi_view_t);
    pub fn lumi_view_set_id(view: *mut lumi_view_t, id: *const c_char);
    pub fn lumi_text_set_content(view: *mut lumi_view_t, text: *const c_char);
    pub fn lumi_text_get_content(view: *mut lumi_view_t) -> *const c_char;
    pub fn lumi_app_set_content(app: *mut lumi_app_t, root: *mut lumi_view_t);

    pub fn lumi_storage_set(key: *const c_char, value: *const c_char) -> lumi_result_t;
    pub fn lumi_storage_get(key: *const c_char) -> *const c_char;
    pub fn lumi_storage_remove(key: *const c_char) -> lumi_result_t;
    pub fn lumi_storage_clear() -> lumi_result_t;

    pub fn lumi_notify_simple(title: *const c_char, body: *const c_char) -> lumi_result_t;
}

// ── Safe Rust wrappers ──────────────────────────────────────────

#[derive(Debug)]
pub enum LumiError {
    OutOfMemory,
    InvalidArgument,
    NotFound,
    IoError,
    Unknown(i32),
}

impl From<lumi_result_t> for Result<(), LumiError> {
    fn from(code: lumi_result_t) -> Self {
        match code {
            0 => Ok(()),
            -1 => Err(LumiError::OutOfMemory),
            -2 => Err(LumiError::InvalidArgument),
            -3 => Err(LumiError::NotFound),
            -4 => Err(LumiError::IoError),
            c => Err(LumiError::Unknown(c)),
        }
    }
}

fn check(r: lumi_result_t) -> Result<(), LumiError> {
    r.into()
}

/// Safe wrapper for key-value storage.
pub mod storage {
    use super::*;

    pub fn set(key: &str, value: &str) -> Result<(), LumiError> {
        let k = CString::new(key).map_err(|_| LumiError::InvalidArgument)?;
        let v = CString::new(value).map_err(|_| LumiError::InvalidArgument)?;
        check(unsafe { lumi_storage_set(k.as_ptr(), v.as_ptr()) })
    }

    pub fn get(key: &str) -> Option<String> {
        let k = CString::new(key).ok()?;
        let ptr = unsafe { lumi_storage_get(k.as_ptr()) };
        if ptr.is_null() { return None; }
        Some(unsafe { CStr::from_ptr(ptr) }.to_string_lossy().into_owned())
    }

    pub fn remove(key: &str) -> Result<(), LumiError> {
        let k = CString::new(key).map_err(|_| LumiError::InvalidArgument)?;
        check(unsafe { lumi_storage_remove(k.as_ptr()) })
    }

    pub fn clear() -> Result<(), LumiError> {
        check(unsafe { lumi_storage_clear() })
    }
}

/// Safe wrapper for logging.
pub mod log {
    use super::*;

    pub fn info(tag: &str, msg: &str) {
        let t = CString::new(tag).unwrap_or_default();
        let m = CString::new(msg).unwrap_or_default();
        let fmt = CString::new("%s").unwrap();
        unsafe { lumi_log(LUMI_LOG_INFO, t.as_ptr(), fmt.as_ptr(), m.as_ptr()); }
    }

    pub fn error(tag: &str, msg: &str) {
        let t = CString::new(tag).unwrap_or_default();
        let m = CString::new(msg).unwrap_or_default();
        let fmt = CString::new("%s").unwrap();
        unsafe { lumi_log(LUMI_LOG_ERROR, t.as_ptr(), fmt.as_ptr(), m.as_ptr()); }
    }
}

/// Safe wrapper for notifications.
pub fn notify(title: &str, body: &str) -> Result<(), LumiError> {
    let t = CString::new(title).map_err(|_| LumiError::InvalidArgument)?;
    let b = CString::new(body).map_err(|_| LumiError::InvalidArgument)?;
    check(unsafe { lumi_notify_simple(t.as_ptr(), b.as_ptr()) })
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_conversion() {
        let ok: Result<(), LumiError> = LUMI_OK.into();
        assert!(ok.is_ok());
        let err: Result<(), LumiError> = LUMI_ERR_NOMEM.into();
        assert!(matches!(err, Err(LumiError::OutOfMemory)));
    }
}
