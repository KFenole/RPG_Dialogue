// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogController.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "RuntimeAudioImporterLibrary.h"
#include "GameFramework/Actor.h"

#include "Components/AudioComponent.h"

#include "RPG_DialogueCharacter.h"

// Sets default values
UDialogController::UDialogController() :
	CurrentDialogID(-1),
	DisplayName(FString(TEXT("Someone's Name-Dialog"))),
	DisplaySubtitle(FString(TEXT("Something interesting that they said.-Dialog"))),
	bIsDialogActive(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	
	/* Setup the DialogUIWidget for code */
	//DialogUIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialogUIWidget"));
	//DialogUIWidget->SetupAttachment(GetRootComponent());
	
	DialogAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("DialogAudioPlayer"));

}

// Called when the game starts or when spawned
void UDialogController::BeginPlay()
{
	Super::BeginPlay();

	FString ContentDirURL = FPaths::ProjectContentDir();
	
	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();

	//DialogAudioPlayer->SetupAttachment(GetRootComponent());
	DialogAudioPlayer->SetUISound(true);
	
	//GetWorldTimerManager().SetTimer(TimeoutTimer, this, &UDialogController::TestFunction, 6.f, false, -1.0f);
	LoadDialogData("CHUNK_X");

	
}

void UDialogController::LoadDialogData(FString ChunkID)
{
	// Temp for testing
	// Dailog 0
	struct FDialogData TestDialog0;
	TestDialog0.SpeakerName = "NPC # 1";
	TestDialog0.FullText = "Ah it is good to see you!";
	TestDialog0.ID = 0;
	TArray<FDialogResponseData> Responses;
	TestDialog0.RelativeAudioURL = "eyes_on_me.wav";
	TestDialog0.AudioDuration = 5.f;

	// Response 0
	struct FDialogResponseData TDR0_0;
	TDR0_0.PreviewText = "Hello There";
	TDR0_0.DialogFullID = 1;

	// Dialog 1 (Response 0)
	struct FDialogData TestDialog1;
	TestDialog1.SpeakerName = "Player";
	TestDialog1.FullText = "Hey, good to see you too!";
	TestDialog1.ID = 1;
	TArray<FDialogResponseData> EmptyResponses;
	TestDialog1.ResponseChoices = EmptyResponses;
	TestDialog1.RelativeAudioURL = "eyes_on_me.wav";
	TestDialog1.AudioDuration = 5.f;

	// Response 1
	struct FDialogResponseData TDR0_1;
	TDR0_1.PreviewText = "Goodbye";
	TDR0_1.DialogFullID = 2;

	// Dialog 2 (Response 1)
	struct FDialogData TestDialog2;
	TestDialog2.SpeakerName = "Player";
	TestDialog2.FullText = "See ya!";
	TestDialog2.ID = 2;
	TestDialog2.ResponseChoices = EmptyResponses;
	TestDialog2.RelativeAudioURL = "eyes_on_me.wav";
	TestDialog2.AudioDuration = 1.f;

	// Response 2
	struct FDialogResponseData TDR0_2;
	TDR0_2.PreviewText = "Pizza Pie";
	TDR0_2.DialogFullID = 0;


	// Add to data array
	Responses.EmplaceAt(0, TDR0_0);
	Responses.EmplaceAt(1, TDR0_1);
	Responses.EmplaceAt(2, TDR0_2);

	TestDialog0.ResponseChoices = Responses;

	AllDialog.EmplaceAt(TestDialog0.ID, TestDialog0);
	AllDialog.EmplaceAt(TestDialog1.ID, TestDialog1);
	AllDialog.EmplaceAt(TestDialog2.ID, TestDialog2);
}

void UDialogController::StartDialog(int StartingID)
{
	if (!bIsDialogActive) {
		//ImportAudioExample();
		PlayDialogue(0);
		bIsDialogActive = true;
	}
}

void UDialogController::DialogOption1Selected()
{
	if (bIsDialogActive && Choice1ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 1 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking

		PlayDialogue(Choice1ID);
	}
}

