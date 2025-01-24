// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpoutContext.h"
#include "CoreMinimal.h"
#include "TickProvider.h"
#include "UObject/Object.h"
#include "SpoutReceiver.generated.h"

UCLASS(ClassGroup = (Spout), meta = (BlueprintSpawnableComponent))
class SPOUT2_API USpoutReceiver : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="SpoutReceiver")
	TArray<FString> GetList() const;

	UFUNCTION(BlueprintPure, Category="SpoutReceiver")
	bool GetFirstSource(FString& Name) const;

	UFUNCTION(BlueprintCallable, Category="SpoutReceiver")
	bool Connect(const FString Name, UTextureRenderTarget2D* Output, const double FrameRate);

	UFUNCTION(BlueprintCallable, Category="SpoutReceiver")
	void StopReceive();

	UFUNCTION(BlueprintCallable, Category = "SpoutReceiver")
	void ChangeFrameRate(double value);


private:
	TSharedPtr<SpoutReceiver> Receiver;
	FTickProvider* TickProvider;
	bool bIsReceiving = false;
	void Tick() const;
};
