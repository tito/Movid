#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "highgui.h"

#include "otFactory.h"
#include "otModule.h"
#include "otPipeline.h"
#include "otDataStream.h"

#define COLOR_WHITE		0xffffffff
#define COLOR_BLACK		0x000000ff
#define COLOR_SELECTED	0x666666ff
#define COLOR_IO		0xeeeeeeff
#define COLOR_LINE		0x4444ffff
#define COLOR_LINE_BAD	0xff2222ff
#define COLOR_LINE_OK	0x44ff44ff
#define COLOR_RUNNING	0x22aa22ff

class World;
class Module;

static World *world = NULL;
static SDL_Surface *screen = NULL;
static bool link_in_progress = false;
static Module *link_module_src = NULL;
static Module *link_module_hover = NULL;
static int link_module_input = -1;
static int link_module_output = -1;
static int mouse_x = 0;
static int mouse_y = 0;

class Widget {
public:
	Widget() {
		this->x = 0;
		this->y = 0;
		this->w = 100;
		this->h = 100;
		this->is_hover = false;
		this->color = COLOR_BLACK;
	}

	virtual ~Widget() {
	}

	virtual void draw() {
		boxColor(screen, this->x, this->y,
			this->x + this->w,
			this->y + this->h,
			this->is_hover ? COLOR_SELECTED : this->color
		);
		rectangleColor(screen, this->x, this->y,
			this->x + this->w,
			this->y + this->h,
			COLOR_WHITE
		);
	}

	virtual void draw_after() {
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

	virtual void draw_current_link() { }

	int x, y, w, h;
	unsigned int color;
	bool is_hover;
};

class World {
public:
	World() {
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
		this->w = info->current_w;
		this->h = info->current_h;
		this->pipeline = new otPipeline();
		this->frametime = 0;
		this->framecount = 0;
		this->fps = 0;
	}

	~World() {
	}

	void draw() {
		SDL_FillRect(screen, NULL, 0);
		std::vector<Widget *>::iterator it;
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			(*it)->draw();
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			(*it)->draw_after();
		if ( link_in_progress )
			((Widget *)link_module_src)->draw_current_link();
		this->dofps();
		SDL_Flip(screen);
	}

	void dofps() {
		double cur = SDL_GetTicks() / 1000.;
		this->framecount++;
		if ( this->frametime == 0 )
			this->frametime = cur;
		if ( cur - this->frametime > 0.5 ) {
			this->fps = this->framecount / (cur - this->frametime);
			this->framecount = 0;
			this->frametime = cur;
		}

		std::ostringstream oss;
		oss << "FPS=" << (int)this->fps;
		stringColor(screen, 0, this->h - 8, oss.str().c_str(), 0xffffffff);
	}

	void update() {
		// handle add/remove widget
		std::vector<Widget *>::iterator it, it2;
		for ( it = this->widgets_to_remove.begin(); it != this->widgets_to_remove.end(); it++ ) {
			for ( it = this->widgets.begin(); it2 != this->widgets.end(); it2++ ) {
				if ( *it2 != *it )
					continue;
				this->widgets.erase(it);
				break;
			}
		}

		for ( it = this->widgets_to_add.begin(); it != this->widgets_to_add.end(); it++ )
			this->widgets.push_back(*it);

		this->widgets_to_add.clear();
		this->widgets_to_remove.clear();

		// ui
		cvWaitKey(2);
		if ( this->pipeline->isStarted() )
			this->pipeline->update();
		for ( it = this->widgets.begin(); it != this->widgets.end(); it++ )
			(*it)->update();
	}

	void on_mousemove(int x, int y, int xrel, int yrel) {
		std::vector<Widget *>::reverse_iterator it;
		mouse_x = x;
		mouse_y = y;
		for ( it = this->widgets.rbegin(); it != this->widgets.rend(); it++ )
			if ( (*it)->on_mousemove(x, y, xrel, yrel) )
				return;
	}

	void on_mousedown(int x, int y) {
		std::vector<Widget *>::reverse_iterator it;
		for ( it = this->widgets.rbegin(); it != this->widgets.rend(); it++ )
			if ( (*it)->on_mousedown(x, y) )
				return;
	}

	void on_mouseup(int x, int y) {
		std::vector<Widget *>::reverse_iterator it;
		for ( it = this->widgets.rbegin(); it != this->widgets.rend(); it++ ) {
			if ( (*it)->on_mouseup(x, y) ) {
				link_in_progress = false;
				return;
			}
		}
		link_in_progress = false;
	}

	void addWidget(Widget *widget) {
		this->widgets_to_add.push_back(widget);
	}

	void removeWidget(Widget *widget) {
		this->widgets_to_remove.push_back(widget);
	}

	otPipeline *pipeline;
	std::vector<Widget *> widgets;
	std::map<otModule *, Module*> mapping;
	std::vector<Widget *> widgets_to_add;
	std::vector<Widget *> widgets_to_remove;
	int w;
	int h;
	int framecount;
	double fps;
	double frametime;
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

class otGuiDisplayModule : public otModule {
public:
	otGuiDisplayModule() : otModule(OT_MODULE_INPUT, 1, 0) {
		this->input = new otDataStream("stream");
		this->output_buffer = NULL;
	}

	void stop() {
		if ( this->output_buffer != NULL ) {
			cvReleaseImage(&this->output_buffer);
			this->output_buffer = NULL;
		}
		otModule::stop();
	}

	void notifyData(otDataStream *source) {
		IplImage* src = (IplImage*)(this->input->getData());
		if ( src == NULL )
			return;
		if ( this->output_buffer == NULL )
			this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	}

