#include "stdafx.h"

#include <Scene\Drawable\Mesh.h>
#include <Utils\StringHelper.h>

bool Mesh::Initialize(HWND hwnd, ID3D11Device* d3dDevice, IEffectFactory* effectFactory, const wchar_t* filename)
{
	if (StringHelper::endWith(filename, L".sdkmesh")) {
		InitializeSDKMeshes(d3dDevice, filename, effectFactory);
	}
	else {
		MessageBox(hwnd, L"Could not initialize the mesh object, format not supported", L"Error", MB_OK);
	}
	m_states = std::make_unique<CommonStates>(d3dDevice);
	return true;
}

void Mesh::Render(ID3D11DeviceContext* deviceContext, const std::function<void()>& draw)
{
	unsigned int stride;
	unsigned int offset;
	//m_model->Draw(deviceContext, *m_states, model, view, proj);

	
	std::vector<std::shared_ptr<ModelMesh>> meshes = m_model->meshes;
	for (shared_ptr<ModelMesh> mesh : meshes) {
		std::vector<std::unique_ptr<ModelMeshPart>>& mesh_parts = mesh->meshParts;
		for (unsigned int i = 0; i < mesh_parts.size(); i++) {
			ModelMeshPart* part = mesh_parts.at(i).get();
			stride = part->vertexStride;
			offset = part->vertexOffset;
			// Set the vertex buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetVertexBuffers(0, 1, part->vertexBuffer.GetAddressOf(), &stride, &offset);

			// Set the index buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetIndexBuffer(part->indexBuffer.Get(), part->indexFormat, 0);

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			deviceContext->IASetPrimitiveTopology(part->primitiveType);

			draw();
		}
	}
	
}

int Mesh::GetIndexCount() const
{
	return m_model->meshes[0]->meshParts.at(0)->indexCount;
}