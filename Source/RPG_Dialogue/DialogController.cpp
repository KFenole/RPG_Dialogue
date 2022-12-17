// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogController.h"

#include "Components/WidgetComponent.h"

// Sets default values
UDialogController::UDialogController()
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

	//if (DialogUIWidget)
		//DialogUIWidget->SetVisibility(true);
	
	//GetWorldTimerManager().SetTimer(TimeoutTimer, this, &UDialogController::TestFunction, 6.f, false, -1.0f);

	
}

void UDialogController::TestFunction() {
	//DisplayName = FString(TEXT("Test Name"));

	return;
}

void UDialogController::StartDialog(int ID)
{
	//if (DialogUIWidget)
		//DialogUIWidget->SetVisibility(true);
}

/*
void UDialogController::SayHello() {
	UE_LOG(LogTemp, Warning, TEXT("HELLO THERE"));
}
*/
/*
// Called every frame
void UDialogController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/



