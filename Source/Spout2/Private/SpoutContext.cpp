#include "SpoutContext.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SpoutCopyShader.h"
#include "RenderGraphUtils.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <d3d11on12.h>
#include <d3d11.h>
#include "SpoutDX.h"
#include "Windows/HideWindowsPlatformTypes.h"

//////////////////////////////////////////////////////////////////////////
///Spout Context
//////////////////////////////////////////////////////////////////////////
struct SpoutContext {

	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11Device* D3D11Device = nullptr;
	ID3D11On12Device* D3D11on12Device = nullptr;
	spoutDX Spout;

private:

	void InitSpout()
	{
		const FString RHIName = GDynamicRHI->GetName();

		if (RHIName == TEXT("D3D11"))
		{
			D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
			D3D11Device->GetImmediateContext(&DeviceContext);
		}
		else if (RHIName == TEXT("D3D12"))
		{
			ID3D12Device* Device12 = static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
			UINT DeviceFlags11 = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

			verify(D3D11On12CreateDevice(
				Device12,
				DeviceFlags11,
				nullptr,
				0,
				nullptr,
				0,
				0,
				&D3D11Device,
				&DeviceContext,
				nullptr
			) == S_OK);

			verify(D3D11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&D3D11on12Device) == S_OK);
		}

		Spout.OpenDirectX11(D3D11Device);
	}

public:
	SpoutContext()
	{
		InitSpout();
	}

	~SpoutContext()
	{
		DisposeSpout();
	}

	void DisposeSpout()
	{
		if (DeviceContext)
		{
			DeviceContext->Release();
			DeviceContext = nullptr;
		}

		if (D3D11on12Device)
		{
			D3D11on12Device->Release();
			D3D11on12Device = nullptr;
		}

		if (D3D11Device)
		{
			D3D11Device->Release();
			D3D11Device = nullptr;
		}

		Spout.ReleaseSender();
		Spout.ReleaseReceiver();
		Spout.CloseDirectX11();
	}

	ID3D11Texture2D* GetD3D11Texture(const FTextureRHIRef& InTexture) const
	{
		if (!InTexture) return nullptr;
		const FString RHIName = GDynamicRHI->GetName();

		if (RHIName == TEXT("D3D11"))
		{
			return static_cast<ID3D11Texture2D*>(InTexture->GetNativeResource());
		}

		if (RHIName == TEXT("D3D12"))
		{
			if (!D3D11on12Device) return nullptr;
			ID3D12Resource* NativeTex = static_cast<ID3D12Resource*>(InTexture->GetNativeResource());
			if (!NativeTex) return nullptr; // Handle potential null resource
		
			ID3D11Resource* WrappedDX11Resource = nullptr;
			D3D11_RESOURCE_FLAGS Flags = {};
			//Flags.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			//Flags.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
			
			HRESULT hr = D3D11on12Device->CreateWrappedResource(
				NativeTex,
				&Flags,
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_COPY_DEST, __uuidof(ID3D11Resource),
				(void**)&WrappedDX11Resource) == S_OK;

			if (FAILED(hr))
			{
				NativeTex->Release();
				return nullptr;
			}

			return static_cast<ID3D11Texture2D*>(WrappedDX11Resource);
		}

		return nullptr;
	}

	static EPixelFormat ToEPixelFormat(DXGI_FORMAT DXGIFormat)
	{
		switch (DXGIFormat)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:		return PF_R8G8B8A8;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return PF_R8G8B8A8;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:		return PF_R8G8B8A8;
		case DXGI_FORMAT_B8G8R8A8_UNORM:		return PF_B8G8R8A8;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:	return PF_B8G8R8A8;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:		return PF_B8G8R8A8;
		case DXGI_FORMAT_R10G10B10A2_UNORM:		return PF_A2B10G10R10;
		case DXGI_FORMAT_R11G11B10_FLOAT:		return PF_FloatRGB;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:	return PF_FloatRGBA;
		case DXGI_FORMAT_R16G16B16A16_UNORM:	return PF_A16B16G16R16;
		//case DXGI_FORMAT_R32G32B32A32_FLOAT:	return PF_A32B32G32R32F;
		default:								return PF_Unknown;
		}
	}

	static DXGI_FORMAT ToDXGIFormat(EPixelFormat Format)
	{
		switch (Format)
		{
			case PF_R8G8B8A8:		return DXGI_FORMAT_R8G8B8A8_UNORM;
			case PF_A2B10G10R10:	return DXGI_FORMAT_R10G10B10A2_UNORM;
			case PF_FloatRGB:		return DXGI_FORMAT_R11G11B10_FLOAT;
			case PF_FloatRGBA:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case PF_A16B16G16R16:	return DXGI_FORMAT_R16G16B16A16_UNORM;
			default:				return DXGI_FORMAT_UNKNOWN;
		}
	}
};

