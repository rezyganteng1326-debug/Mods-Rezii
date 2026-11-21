#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>

#define LOG_TAG "R3ZiMods"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════
//          R3Zi MODS v1.0
//     Drag Bike 201M Sunmori
// ═══════════════════════════════════

namespace Features {
    bool UnlimitedCoin   = false;
    bool UnlimitedNitro  = false;
    bool MaxSpeed        = false;
    bool UnlockAllBike   = false;
    bool AntiBan         = false;
}

// Offset placeholder — isi setelah dump IL2CPP
namespace Offsets {
    uintptr_t CoinValue    = 0x0; // FILL: dari il2cppdumper
    uintptr_t NitroValue   = 0x0; // FILL: dari il2cppdumper
    uintptr_t SpeedValue   = 0x0; // FILL: dari il2cppdumper
    uintptr_t BikeUnlock   = 0x0; // FILL: dari il2cppdumper
}

uintptr_t getBase() {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return 0;
    char line[256];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "libil2cpp.so")) {
            base = (uintptr_t)strtoul(line, nullptr, 16);
            break;
        }
    }
    fclose(f);
    return base;
}

void patchMemory(uintptr_t addr, uint32_t value) {
    uintptr_t page = addr & ~(0xFFF);
    mprotect((void*)page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    *(uint32_t*)addr = value;
    mprotect((void*)page, 0x1000, PROT_READ | PROT_EXEC);
    LOGI("Patched 0x%lx = 0x%x", addr, value);
}

void applyFeatures() {
    uintptr_t base = getBase();
    if (!base) {
        LOGE("Failed to get base address");
        return;
    }
    LOGI("Base: 0x%lx", base);

    if (Features::UnlimitedCoin && Offsets::CoinValue)
        patchMemory(base + Offsets::CoinValue, 0x05F5E0FF); // 99999999

    if (Features::UnlimitedNitro && Offsets::NitroValue)
        patchMemory(base + Offsets::NitroValue, 0x461C4000); // float 9999

    if (Features::MaxSpeed && Offsets::SpeedValue)
        patchMemory(base + Offsets::SpeedValue, 0x47C35000); // float 99999

    if (Features::UnlockAllBike && Offsets::BikeUnlock)
        patchMemory(base + Offsets::BikeUnlock, 0x00000001); // true

    if (Features::AntiBan)
        LOGI("AntiBan active");
}

void* mainThread(void*) {
    sleep(3); // tunggu game load
    LOGI("R3Zi MODS loaded");

    // aktifin semua fitur
    Features::UnlimitedCoin  = true;
    Features::UnlimitedNitro = true;
    Features::MaxSpeed        = true;
    Features::UnlockAllBike   = true;
    Features::AntiBan         = true;

    applyFeatures();
    return nullptr;
}

__attribute__((constructor))
void init() {
    pthread_t t;
    pthread_create(&t, nullptr, mainThread, nullptr);
    pthread_detach(t);
}
