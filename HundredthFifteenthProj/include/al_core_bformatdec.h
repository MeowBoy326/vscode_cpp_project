#ifndef CORE_BFORMATDEC_H
#define CORE_BFORMATDEC_H

#include <array>
#include <cstddef>
#include <memory>
#include <variant>
#include <vector>

#include "al_common_alspan.h"
#include "al_core_ambidefs.h"
#include "al_core_bufferline.h"
#include "al_core_devformat.h"
#include "al_core_splitter.h"
#include "al_core_front_stablizer.h"


using ChannelDec = std::array<float,MaxAmbiChannels>;

class BFormatDec {
    static constexpr size_t sHFBand{0};
    static constexpr size_t sLFBand{1};
    static constexpr size_t sNumBands{2};

    struct ChannelDecoderSingle {
        std::array<float,MaxOutputChannels> mGains{};
    };

    struct ChannelDecoderDual {
        BandSplitter mXOver;
        std::array<std::array<float,MaxOutputChannels>,sNumBands> mGains{};
    };

    alignas(16) std::array<FloatBufferLine,2> mSamples{};

    const std::unique_ptr<FrontStablizer> mStablizer;

    std::variant<std::vector<ChannelDecoderSingle>,std::vector<ChannelDecoderDual>> mChannelDec;

public:
    BFormatDec(const size_t inchans, const al::span<const ChannelDec> coeffs,
        const al::span<const ChannelDec> coeffslf, const float xover_f0norm,
        std::unique_ptr<FrontStablizer> stablizer);

    [[nodiscard]] auto hasStablizer() const noexcept -> bool { return mStablizer != nullptr; }

    /* Decodes the ambisonic input to the given output channels. */
    void process(const al::span<FloatBufferLine> OutBuffer,
        const al::span<const FloatBufferLine> InSamples, const size_t SamplesToDo);

    /* Decodes the ambisonic input to the given output channels with stablization. */
    void processStablize(const al::span<FloatBufferLine> OutBuffer,
        const al::span<const FloatBufferLine> InSamples, const size_t lidx, const size_t ridx,
        const size_t cidx, const size_t SamplesToDo);

    static std::unique_ptr<BFormatDec> Create(const size_t inchans,
        const al::span<const ChannelDec> coeffs, const al::span<const ChannelDec> coeffslf,
        const float xover_f0norm, std::unique_ptr<FrontStablizer> stablizer);
};

#endif /* CORE_BFORMATDEC_H */
