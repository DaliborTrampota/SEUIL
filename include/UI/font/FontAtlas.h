#pragma once

#include <variant>
#include <vector>


#include <msdf-atlas-gen/msdf-atlas-gen.h>


namespace ui {

    //template <typename PixelT = byte, int N_CHANNEL = 3>
    class FontAtlas {
      public:
        FontAtlas(const FontAtlas&) = delete;
        FontAtlas& operator=(const FontAtlas&) = delete;
        FontAtlas(FontAtlas&&) noexcept;
        FontAtlas& operator=(FontAtlas&&) noexcept;

        using StorageR = msdfgen::BitmapConstRef<msdf_atlas::byte, 3>;
        using StorageW = msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>;
        using AtlasGenerator =
            msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, StorageW>;
        using DynAtlasGenerator = msdf_atlas::DynamicAtlas<
            ThreadPoolAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, StorageW>>;

        /// @brief Create a static atlas; new glyphs cannot be added after creation
        /// @param w Width of the atlas in pixels
        /// @param h Height of the atlas in pixels
        /// @param glyphs Vector of glyph geometries to add to the atlas
        /// @returns FontAtlas object containing the packed glyphs and pixel data
        /// @note Glyphs must have edge coloring and wrap box already applied
        static FontAtlas createStatic(
            int w, int h, const std::vector<msdf_atlas::GlyphGeometry>& glyphs
        );

        /// @brief Create a dynamic atlas; more characters can be added to it even after it has been created
        /// @note Glyphs must have edge coloring and wrap box already applied
        static FontAtlas createDynamic(int threadCount = 2);

        /// @brief Add preprocessed glyphs to a dynamic atlas
        /// @note Glyphs must have edge coloring and wrap box already applied
        /// @note Throws if called on a static atlas
        void addGlyphs(msdf_atlas::GlyphGeometry* glyphs, int count);

        /// @brief Get the pixel data of the atlas
        const unsigned char* data() const { return m_bitmap.pixels; }
        int width() const { return m_width; }
        int height() const { return m_height; }

      private:
        FontAtlas(AtlasGenerator gen);
        FontAtlas(DynAtlasGenerator gen);

        int m_width = -1;
        int m_height = -1;
        StorageR m_bitmap;
        std::variant<AtlasGenerator, DynAtlasGenerator> m_generator;
    };
}  // namespace ui