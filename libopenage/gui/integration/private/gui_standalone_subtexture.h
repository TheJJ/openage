// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QSGTexture>
#include <QtGui>

namespace openage {
namespace gui {

class GuiStandaloneSubtexture : public QSGTexture {
	Q_OBJECT

public:
	explicit GuiStandaloneSubtexture(GLuint id, const QSize &size);
	virtual ~GuiStandaloneSubtexture();

private:
	virtual qint64 comparisonKey() const override;
	virtual bool hasAlphaChannel() const override;
	virtual bool hasMipmaps() const override;
	virtual bool isAtlasTexture() const override;
	virtual QSize textureSize() const override;

	// ASDF: Qt5
	virtual void bind();
	virtual int textureId() const;

	const GLuint id;
	const QSize size;
};

} // namespace gui
} // namespace openage
