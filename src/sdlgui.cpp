#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdio.h>

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "highgui.h"

#include "otFactory.h"
#include "otModule.h"
#include "otPipeline.h"

#define COLOR_WHITE		0xffffffff
#define COLOR_BLACK		0x000000ff
#define COLOR_SELECTED	0x666666ff
#define COLOR_IO		0xeeeeeeff

class World;
class Module;

static World *world = NULL;
static SDL_Surface *screen = NULL;
static bool link_in_progress = false;
static Module *link_module_src = NULL;

class Widget {
public:
	Widget() {
		this->x = 0;
		this->y = 0;
		this->w = 100;
		this->h = 100;
		this->is_hover = false;
	}

	virtual ~Widget() {
	}

	virtual void draw() {
		boxColor(screen, this->x, this->y,
			this->x + this->w,
			this->y + this->h,
			this->is_hover ? COLOR_SELECTED : COLOR_BLACK
		);
		rectangleColor(screen, this->x, this->y,
			this->x + this->w,
			this->y + this->h,
			COLOR_WHITE
		);
	}

	virtual void update() {
	}

	virtual bool on_mousemove(int x, int y, int xrel, int yrel) {
		this->is_hover = this->in_bbox(x, y) ? true : false;
		return false;
	}

	virtual bool on_mousedown(int x, int y) {
		return false;
	}

	virtual bool on_mouseup(int x, int y) {
		return false;
	}


	virtual bool in_bbox(int x, int y) {
		return ( x >= this->x && x <= this->w + this->x &&
			 y >= this->y && y <= this->h + this->y ) ? true : false;
	}

	int x, y, w, h;
	bool is_hover;
};

class World {
public:
	World() {
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
		this->w = info->current_w;
		this->h = info->current_h;
		this->pipeline = new otPipeline();
	}

	~World() {
	}

	void draw() {
		SDL_FillRect(screen, NULL, 0);
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			(*it)->draw();
		SDL_Flip(screen);
	}

	void update() {
		cvWaitKey(5);
		if ( this->pipeline->isStarted() )
			this->pipeline->update();
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			(*it)->update();
	}

	void on_mousemove(int x, int y, int xrel, int yrel) {
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			if ( (*it)->on_mousemove(x, y, xrel, yrel) )
				return;
	}

	void on_mousedown(int x, int y) {
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			if ( (*it)->on_mousedown(x, y) )
				return;
	}

	void on_mouseup(int x, int y) {
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ ) {
			if ( (*it)->on_mouseup(x, y) ) {
				link_in_progress = false;
				return;
			}
		}
		link_in_progress = false;
	}

	void addWidget(Widget *widget) {
		this->widgets.push_back(widget);
	}

	void removeWidget(Widget *widget) {
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ ) {
			if ( *it == widget ) {
				this->widgets.erase(it);
				return;
			}
		}
	}

	otPipeline *pipeline;
	std::vector<Widget *> widgets;
	int w;
	int h;
};

class Button;
typedef void (*button_callback_t)(Button *src);

class Button : public Widget {
public:
	Button() {
		this->callback = NULL;
	}

	virtual void draw() {
		Widget::draw();
		stringRGBA(screen, this->x + (this->w - this->label.size() * 8) / 2,
				this->y + this->h / 2 - 4,
				this->label.c_str(), 255, 255, 255, 255);
	}

	virtual bool on_mousedown(int x, int y) {
		if ( this->in_bbox(x, y) )
			this->on_click();
		return Widget::on_mousedown(x, y);
	}

	virtual void on_click() {
		if ( this->callback )
			this->callback(this);
	}

	std::string label;
	button_callback_t callback;

};

class Module : public Widget {
public:
	Module(std::string name) {
		this->name = name;
		this->module = otFactory::create(name.c_str());

		world->pipeline->addElement(this->module);

		this->input_selected = -1;
		this->output_selected = -1;
		this->selected = false;
	}

	int in_input(int x, int y) {
		x -= this->x;
		y -= this->y;
		if ( x < 0 || x > 10 )
			return -1;
		if ( y < 0 || y > this->h )
			return -1;

		int idx = (y / 20);
		float fidx = (float(y) / 20.);
		if ( (fidx - idx) < 0.5 )
			return -1;

		if ( idx >= this->module->getInputCount() )
			return -1;
		return idx;
	}

	int in_output(int x, int y) {
		x -= this->x;
		y -= this->y;
		if ( x < this->w - 10 || x > this->w )
			return -1;
		if ( y < 0 || y > this->h )
			return -1;

		int idx = (y / 20);
		float fidx = (float(y) / 20.);
		if ( (fidx - idx) < 0.5 )
			return -1;

		if ( idx >= this->module->getOutputCount() )
			return -1;
		return idx;
	}

