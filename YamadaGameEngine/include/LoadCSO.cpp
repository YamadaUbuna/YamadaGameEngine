#include "include\pch.h"
#include "include/LoadCSO.h"

// Blobを返すユーティリティ関数
Microsoft::WRL::ComPtr<ID3DBlob> LoadCSO(const std::wstring& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // ファイルを末尾から開く
    if (!file)
    {
        throw std::runtime_error("Failed to open CSO file.");
    }

    std::streamsize size = file.tellg();  // ファイルサイズを取得
    file.seekg(0, std::ios::beg);         // 先頭に戻る

    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    if (FAILED(D3DCreateBlob(size, &blob)))
    {
        throw std::runtime_error("Failed to create blob.");
    }

    if (!file.read(reinterpret_cast<char*>(blob->GetBufferPointer()), size))
    {
        throw std::runtime_error("Failed to read CSO file.");
    }

    return blob;
}
