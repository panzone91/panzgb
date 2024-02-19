#include "gb-impl.h"
#include "gb-sound.h"

void increase_sound_timers(gb *cpu) {
    if (CHANNEL_1_LENGTH_ENABLED != 0) {
        cpu->channel_1.length_timer++;
        if (cpu->channel_1.length_timer >= 64) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFE;
        }
    }
    if(CHANNEL_2_LENGTH_ENABLED != 0){
        cpu->channel_2.length_timer++;
        if(cpu->channel_2.length_timer >= 64) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFD;
        }
    }
    if(CHANNEL_3_LENGTH_ENABLED != 0){
        cpu->channel_3.length_timer++;
        if(cpu->channel_3.length_timer >= 256) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFB;
        }
    }
    if(CHANNEL_4_LENGTH_ENABLED != 0){
        cpu->channel_4.length_timer++;
        if(cpu->channel_4.length_timer >= 64) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xF7;
        }
    }
}

void increase_vol_envelope(gb *cpu){
    if (cpu->channel_1.env_sweep_pace != 0) {
        cpu->channel_1.env_vol += (cpu->channel_1.env_sweep_pace == 0) ? -1 : 1;
        if (cpu->channel_1.env_vol < 0) cpu->channel_1.env_vol = 0;
        if (cpu->channel_1.env_vol > 15) cpu->channel_1.env_vol = 15;
    }
    if(cpu->channel_2.env_sweep_pace !=0) {
        cpu->channel_2.env_vol += (cpu->channel_2.env_dir == 0) ? -1 : 1;
        if (cpu->channel_2.env_vol < 0) cpu->channel_2.env_vol = 0;
        if (cpu->channel_2.env_vol > 15) cpu->channel_2.env_vol = 15;
    }
    if(cpu->channel_4.env_sweep_pace !=0) {
        cpu->channel_4.env_vol += (cpu->channel_4.env_dir == 0) ? -1 : 1;
        if (cpu->channel_4.env_vol < 0) cpu->channel_4.env_vol = 0;
        if (cpu->channel_4.env_vol > 15) cpu->channel_4.env_vol = 15;
    }
}

void increase_sweep(gb *cpu){
    if(CHANNEL_1_SWEEP_PACE !=0) {
        WORD newPeriod = CHANNEL_1_PERIOD_CLOCK;
        newPeriod = CHANNEL_1_SWEEP_DIR != 0? newPeriod - (CHANNEL_1_PERIOD_CLOCK >> CHANNEL_1_SWEEP_STEP) : newPeriod + (CHANNEL_1_PERIOD_CLOCK >> CHANNEL_1_SWEEP_STEP);
        NR13 = newPeriod & 0xFF;
        NR14 = (NR14 & 0xF8) | (newPeriod >> 8);
    }
}

void handleSound(gb *cpu, BYTE clocks) {

    BYTE s = 0;

    if (AUDIO_OFF)
        return;

    cpu->channel_1.period_clock -= clocks;
    if (cpu->channel_1.period_clock < 0) {
        s = squareWave[CHANNEL_1_WAVE][cpu->channel_1.pos & 7];
        cpu->channel_1.pos++;
        cpu->channel_1.period_clock += (2048 - CHANNEL_1_PERIOD_CLOCK) *4;

        s *= cpu->channel_1.env_vol;

        if (CHANNEL_1_ON != 0 && CHANNEL_1_RIGHT_ACTIVE != 0) cpu->rightSoundBuffer += s;
        if (CHANNEL_1_ON != 0 && CHANNEL_1_LEFT_ACTIVE != 0) cpu->leftSoundBuffer += s;
    }

    cpu->channel_2.period_clock -= clocks;
    if(cpu->channel_2.period_clock < 0) {
        s = squareWave[CHANNEL_2_WAVE][cpu->channel_2.pos & 7];
        cpu->channel_2.pos++;
        cpu->channel_2.period_clock += (2048 - CHANNEL_2_PERIOD_CLOCK) *4;

        s *= cpu->channel_2.env_vol;
        if (CHANNEL_2_ON != 0 && CHANNEL_2_RIGHT_ACTIVE != 0) cpu->rightSoundBuffer += s;
        if (CHANNEL_2_ON != 0 && CHANNEL_2_LEFT_ACTIVE !=0) cpu->leftSoundBuffer += s;
    }

     cpu->channel_3.period_clock -= clocks;
     if(cpu->channel_3.period_clock < 0) {
         s = CHANNEL_3_WAVE[cpu->channel_3.pos / 2] &0xFF;
         //If pos is even, we need the upper nibble, otherwise the lower one
         s = cpu->channel_3.pos % 2 == 0 ? s >> 4 : s & 0xF;
         cpu-> channel_3.pos = cpu->channel_3.pos++ &0x1F;
         cpu->channel_3.period_clock += (2048 - CHANNEL_3_PERIOD_CLOCK) *2;
         if(CHANNEL_3_OUTPUT_LEVEL == 0) {
             s = 0;
         } else {
             s >>= CHANNEL_3_OUTPUT_LEVEL -1;
         }

         if (CHANNEL_3_ON != 0 && CHANNEL_3_RIGHT_ACTIVE != 0) cpu->rightSoundBuffer += s;
         if (CHANNEL_3_ON != 0 && CHANNEL_3_LEFT_ACTIVE !=0) cpu->leftSoundBuffer += s;
     }

    cpu->channel_4.period_clock -= clocks;
    if(cpu->channel_4.period_clock <= 0) {
        s = noise[cpu->channel_4.pos & 0xF] & 0x1;
        s *= cpu->channel_4.env_vol;
        cpu->channel_4.pos++;
        cpu->channel_4.period_clock += CHANNEL_4_FREQ_MULTIPLIER;

        if (CHANNEL_4_ON != 0 && CHANNEL_4_RIGHT_ACTIVE != 0) cpu->rightSoundBuffer += s;
        if (CHANNEL_4_ON != 0 && CHANNEL_4_LEFT_ACTIVE != 0) cpu->leftSoundBuffer += s;
    }

    cpu->soundFrameClock -= clocks;
    if(cpu->soundFrameClock <= 0){
        cpu->soundFrameClock += 8192; //512 hz
        switch (cpu->soundFramePos) {
            case 0:
            case 4:
                increase_sound_timers(cpu);
                break;
            case 2:
            case 6:
                increase_sound_timers(cpu);
                //increase sweep
                break;
            case 7:
                increase_vol_envelope(cpu);
                break;
            default:
                break;
        }
        cpu->soundFramePos = cpu->soundFramePos++ & 0x7;
    }

    cpu->soundMasterClock -= clocks;
    if (cpu->soundMasterClock <= 0) {
        cpu->soundMasterClock += GB_CLOCK / 44100;
        cpu->leftSoundBuffer *= VOLUME_LEFT;
        cpu->rightSoundBuffer *= VOLUME_RIGHT;
        if (cpu->leftSoundBuffer > 127) cpu->leftSoundBuffer = 127;
        else if (cpu->leftSoundBuffer < -128) cpu->leftSoundBuffer = -128;
        if (cpu->rightSoundBuffer > 127) cpu->rightSoundBuffer = 127;
        else if (cpu->rightSoundBuffer < -128) cpu->rightSoundBuffer = -128;

        cpu->soundData[cpu->numerOfBytes++] = cpu->leftSoundBuffer + 128;
        cpu->soundData[cpu->numerOfBytes++] = cpu->rightSoundBuffer + 128;
        cpu->leftSoundBuffer = 0;
        cpu->rightSoundBuffer = 0;
    }
}