void UDialogController::DialogOption2Selected()
{
	if (bIsDialogActive && Choice2ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 2 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking

		PlayDialogue(Choice2ID);
	}
}

void UDialogController::DialogOption3Selected()
{
	if (bIsDialogActive && Choice3ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 3 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking

		PlayDialogue(Choice3ID);
	}
}

void UDialogController::DialogOption4Selected()
{
	if (bIsDialogActive && Choice4ID != -1  /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 4 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking

		PlayDialogue(Choice4ID);
	}
}

void UDialogController::PlayDialogue(int ID)
{
	if (DialogAudioPlayer)
		DialogAudioPlayer->Stop();
	ResetDialogDisplayVariables();
	CurrentDialogID = ID;
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
			//int t = DialogAudioPlayer->durati
			GetWorld()->GetTimerManager().SetTimer(DialogTimer, this, &UDialogController::EndDialog, CurrentDialog->AudioDuration, false, -1.0f);
			// If user skips (if they are given the option), then invalidate above timer, stop audio, and manually call EndDialog()
			

			//return;

		} // else if x1 item -> Queue Item for next dialog
		else if (NumResponses == 1) {
			//Wait for this to end, then run next dialog
			//  Set Timer with delegate to pass paramter into PlayDialog for next Dialog ID
		}
		else {// else -> For each item, check for criteria-> Set appropraite DisplayVariables
			uint8 ResponseNumber = 0;
			for (FDialogResponseData Response : *DialogResponseChoices) {
				if (/* Response Criteria Met*/ true) {
					if (ResponseNumber == 0) {
						DisplayChoice1Text = Response.PreviewText;
						Choice1ID = Response.DialogFullID;
						ResponseNumber++; // Makes sure that there arent empty spaces for unavailable dialog
					} else if (ResponseNumber == 1) {
						DisplayChoice2Text = Response.PreviewText;
						Choice2ID = Response.DialogFullID;
						ResponseNumber++;
					} else if (ResponseNumber == 2) {
						DisplayChoice3Text = Response.PreviewText;
						Choice3ID = Response.DialogFullID;
						ResponseNumber++;
					} else if (ResponseNumber == 3) {
						DisplayChoice4Text = Response.PreviewText;
						Choice4ID = Response.DialogFullID;
						ResponseNumber++;
					}
					
				}
			}
		}
		
			
			
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load dialog data w/ ID: %d"), ID);
	}

	
	
}

void UDialogController::LoadAndPlayAudio(FString AudioFileRelativeURL)
{
	// Create the Audio Importer again if not already created
	if (!IsValid(RuntimeAudioImporter))
	{
		RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	}
	// Quit if error setting up Audio Importer
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
				//UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
				DialogAudioPlayer->SetSound(ImportedSoundWave);
				DialogAudioPlayer->Play();

			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			//RuntimeAudioImporter = nullptr;
		});

	FString PathToAudio = FString("C:/") + AudioFileRelativeURL;
	UE_LOG(LogTemp, Warning, TEXT("This: %s"), *PathToAudio);
	RuntimeAudioImporter->ImportAudioFromFile(PathToAudio, EAudioFormat::Auto);
}

void UDialogController::ResetDialogDisplayVariables()
{
	DisplayName = "";
	DisplaySubtitle = "";

	DisplayChoice1Text = "";
	Choice1ID = -1;
	DisplayChoice2Text = "";
	Choice2ID = -1;
	DisplayChoice3Text = "";
	Choice3ID = -1;
	DisplayChoice4Text = "";
	Choice4ID = -1;
}

void UDialogController::EndDialog()
{
	UE_LOG(LogTemp, Warning, TEXT("DIALOG ENDED - Should notify player character who will hide UI and get back control"));
	ARPG_DialogueCharacter* PlayerCharacter = Cast<ARPG_DialogueCharacter>(this->GetOwner());
	if (DialogAudioPlayer)
		DialogAudioPlayer->Stop();
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

/*
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

*/

