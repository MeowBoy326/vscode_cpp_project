// opengl_renderer.hpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2016 Leigh Johnston
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <neogfx.hpp>
#include <set>
#include <map>
#include "opengl.hpp"
#include <neogfx_i_rendering_engine.hpp>
#include <neogfx_font_manager.hpp>
#include "opengl_texture_manager.hpp"

std::string glErrorString(GLenum aErrorCode);
GLenum glCheckError(const char* file, unsigned int line);

namespace neogfx
{
	namespace detail
	{
		class screen_metrics : public i_screen_metrics
		{
		public:
			struct unsupported_function : std::logic_error { unsupported_function() : std::logic_error("neogfx::detail::screen_metrics::unsupported_function") {} };
		public:
			screen_metrics();
		public:
			virtual dimension horizontal_dpi() const;
			virtual dimension vertical_dpi() const;
			virtual bool metrics_available() const;
			virtual size extents() const;
			virtual dimension em_size() const;
			virtual subpixel_format_e subpixel_format() const;
		private:
			size iPixelDensityDpi;
			subpixel_format_e iSubpixelFormat;
		};
	}

	class opengl_renderer : public i_rendering_engine
	{
	public:
		struct shader_program_error : i_rendering_engine::shader_program_error {
			shader_program_error(GLenum aErrorCode) :
				i_rendering_engine::shader_program_error(glErrorString(aErrorCode)) {}
		};
		struct failed_to_create_framebuffer : std::runtime_error {
			failed_to_create_framebuffer(GLenum aErrorCode) :
				std::runtime_error("neogfx::opengl_renderer::failed_to_create_framebuffer: Failed to create frame buffer, reason: " + glErrorString(aErrorCode)) {}
		};
	public:
		class shader_program : public i_shader_program
		{
		public:
			typedef std::map<std::string, GLuint> variable_map;
		public:
			shader_program(GLuint aHandle, bool aHasProjectionMatrix);
		public:
			void* handle() const override;
			bool has_projection_matrix() const override;
			void set_projection_matrix(const i_native_graphics_context& aGraphicsContext) override;
			void* variable(const std::string& aVariableName) const override;
			void set_uniform_variable(const std::string& aName, float aValue) override;
			void set_uniform_variable(const std::string& aName, double aValue) override;
			void set_uniform_variable(const std::string& aName, int aValue) override;
			void set_uniform_variable(const std::string& aName, float aValue1, float aValue2) override;
			void set_uniform_variable(const std::string& aName, double aValue1, double aValue2) override;
			void set_uniform_array(const std::string& aName, uint32_t aSize, const float* aArray) override;
			void set_uniform_matrix(const std::string& aName, const mat44& aMatrix) override;
		public:
			GLuint register_variable(const std::string& aVariableName);
		public:
			bool operator<(const shader_program& aRhs) const;
		private:
			GLint uniform_location(const std::string& aName);
		private:
			GLuint iHandle;
			bool iHasProjectionMatrix;
			std::pair<vec2, vec2> iLogicalCoordinates;
			variable_map iVariables;
		};
	private:
		typedef std::vector<std::pair<std::string, GLenum>> shaders;
		typedef std::list<shader_program> shader_programs;
	public:
		opengl_renderer(neogfx::renderer aRenderer);
		~opengl_renderer();
	public:
		virtual neogfx::renderer renderer() const;
		virtual void initialize();
		virtual const i_screen_metrics& screen_metrics() const;
		virtual i_font_manager& font_manager();
		virtual i_texture_manager& texture_manager();
		virtual bool shader_program_active() const;
		virtual void activate_shader_program(i_native_graphics_context& aGraphicsContext, i_shader_program& aProgram);
		virtual void deactivate_shader_program();
		virtual const i_shader_program& active_shader_program() const;
		virtual i_shader_program& active_shader_program();
		virtual const i_shader_program& default_shader_program() const;
		virtual i_shader_program& default_shader_program();
		virtual const i_shader_program& texture_shader_program() const;
		virtual i_shader_program& texture_shader_program();
		virtual const i_shader_program& monochrome_shader_program() const;
		virtual i_shader_program& monochrome_shader_program();
		virtual const i_shader_program& glyph_shader_program(bool aSubpixel) const;
		virtual i_shader_program& glyph_shader_program(bool aSubpixel);
		virtual const i_shader_program& gradient_shader_program() const;
		virtual i_shader_program& gradient_shader_program();
	public:
		virtual bool is_subpixel_rendering_on() const;
		virtual void subpixel_rendering_on();
		virtual void subpixel_rendering_off();
	public:
		static const uint32_t GRADIENT_FILTER_SIZE = 33;
		const std::array<GLuint, 3>& gradient_textures() const; // todo: use texture class and add to base class interface
	public:
		virtual bool process_events();
	private:
		shader_programs::iterator create_shader_program(const shaders& aShaders, const std::vector<std::string>& aVariables);
	private:
		neogfx::renderer iRenderer;
		detail::screen_metrics iScreenMetrics;		
		opengl_texture_manager iTextureManager;
		neogfx::font_manager iFontManager;
		shader_programs iShaderPrograms;
		shader_programs::iterator iActiveProgram;
		shader_programs::iterator iDefaultProgram;
		shader_programs::iterator iTextureProgram;
		shader_programs::iterator iMonochromeProgram;
		shader_programs::iterator iGlyphProgram;
		shader_programs::iterator iGlyphSubpixelProgram;
		shader_programs::iterator iGradientProgram;
		bool iSubpixelRendering;
		mutable boost::optional<std::array<GLuint, 3>> iGradientTextures;
	};
}