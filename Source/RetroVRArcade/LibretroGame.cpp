// Fill out your copyright notice in the Description page of Project Settings.

#include "LibretroGame.h"
#include "libretro.h"
#include "Engine/Texture2D.h"
#include <iostream>
// #include <string>
#include <algorithm>

// Sets default values
ALibretroGame::ALibretroGame()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    KeyPresses.Init(false, 322);
}

// Called when the game starts or when spawned
void ALibretroGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALibretroGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

std::unique_ptr<FUpdateTextureRegion2D> g_echoUpdateTextureRegion;
UTexture2D* g_GameCanvas;
TArray<bool> g_KeyPresses;
bool lg_isInsertCoinPressed;
bool lg_isStartGamePressed;
bool lg_isUpPressed;
bool lg_isDownPressed;
bool lg_isRightPressed;
bool lg_isLeftPressed;
bool lg_isP1B1Pressed;
bool lg_isP1B2Pressed;
bool lg_isP1B3Pressed;
bool lg_isEnterPressed;
bool lg_isTabPressed;

/// <summary>
/// Method to import a DLL.
/// The DLL should be in a Plugin folder for the project
/// </summary>
/// <param name="folder"></param>
/// <param name="name"></param>
/// <returns></returns>
bool ALibretroGame::importDLL(FString folder, FString name)
{
    FString filePath = *FPaths::ProjectPluginsDir() + folder + "/" + name;

    if (FPaths::FileExists(filePath))
    {
        _dllHandle = FPlatformProcess::GetDllHandle(*filePath); // Retrieve the DLL.

        if (_dllHandle != NULL)
        {
            return true;
        }
    }

    return false;    // Return an error.
}

/// <summary>
/// Import all libretro methods to be used
/// </summary>
/// <returns></returns>
bool ALibretroGame::ImportMethods() {
    FString procName;

    if (_dllHandle == NULL)
    {
        return false;
    }

    procName = "retro_api_version";
    m_retroApiVersion = (_retro_api_version)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_init";    // Needs to be the exact name of the DLL method.
    m_retroInit = (_retro_init)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_deinit";    // Needs to be the exact name of the DLL method.
    m_retroDeInit = (_retro_init)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_load_game";
    m_retroLoadGame = (_retro_load_game)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_unload_game";
    m_retroUnloadGame = (_retro_unload_game)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_environment";
    m_retroSetEnvironment = (_retro_set_environment)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_video_refresh";
    m_retroSetVideoRefresh = (_retro_set_video_refresh)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_audio_sample";
    m_retroSetAudioSample = (_retro_set_audio_sample)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_audio_sample_batch";
    m_retroSetAudioSampleBatch = (_retro_set_audio_sample_batch)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_input_poll";
    m_retroSetInputPoll = (_retro_set_input_poll)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_set_input_state";
    m_retroSetInputState = (_retro_set_input_state)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_get_system_info";
    m_retroGetSystemInfo = (_retro_get_system_info)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    procName = "retro_run";
    m_retroRun = (_retro_run)FPlatformProcess::GetDllExport(_dllHandle, *procName);

    if (m_retroSetInputState == NULL || m_retroSetInputPoll == NULL || m_retroSetAudioSampleBatch == NULL
        || m_retroSetAudioSample == NULL || m_retroSetVideoRefresh == NULL || m_retroSetEnvironment == NULL
        || m_retroUnloadGame == NULL || m_retroLoadGame == NULL || m_retroInit == NULL || m_retroApiVersion == NULL) {

        return false;
    }

    return true;    // Return an error.
}

uint8* ALibretroGame::organizePixels(int width, int height, uint8* buffer) {
    int bytesPerPixel = 3;
    uint8* Pixels = new uint8[width * height * bytesPerPixel];
    for (int32 y = 0; y < height; y++)
    {
        for (int32 x = 0; x < width; x++)
        {
            //Get the current pixel
            int32 CurrentPixelIndex = ((y * width) + x);
            //Get a random vector that will represent the RGB values for the current pixel
            uint8* dest = &buffer[((y * width) + x) * bytesPerPixel];

            Pixels[4 * CurrentPixelIndex] = *dest;
            Pixels[4 * CurrentPixelIndex + 1] = *(dest + 1);
            Pixels[4 * CurrentPixelIndex + 2] = *(dest + 2);
            //Pixels[4 * CurrentPixelIndex + 3] = 255; //set A channel always to maximum
        }
    }

    return Pixels;
}

