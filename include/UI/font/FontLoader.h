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

        FontLoader(FontLoader&& other) noexcept;
        FontLoader& operator=(FontLoader&& other) noexcept;

        static constexpr double MAX_CORNER_ANGLE = 3.0;
        static constexpr double MINIMUM_SCALE = 24.0;
        static constexpr double PIXEL_RANGE = 2.0;
        static constexpr double MITER_LIMIT = 1.0;

        /// @brief Load a font from a file
        void loadFont(const std::string& fontPath);

        /// @brief Generate glyph geometries for the given text with loaded font
        /// @returns Preprocessed glyphs vector for each character in the string in order
        std::vector<const msdf_atlas::GlyphGeometry*> generateText(
            const std::string& text, unsigned pxSize, int& startIndex, int& count
        );

        /// @brief Unload and destroy the loaded font
        void unloadFont();

        /// @brief Pack glyphs into an texture atlas
        /// @param dynamic If true, the atlas will be dynamic, meaning more characters can be added to it over time and will be resized as needed
        /// @returns FontAtlas object containing the packed glyphs and pixel data
        FontAtlas packToAtlas(bool dynamic = true);

        /// @returns Reference to the vector of glyph geometries
        std::vector<msdf_atlas::GlyphGeometry>& glyphs() { return m_glyphs; }

        /// @brief Get font metrics for a specific font size (returns null if not loaded for that size)
        /// @param fontName Name of the font
        /// @param pxSize Font size in pixels
        /// @returns Pointer to the font geometry
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