/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#include <etr_config.h>
#endif

#include "intro.h"
#include "audio.h"
#include "course_render.h"
#include "ogl.h"
#include "view.h"
#include "env.h"
#include "hud.h"
#include "course.h"
#include "track_marks.h"
#include "particles.h"
#include "game_ctrl.h"
#include "racing.h"
#include "winsys.h"
#include "physics.h"
#include "tux.h"

CIntro Intro;
static CKeyframe *startframe;

void abort_intro (CControl *ctrl) {
	TVector2d start_pt = Course.GetStartPoint ();
	State::manager.RequestEnterState (Racing);
	ctrl->orientation_initialized = false;
	ctrl->view_init = false;
	ctrl->cpos.x = start_pt.x;
	ctrl->cpos.z = start_pt.y;
}

// =================================================================
void CIntro::Enter() {
	CControl *ctrl = g_game.player->ctrl;
	TVector2d start_pt = Course.GetStartPoint ();
	ctrl->orientation_initialized = false;
	ctrl->view_init = false;
	ctrl->cpos.x = start_pt.x;
	ctrl->cpos.z = start_pt.y;

	startframe = g_game.character->GetKeyframe(START);
	if (startframe->loaded) {
		startframe->Init(ctrl->cpos, -0.05, g_game.character->shape);
	}

	// reset of result values
	g_game.herring = 0;
	g_game.score = 0;
	g_game.time = 0.0;
	g_game.race_result = -1;
	g_game.raceaborted = false;

	ctrl->Init ();

	ctrl->cvel = TVector3d(0, 0, 0);
	clear_particles();
	set_view_mode (ctrl, ABOVE);
	SetCameraDistance (4.0);
	SetStationaryCamera (false);
	update_view (ctrl, EPS);
	size_t num_items = Course.NocollArr.size();
	TItem* item_locs = &Course.NocollArr[0];
	for (size_t i = 0; i < num_items; i++) {
		if (item_locs[i].collectable != -1) {
			item_locs[i].collectable = 1;
		}
	}

	InitSnow (ctrl);
	InitWind ();

	Music.PlayTheme (g_game.theme_id, MUS_RACING);
	param.show_hud = true;
	InitViewFrustum ();
}

void CIntro::Loop () {
	CControl *ctrl = g_game.player->ctrl;
	int width = Winsys.resolution.width;
	int height = Winsys.resolution.height;
	check_gl_error();

	if (startframe->active) {
		startframe->Update ();
	} else State::manager.RequestEnterState (Racing);

	ClearRenderContext ();
	Env.SetupFog ();

	update_view (ctrl, g_game.time_step);
	SetupViewFrustum (ctrl);

	Music.Update ();
	Env.DrawSkybox (ctrl->viewpos);

	Env.DrawFog ();
	Env.SetupLight ();
	RenderCourse ();
	DrawTrackmarks ();
	DrawTrees ();
	UpdateWind ();
	UpdateSnow (ctrl);
	DrawSnow (ctrl);

	g_game.character->shape->Draw();
	DrawHud (ctrl);

	Reshape (width, height);
	Winsys.SwapBuffers ();
}
// -----------------------------------------------------------------------

void CIntro::Keyb (unsigned int key, bool special, bool release, int x, int y) {
	CControl *ctrl = g_game.player->ctrl;
	if (release) return;
	abort_intro (ctrl);
}