void lg_rarch_log_libretro(
    enum retro_log_level level,
    const char* fmt, ...)
{
    va_list vp;
    char buffer[256];
    va_start(vp, fmt);

    switch (level)
    {
    case RETRO_LOG_DEBUG:
        std::cout << "\n[libretro DEBUG]";
        vsprintf_s(buffer, fmt, vp);
        printf(buffer);
        break;

    case RETRO_LOG_INFO:
        std::cout << "\n[libretro INFO]";
        vsprintf_s(buffer, fmt, vp);
        printf(buffer);
        break;

    case RETRO_LOG_WARN:
        std::cout << "\n[libretro WARN]";
        vsprintf_s(buffer, fmt, vp);
        printf(buffer);
        break;

    case RETRO_LOG_ERROR:
        std::cout << "\n[libretro ERROR]";
        vsprintf_s(buffer, fmt, vp);
        printf(buffer);
        break;

    default:
        break;
    }

    va_end(vp);
}

bool lg_retro_set_environment(unsigned cmd, void* data) {
    FString filePath;

    switch (cmd) {
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        filePath = *FPaths::ProjectPluginsDir();
        filePath.AppendChars("mameSystem/", 9);
        *(const char**)data = TCHAR_TO_ANSI(*filePath);
        break;
    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
        filePath = *FPaths::ProjectPluginsDir();
        filePath.AppendChars("save/", 5);
        *(const char**)data = TCHAR_TO_ANSI(*filePath);
        break;
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    {
        struct retro_log_callback* cb = (struct retro_log_callback*)data;
        cb->log = lg_rarch_log_libretro;
        break;
    }
    }
    return true;
};

void lg_setupMipMapV1(UTexture2D* texture, uint8* buffer, int width, int height, int pitch) {
    int bytesPerPixel = pitch / width;
    FTexture2DMipMap* Mip;

    if (texture->PlatformData->Mips.Num() == 0) {
        Mip = new(texture->PlatformData->Mips) FTexture2DMipMap(); // add first mip to an empty texture...
        Mip->SizeX = width;
        Mip->SizeY = height;
    }
    else {
        Mip = &texture->PlatformData->Mips[0];
    }

    // Lock the texture so it can be modified

    Mip->BulkData.Lock(LOCK_READ_WRITE); // BulkData contains the texture data. Lock to prevent crashes
    uint8* TextureData = (uint8*)Mip->BulkData.Realloc(width * height * bytesPerPixel); // make sure space is available
    FMemory::Memcpy(TextureData, buffer, sizeof(uint8) * height * width * bytesPerPixel); // copy data into bulkdata
    Mip->BulkData.Unlock(); // unlock when done

    /// <summary>
    /// test
    /// </summary>
   // width = 512;
   // height = 512;
   // NewTexture->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_BGRA8, buffer);
    //texture->UpdateTextureRegions((int32)0, (uint32)1, g_echoUpdateTextureRegion.get(), (uint32)pitch, (uint32)bytesPerPixel, buffer);
    texture->UpdateResource();
}

void lg_video_driver_frame(const void* data, unsigned width,
    unsigned height, size_t pitch) {

    // Allocate first mipmap.
    // when mipmap is disabled, nothing is displayed in UE4

    lg_setupMipMapV1(g_GameCanvas, (uint8*)data, width, height, pitch);
}

void lg_audio_driver_sample(int16_t left, int16_t right)
{
    int x = 1;
}

size_t lg_audio_driver_sample_batch(const int16_t* data, size_t frames)
{
    return 0;
}

void lg_input_driver_poll(void) {
    int x = 1;
}

int16_t lg_input_state_get_last(unsigned port,
    unsigned device, unsigned index, unsigned id)
{
    if (device == RETRO_DEVICE_KEYBOARD) {
        return g_KeyPresses[id] ? 1 : 0;
    }

    return 0;
}

