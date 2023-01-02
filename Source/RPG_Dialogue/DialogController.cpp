// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogController.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "RuntimeAudioImporterLibrary.h"
#include "GameFramework/Actor.h"

#include "Components/AudioComponent.h"
#include "XmlFile.h"

#include "RPG_DialogueCharacter.h"

// Sets default values
UDialogController::UDialogController() :
	CurrentDialogID(-1),
	DisplayName(FString(TEXT("Someone's Name-Dialog"))),
	DisplaySubtitle(FString(TEXT("Something interesting that they said.-Dialog"))),
	bIsDialogActive(false)
{

	
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
	// Load the dialog file, and check if valid
	FXmlFile ImportedFile(TEXT("C:/dialog.xml"), EConstructMethod::ConstructFromFile); // <-- Currently using root directory for convenience
	if (!ImportedFile.IsValid()) {
		FString err = ImportedFile.GetLastError();
		UE_LOG(LogTemp, Error, TEXT("%s"), *err);
		UE_LOG(LogTemp, Error, TEXT("FAILED TO LOAD DIALOG FILE"));
		return;
	}
	FXmlNode* FileRoot = ImportedFile.GetRootNode();
	FString RootTag = FileRoot->GetTag();
	
	// Get all the dialog nodes
	TArray<FXmlNode*>DialogNodes = FileRoot->GetChildrenNodes();
	// Go through each dialog node
	for (FXmlNode* dialogNode : DialogNodes) {
		// This Node's Dialog to be stored
		struct FDialogData Dialog;

		// Go through the data (id, text, responses, etc.) in each of the dialog nodes
		TArray<FXmlNode*>ChildNodes = dialogNode->GetChildrenNodes();
		for (FXmlNode* childNode : ChildNodes) {
			FString tag = childNode->GetTag();
			FString Content = childNode->GetContent();

			if (tag == "ID") {
				int DialogID = FCString::Atoi(*Content);
				Dialog.ID = DialogID;
			}
			else if (tag == "SPEAKER") {
				Dialog.SpeakerName = Content;
			}
			else if (tag == "FULL_TEXT") {
				Dialog.FullText = Content;
			}
			else if (tag == "AUDIO_DURATION") {
				float DialogDuration = FCString::Atof(*Content);
				Dialog.AudioDuration = DialogDuration;
			}
			else if (tag == "RELATIVE_AUIDO_URL") {
				Dialog.RelativeAudioURL = Content;
			}
			else if (tag == "RESPONSES") {
				// Load Responses
				TArray<FDialogResponseData> DialogResponses;
				TArray<FXmlNode*> ResponseNodes = childNode->GetChildrenNodes();
				// Iterate through each response
				for (FXmlNode* ChoiceNode : ResponseNodes) {
					TArray<FXmlNode*> ChoiceDataNodes = ChoiceNode->GetChildrenNodes();
					// Create the response and populate data
					struct FDialogResponseData DialogResponse;
					for (FXmlNode* ChoiceChildNode : ChoiceDataNodes) {
						FString ChoiceTag = ChoiceChildNode->GetTag();
						FString ChoiceContent = ChoiceChildNode->GetContent();

						if (ChoiceTag == "PREVIEW") {
							DialogResponse.PreviewText = ChoiceContent;
						}
						else if (ChoiceTag == "ID") {
							int FullDialogID = FCString::Atoi(*ChoiceContent);
							DialogResponse.DialogFullID = FullDialogID;
						}
					}
					DialogResponses.Emplace(DialogResponse);
				}
				Dialog.ResponseChoices = DialogResponses;
			}
		}
		// Add to global dialog
		AllDialog.EmplaceAt(Dialog.ID, Dialog);
	}
}

void UDialogController::StartDialog(int StartingID)
{
	if (!bIsDialogActive) {
		PlayDialogue(StartingID);
		bIsDialogActive = true;
	}
}

void UDialogController::DialogOption1Selected()
{
	if (bIsDialogActive && Choice1ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 1 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking
		GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);

		PlayDialogue(Choice1ID);
	}
}

void UDialogController::DialogOption2Selected()
{
	if (bIsDialogActive && Choice2ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 2 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking
		GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);

		PlayDialogue(Choice2ID);
	}
}

void UDialogController::DialogOption3Selected()
{
	if (bIsDialogActive && Choice3ID != -1 /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 3 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking
		GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);

		PlayDialogue(Choice3ID);
	}
}

void UDialogController::DialogOption4Selected()
{
	if (bIsDialogActive && Choice4ID != -1  /* && bCanInteractWithInterface*/) {
		UE_LOG(LogTemp, Warning, TEXT("Dialog Option 4 Selected"));
		// Add any changes to characters variables (from choice)
		// Should probably implement an OnDeck (or stack) for resposnes that have been selected before the NPC finsihes talking
		GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);

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
	if (!AllDialog.IsValidIndex(ID)) {
		UE_LOG(LogTemp, Error, TEXT("%d is not a valid dialog id!"));
		return;
	}
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
			//Wait for this to end, then run next dialog (from only option)
			//  Set Timer with delegate to pass paramter into PlayDialog for next Dialog ID
			FTimerDelegate TimerDelegate;
			FDialogResponseData* OnlyResponse = &(*DialogResponseChoices)[0];
			TimerDelegate.BindUObject(this, &UDialogController::PlayDialogue, OnlyResponse->DialogFullID);
			GetWorld()->GetTimerManager().SetTimer(DialogTimer, TimerDelegate, CurrentDialog->AudioDuration, false, -1.0f);

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

			// TIMEOUT, if player takes too long, Set Timer with delegate to pass paramter into PlayDialog for next Dialog ID
			// Need to load this from the file now
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUObject(this, &UDialogController::PlayDialogue, 6);
			GetWorld()->GetTimerManager().SetTimer(TimeoutTimer, TimerDelegate, CurrentDialog->AudioDuration + 3.f, false, -1.0f);
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