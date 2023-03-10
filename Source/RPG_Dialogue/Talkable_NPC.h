// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Talkable_NPC.generated.h"

UCLASS()
class RPG_DIALOGUE_API ATalkable_NPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATalkable_NPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/* Widget prompting player to talk when looking at NPC */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Items", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* TalkPromptWidget;

	/* Player can talk when in this area */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Items", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TalkableAreaSphere;

	/* True when the player is in the interact sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact Items", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerInInteractSphere;

	/* True when the player can interact with this NPC */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact Items", meta = (AllowPrivateAccess = "true"))
	bool bCanPlayerInteractWith;

	/* The ID of the Dialogue to be used when starting a conversation/interaction with this NPC */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Items", meta = (AllowPrivateAccess = "true"))
	int StartingDialogID;

public:
	FORCEINLINE UWidgetComponent* GetInteractPromptWidget() const { return TalkPromptWidget; }

	FORCEINLINE int GetStartingDialogID() const { return StartingDialogID; }

	FORCEINLINE void SetCanInteractWithFlag(bool canInteractWith) { bCanPlayerInteractWith = canInteractWith; }

};
