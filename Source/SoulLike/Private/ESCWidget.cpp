// Fill out your copyright notice in the Description page of Project Settings.

#include "ESCWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UESCWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button click events safely (RemoveDynamic prevents duplicate bindings)
	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(this, &UESCWidget::OnResumeClicked);
		ResumeButton->OnClicked.AddDynamic(this, &UESCWidget::OnResumeClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(this, &UESCWidget::OnMainMenuClicked);
		MainMenuButton->OnClicked.AddDynamic(this, &UESCWidget::OnMainMenuClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UESCWidget::OnQuitClicked);
		QuitButton->OnClicked.AddDynamic(this, &UESCWidget::OnQuitClicked);
	}
}

void UESCWidget::TogglePauseMenu()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
	if (!PC) return;

	if (IsInViewport())
	{
		// Close menu and resume game
		OnResumeClicked();
	}
	else
	{
		// Show menu on top of HUD (ZOrder = 10)
		AddToViewport(10);

		// Pause game time
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		// Show mouse cursor and allow UI button clicks
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void UESCWidget::OnResumeClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	// 1. Remove pause widget from screen
	RemoveFromParent();

	// 2. Unpause game time
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// 3. Hide mouse cursor and return control to character
	if (PC)
	{
		PC->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void UESCWidget::OnMainMenuClicked()
{
	// Always unpause before opening another level
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}

void UESCWidget::OnQuitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
}