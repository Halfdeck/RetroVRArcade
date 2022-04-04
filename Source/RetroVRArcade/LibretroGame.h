// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "libretro.h"
#include "LibretroGame.generated.h"

UCLASS()
class RETROVRARCADE_API ALibretroGame : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALibretroGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Libretro Game")
		UTexture2D* GameCanvas;

	UPROPERTY(BlueprintReadWrite, Category = "Libretro Game")
		TArray<bool> KeyPresses;

	UFUNCTION(BlueprintCallable, Category = "Libretro Game")
		void CreateTexture(int width, int height);

	UFUNCTION(BlueprintCallable, Category = "Libretro Game")
		bool InitializeCoreV2(FString folder, FString dllName, FString gamePath);

	// Sets default values for this actor's properties
	UFUNCTION(BlueprintCallable, Category = "Libretro Game")
		UTexture2D* RetroRunV2();

	UFUNCTION(BlueprintCallable, Category = "Libretro Game")
		void UpdateInputsV2(bool insertCoin, bool startGame, bool up, bool down, bool right, bool left, bool p1b1,
			bool p1b2, bool p1b3,
			bool enter, bool tab);

	UFUNCTION(BlueprintCallable, Category = "Libretro Game")
		void UnloadV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool importDLL(FString folder, FString name);
	bool ImportMethods();
	uint8* organizePixels(int width, int height, uint8* buffer);
	int retro_api_version();
	void retro_get_system_info();
	bool retro_load_game(FString path);
	void retro_unload_game();
	void retro_deinit();
	void freeDLL();
	void setupMipMapV1(UTexture2D* texture, uint8* buffer, int width, int height, int pitch);

	_retro_api_version m_retroApiVersion;
	_retro_init m_retroInit;
	_retro_deinit m_retroDeInit;
	_retro_load_game m_retroLoadGame;
	_retro_unload_game m_retroUnloadGame;
	_retro_set_environment m_retroSetEnvironment;
	_retro_set_video_refresh m_retroSetVideoRefresh;
	_retro_set_audio_sample m_retroSetAudioSample;
	_retro_set_audio_sample_batch m_retroSetAudioSampleBatch;
	_retro_set_input_poll m_retroSetInputPoll;
	_retro_set_input_state m_retroSetInputState;
	_retro_get_system_info m_retroGetSystemInfo;
	_retro_run m_retroRun;

	uint8* m_frameBuffer;
	void* _dllHandle;
};
