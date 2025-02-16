// Fill out your copyright notice in the Description page of Project Settings.
#include "SpoutReceiver.h"

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

void USpoutReceiver::ChangeFrameRate(const double FrameRate)
{
	if (!TickProvider) return;
	TickProvider->SetFPS(FrameRate);
}

bool USpoutReceiver::Connect(
	const FString Name,
	UTextureRenderTarget2D* Output,
	const double FrameRate)
{
	if (!Output) return false;
	if (bIsReceiving) StopReceive();

	bIsReceiving = Receiver->Connect(Name, Output);

	if (bIsReceiving)
	{
		TickProvider = new FTickProvider(FrameRate);
		TickProvider->Tick.AddUObject(this, &USpoutReceiver::TickThread);
	}
	
	return bIsReceiving;
}

void USpoutReceiver::StopReceive()
{
	if (TickProvider) delete TickProvider;
	TickProvider = nullptr;
	bIsReceiving = false;
}

void USpoutReceiver::TickThread() const
{
	if (!Receiver.IsValid() || !bIsReceiving) return;
	Receiver->ReceiveCurrentFrame();
}
