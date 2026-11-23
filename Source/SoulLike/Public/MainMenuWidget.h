// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class SOULLIKE_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Name of the gameplay level to load when Start is clicked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|MainMenu")
	FName GameLevelName = FName("Demo_Necropole");

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnQuitClicked();
};