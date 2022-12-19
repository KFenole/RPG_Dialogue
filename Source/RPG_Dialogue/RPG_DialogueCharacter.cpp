// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPG_DialogueCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Talkable_NPC.h"
#include "DialogController.h"

#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"


//////////////////////////////////////////////////////////////////////////
// ARPG_DialogueCharacter

ARPG_DialogueCharacter::ARPG_DialogueCharacter():
	bIsInInteraction(false)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	/* Setup the DialogUIWidget for code */
	DialogUIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialogUIWidget"));
	DialogUIWidget->SetupAttachment(GetRootComponent());

	CharacterDialogController = CreateDefaultSubobject<UDialogController>(TEXT("DialogController"));
	//CharacterDialogController->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ARPG_DialogueCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DialogUIWidget)
		DialogUIWidget->SetVisibility(false);



}

void ARPG_DialogueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForInteractables();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARPG_DialogueCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ARPG_DialogueCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ARPG_DialogueCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ARPG_DialogueCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ARPG_DialogueCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPG_DialogueCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPG_DialogueCharacter::TouchStopped);

	// Interact
	//PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARPG_DialogueCharacter::Interact);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Released, this, &ARPG_DialogueCharacter::Interact);
}

void ARPG_DialogueCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ARPG_DialogueCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ARPG_DialogueCharacter::TurnAtRate(float Rate)
{
	if (!bIsInInteraction) {
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	}
}

void ARPG_DialogueCharacter::LookUpAtRate(float Rate)
{
	if (!bIsInInteraction) {
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	}
}

void ARPG_DialogueCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsInInteraction)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARPG_DialogueCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f ) && !bIsInInteraction)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		
	}
}

void ARPG_DialogueCharacter::IncrementOverlappedInteractablesCount(int8 Value)
{
	if (OverlappedInteractablesCount + Value <= 0) {
		OverlappedInteractablesCount = 0;
		bShouldTraceForInteractables = false;
	}
	else {
		OverlappedInteractablesCount += Value;
		bShouldTraceForInteractables = true;
	}
}


void ARPG_DialogueCharacter::TraceForInteractables()
{
	if (bShouldTraceForInteractables) {
		FHitResult TraceResult;
		FVector HitLocation;
		TraceFromCharacter(TraceResult, HitLocation);

		if (TraceResult.bBlockingHit) {
			ATalkable_NPC* HitInteractable = Cast<ATalkable_NPC>(TraceResult.GetActor());
			if (HitInteractable /* && HitInteractable->GetDialogWidget()*/) {
				//HitInteractable->GetWidget()->SetVisibility(true);
				
				
				//CharacterDialogController->SayHello();
			}

			
			// We hit an Interactable last frame
			if (TraceHitInteractableLastFrame) {
				if (HitInteractable != TraceHitInteractableLastFrame) {
					// We are hitting a different Interactable this frame from last frame, or Interactable is null
					//TraceHitInteractableLastFrame->GetWidget()->SetVisibility(false);
					//DialogUIWidget->SetVisibility(true);
				}
			}
			// Store reference to HitItem for next frame
			TraceHitInteractableLastFrame = HitInteractable;
		}

	} else if (TraceHitInteractableLastFrame) {
		//
	}
}

bool ARPG_DialogueCharacter::TraceFromViewport(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get size of viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get location of center of screen
	FVector2D CenterLocationOnScreen(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CenterScreenWorldPosition;
	FVector CenterScreenWorldDirection;

	// Project Screen to World
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CenterLocationOnScreen,
		CenterScreenWorldPosition,
		CenterScreenWorldDirection);

	if (bScreenToWorld) {
		// Trace from Crosshair world location outward
		const FVector Start{ CenterScreenWorldPosition };
		const FVector End{ Start + CenterScreenWorldDirection * 500.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit) {
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

bool ARPG_DialogueCharacter::TraceFromCharacter(FHitResult& OutHitResult, FVector& OutHitLocation)
{	
	FVector CharacterLocation = GetActorLocation();
	FVector CharacterForwardDirection =  GetActorForwardVector();

	FVector Start = CharacterLocation;
	FVector End = Start + CharacterForwardDirection * 500.f;

	GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.5f);
	//UE_LOG(LogTemp, Warning, TEXT("Drawing...%d, %d, %d"), int8(End.X), OverlappedInteractablesCount, bShouldTraceForInteractables);

	if (OutHitResult.bBlockingHit) {
		OutHitLocation = OutHitResult.Location;
		return true;
	}

	return false;
}

void ARPG_DialogueCharacter::Interact()
{
	// Check if there is an item we can interact with
	if (IsValid(TraceHitInteractableLastFrame) && TraceHitInteractableLastFrame) {
		// Display the Dialog UI
		if (DialogUIWidget)
			DialogUIWidget->SetVisibility(true);

		// Tell the DialogController to start dialog session
		if (CharacterDialogController && IsValid(CharacterDialogController)) {
			CharacterDialogController->StartDialog(0);
			bIsInInteraction = true;
			//return true;
		}
	}

}

void ARPG_DialogueCharacter::EndInteraction()
{
	bIsInInteraction = false;
	if (DialogUIWidget)
		DialogUIWidget->SetVisibility(false);

	// Relinquish camera and player control back to user
	//...
}