void ALibretroGame::UpdateInputsV2(bool insertCoin, bool startGame, bool up, bool down, bool right, bool left, bool p1b1,
    bool p1b2, bool p1b3,
    bool enter, bool tab) {

    g_KeyPresses = KeyPresses;

    return;
}

int ALibretroGame::retro_api_version() {
    int version = int(m_retroApiVersion());
    std::cout << "\nVersion is " << version;
    return version;
}

void ALibretroGame::retro_get_system_info() {
    struct retro_system_info m_system_info = { };
    m_system_info.need_fullpath = true;
    m_system_info.block_extract = true;

    m_retroGetSystemInfo(&m_system_info);
}

/// <summary>
/// Paths need to use backslashes on Windows
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool ALibretroGame::retro_load_game(FString path) {
    struct retro_game_info game;

    FString filePath = *FPaths::ProjectContentDir();
    filePath += "Roms/" + path;
    filePath = FPaths::ConvertRelativePathToFull(filePath);

    std::replace(filePath.begin(), filePath.end(), '/', '\\'); // Unreal returns forward slashes. Windows uses backslashes

    game.path = TCHAR_TO_ANSI(*filePath);

    if (m_retroLoadGame != NULL) {
        return m_retroLoadGame(&game);
    }

    return false;
}

bool ALibretroGame::InitializeCoreV2(FString folder, FString dllName, FString gamePath) {
    if (importDLL(folder, dllName) == false) {
        return false;
    }

    if (ImportMethods() == false) {
        return false;
    }

    retro_api_version();

    // set environment
    m_retroSetEnvironment(lg_retro_set_environment);
    m_retroSetVideoRefresh(lg_video_driver_frame);
    m_retroSetAudioSample(lg_audio_driver_sample);
    m_retroSetAudioSampleBatch(lg_audio_driver_sample_batch);
    m_retroSetInputPoll(lg_input_driver_poll);
    m_retroSetInputState(lg_input_state_get_last);

    retro_get_system_info();
    m_retroInit();
    retro_load_game(gamePath);

    return true;
}

void ALibretroGame::CreateTexture(int width, int height) {
    
    GameCanvas = UTexture2D::CreateTransient(width, height, PF_R5G6B5_UNORM, "MameTexture");

    GameCanvas->AddToRoot(); // prevent garbage collection
    GameCanvas->PlatformData = new FTexturePlatformData();
    GameCanvas->PlatformData->SizeX = width;
    GameCanvas->PlatformData->SizeY = height;
    GameCanvas->PlatformData->SetNumSlices(1);
    GameCanvas->PlatformData->PixelFormat = EPixelFormat::PF_R5G6B5_UNORM;
    GameCanvas->UpdateResource();
    g_GameCanvas = GameCanvas;
    g_echoUpdateTextureRegion  = std::unique_ptr<FUpdateTextureRegion2D>(new FUpdateTextureRegion2D(0, 0, 0, 0, width, height));
}



UTexture2D* ALibretroGame::RetroRunV2() {
    m_retroRun();
    // possibly return video buffer here?
    //UMaterialInstanceDynamic* RV_MatInst = UMaterialInstanceDynamic::Create(MasterMaterialRef, this);
   // RV_MatInst->SetTextureParameterValue(FName("T2DParam"), NewTexture);
    return GameCanvas;
}

void ALibretroGame::retro_unload_game() {
    if (m_retroUnloadGame != NULL) {
        m_retroUnloadGame();
    }
}

void ALibretroGame::retro_deinit() {
    if (m_retroDeInit != NULL)
    {
        m_retroDeInit(); // Call the DLL method with arguments corresponding to the exact signature and return type of the method.
    }
}

void ALibretroGame::freeDLL()
{
    if (_dllHandle != NULL)
    {
        m_retroApiVersion = NULL;
        m_retroInit = NULL;

        FPlatformProcess::FreeDllHandle(_dllHandle);
        _dllHandle = NULL;
    }
}


void ALibretroGame::UnloadV2() {
    retro_unload_game();
    retro_deinit();
    freeDLL();
}


