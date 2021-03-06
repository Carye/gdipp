#include "stdafx.h"
#include "setting_cache.h"
#include "gdimm.h"
#include "lock.h"

font_setting_cache::font_gamma::font_gamma()
	: red(1.0),
	green(1.0),
	blue(1.0)
{
}

font_setting_cache::font_render_mode::font_render_mode()
	: mono(0),
	gray(1),
	subpixel(1),
	pixel_geometry(PIXEL_GEOMETRY_RGB),
	aliased_text(0)
{
};

font_setting_cache::font_shadow::font_shadow()
	: offset_x(0),
	offset_y(0),
	alpha(0)
{
}

font_setting_cache::font_setting_cache()
	: auto_hinting(1),
	embedded_bitmap(false),
	embolden(0),
	hinting(1),
	kerning(false),
	renderer(RENDERER_FREETYPE)
{
}

const font_setting_cache *gdimm_setting_cache::lookup(const gdimm_setting_trait *setting_trait)
{
#ifdef _M_X64
	uint64_t cache_trait = MurmurHash64A(setting_trait, sizeof(gdimm_setting_trait), 0);
#else
	uint64_t cache_trait = MurmurHash64B(setting_trait, sizeof(gdimm_setting_trait), 0);
#endif // _M_X64

	// if the setting for the specified font is not found
	// construct setting cache for the font and return
	map<uint64_t, font_setting_cache>::const_iterator iter = _cache.find(cache_trait);
	if (iter == _cache.end())
	{
		// double-check lock
		gdimm_lock lock(LOCK_SETTING_CACHE);
		iter = _cache.find(cache_trait);
		if (iter == _cache.end())
		{
			font_setting_cache new_cache;

			wcs_convert(gdipp_get_gdimm_setting(L"auto_hinting", setting_trait), reinterpret_cast<WORD *>(&new_cache.auto_hinting));
			wcs_convert(gdipp_get_gdimm_setting(L"embedded_bitmap", setting_trait), &new_cache.embedded_bitmap);
			wcs_convert(gdipp_get_gdimm_setting(L"embolden", setting_trait), &new_cache.embolden);

			wcs_convert(gdipp_get_gdimm_setting(L"gamma/red", setting_trait), &new_cache.gamma.red);
			wcs_convert(gdipp_get_gdimm_setting(L"gamma/green", setting_trait), &new_cache.gamma.green);
			wcs_convert(gdipp_get_gdimm_setting(L"gamma/blue", setting_trait), &new_cache.gamma.blue);

			wcs_convert(gdipp_get_gdimm_setting(L"hinting", setting_trait), reinterpret_cast<WORD *>(&new_cache.hinting));
			wcs_convert(gdipp_get_gdimm_setting(L"kerning", setting_trait), &new_cache.kerning);

			wcs_convert(gdipp_get_gdimm_setting(L"renderer", setting_trait), reinterpret_cast<WORD *>(&new_cache.renderer));
			wcs_convert(gdipp_get_gdimm_setting(L"render_mode/mono", setting_trait), reinterpret_cast<WORD *>(&new_cache.render_mode.mono));
			wcs_convert(gdipp_get_gdimm_setting(L"render_mode/gray", setting_trait), reinterpret_cast<WORD *>(&new_cache.render_mode.gray));
			wcs_convert(gdipp_get_gdimm_setting(L"render_mode/subpixel", setting_trait), reinterpret_cast<WORD *>(&new_cache.render_mode.subpixel));
			wcs_convert(gdipp_get_gdimm_setting(L"render_mode/pixel_geometry", setting_trait), reinterpret_cast<WORD *>(&new_cache.render_mode.pixel_geometry));
			wcs_convert(gdipp_get_gdimm_setting(L"render_mode/aliased_text", setting_trait), reinterpret_cast<WORD *>(&new_cache.render_mode.aliased_text));

			wcs_convert(gdipp_get_gdimm_setting(L"shadow/offset_x", setting_trait), &new_cache.shadow.offset_x);
			wcs_convert(gdipp_get_gdimm_setting(L"shadow/offset_x", setting_trait), &new_cache.shadow.offset_y);
			wcs_convert(gdipp_get_gdimm_setting(L"shadow/alpha", setting_trait), reinterpret_cast<WORD *>(&new_cache.shadow.alpha));

			_cache[cache_trait] = new_cache;
		}
	}

	return &_cache[cache_trait];
}