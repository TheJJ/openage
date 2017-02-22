// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "physics.h"

#include <ncurses.h>

#include <cmath>

namespace openage {
namespace tubepong {

constexpr float extrapolating_time = 100.0f;
constexpr int init_recursion_limit = 50;


void Physics::processInput(PongState &state, PongPlayer &player, std::vector<event> &events, const tube::tube_time_t &now) {
	for (auto evnt : events) {
		//Process only if the future has changed
		if (player.state.at(now).state != evnt.state) {
			player.state.set_end(now, evnt);

			switch(evnt.state) {
			case event::UP:
			case event::DOWN: {
				if (evnt.state == event::UP) {
					player.speed.set_end(now, -2);
				}
				else if (evnt.state == event::DOWN) {
					player.speed.set_end(now, 2);
				}

				player.speed.set_end(now + extrapolating_time, 0);

				float new_pos = player.position.at(now) +
				                (player.speed.at(now+extrapolating_time) - player.speed.at(now) / 2 +
				                 player.speed.at(now));

				if (new_pos < 0) {
					new_pos = 0;
				}

				if (new_pos > state.resolution[1]) {
					new_pos = state.resolution[1];
				}

				player.position.set_end(now+extrapolating_time, new_pos);
				evnt.state = event::IDLE;
				player.state.set_end(now + extrapolating_time, evnt);
				break;
			}
			case event::IDLE:
				player.position.set_end(now+extrapolating_time,
				                        player.position.at(now));
				break;

			case event::START:
				if (player.state.at(now).state == event::LOST) {
					state.ball.position.set_end(now, state.resolution * 0.5);
				}
				update_ball(state, now, init_recursion_limit);
				break;

			default:
				break;
			}
		}
	}
}

void Physics::update(PongState &state, const tube::tube_time_t &now) {
	auto pos = state.ball.position.at(now);

	// Handle panel p1
	if (pos[0] <= 1 and
	    pos[1] > state.p1.position.at(now) - state.p1.size.at(now) / 2 and
	    pos[1] < state.p1.position.at(now) + state.p1.size.at(now) / 2 and
	    state.ball.speed.at(now)[0] < 0) {

		// Ball hit the paddel in this frame
		auto s = state.ball.speed.at(now);
		s[0] *= -1.0;
		state.ball.speed.set_end(now, s);
		state.ball.position.set_end(now, pos); // this line can handle the future!

		update_ball(state, now, init_recursion_limit);
	}
	else if (pos[0] >= state.resolution[0] - 1 and
	         pos[1] > state.p2.position.at(now) - state.p2.size.at(now) / 2 and
	         pos[1] < state.p2.position.at(now) + state.p2.size.at(now) / 2 and
	         state.ball.speed.at(now)[0] > 0) {

		// Ball hit the paddel in this frame
		auto s = state.ball.speed.at(now);
		s[0] *= -1.0;
		state.ball.speed.set_end(now, s);
		state.ball.position.set_end(now, pos); // this line can handle the future!

		update_ball(state, now, init_recursion_limit);
	}
	else if (state.ball.position.needs_update(now)) {
		update_ball(state, now, init_recursion_limit);
	}

	// Game loose condition
	if (pos[0] < 0) {
		state.p1.lives.set_end(now, state.p1.lives.at(now) - 1);
		state.p1.state.set_end(now, event.at(state.p1.id, event::LOST));
	}
	if (pos[0] > state.resolution[0]) {
		state.p2.lives.set_end(now, state.p2.lives.at(now) - 1);
		state.p2.state.set_end(now, event.at(state.p2.id, event::LOST));
	}
}


void Physics::update_ball(PongState &state, const tube::tube_time_t &now, int recursion_limit) {
	// calculate the ball takes to hit a wall.
	auto speed = state.ball.speed.at(now);
	auto pos = state.ball.position.at(now);

	// predicted collision time
	float ty = 0;

	if (speed[1] > 0) {
		ty = (state.resolution[1] - pos[1]) / speed[1];
	}
	else if (speed[1] < 0) {
		ty = pos[1] / -speed[1];
	}

	if (ty > 0) {
		auto hit_pos = pos + speed * ty;
		state.ball.position.set_end(now + ty, hit_pos);
		speed[1] *= -1;
		state.ball.speed.set_end(now + ty, speed);

		if (recursion_limit > 1) {
			update_ball(state, now + ty, recursion_limit - 1);
		}
	}
}

}} // openage::tubepong
