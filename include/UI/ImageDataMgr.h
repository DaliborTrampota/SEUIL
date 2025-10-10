#pragma once

#include <UI/elements/Image.h>

#include <unordered_map>
#include <vector>


namespace ui {

    struct ImageDataMgr {
        void add(std::string key, ImageData&& data) { m_data.emplace(key, std::move(data)); }
        const ImageData* get(std::string key) const {
            auto it = m_data.find(key);
            if (it != m_data.end())
                return &it->second;
            return nullptr;
        }

        std::unordered_map<std::string, ImageData> m_data;
    };
}  // namespace ui