#pragma once

#include <spine/spine.h>
#include <base/buffer/gpu_buffer.h>
#include <base/proxy/render_proxy.h>
#include "base/texture/texture.h"

extern "C"
{
	typedef struct EchoAttachmentLoader
	{
		spAttachmentLoader		 super;
		spAtlasAttachmentLoader* atlasAttachmentLoader;
	} EchoAttachmentLoader;

	/* The EchoAttachmentLoader must not be disposed until after the skeleton data has been disposed. */
	EchoAttachmentLoader* EchoAttachmentLoader_create(spAtlas* atlas);
}

namespace Echo
{
	struct SpineVertexFormat
	{
		Vector3	m_position;		// λ��
		ui32	m_diffuse;		// ��ɫֵ
		Vector2	m_uv;			// UV����
	};

	/**
	* Spine AttachmentVertices
	*/
	class AttachmentVertices
	{
	public:
		AttachmentVertices();
		AttachmentVertices(Texture* texture, int verticesCount, ui16* triangles, int indicesCount);
		virtual ~AttachmentVertices();

		// render
		void merge(const AttachmentVertices& other);

		// clear
		void clear();

	public:
		TexturePtr						m_texture;			// ����
		vector<SpineVertexFormat>::type	m_verticesData;		// ��������
		vector<ui16>::type				m_indicesData;		// ��������
	};
}
