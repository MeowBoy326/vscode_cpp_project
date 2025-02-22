

#ifndef METAENGINE_AUDIO_H
#define METAENGINE_AUDIO_H

#include <math.h>

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "FallingSand_core.hpp"
#include "FallingSand_macros.hpp"
#include "FallingSand_mathlib_base.hpp"
#include "FallingSand_engine.hpp"

// FMOD header
#include <fmod.hpp>
#include <fmod_studio.hpp>

namespace FMOD {
namespace Studio {
class EventDescription;
class Bank;
}  // namespace Studio
}  // namespace FMOD

namespace ME {

#pragma region FMODWrapper

class event;

class event_instance {
    event *type;

    void start();
    void stop();
};

/**
   Models FMOD event descriptions
*/
struct event {
    FMOD::Studio::EventDescription *fmod_bank;
    std::string path;

    event_instance instance();
};

struct bank {
    /** Instance of the fmod bank object */
    FMOD::Studio::Bank *fmod_bank;

    std::unordered_map<std::string, event> event_map;

    bank(FMOD::Studio::Bank *fmod_bank);

    /** Load an event description, caching if already loaded */
    event *load_event(const char *path);
};

/** Singleton for storing banks */
class bank_manager {
    std::unordered_map<std::string, bank> bank_map;

    bank_manager();

public:
    static bank_manager &instance();

    /** Unload a bank */
    void unload(const char *bank_path);

    /** Returns a pointer to the bank if loaded, or null for a failure (error
      printed to console). Caches, so future calls will load the same bank. */
    bank *load(const char *path);
};

/** Initialise the fmod system */
void init_fmod_system();

/** Get the fmod system - asserts if not initialised */
FMOD::Studio::System *get_fmod_system();
FMOD::System *get_fmod_core_system();

struct fmod_exception {
    const char *message;
};

/** Checks the result, printing it out and throwing an 'fmod_exception' if the
    result is an error. */
inline void check_err(FMOD_RESULT res) {
    if (res != FMOD_OK) {
        // throw fmod_exception{FMOD_ErrorString(res)};
    }
}

#pragma endregion FMODWrapper

struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    int mnNextChannelId;

    typedef std::map<std::string, FMOD::Sound *> SoundMap;
    typedef std::map<int, FMOD::Channel *> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance *> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank *> BankMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class Audio {
public:
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    void LoadBank(const std::string &strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    FMOD::Studio::Bank *GetBank(const std::string &strBankName);
    void LoadEvent(const std::string &strEventName);
    void LoadSound(const std::string &strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string &strSoundName);
    void Set3dListenerAndOrientation(const MEvec3 &vPosition, const MEvec3 &vLook, const MEvec3 &vUp);
    int PlaySounds(const std::string &strSoundName, const MEvec3 &vPos = MEvec3{0, 0, 0}, float fVolumedB = 0.0f);
    void PlayEvent(const std::string &strEventName);
    FMOD::Studio::EventInstance *GetEvent(const std::string &strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const std::string &strEventName, bool bImmediate = false);
    void GetEventParameter(const std::string &strEventName, const std::string &strEventParameter, float *parameter);
    void SetEventParameter(const std::string &strEventName, const std::string &strParameterName, float fValue);
    void SetGlobalParameter(const std::string &strParameterName, float fValue);
    void GetGlobalParameter(const std::string &strEventParameter, float *parameter);
    void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const MEvec3 &vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const std::string &strEventName) const;
    float dbToVolume(float dB);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const MEvec3 &vPosition);
};

void AudioEngineInit();

}  // namespace ME

#endif  // METAENGINE_AUDIO_H
