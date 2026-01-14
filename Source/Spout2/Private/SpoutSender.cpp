#include "SpoutSender.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SpoutCopyShader.h"

USpoutSender::USpoutSender()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// Tick after update work to capture the latest frame content
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void USpoutSender::BeginDestroy()
{
	Stop();
	Super::BeginDestroy();
}

void USpoutSender::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Sender.IsValid() || !bIsInitialized) return;
	Sender->SendCurrentFrame();
}

bool USpoutSender::Start(
	const FString& SenderName,
	UTextureRenderTarget2D* Source,
	int32 BufferCount)
{
	//if (!Source) return false;
	//if (bIsInitialized || Sender.IsValid()) Stop();
	if (!Source || bIsInitialized || Sender.IsValid()) return false;

	Sender = MakeShared<SpoutSender, ESPMode::ThreadSafe>();
	
	Sender->SetSenderName(SenderName);
	bIsInitialized = Sender->SetSenderTexture(Source, BufferCount);
	
	return bIsInitialized;
}

void USpoutSender::Stop()
{
	if (Sender.IsValid()) Sender.Reset();
	Sender = nullptr;
	bIsInitialized = false;
}
