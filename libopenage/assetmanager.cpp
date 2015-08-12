// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "assetmanager.h"

#include "error/error.h"
#include "log/log.h"
#include "texture.h"
#include "util/compiler.h"
#include "util/file.h"
#include "watch/watch.h"

namespace openage {

AssetManager::AssetManager(util::Dir *root)
	:
	root{root},
	missing_tex{nullptr} {

	this->watch_manager = watch::WatchManager::create();
}

util::Dir *AssetManager::get_data_dir() {
	return this->root;
}

bool AssetManager::can_load(const std::string &name) const {
	return util::file_size(this->root->join(name)) > 0;
}

std::shared_ptr<Texture> AssetManager::load_texture(const std::string &name) {
	std::string filename = this->root->join(name);

	// the texture to be associated with the given filename
	std::shared_ptr<Texture> tex;

	// try to open the texture filename.
	if (not this->can_load(name)) {
		log::log(MSG(warn) <<  "   file " << filename << " is not there...");

		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		// return the big X texture instead
		tex = this->get_missing_tex();
	} else {
		// create the texture!
		tex = std::make_shared<Texture>(filename, true);

		this->watch_manager->watch_file(
			filename,
			[=](watch::event_type, std::string) {
				tex->reload();
			}
		);
	}

	// insert the texture into the map and return the texture.
	this->textures[filename] = tex;

	// pass back the shared_ptr<Texture>
	return tex;
}

Texture *AssetManager::get_texture(const std::string &name) {
	// check whether the requested texture was loaded already
	auto tex_it = this->textures.find(this->root->join(name));

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		return this->load_texture(name).get();
	}

	return tex_it->second.get();
}

void AssetManager::check_updates() {
	this->watch_manager->check_changes();
}

std::shared_ptr<Texture> AssetManager::get_missing_tex() {

	// if not loaded, fetch the "missing" texture (big red X).
	if (unlikely(this->missing_tex.get() == nullptr)) {
		this->missing_tex = std::make_shared<Texture>(root->join("missing.png"), false);
	}

	return this->missing_tex;
}

}
