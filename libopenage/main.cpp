// Copyright 2015-2021 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "cvar/cvar.h"
#include "engine.h"
#include "game_control.h"
#include "game_renderer.h"
#include "gamedata/color_dummy.h"
#include "log/log.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "util/file.h"


namespace openage {

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	log::log(MSG(info)
	         << "launching legacy engine with "
	         << args.root_path
	         << " and fps limit "
	         << args.fps_limit);

	// TODO: store args.fps_limit and args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// TODO: select run_mode by launch argument
	Engine::mode run_mode = Engine::mode::LEGACY;

	// TODO: remove all legacy mode!
	if (run_mode == Engine::mode::LEGACY) {

		Engine engine{Engine::mode::LEGACY, args.root_path, cvar_manager};

		log::log(MSG(info).fmt("Loading time [engine]: %5.3f s", timer.getval() / 1.0e9));

		timer.start();

		{
			// create components that use the engine.
			GameRenderer renderer{&engine};

			log::log(MSG(info).fmt("Loading time   [game]: %5.3f s", timer.getval() / 1.0e9));

			// run main loop
			engine.run();
		}

		log::log(INFO << "cya!");
	}

	return 0;
}

} // openage
