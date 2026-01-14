#include "SpoutCopyShader.h"
#include "ShaderParameterUtils.h"

IMPLEMENT_GLOBAL_SHADER(FSpoutCopyCS, "/Plugin/Spout2/Private/SpoutCopy.usf", "MainCS", SF_Compute);
