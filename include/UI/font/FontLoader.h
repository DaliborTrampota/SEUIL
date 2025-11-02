#pragma once

#include <map>
#include <string>


#include <UI/font/FontAtlas.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>


namespace msdfgen {
    class FontHandle;
    class FreetypeHandle;
};  // namespace msdfgen


namespace ui {

    class FontLoader {
      public:
        FontLoader();
        ~FontLoader();

        FontLoader(const FontLoader& other) = delete;
        FontLoader& operator=(const FontLoader& other) = delete;

        FontLoader(FontLoader&& other) noexcept = default;
        FontLoader& operator=(FontLoader&& other) noexcept = default;

        void loadFont(const std::string& fontPath);

        /// @brief Generate glyph geometries for the given text
        /// @returns Preprocessed glyphs vector for each character in the string in order
        std::vector<const msdf_atlas::GlyphGeometry*> generateText(
            const std::string& text, unsigned pxSize, int& startIndex, int& count
        );

        void unloadFont();

        FontAtlas packToAtlas(bool dynamic = false);

        std::vector<msdf_atlas::GlyphGeometry>& glyphs() { return m_glyphs; }

        /// @returns Returns pointer to the first new glyph
        //msdf_atlas::GlyphGeometry* glyphs() const { return m_glyphs.data();}

        /// Get font metrics for a specific font size (returns null if not loaded for that size)
        const msdf_atlas::FontGeometry* getFontGeometry(
            const std::string& fontName, unsigned pxSize
        ) const;


      private:
        struct FontID {
            std::string font;
            unsigned size;

            bool operator<(const FontID& other) const {
                int cmpres = font.compare(other.font);
                if (cmpres != 0)
                    return cmpres < 0;
                return size < other.size;
            }
        };

        msdfgen::FreetypeHandle* m_ftHandle = nullptr;
        msdfgen::FontHandle* m_loadedFont = nullptr;
        std::string m_loadedFontName;

        std::map<FontID, msdf_atlas::FontGeometry> m_fontGeometries;
        std::map<FontID, msdf_atlas::Charset> m_charsets;
        std::vector<msdf_atlas::GlyphGeometry> m_glyphs;
    };
}  // namespace ui