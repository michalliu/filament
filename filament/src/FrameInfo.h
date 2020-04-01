/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TNT_FILAMENT_FRAMEINFO_H
#define TNT_FILAMENT_FRAMEINFO_H

#include "details/Engine.h"

#include "backend/Handle.h"

#include <array>
#include <chrono>

#include <assert.h>
#include <stdint.h>

namespace filament {
namespace details {
class FEngine;
} // namespace details

struct FrameInfo {
    using duration = std::chrono::duration<float, std::milli>;
    duration frameTime{};            // frame time in ms
    duration denoisedFrameTime{};    // frame time in ms (median filter)
    float workLoad{};               // instant workload (from denoised frame time)
    float smoothedWorkLoad{};       // filtered workload
};

class FrameInfoManager {
    static constexpr size_t POOL_COUNT = 8;
    static constexpr size_t MAX_FRAMETIME_HISTORY = 32u;

public:
    using duration = FrameInfo::duration;

    explicit FrameInfoManager(details::FEngine& engine);
    ~FrameInfoManager() noexcept;
    void terminate();
    void beginFrame(uint32_t frameId);  // call this immediately after "make current"
    void endFrame(); // call this immediately before "swap buffers"

    FrameInfo const& getLastFrameInfo() const {
        return mFrameTimeHistory[0];
    }

    duration getLastFrameTime() const noexcept {
        return getLastFrameInfo().frameTime;
    }

private:
    void update(duration lastFrameTime);

    details::FEngine& mEngine;
    backend::Handle<backend::HwTimerQuery> mQueries[POOL_COUNT];
    duration mFrameTime{};
    uint32_t mIndex = 0;
    uint32_t mLast = 0;

    std::array<FrameInfo, MAX_FRAMETIME_HISTORY> mFrameTimeHistory;
    size_t mFrameTimeHistorySize = 0;
};


} // namespace filament

#endif // TNT_FILAMENT_FRAMEINFO_H
