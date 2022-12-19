// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogController.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "RuntimeAudioImporterLibrary.h"
#include "GameFramework/Actor.h"

#include "RPG_DialogueCharacter.h"

// Sets default values
UDialogController::UDialogController() :
	DisplayName(FString(TEXT("Someone's Name-Dialog"))),
	DisplaySubtitle(FString(TEXT("Something interesting that they said.-Dialog"))),
	bIsDialogActive(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	
	/* Setup the DialogUIWidget for code */
	//DialogUIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialogUIWidget"));
	//DialogUIWidget->SetupAttachment(GetRootComponent());
	

}

// Called when the game starts or when spawned
void UDialogController::BeginPlay()
{
	Super::BeginPlay();

	FString ContentDirURL = FPaths::ProjectContentDir();
	//if (DialogUIWidget)
		//DialogUIWidget->SetVisibility(true);
	
	//GetWorldTimerManager().SetTimer(TimeoutTimer, this, &UDialogController::TestFunction, 6.f, false, -1.0f);
	LoadDialogData("CHUNK_X");

	
}

void UDialogController::LoadDialogData(FString ChunkID)
{
	// Temp for testing
	struct FDialogData TestDialog;
	TestDialog.SpeakerName = "Big Chungus";
	TestDialog.FullText = "Ah it is good to see you!";
	TestDialog.ID = 0;
	TArray<FDialogResponseData> Responses;
	TestDialog.ResponseChoices = Responses;
	TestDialog.RelativeAudioURL = "eyes_on_me.wav";
	TestDialog.AudioDuration = 5.f;

	AllDialog.EmplaceAt(0, TestDialog);
}

void UDialogController::StartDialog(int StartingID)
{
	if (!bIsDialogActive) {
		//ImportAudioExample();
		PlayDialogue(0);
		bIsDialogActive = true;
	}
}

void UDialogController::PlayDialogue(int ID)
{
	// Read Dialogue w/ ID
	FDialogData* CurrentDialog = &AllDialog[ID];
	if (CurrentDialog) {
		// Set DisplaySpeaker and DisplaySubtitle to "SpeakerName" and "Text"
		DisplayName = CurrentDialog->SpeakerName;
		DisplaySubtitle = CurrentDialog->FullText;

		// Read and Play Audio File
		LoadAndPlayAudio(CurrentDialog->RelativeAudioURL);

		// Check Response Choices:
		TArray<FDialogResponseData>* DialogResponseChoices = &(CurrentDialog->ResponseChoices);
		int NumResponses = DialogResponseChoices->Num();
		// if empty->EndDialog()
		if (NumResponses == 0) {
			// Wait for dialog to end, then tear down
			GetWorld()->GetTimerManager().SetTimer(DialogTimer, this, &UDialogController::EndDialog, CurrentDialog->AudioDuration, false, -1.0f);
			// If user skips (if they are given the option), then invalidate above timer, stop audio, and manually call EndDialog()
			

			//return;

		} // else if x1 item -> Queue Item for next dialog
		else if (NumResponses == 1) {
			//Wait for this to end, then run next dialog
			//  Set Timer with delegate to pass paramter into PlayDialog for next Dialog ID
		}
		else {// else -> For each item, check for criteria-> Set appropraite DisplayVariables
			for (FDialogResponseData Response : *DialogResponseChoices) {

			}
		}
		
			
			
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load dialog data w/ ID: %d"), ID);
	}

	
	
}

void UDialogController::LoadAndPlayAudio(FString AudioFileRelativeURL)
{
	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();

	if (!IsValid(RuntimeAudioImporter))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create audio importer"));
		return;
	}

	RuntimeAudioImporter->OnProgressNative.AddWeakLambda(this, [](int32 Percentage)
		{
			UE_LOG(LogTemp, Log, TEXT("Audio importing percentage: %d"), Percentage);
		});

	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ETranscodingStatus Status)
		{
			if (Status == ETranscodingStatus::SuccessfulImport)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully imported audio with sound wave %s"), *ImportedSoundWave->GetName());
				// Here you can handle ImportedSoundWave playback, like "UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);"
				UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			RuntimeAudioImporter = nullptr;
		});

	FString PathToAudio = FString("C:/") + AudioFileRelativeURL;
	UE_LOG(LogTemp, Warning, TEXT("This: %s"), *PathToAudio);
	RuntimeAudioImporter->ImportAudioFromFile(PathToAudio, EAudioFormat::Auto);
}

void UDialogController::EndDialog()
{
	UE_LOG(LogTemp, Warning, TEXT("DIALOG ENDED - Should notify player character who will hide UI and get back control"));
	ARPG_DialogueCharacter* PlayerCharacter = Cast<ARPG_DialogueCharacter>(this->GetOwner());
	PlayerCharacter->EndInteraction();
	// Need to stop actively playing audio if there is any, reference to loaded audio from GameplayStatics
	bIsDialogActive = false;

}

void UDialogController::TestFunction() {
	//DisplayName = FString(TEXT("Test Name"));

	return;
}




void UDialogController::SayHello() {
	//UE_LOG(LogTemp, Warning, TEXT("HELLO THERE"));
	
}


void UDialogController::ImportAudioExample()
{
	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();

	if (!IsValid(RuntimeAudioImporter))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create audio importer"));
		return;
	}

	RuntimeAudioImporter->OnProgressNative.AddWeakLambda(this, [](int32 Percentage)
		{
			UE_LOG(LogTemp, Log, TEXT("Audio importing percentage: %d"), Percentage);
		});

	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ETranscodingStatus Status)
		{
			if (Status == ETranscodingStatus::SuccessfulImport)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully imported audio with sound wave %s"), *ImportedSoundWave->GetName());
				// Here you can handle ImportedSoundWave playback, like "UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);"
				UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			RuntimeAudioImporter = nullptr;
		});

	RuntimeAudioImporter->ImportAudioFromFile(TEXT("C:/Eyes_on_me.wav"), EAudioFormat::Auto);
}