	virtual void draw() {
		Widget::draw();
		stringColor(screen, this->x + (this->w - this->name.size() * 8) / 2,
			this->y + this->h / 2 - 4,
			this->name.c_str(), COLOR_WHITE);

		// draw input/output
		int y = this->y + 10;
		int x = this->x;
		for ( int i = 0; i < this->module->getInputCount(); i++ ) {
			boxColor(screen, x, y, x + 10, y + 10, COLOR_IO);
			y += 20;
		}

		y = this->y + 10;
		x = this->x + this->w;
		for ( int i = 0; i < this->module->getOutputCount(); i++ ) {
			boxColor(screen, x - 10, y, x, y + 10, COLOR_IO);
			y += 20;
		}
	}

	virtual bool on_mousedown(int x, int y) {
		this->input_selected = this->in_input(x, y);
		this->output_selected = this->in_output(x, y);
		if ( this->input_selected >= 0 || this->output_selected >= 0 ) {
			link_in_progress = true;
			link_module_src = this;
			return true;
		}

		this->selected = this->in_bbox(x, y);
		this->select_x = x - this->x;
		this->select_y = y - this->y;
		return this->selected;
	}

	virtual bool on_mouseup(int x, int y) {
		if ( this->is_hover && link_in_progress ) {
			int idx;

			if ( link_module_src->input_selected >= 0 ) {
				idx = this->in_output(x, y);
				if ( idx < 0 )
					return false;

				// connect !
				std::cout << "A:" << idx << ":" << link_module_src->input_selected << std::endl;
				link_module_src->module->setInput(
					this->module->getOutput(idx),
					link_module_src->input_selected);
			}

			if ( link_module_src->output_selected >= 0 ) {
				idx = this->in_input(x, y);
				if ( idx < 0 )
					return false;

				// connect !
				std::cout << "B:" << idx << ":" << link_module_src->output_selected << std::endl;
				this->module->setInput(
					link_module_src->module->getOutput(link_module_src->output_selected),
					idx);
			}
		}

		Widget::on_mouseup(x, y);
		this->selected = false;
		return false;
	}

	virtual bool on_mousemove(int x, int y, int xrel, int yrel) {
		//std::cout << this->in_input(x, y) << ":" << this->in_output(x, y) << std::endl;
		if ( this->selected ) {
			this->x = x - this->select_x;
			this->y = y - this->select_y;
		}
		return Widget::on_mousemove(x, y, xrel, yrel);
	}

	std::string name;
	otModule *module;
	int input_selected;
	int output_selected;

protected:
	bool selected;
	int select_x;
	int select_y;
};

class ButtonCreateModule : public Button {
public:
	virtual void on_click() {
		Module *m = new Module(this->label);
		m->x = (world->w + this->w) / 2;
		m->y = (world->h + this->h) / 2;
		m->w = 150;
		m->h = 150;
		world->addWidget(m);
	}
};


void _callback_buttonstart(Button *src) {
	if ( world->pipeline->isStarted() )
		world->pipeline->stop();
	else
		world->pipeline->start();
}

void gui_createWorld() {
	int y = 0;

	world = new World();

	Button *bstart = new Button();
	bstart->y = y;
	bstart->w = 150;
	bstart->h = 30;
	bstart->label = "Start / Stop";
	bstart->callback = _callback_buttonstart;
	world->addWidget(bstart);

	// ask factory availables 
	std::vector<std::string> l = otFactory::list();
	std::vector<std::string>::iterator it;

	y += 40;

	for ( it = l.begin(); it != l.end(); it++ )
	{
		Button *button = new ButtonCreateModule();
		button->label = (*it);
		button->w = 150;
		button->h = 16;
		button->y = y;
		world->addWidget(button);

		y += 18;
	}
}



int main(int argc, char **argv) {
	SDL_Event event;
	int running = 1;

	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	screen = SDL_SetVideoMode(800, 600, 16, SDL_DOUBLEBUF);
	if ( screen == NULL ) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	gui_createWorld();

	while(running) {

		while ( SDL_PollEvent(&event) ) {

			switch ( event.type ) {
				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == 27 )
						running = 0;
					break;
				case SDL_MOUSEBUTTONDOWN:
					world->on_mousedown(event.button.x, event.button.y);
					break;
				case SDL_MOUSEBUTTONUP:
					world->on_mouseup(event.button.x, event.button.y);
					break;
				case SDL_MOUSEMOTION:
					world->on_mousemove(event.motion.x, event.motion.y,
							event.motion.xrel, event.motion.yrel);
					break;
				case SDL_QUIT:
					running = 0;
					break;
			}
		}

		world->update();
		world->draw();
	}

	return 0;
}
