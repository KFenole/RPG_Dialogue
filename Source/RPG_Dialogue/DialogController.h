// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogController.generated.h"

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

	UFUNCTION()
	void TestFunction();

	void StartDialog(int ID);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
	/* Widget Containing Dialog UI*/
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	//class UWidgetComponent* DialogUIWidget;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimeoutTimer;

//public:
	//FORCEINLINE UWidgetComponent* GetDialogWidget() const { return DialogUIWidget; }

	//void SayHello();

};
