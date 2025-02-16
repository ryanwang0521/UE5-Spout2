#include "SpoutSender.h"
#include "Engine/TextureRenderTarget2D.h"

void USpoutSender::BeginDestroy()
{
	Stop();
	Super::BeginDestroy();
}

bool USpoutSender::Start(
	const FString& SenderName,
	UTextureRenderTarget2D* Source,
	const double FrameRate)
{
	if (!Source) return false;
	if (bIsInitialized || Sender.IsValid()) Stop();

	Sender = MakeShared<SpoutSender, ESPMode::ThreadSafe>();
	
	Sender->SetSenderName(SenderName);
	bIsInitialized = Sender->SetSenderTexture(Source);

	if (bIsInitialized)
	{
		TickProvider = new FTickProvider(FrameRate);
		TickProvider->Tick.AddUObject(this, &USpoutSender::TickThread);

	}
	
	return bIsInitialized;
}

void USpoutSender::Stop()
{
	if (TickProvider) delete TickProvider;
	if (Sender.IsValid()) Sender.Reset();
	Sender = nullptr;
	TickProvider = nullptr;
	bIsInitialized = false;
}

void USpoutSender::ChangeFrameRate(const double FrameRate)
{
	if (!TickProvider) return;
	TickProvider->SetFPS(FrameRate);
}

void USpoutSender::TickThread() const
{
	if (!Sender.IsValid() || !bIsInitialized) return;
	Sender->SendCurrentFrame();
}
