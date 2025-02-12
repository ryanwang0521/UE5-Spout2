#include "SpoutSender.h"
#include "Engine/TextureRenderTarget2D.h"

void USpoutSender::BeginDestroy()
{
	Super::BeginDestroy();
	delete TickProvider;
	Sender.Reset();
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
	Sender.Reset();
	Sender = nullptr;
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