/////////////////////////////////////////////////////////////////////////
///Spout Sender
/////////////////////////////////////////////////////////////////////////

SpoutSender::SpoutSender()
{
	Context = MakeShared<SpoutContext, ESPMode::ThreadSafe>();
	Format = PF_Unknown;
	Height = Width = 0;
	Source = nullptr;
}

SpoutSender::~SpoutSender() {
	Context.Reset();
}

void SpoutSender::SetSenderName(const FString& newName)
{
	Context->Spout.SetSenderName(toStr(newName));
}

bool SpoutSender::SetSenderTexture(UTextureRenderTarget2D* Texture, int32 InBufferCount)
{
	// Ensure all pending render commands (using old buffers) are finished
	FlushRenderingCommands();

	ResetInfo();

	BufferCount = FMath::Max(1, InBufferCount);
	BufferIndex = 0;
	
	ENQUEUE_RENDER_COMMAND(SpoutSend)
	([this, Texture](FRHICommandListImmediate& RHICmd)
	{
		OutputTexture = Texture->GetRenderTargetResource()->GetRenderTargetTexture();
		
		for (int32 i = 0; i < BufferCount; i++)
		{
			FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create2D(
				TEXT("TextureBuffer"),
				Texture->SizeX,
				Texture->SizeY,
				Texture->GetFormat());
			Desc.AddFlags(ETextureCreateFlags::UAV);

			FTextureRHIRef NewBuffer = RHICreateTexture(Desc);
			OutputTextureBuffers.Add(NewBuffer);

			ID3D11Texture2D* NewBufferD3D = Context->GetD3D11Texture(NewBuffer);
			OutputTextureBuffersD3D.Add(NewBufferD3D);
		}

		DXGI_FORMAT DXGIFormat = Context->ToDXGIFormat(Texture->GetFormat());

		Context->Spout.SetSenderFormat(DXGIFormat);
		Format = Texture->GetFormat();
		Width = Texture->SizeX;
		Height = Texture->SizeY;
		Source = Texture;
	});

	FlushRenderingCommands();

	if (OutputTextureBuffersD3D.Num() != BufferCount || OutputTextureBuffersD3D.Contains(nullptr))
	{
		ResetInfo();
		return false;
	}
	return true;
}

bool SpoutSender::CheckOutputInfoChanged() const
{
	bool bIsChanged =
		Source->GetFormat() != Format
		|| Source->SizeX	!= Width
		|| Source->SizeY	!= Height;
	return bIsChanged;
}

void SpoutSender::ResetInfo()
{
	Source = nullptr;
	OutputTexture = nullptr;
	OutputTextureBuffers.Empty();

	for (ID3D11Texture2D* BufferD3D : OutputTextureBuffersD3D)
	{
		if (BufferD3D)
		{
			BufferD3D->Release();
		}
	}
	OutputTextureBuffersD3D.Empty();

	Format = PF_Unknown;
	Width = Height = 0;
}

void SpoutSender::SendCurrentFrame()
{
	if (!Context->DeviceContext
		|| !OutputTexture.IsValid()
		|| OutputTextureBuffers.Num() == 0
		|| OutputTextureBuffersD3D.Num() == 0) return;

	if (CheckOutputInfoChanged())
	{
		SetSenderTexture(Source, BufferCount);
		return;
	}

	ENQUEUE_RENDER_COMMAND(SpoutSend)([this](FRHICommandListImmediate& RHICmd)
	{
		if (!Context.IsValid()) return;
		
		//Avoid directly using the source texture
		//Engine may update the texture during the spout sending process
		FTextureRHIRef CurrentBuffer = OutputTextureBuffers[BufferIndex];
		ID3D11Texture2D* CurrentBufferD3D = OutputTextureBuffersD3D[BufferIndex];

		// Dispatch Compute Shader for Copy
		FSpoutCopyCS::FParameters PassParameters;
		PassParameters.InputTexture = OutputTexture;

		FUnorderedAccessViewRHIRef CurrentBufferUAV = RHICmd.CreateUnorderedAccessView(CurrentBuffer);
		PassParameters.OutputTexture = CurrentBufferUAV;

		RHICmd.Transition(FRHITransitionInfo(OutputTexture.GetReference(), ERHIAccess::Unknown, ERHIAccess::SRVCompute));
		RHICmd.Transition(FRHITransitionInfo(CurrentBuffer.GetReference(), ERHIAccess::Unknown, ERHIAccess::UAVCompute));

		TShaderMapRef<FSpoutCopyCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		FComputeShaderUtils::Dispatch(RHICmd, ComputeShader, PassParameters,
			FIntVector(FMath::DivideAndRoundUp(CurrentBuffer->GetSizeX(), 8u),
					   FMath::DivideAndRoundUp(CurrentBuffer->GetSizeY(), 8u),
					   1));

		RHICmd.Transition(FRHITransitionInfo(CurrentBuffer.GetReference(), ERHIAccess::UAVCompute, ERHIAccess::SRVCompute));

		Context->Spout.SendTexture(CurrentBufferD3D);

		BufferIndex = (BufferIndex + 1) % BufferCount;
	});
}

