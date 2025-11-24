#ifndef __GB_SOUND_H__
#define __GB_SOUND_H__

#include "gb-impl.h"

const static BYTE squareWave[4][8] ={
        { 0, 0,0, 0, 0, 0, 0, 1 },
        { 1,0, 0, 0, 0, 0, 0, 1 },
        { 1,0,0,0, 0, 1, 1, 1 },
        { 0,1,1,1,1,1, 1, 0 }
};

const static BYTE noise[] = {
        0xfb,0xe7,0xae,0x1b,0xa6,0x2b,0x05,0xe3,
        0xb6,0x4a,0x42,0x72,0xd1,0x19,0xaa,0x03,
};

#define NR50 cpu->memory[0xFF24]
#define NR51 cpu->memory[0xFF25]
#define NR52 cpu->memory[0xFF26]

#define NR10 cpu->memory[0xFF10]
#define NR11 cpu->memory[0xFF11]
#define NR12 cpu->memory[0xFF12]
#define NR13 cpu->memory[0xFF13]
#define NR14 cpu->memory[0xFF14]

#define NR21 cpu->memory[0xFF16]
#define NR22 cpu->memory[0xFF17]
#define NR23 cpu->memory[0xFF18]
#define NR24 cpu->memory[0xFF19]

#define NR32 cpu->memory[0xFF1C]
#define NR33 cpu->memory[0xFF1D]
#define NR34 cpu->memory[0xFF1E]

#define NR41 cpu->memory[0xFF20]
#define NR42 cpu->memory[0xFF21]
#define NR43 cpu->memory[0xFF22]
#define NR44 cpu->memory[0xFF23]

#define AUDIO_OFF ((NR52 & 0x80) == 0)
#define VOLUME_LEFT ((NR50 & 0x70 )>> 4 )
#define VOLUME_RIGHT (NR50 & 0x7 )


#define CHANNEL_1_ON (NR52 & 0x1)
#define CHANNEL_1_PERIOD_CLOCK (((NR14 & 0x7) << 8) |  NR13)
#define CHANNEL_1_WAVE ((NR11 & 0xC0) >> 6)
#define CHANNEL_1_LENGTH_ENABLED (NR14 & 0x40)
#define CHANNEL_1_ENVELOP_DIR (NR12 & 0x8)
#define CHANNEL_1_SWEEP_PACE ((NR11 & 0x70) >> 4)
#define CHANNEL_1_SWEEP_DIR ((NR11 & 0x08) >> 3)
#define CHANNEL_1_SWEEP_STEP (NR11 & 0x07)

#define CHANNEL_1_LEFT_ACTIVE (NR51 & 0x10 )
#define CHANNEL_1_RIGHT_ACTIVE (NR51 & 0x1 )

#define CHANNEL_2_ON (NR52 & 0x2)
#define CHANNEL_2_PERIOD_CLOCK (((NR24 & 0x7) << 8) |  NR23)
#define CHANNEL_2_WAVE ((NR21 & 0xC0) >> 6)
#define CHANNEL_2_LENGTH_ENABLED (NR24 & 0x40)
#define CHANNEL_2_SWEEP_PACE (NR22 & 0x7)
#define CHANNEL_2_ENVELOP_DIR (NR22 & 0x8)
#define CHANNEL_2_LEFT_ACTIVE (NR51 & 0x20 )
#define CHANNEL_2_RIGHT_ACTIVE (NR51 & 0x2 )

#define CHANNEL_3_ON (NR52 & 0x4)
#define CHANNEL_3_OUTPUT_LEVEL ((NR32 >> 5) & 0x3)
#define CHANNEL_3_LENGTH_ENABLED (NR34 & 0x40)
#define CHANNEL_3_WAVE (cpu->memory + 0xFF30)
#define CHANNEL_3_PERIOD_CLOCK (((NR34 & 0x7) << 8) |  NR33)

#define CHANNEL_3_LEFT_ACTIVE (NR51 & 0x40 )
#define CHANNEL_3_RIGHT_ACTIVE (NR51 & 0x4 )

#define CHANNEL_4_ON (NR52 & 0x4)
#define CHANNEL_4_LFSR (NR43 & 0x8)
#define CHANNEL_4_FREQ_MULTIPLIER ((NR43 & 0x7) << (NR43 >> 4))
#define CHANNEL_4_LENGTH_ENABLED (NR44 & 0x40)
#define CHANNEL_4_SWEEP_PACE (NR42 & 0x7)
#define CHANNEL_4_ENVELOP_DIR (NR42 & 0x8)
#define CHANNEL_4_LEFT_ACTIVE (NR51 & 0x80 )
#define CHANNEL_4_RIGHT_ACTIVE (NR51 & 0x8 )

#endif
