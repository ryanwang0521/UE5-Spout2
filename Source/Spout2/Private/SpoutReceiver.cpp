// Fill out your copyright notice in the Description page of Project Settings.
#include "SpoutReceiver.h"

USpoutReceiver::USpoutReceiver()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics; // Receive early
}

void USpoutReceiver::BeginDestroy()
{
	if (Receiver.IsValid()) Receiver.Reset();
	Receiver = nullptr;
	Super::BeginDestroy();
}

void USpoutReceiver::BeginPlay()
{
	Super::BeginPlay();
	Receiver = MakeShared<SpoutReceiver>();
}

void USpoutReceiver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Receiver.IsValid() || !bIsReceiving) return;
	Receiver->ReceiveCurrentFrame();
}

TArray<FString> USpoutReceiver::GetList() const
{
	if (!Receiver.IsValid()) return TArray<FString>();
	return Receiver->GetList();
}

bool USpoutReceiver::GetFirstSource(FString& Name) const
{
	TArray<FString> List = GetList();
	bool bSuccess = !List.IsEmpty();
	if (bSuccess) Name = List[0];
	return bSuccess;
}

bool USpoutReceiver::Connect(
	const FString Name,
	UTextureRenderTarget2D* Output)
{
	if (!Output) return false;
	if (bIsReceiving) StopReceive();

	bIsReceiving = Receiver->Connect(Name, Output);
	
	return bIsReceiving;
}

void USpoutReceiver::StopReceive()
{
	bIsReceiving = false;
}
