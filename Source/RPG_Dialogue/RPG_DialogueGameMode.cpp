// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPG_DialogueGameMode.h"
#include "RPG_DialogueCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPG_DialogueGameMode::ARPG_DialogueGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
