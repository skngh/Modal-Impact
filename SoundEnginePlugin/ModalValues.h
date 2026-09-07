//
//  ModalValues.h
//  MetalMakerSource
//
//  Created by Sam Knight on 9/4/26.
//

static constexpr int kNumModes = 15;

struct ModalBankPreset
{
    const float filter_freqs_[kNumModes];
    const float filter_t60_[kNumModes];
    const float filter_gain_[kNumModes];
    const float post_gain_ = 1.0f;
};

static constexpr ModalBankPreset kModalBanks[] =
{
    // water bottle
    {
        {270.1f, 810.6f, 1119.0f, 1362.0f, 1681.0f, 1883.0f, 2314.0f,
         2455.0f, 2635.0f, 2686.0f, 3605.0f, 4875.0f, 7035.0f, 8353.0f, 9980.0f},
        {4.5f, 3.0f, 1.4f, 1.2f, 2.0f, 2.5f, 1.1f,
         1.4f, 1.5f, 1.3f, 1.2f, 0.7f, 0.5f, 0.45f, 0.1f},
        {1.0f, 0.45f, 0.03f, 0.15f, 0.13f, 0.32f, 0.05f,
         0.15f, 0.13f, 0.20f, 0.14f, 0.03f, 0.01f, 0.01f, 0.007f},
        1.0f,
    },
    // sword impact
    {
        {1096.0f, 1601.0f, 2218.0f, 2798.0f, 2951.0f, 3092.0f, 4312.0f,
         5605.0f, 6252.0f, 7147.0f, 7777.0f, 8981.0f, 11583.0f, 13749.0f, 19503.0f},
        {0.25f, 0.2f, 0.37f, 0.35f, 0.15f, 0.35f, 0.33f,
         0.25f, 0.3f, 0.35f, 0.25f, 0.25f, 0.3f, 0.34f, 0.1f},
        {0.04f, 0.02f, 0.1f, 0.15f, 0.18f, 0.28f, 0.48f,
         1.0f, 0.23f, 0.31f, 0.18f, 0.1f, 0.08f, 0.07f, 0.03f},
        0.18f,
    },
    {
        // pipe
        {852.0f, 1182.0f, 1432.0f, 1560.0f, 1995.0f, 2473.0f, 2999.0f,
         3550.0f, 4150.0f, 4783.0f, 5459.0f, 6164.0f, 6890.0f, 7656.0f, 11798.0f},
        {0.26f, 0.5f, 0.5f, 0.35f, 0.4f, 0.38f, 0.55f,
         0.42f, 0.35f, 0.35f, 0.36f, 0.28f, 0.2f, 0.24f, 0.16f},
        {0.01f, 0.04f, 0.03f, 0.06f, 0.52f, 0.66f, 0.12f,
         1.0f, 0.37f, 0.09f, 0.05f, 0.16f, 0.16f, 0.03f, 0.06f},
        0.4f,
    },
};
