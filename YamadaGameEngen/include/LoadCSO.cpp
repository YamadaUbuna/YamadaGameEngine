#include "include\pch.h"
#include "include/LoadCSO.h"

// Blob��Ԃ����[�e�B���e�B�֐�
Microsoft::WRL::ComPtr<ID3DBlob> LoadCSO(const std::wstring& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // �t�@�C���𖖔�����J��
    if (!file)
    {
        throw std::runtime_error("Failed to open CSO file.");
    }

    std::streamsize size = file.tellg();  // �t�@�C���T�C�Y���擾
    file.seekg(0, std::ios::beg);         // �擪�ɖ߂�

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
