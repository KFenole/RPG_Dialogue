// Fill out your copyright notice in the Description page of Project Settings.


#include "Talkable_NPC.h"

#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

#include "RPG_DialogueCharacter.h"

// Sets default values
ATalkable_NPC::ATalkable_NPC(): 
	StartingDialogID(0)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TalkPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TalkPromptWidget"));
	TalkPromptWidget->SetupAttachment(GetRootComponent());

	TalkableAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TalkableAreaSphere"));
	TalkableAreaSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ATalkable_NPC::BeginPlay()
{
	Super::BeginPlay();

	// Setup Overlap Functions for AreaSphere
	TalkableAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ATalkable_NPC::OnSphereOverlap);
	TalkableAreaSphere->OnComponentEndOverlap.AddDynamic(this, &ATalkable_NPC::OnSphereEndOverlap);
	
}

void ATalkable_NPC::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		ARPG_DialogueCharacter* PlayerCharacter = Cast<ARPG_DialogueCharacter>(OtherActor);
		if (PlayerCharacter) { // Player Character Overlapped
			PlayerCharacter->IncrementOverlappedInteractablesCount(1);
		}
	}
}

void ATalkable_NPC::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		ARPG_DialogueCharacter* PlayerCharacter = Cast<ARPG_DialogueCharacter>(OtherActor);
		if (PlayerCharacter) { // Player Character End Overlapped
			PlayerCharacter->IncrementOverlappedInteractablesCount(-1);
		}
	}
}

// Called every frame
void ATalkable_NPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATalkable_NPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

