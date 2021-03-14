#include <Audio/Audio.h>
#include "AudioInternal.h"
#include <Sound/Sound.h>
#include "../Sound/SoundInternal.h"

using namespace PGE;

Audio* Audio::create(ThreadManager* threadMgr) {
    return new AudioInternal(threadMgr);
}

AudioInternal::AudioInternal(ThreadManager* threadMgr) {
    threadManager = threadMgr;

    errorState = ERROR_STATE::NONE;

    alcDevice = alcOpenDevice(nullptr);
    if (alcDevice == nullptr) {
        errorState = ERROR_STATE::DEVICE_INIT_FAILED; return;
    }

    alcContext = alcCreateContext(alcDevice,nullptr);
    if (alcContext == nullptr) {
        errorState = ERROR_STATE::CONTEXT_INIT_FAILED;
        alcCloseDevice(alcDevice); alcDevice = nullptr;
        return;
    }

    if (!alcMakeContextCurrent(alcContext)) {
        errorState = ERROR_STATE::CONTEXT_MAKECURRENT_FAILED;
        alcDestroyContext(alcContext); alcContext = nullptr;
        alcCloseDevice(alcDevice); alcDevice = nullptr;
        return;
    }

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR) {
        errorState = ERROR_STATE::AL_MISC_CALL_FAILED;
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(alcContext); alcContext = nullptr;
        alcCloseDevice(alcDevice); alcDevice = nullptr;
        return;
    }

    for (int i=0;i<CHANNEL_COUNT;i++) {
        alSources[i] = 0;
        playingChannels[i] = nullptr;
    }
    alGenSources(CHANNEL_COUNT,alSources);
    alError = alGetError();
    if (alError != AL_NO_ERROR) {
        errorState = ERROR_STATE::SOURCE_ALLOC_FAILED;
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(alcContext); alcContext = nullptr;
        alcCloseDevice(alcDevice); alcDevice = nullptr;
        return;
    }

    audioThreadRequest = nullptr;
}

AudioInternal::~AudioInternal() {
    if (audioThreadRequest != nullptr) {
        audioThreadRequest->abort();
        std::lock_guard<std::mutex> lockGuard(audioThreadMutex);
    }
    //TODO: cleanup buffers and sources
    alcMakeContextCurrent(nullptr);
    if (alcContext != nullptr) { alcDestroyContext(alcContext); }
    if (alcDevice != nullptr) { alcCloseDevice(alcDevice); }
}

Audio::ERROR_STATE Audio::getErrorState() const {
    return errorState;
}

void AudioInternal::registerSound(SoundInternal* snd) {
    if (getErrorState() != ERROR_STATE::NONE) { return; }
    for (int i = 0; i < (int)loadedSounds.size(); i++) {
        if (loadedSounds[i] == snd) {
            return;
        }
    }
    loadedSounds.push_back(snd);
}

void AudioInternal::unregisterSound(SoundInternal* snd) {
    if (getErrorState() != ERROR_STATE::NONE) { return; }
    for (int i = 0; i < (int)loadedSounds.size(); i++) {
        if (loadedSounds[i] == snd) {
            loadedSounds.erase(loadedSounds.begin()+i);
            return;
        }
    }
}

bool AudioInternal::registerSoundChannel(SoundInternal::ChannelInternal* chn,ALuint& alSource) {
    if (getErrorState() != ERROR_STATE::NONE) { return false; }
    std::lock_guard<std::mutex> lockGuard(audioThreadMutex);
    alSource = 0;
    for (int i=0;i<CHANNEL_COUNT;i++) {
        bool chnAvailable = playingChannels[i]==nullptr;
        if (!chnAvailable) {
            chnAvailable = !playingChannels[i]->isPlaying();
        }
        if (chnAvailable) {
            playingChannels[i] = chn;
            alSource = alSources[i];
            if (chn->isStream()) {
                initStreamThread();
            }
            return true;
        }
    }
    return false;
}

void AudioInternal::unregisterSoundChannel(SoundInternal::ChannelInternal* chn) {
    if (getErrorState() != ERROR_STATE::NONE) { return; }
    std::lock_guard<std::mutex> lockGuard(audioThreadMutex);
    for (int i=0;i<CHANNEL_COUNT;i++) {
        if (playingChannels[i] == chn) {
            playingChannels[i] = nullptr;
            break;
        }
    }
}

void AudioInternal::initStreamThread() {
    if (getErrorState() != ERROR_STATE::NONE) { return; }
    if (audioThreadRequest == nullptr) {
        audioThreadRequest = new AudioThreadRequest(this);
        threadManager->requestExecutionOnNewThread(audioThreadRequest);
    }
}

bool AudioInternal::updateStreamThread() {
    if (getErrorState() != ERROR_STATE::NONE) { return false; }
    std::lock_guard<std::mutex> lockGuard(audioThreadMutex);
    if (audioThreadRequest == nullptr) {
        return false;
    }
    bool retVal = false;
    for (int i=0;i<CHANNEL_COUNT;i++) {
        if (playingChannels[i] != nullptr) {
            if (playingChannels[i]->isStream()) {
                if (playingChannels[i]->isPlaying()) {
                    retVal = true;
                    playingChannels[i]->updateStream();
                } else if (!playingChannels[i]->isStreamReady()) {
                    retVal = true;
                } else {
                    playingChannels[i] = nullptr;
                }
            }
        }
    }
    if (!retVal) {
        audioThreadRequest = nullptr;
    }
    return retVal;
}

AudioInternal::AudioThreadRequest::AudioThreadRequest(AudioInternal* a) {
    audio = a;
    abortRequested = false;
}

const int AudioInternal::AudioThreadRequest::SLEEP_MS;

void AudioInternal::AudioThreadRequest::execute() {
    while (!abortRequested) {
        if (!audio->updateStreamThread()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MS));
    }

    markAsDone();
}

void AudioInternal::AudioThreadRequest::abort() {
    abortRequested = true;
}
