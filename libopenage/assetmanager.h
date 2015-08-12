// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ASSETMANAGER_H_
#define OPENAGE_ASSETMANAGER_H_

#include <unordered_map>
#include <string>
#include <memory>

#include "util/dir.h"
#include "watch/watch.h"

namespace openage {

class Texture;

/**
 * Container class for all available assets.
 * Responsible for loading, providing and updating requested files.
 */
class AssetManager final {
public:
	AssetManager(util::Dir *root);

	util::Dir *get_data_dir();

	/**
	 * Test whether a requested asset filename can be loaded.
	 *
	 * @param name: asset filename.
	 * @returns this filename can be loaded.
	 */
	bool can_load(const std::string &name) const;

	/**
	 * Query the Texture for a given filename.
	 *
	 * @param name: the asset file name relative to the asset root.
	 * @returns the queried texture handle.
	 */
	Texture *get_texture(const std::string &name);

	/**
	 * Ask the kernel whether there were updates to watched files.
	 */
	void check_updates();

protected:
	/**
	 * File change monitoring and automatic reloading.
	 */
	std::unique_ptr<watch::WatchManager> watch_manager;

	/**
	 * Create an internal texture handle.
	 */
	std::shared_ptr<Texture> load_texture(const std::string &name);

	/**
	 * Retrieves the texture for missing textures.
	 */
	std::shared_ptr<Texture> get_missing_tex();

private:
	/**
	 * The root directory for the available assets.
	 */
	util::Dir *root;

	/**
	 * The replacement texture for missing textures.
	 */
	std::shared_ptr<Texture> missing_tex;

	/**
	 * Map from texture filename to texture instance ptr.
	 */
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

}

#endif