void writeSoundRegistry(gb *cpu, WORD addr, BYTE data){

    if(addr == 0xFF11) {
        BYTE lengthTimer = data &0x3F;
        cpu->channel_1.length_timer = lengthTimer;
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF12) {
        cpu->memory[addr] = data;
        if((data & 0xF8) == 0){
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFE;
        }
    }
    else if(addr == 0xFF13) {
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF14) {
        BYTE triggering = data &0x80;
        if(triggering != 0) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] | 0x1;
            cpu->channel_1.pos = 0;
            cpu->channel_1.period_clock = (2048 - CHANNEL_1_PERIOD_CLOCK) *4;
            cpu->channel_1.length_timer = NR11 & 0x3F;
            cpu->channel_1.env_vol = NR12 >> 4;
            cpu->channel_1.env_dir = (NR12 & 0x8) >> 4;
            cpu->channel_1.env_sweep_pace = NR12 & 0x7;
        }
        cpu->memory[addr] = data;
    }

    else if(addr == 0xFF16) {
        BYTE lengthTimer = data &0x3F;
        cpu->channel_2.length_timer = lengthTimer;
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF17) {
        cpu->memory[addr] = data;
        if((data & 0xF8) == 0){
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFD;
        }
    }
    else if(addr == 0xFF18) {
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF19) {
        BYTE triggering = data &0x80;
        if(triggering != 0) {
            cpu->channel_2.pos = 0;
            cpu->channel_2.period_clock = (2048 - CHANNEL_2_PERIOD_CLOCK) *4;
            cpu->channel_2.length_timer = NR21 & 0x3F;
            cpu->channel_2.env_vol = NR22 >> 4;
            cpu->channel_2.env_dir = (NR22 & 0x8) >> 4;
            cpu->channel_2.env_sweep_pace = NR22 & 0x7;
            cpu->memory[0xFF26] = cpu->memory[0xFF26] | 0x2;
        }
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF1A) {
        BYTE on = data & 0x80;
        if(on == 0) {
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xFB;
        }
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF1B) {
        cpu->channel_3.length_timer = data;
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF1C) {
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF1D) {
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF1E) {
        BYTE triggering = data &0x80;
        if(triggering != 0) {
            cpu->channel_3.pos = 1;
            cpu->channel_3.period_clock = (2048 - CHANNEL_3_PERIOD_CLOCK) *2;
            cpu->channel_3.length_timer = NR32;
            cpu->memory[0xFF26] = cpu->memory[0xFF26] | 0x4;
        }
        cpu->memory[addr] = data;
    }


    else if(addr == 0xFF41) {
        BYTE lengthTimer = data &0x3F;
        cpu->channel_4.length_timer = lengthTimer;
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF42) {
        cpu->memory[addr] = data;
        if((data & 0xF8) == 0){
            cpu->memory[0xFF26] = cpu->memory[0xFF26] & 0xF7;
        }
    }
    else if(addr == 0xFF43) {
        cpu->memory[addr] = data;
    }
    else if(addr == 0xFF44) {
        BYTE triggering = data &0x80;
        if(triggering != 0) {
            cpu->channel_4.env_vol = NR42 >> 4;
            cpu->channel_4.length_timer = NR41 & 0x3F;
            cpu->channel_4.pos = 0;
            cpu->channel_4.env_dir = (NR42 & 0x8) >> 4;
            cpu->channel_4.env_sweep_pace = NR42 & 0x7;
            cpu->channel_4.period_clock = 0;
            cpu->memory[0xFF26] = cpu->memory[0xFF26] | 0x8;
        }
        cpu->memory[addr] = data;
    }

    else if(addr == 0xFF26) {
        BYTE val = data & 0x80;
        cpu->memory[addr] = val | cpu->memory[addr];
    }
    else {
        cpu->memory[addr] = data;
    }
}

