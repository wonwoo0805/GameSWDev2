// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ESCWidget.generated.h"

class UButton;

UCLASS()
class SOULLIKE_API UESCWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Opens or closes the pause menu, toggles game pause and mouse cursor
	UFUNCTION(BlueprintCallable, Category = "UI|PauseMenu")
	void TogglePauseMenu();

	// Name of your Main Menu level asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|PauseMenu")
	FName MainMenuLevelName = FName("MainMenu");

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitClicked();
};