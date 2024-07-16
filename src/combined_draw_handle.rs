use raylib::{
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureMode, RaylibTextureModeExt},
    text::WeakFont,
    RaylibThread,
};

// Due to issues with ojbect safety, we can't use the trait directly. Instead, we use this enum
// to wrap the two possible structs.
pub enum CombinedDrawHandle<'a> {
    RaylibDrawHandle(RaylibDrawHandle<'a>),
    RaylibTextureMode(RaylibTextureMode<'a, RaylibDrawHandle<'a>>),
}

impl RaylibDraw for CombinedDrawHandle<'_> {}

impl CombinedDrawHandle<'_> {
    pub fn get_font_default(&self) -> WeakFont {
        match self {
            CombinedDrawHandle::RaylibDrawHandle(d) => d.get_font_default(),
            CombinedDrawHandle::RaylibTextureMode(d) => d.get_font_default(),
        }
    }

    pub fn begin_texture_mode<'a>(
        &'a mut self,
        t: &RaylibThread,
        framebuffer: &'a mut raylib::ffi::RenderTexture2D,
    ) -> CombinedDrawHandle<'a> {
        match self {
            CombinedDrawHandle::RaylibDrawHandle(d) => {
                CombinedDrawHandle::RaylibTextureMode(d.begin_texture_mode(t, framebuffer))
            }
            CombinedDrawHandle::RaylibTextureMode(d) => {
                panic!("Cannot start texture mode inside a texture mode")
            }
        }
    }
}
