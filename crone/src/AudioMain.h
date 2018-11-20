//
// Created by emb on 11/19/18.
//

#ifndef CRONE_AUDIOMAIN_H
#define CRONE_AUDIOMAIN_H

#include "Bus.h"

namespace  crone {
    class AudioMain {
        enum {
            MAX_BUF_SIZE = 512
        };
    public:
        AudioMain();

        void processBlock(
                const float *in_adc[2],
                const float *in_ext[2],
                float *out[2],
                size_t numFrames);

    private:
        typedef Bus<2, MAX_BUF_SIZE> StereoBus;
        struct BusList {
            StereoBus adc_out;
            StereoBus dac_in;
            StereoBus ins_in;
            StereoBus ins_out;
            StereoBus aux_in;
            StereoBus aux_out;
            StereoBus adc_monitor;
            BusList();
        };
        BusList bus;

        struct SmoothLevelList{
        public:
            LogRamp adc;
            LogRamp dac;
            LogRamp monitor;
            LogRamp ins_mix;
            LogRamp monitor_aux;
            LogRamp aux_dac;
            SmoothLevelList();
        };
        SmoothLevelList smoothLevels;

        struct StaticLevelList {
            float monitor_l_l;
            float monitor_l_r;
            float monitor_r_l;
            float monitor_r_r;
            StaticLevelList();
        };
        StaticLevelList staticLevels;
    };
}

#endif //CRONE_AUDIOMAIN_H
