/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "moDaemon.h"
#include "moLog.h"
#include "moFactory.h"
#include "moPipeline.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "SDL_gfxPrimitives.h"

LOG_DECLARE("Daemon");

static bool daemon_show_gui			= false;
static bool	daemon_gui_active		= false;
static std::string daemon_gui_id	= "";
static SDL_Surface *gui_display		= NULL;

static void daemon_gui_open(void) {
	daemon_gui_active = true;
	SDL_VideoInit(NULL, 0);
	SDL_SetVideoMode(0, 0, 0, SDL_DOUBLEBUF|SDL_FULLSCREEN);
	gui_display = SDL_GetVideoSurface();
	rectangleRGBA(gui_display, 0, 0, 100, 100, 255, 0, 0, 255);
	SDL_Flip(gui_display);
}

static void daemon_gui_close(void) {
	daemon_gui_active = false;
	SDL_VideoQuit();
}

static void daemon_gui_update(moModule *module) {
	double viewport_w = 1,
		   viewport_h = 1.;
	unsigned int color_r = 0,
				 color_g = 0,
				 color_b = 0,
				 i = 0;

	// clear the screen
	boxRGBA(gui_display, 0, 0, gui_display->w, gui_display->h, 0, 0, 0, 255);

	std::vector<std::string> gui = module->getGui();
	for ( i = 0; i < gui.size(); i++ ) {
		std::istringstream iss(gui[i]);
		std::vector<std::string> tokens;
		std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter<std::vector<std::string> >(tokens));

		if ( tokens[0] == "viewport" ) {
			viewport_w = atof(tokens[1].c_str());
			viewport_h = atof(tokens[2].c_str());
		} else if ( tokens[0] == "color" ) {
			color_r = atoi(tokens[1].c_str());
			color_g = atoi(tokens[2].c_str());
			color_b = atoi(tokens[3].c_str());
		} else if ( tokens[0] == "circle" ) {
			filledCircleRGBA(
				gui_display,
				(atoi(tokens[1].c_str()) / viewport_w) * gui_display->w,
				(atoi(tokens[2].c_str()) / viewport_h) * gui_display->h,
				(atoi(tokens[3].c_str()) / viewport_w) * gui_display->w,
				color_r, color_g, color_b, 255
			);
		} else if ( tokens[0] == "line" ) {
			aalineRGBA(
				gui_display,
				(atoi(tokens[1].c_str()) / viewport_w) * gui_display->w,
				(atoi(tokens[2].c_str()) / viewport_h) * gui_display->h,
				(atoi(tokens[3].c_str()) / viewport_w) * gui_display->w,
				(atoi(tokens[4].c_str()) / viewport_h) * gui_display->h,
				color_r, color_g, color_b, 255
			);
		}
	}

	SDL_Flip(gui_display);

	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		switch ( event.type ) {
			case SDL_QUIT:
				daemon_show_gui = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					daemon_show_gui = false;
				break;
			default:
				break;
		}
	}
}


void moDaemon::init() {
	moFactory::init();

	SDL_Init(SDL_INIT_VIDEO);

#ifdef WIN32
	// initialize network for Win32 platform
	{
		WSADATA wsaData;
		if ( WSAStartup(MAKEWORD(2, 2), &wsaData) == -1 )
			LOG(MO_CRITICAL, "unable to initialize WinSock (v2.2)");
	}
#endif
}

bool moDaemon::detach(std::string pidfilename) {
#ifndef WIN32
	pid_t pid = fork();
	if (pid > 0) {
		LOG(MO_INFO, "child process created with pid " << pid);
		try {
			std::ofstream pidfile(pidfilename.c_str(), std::ios::out|std::ios::trunc);
			if (pidfile) {
				pidfile << pid << std::endl;
				pidfile.close();
			} else {
				LOG(MO_ERROR, "Cannot write pidfile " << pidfilename);
			}
		}
		catch(std::exception x) {
			LOG(MO_ERROR, "Cannot write pidfile " << pidfilename << ": " << x.what());
		}
	}
	if (pid < 0)
		LOG(MO_ERROR, "no child process could be created, but this process is still living");
	return(pid <= 0);
#endif
	// TODO implement other platform
	LOG(MO_INFO, "This platform don't support detach yet.");
	return true;
}

void moDaemon::cleanup() {
#ifdef _WIN32
	WSACleanup();
#endif

	moFactory::cleanup();
	moLog::cleanup();
}

void moDaemon::poll(moPipeline *pipeline) {
	moModule *module;
	if ( daemon_show_gui ) {
		module = pipeline->getModuleById(daemon_gui_id);
		if ( module == NULL || !(module->getCapabilities() & MO_MODULE_GUI) ) {
			daemon_show_gui = false;
			if ( daemon_gui_active )
				daemon_gui_close();
			return;
		}

		if ( !daemon_gui_active )
			daemon_gui_open();

		daemon_gui_update(module);
	} else {
		if ( daemon_gui_active )
			daemon_gui_close();
	}
}

void moDaemon::showGui(const char *moduleid) {
	daemon_show_gui = true;
	daemon_gui_id = moduleid;
}

void moDaemon::hideGui() {
	daemon_show_gui = false;
}

bool moDaemon::isGuiShowed() {
	return daemon_show_gui;
}