/////////////////////////////////////////////////////////////////////////
///Spout Receiver
/////////////////////////////////////////////////////////////////////////

SpoutReceiver::SpoutReceiver()
{
	Width = Height = 0;
	Format = DXGI_FORMAT_UNKNOWN;
	Context = MakeShared<SpoutContext, ESPMode::ThreadSafe>();
	//Context->Spout.EnableSpoutLog();
}

SpoutReceiver::~SpoutReceiver()
{
	Context.Reset();
}

TArray<FString> SpoutReceiver::GetList() const
{
	int SenderCount = Context->Spout.GetSenderCount();
	TArray<FString> Result = {};
	if (SenderCount < 1) return Result;

	char sendername[256]{};
	for (int i = 0; i < SenderCount; i++)
	{
		if (Context->Spout.GetSender(i, sendername))
		{
			Result.Add(FString(sendername));
		}
	}

	return Result;
}

bool SpoutReceiver::Connect(const FString& Name, UTextureRenderTarget2D* BindTexture)
{
	if (Name.Len() >= 256)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10,
			FColor::Green, TEXT("Sender Name too long"));
		return false;
	}
	
	char NameChar[256]{};
	strcpy_s(NameChar, 256, TCHAR_TO_ANSI(*Name));

	TargetTexture = BindTexture;
	Context->Spout.SetReceiverName(NameChar);
	Context->Spout.ReceiveTexture();

	bool bSuccess = UpdateSpoutInfo();
	if (!bSuccess) Context->Spout.SetReceiverName();

	return bSuccess;
}

bool SpoutReceiver::UpdateSpoutInfo()
{
	if (!TargetTexture) return false;
	spoutDX* Spout = &Context->Spout;
	if (!Spout || !Spout->IsConnected()) return false;

	int32 newWidth = Spout->GetSenderWidth();
	int32 newHeight = Spout->GetSenderHeight();
	DXGI_FORMAT newFormat = Spout->GetSenderFormat();
	EPixelFormat newEPixelFormat = Context->ToEPixelFormat(newFormat);
	
	if (newWidth < 1
		|| newHeight < 1
		|| newEPixelFormat == PF_Unknown) return false;
	
	if(newWidth != Width
	|| newHeight != Height
	|| newFormat != Format)
	{
		TargetTexture->ResizeTarget(newWidth, newHeight);
		TargetTexture->OverrideFormat = newEPixelFormat;
		TargetTexture->UpdateResource();
		
		ID3D11Texture2D* newTextureD3D = nullptr;

		ENQUEUE_RENDER_COMMAND(SpoutRecv)
		([this, &newTextureD3D]
		(FRHICommandListImmediate& RHICmd)
		{
			FTextureRHIRef RHI = TargetTexture->GetResource()->GetTextureRHI();
			newTextureD3D = Context->GetD3D11Texture(RHI);
		});

		//Wait Rendering Thread get RHI
		FlushRenderingCommands();
		if (!newTextureD3D) return false;

		Width = newWidth;
		Height = newHeight;
		Format = newFormat;
		TargetTextureD3D = newTextureD3D;
	}
	return true;
}

void SpoutReceiver::ReceiveCurrentFrame()
{
	if (!TargetTexture || !TargetTextureD3D || !Context.IsValid()) return;

	if (Context->Spout.ReceiveTexture(&TargetTextureD3D)){
		if (Context->Spout.IsUpdated()) UpdateSpoutInfo();
	}
}