	void setInput(otDataStream* stream, int n=0) {
		this->input = stream;
		stream->addObserver(this);
	}

	virtual otDataStream *getInput(int n=0) {
		return this->input;
	}

	virtual otDataStream *getOutput(int n=0) {
		return NULL;
	}

	void copy() {
		if ( this->output_buffer == NULL )
			return;
		this->input->lock();
		IplImage* src = (IplImage*)(this->input->getData());
		if ( src == NULL )
			return;
		cvCopy(src, this->output_buffer);
		this->input->unlock();
	}

	void update() {}
	std::string getName() { return "GuiDisplay"; }
	std::string getDescription() { return "Get image from an output for GUI"; }
	std::string getAuthor() { return "sdlgui"; }

	otDataStream *input;
	IplImage* output_buffer;
};

class Module : public Widget {
public:
	Module(std::string name) {
		this->name = name;
		this->module = otFactory::getInstance()->create(name.c_str());

		world->pipeline->addElement(this->module);
		world->mapping[this->module] = this;

		for ( int i = 0; i < this->module->getOutputCount(); i++ ) {
			otGuiDisplayModule *output = new otGuiDisplayModule();
			this->output_video[i] = output;
			output->setInput(this->module->getOutput(i));
		}

		this->input_selected = -1;
		this->output_selected = -1;
		this->selected = false;
	}

	void update() {
		if ( this->is_hover && link_in_progress ) {
			int idx;
			if ( link_module_src->input_selected >= 0 ) {
				idx = this->in_output(mouse_x, mouse_y);
				if ( idx >= 0 ) {
					link_module_hover = this;
					link_module_output = idx;
				}
			}

			if ( link_module_src->output_selected >= 0 ) {
				idx = this->in_input(mouse_x, mouse_y);
				if ( idx >= 0 ) {
					link_module_hover = this;
					link_module_input = idx;
				}
			}
		}
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
		this->color = this->module->isStarted() ? COLOR_RUNNING : COLOR_BLACK;
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

	virtual void draw_after() {
		otDataStream *stream;
		otModule *module;
		Module *g_module;
		int k;
		bool found;
		Widget::draw_after();

		for ( int i = 0; i < this->module->getOutputCount(); i++ ) {
			stream = this->module->getOutput(i);

			for ( unsigned int j = 0; j < stream->getObserverCount(); j++ ) {
				module = stream->getObserver(j);
				g_module = world->mapping[module];
				if ( g_module == NULL )
					continue;

				// search the index inside the module
				found = true;
				for ( k = 0; k < module->getInputCount(); k++ ) {
					if ( module->getInput(k) == stream ) {
						found = true;
						break;
					}
				}

				if ( !found )
					continue;

				// i = index output source
				// k = index input destination
				lineColor(screen, this->x + this->w,
						this->y + 15 + 20 * i,
						g_module->x,
						g_module->y + 15 + 20 * k,
						COLOR_LINE);
			}
		}

		// draw video ?
		if ( this->module->isStarted() ) {
			int idx = this->in_output(mouse_x, mouse_y);
			if ( idx >= 0 ) {
				this->output_video[idx]->copy();
				IplImage *opencvimg = this->output_video[idx]->output_buffer;
				if ( opencvimg ) {
					// XXX may be a long operation if SDL copy pixels, but i'm not sure...
					SDL_Rect r;
					SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)opencvimg->imageData,
						opencvimg->width,
						opencvimg->height,
						opencvimg->depth * opencvimg->nChannels,
						opencvimg->widthStep,
						0xff0000, 0x00ff00, 0x0000ff, 0
					);
					r.x = (world->w - opencvimg->width) / 2.;
					r.y = (world->h - opencvimg->height) / 2.;
					r.w = opencvimg->width;
					r.h = opencvimg->height;
					SDL_BlitSurface(surface, NULL, screen, &r);
					SDL_FreeSurface(surface);
					rectangleColor(screen, r.x, r.y, r.x + r.w, r.y + r.h, COLOR_WHITE);
				}
			}
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
		if ( this->selected ) {
			this->x = x - this->select_x;
			this->y = y - this->select_y;
		}
		return Widget::on_mousemove(x, y, xrel, yrel);
	}

	void draw_current_link() {
		if ( link_in_progress ) {
			int ox, oy;
			if ( this->input_selected >= 0 ) {
				ox = link_module_src->x;
				oy = this->y + 15 + 20 * this->input_selected;
			} else {
				ox = link_module_src->x + link_module_src->w;
				oy = this->y + 15 + 20 * this->output_selected;
			}

			lineColor(screen, ox, oy, mouse_x, mouse_y,
				link_module_hover ? COLOR_LINE_OK : COLOR_LINE_BAD);
		}

		// reset !
		link_module_hover = NULL;
		link_module_output = -1;
		link_module_input = -1;
	}


	std::string name;
	otModule *module;
	int input_selected;
	int output_selected;
	std::map<int, otGuiDisplayModule *> output_video;

protected:
	bool selected;
	int select_x;
	int select_y;
};

class ButtonCreateModule : public Button {
public:
	virtual void on_click() {
		Module *m = new Module(this->label);
		m->x = (world->w - this->w) / 2;
		m->y = (world->h - this->h) / 2;
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

	src->color = world->pipeline->isStarted() ? COLOR_RUNNING : COLOR_BLACK;
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
	std::vector<std::string> l = otFactory::getInstance()->list();
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

	screen = SDL_SetVideoMode(1024, 768, 16, SDL_DOUBLEBUF);
	if ( screen == NULL ) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	otFactory::init();

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
