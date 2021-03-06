/*
 * Copyright 2010 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrGlyph_DEFINED
#define GrGlyph_DEFINED

#include "GrDrawOpAtlas.h"
#include "GrRect.h"
#include "GrTypes.h"

#include "SkChecksum.h"
#include "SkFixed.h"
#include "SkPath.h"

struct GrGlyph {
    enum MaskStyle {
        kCoverage_MaskStyle,
        kDistance_MaskStyle
    };

    static GrMaskFormat FormatFromSkGlyph(const SkGlyph& glyph) {
        SkMask::Format format = static_cast<SkMask::Format>(glyph.fMaskFormat);
        switch (format) {
            case SkMask::kBW_Format:
            case SkMask::kSDF_Format:
                // fall through to kA8 -- we store BW and SDF glyphs in our 8-bit cache
            case SkMask::kA8_Format:
                return kA8_GrMaskFormat;
            case SkMask::k3D_Format:
                return kA8_GrMaskFormat; // ignore the mul and add planes, just use the mask
            case SkMask::kLCD16_Format:
                return kA565_GrMaskFormat;
            case SkMask::kARGB32_Format:
                return kARGB_GrMaskFormat;
            default:
                SkDEBUGFAIL("unsupported SkMask::Format");
                return kA8_GrMaskFormat;
        }
    }

    static GrIRect16 BoundsFromSkGlyph(const SkGlyph& glyph) {
        return GrIRect16::MakeXYWH(glyph.fLeft,
                                   glyph.fTop,
                                   glyph.fWidth,
                                   glyph.fHeight);
    }

    static MaskStyle MaskStyleFromSkGlyph(const SkGlyph& skGlyph) {
        return (SkMask::Format)skGlyph.fMaskFormat == SkMask::kSDF_Format
           ? GrGlyph::MaskStyle::kDistance_MaskStyle
           : GrGlyph::MaskStyle::kCoverage_MaskStyle;
    }

    GrGlyph(const SkGlyph& skGlyph)
        : fPackedID{skGlyph.getPackedID()}
        , fMaskFormat{FormatFromSkGlyph(skGlyph)}
        , fMaskStyle{MaskStyleFromSkGlyph(skGlyph)}
        , fBounds{BoundsFromSkGlyph(skGlyph)} {}

    const SkPackedGlyphID  fPackedID;
    const GrMaskFormat     fMaskFormat;
    const MaskStyle        fMaskStyle;
    const GrIRect16        fBounds;
    SkIPoint16             fAtlasLocation{0, 0};
    GrDrawOpAtlas::AtlasID fID{GrDrawOpAtlas::kInvalidAtlasID};

    int width() const { return fBounds.width(); }
    int height() const { return fBounds.height(); }
    uint32_t pageIndex() const { return GrDrawOpAtlas::GetPageIndexFromID(fID); }
    MaskStyle maskStyle() const { return fMaskStyle; }

    // GetKey and Hash for the the hash table.
    static const SkPackedGlyphID& GetKey(const GrGlyph& glyph) {
        return glyph.fPackedID;
    }

    static uint32_t Hash(SkPackedGlyphID key) {
        return SkChecksum::Mix(key.hash());
    }
};

#endif
