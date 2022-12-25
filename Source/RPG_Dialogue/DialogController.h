// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogController.generated.h"


USTRUCT(BlueprintType)
struct FDialogResponseData {
	GENERATED_BODY()

	FString PreviewText;
	int DialogFullID;
};

USTRUCT(BlueprintType)
struct FDialogData {
	GENERATED_BODY()

	int ID;
	FString SpeakerName;
	FString FullText;

	float AudioDuration;
	FString RelativeAudioURL;

	TArray<struct FDialogResponseData> ResponseChoices;

};


UCLASS()
class RPG_DIALOGUE_API UDialogController : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UDialogController();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Loads the Dialog Data (excluding audio data) into memory
	void LoadDialogData(FString ChunkID);

	// Will play dialogue at ID and present dialog options
	void PlayDialogue(int ID);

	// Load Audio from file URLand play it
	void LoadAndPlayAudio(FString AudioFileRelativeURL);

	// Reset the Display Variables, should be run before playing new dialog
	void ResetDialogDisplayVariables();

	UFUNCTION()
	void EndDialog();

	UFUNCTION()
	void TestFunction();


public:	
	// Called by character to start Dialog session
	void StartDialog(int StartingID);

	// Dialog Options Selected
	UFUNCTION(BlueprintCallable)
	void DialogOption1Selected();

	UFUNCTION(BlueprintCallable)
	void DialogOption2Selected();

	UFUNCTION(BlueprintCallable)
	void DialogOption3Selected();

	UFUNCTION(BlueprintCallable)
	void DialogOption4Selected();

private:
	// List of all dialog in loaded chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	TArray<FDialogData> AllDialog;

	// Whether we are in an active dialog session or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsDialogActive;

	int CurrentDialogID;

	// The Name to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplayName;

	// The Subtitle to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplaySubtitle;

	// The Text for Choice #1 to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplayChoice1Text;
	int Choice1ID;

	// The Text for Choice #2 to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplayChoice2Text;
	int Choice2ID;

	// The Text for Choice #3 to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplayChoice3Text;
	int Choice3ID;

	// The Text for Choice #4 to be displayed on the UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display Properties", meta = (AllowPrivateAccess = "true"))
	FString DisplayChoice4Text;
	int Choice4ID;

	// Timer for moving to next dialogue/exiting, set to the amount of FDialogData->AudioDuration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle DialogTimer;

	// Timer for if the user doesnt respond in time
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimeoutTimer;

	// Reads (from disk) and Plays Audio files at runtime
	UPROPERTY()
	class URuntimeAudioImporterLibrary* RuntimeAudioImporter;

	class UAudioComponent* DialogAudioPlayer;

public:
	//FORCEINLINE UWidgetComponent* GetDialogWidget() const { return DialogUIWidget; }

	void SayHello();

};
