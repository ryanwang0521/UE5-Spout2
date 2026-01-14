#pragma once

struct SpoutContext;
enum DXGI_FORMAT : int;
class ID3D11Texture2D;
class FWarppedTexture;

struct SpoutSender {
	
private:
	int32 Width, Height;
	EPixelFormat Format;
	
	TSharedPtr<SpoutContext> Context;
	UTextureRenderTarget2D* Source;
	FTextureRHIRef OutputTexture = nullptr;
	TArray<FTextureRHIRef> OutputTextureBuffers;
	TArray<ID3D11Texture2D*> OutputTextureBuffersD3D;
	int32 BufferIndex = 0;
	int32 BufferCount = 1;
	
	double NextFrameTime = 0;
	
	char* toStr(FString fstring) { return TCHAR_TO_ANSI(*fstring); }
	bool CheckOutputInfoChanged() const;
	void ResetInfo();
	
public:
	SpoutSender();
	~SpoutSender();

	void SetSenderName(const FString& newName);
	bool SetSenderTexture(UTextureRenderTarget2D* Texture, int32 BufferCount = 1);
	void SendCurrentFrame();
};

struct SpoutReceiver
{

private:
	TSharedPtr<SpoutContext> Context;
	UTextureRenderTarget2D* TargetTexture = nullptr;
	ID3D11Texture2D* TargetTextureD3D = nullptr;
	
	FString ConnectedName = "";
	int32 Width, Height;
	DXGI_FORMAT Format;
	
	double NextFrameTime = 0;

	bool UpdateSpoutInfo();
	
public:
	SpoutReceiver();
	~SpoutReceiver();
	
	TArray<FString> GetList() const;
	bool Connect(const FString& ConnectedName, UTextureRenderTarget2D* BindTexture);
	void ReceiveCurrentFrame();
};
