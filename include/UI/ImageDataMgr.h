#pragma once

#include "elements/Image.h"

#include <filesystem>
#include <unordered_map>


namespace fs = std::filesystem;

namespace ui {

    struct ImageDataMgr {
        void add(fs::path key, ImageData&& data) { m_data.emplace(key, std::move(data)); }
        const ImageData* get(fs::path key) const {
            auto it = m_data.find(key);
            if (it != m_data.end())
                return &it->second;
            return nullptr;
        }

        std::unordered_map<fs::path, ImageData> m_data;
    };
}  // namespace ui