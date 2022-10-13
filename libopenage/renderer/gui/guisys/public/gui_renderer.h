// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#ifndef __APPLE__
#ifdef _MSC_VER
#define NOMINMAX
#include <windows.h>
#endif //_MSC_VER
#include <GL/gl.h>
#else // __APPLE__
#include <OpenGL/gl.h>
#endif

namespace openage::renderer {
class Window;
}

namespace qtgui {

class GuiRendererImpl;

/**
 * Passes the native graphic context to Qt.
 */
class GuiRenderer {
public:
	// TODO: allow FBO variant
	explicit GuiRenderer(std::shared_ptr<openage::renderer::Window> window);
	~GuiRenderer();

	GLuint render();
	void resize(int w, int h);

private:
	friend class GuiRendererImpl;
	std::unique_ptr<GuiRendererImpl> impl;
};

} // namespace qtgui
