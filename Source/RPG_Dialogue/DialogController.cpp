// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogController.h"

#include "Components/WidgetComponent.h"

// Sets default values
ADialogController::ADialogController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Setup the DialogUIWidget for code */
	DialogUIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialogUIWidget"));
	DialogUIWidget->SetupAttachment(GetRootComponent());

	DisplayName = FString(TEXT("Someone's Name"));
	DisplaySubtitle = FString(TEXT("Something Interesting that they said."));



}

// Called when the game starts or when spawned
void ADialogController::BeginPlay()
{
	Super::BeginPlay();

	if (DialogUIWidget) {
		DialogUIWidget->SetVisibility(false);
	}
	//GetWorldTimerManager().SetTimer(TimeoutTimer, this, &ADialogController::TestFunction, 6.f, false, -1.0f);

	
}

void ADialogController::TestFunction() {
	DisplayName = FString(TEXT("Test Name"));

	return;
}

// Called every frame
void ADialogController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



