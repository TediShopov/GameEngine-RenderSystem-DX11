#pragma once
// Texture
// Loads and stores a texture ready for rendering.
// Handles mipmap generation on load.

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <d3d11.h>
//#include "../DirectXTK/Inc/DDSTextureLoader.h"
//#include "../DirectXTK/Inc/WICTextureLoader.h"
#include "DTK\include\DDSTextureLoader.h"
#include "DTK\include\WICTextureLoader.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <mutex>
//#include "Texture.h"

using namespace DirectX;

class TextureManager
{
public:
	TextureManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TextureManager();

	void loadTexture(std::wstring uid, std::wstring filename);
	ID3D11ShaderResourceView* getTexture(std::wstring uid);
	ID3D11Resource* getTextureResource(std::wstring uid);
	std::map<std::wstring, ID3D11ShaderResourceView*> textureMap;
	std::map<std::wstring, ID3D11Resource*> textureResourceMap;
	//Maps the name/alias of the texture to its filepath
	std::map<std::wstring, std::wstring> textureAliasMap;
	void addDefaultTexture();

private:
	std::mutex textureResourceMutex;
	bool does_file_exist(std::wstring fileName);
	void generateTexture(ID3D11Device* device);

	//ID3D11ShaderResourceView* texture;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	ID3D11Texture2D *pTexture;
};

#endif