#pragma once
#include <fstream>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <d3dcompiler.h>

// Blobを返すユーティリティ関数です

Microsoft::WRL::ComPtr<ID3DBlob> LoadCSO(const std::wstring& filename);