#pragma once

#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include <SDL2/SDL.h>

#include "global.h"
#include "queue_processor.h"

Uint8 lum(const double &d) { return d < 0.0 ? 0 : d > 1.0 ? 255 : (Uint8)(255 * d); }

struct XY {
	double x;
	double y;

	XY() : x(0), y(0) {}
	XY(const double &d) : x(d), y(d) {}
	XY(const double &x_, const double &y_) : x(x_), y(y_) {}
	XY(const XY &xy_) : x(xy_.x), y(xy_.y) {}

	XY& rand_pos(const XY &center, const XY &amplitude)
	{
		x = (urd(gen) - 0.5) * amplitude.x * 2.0 + center.x;
		y = (urd(gen) - 0.5) * amplitude.y * 2.0 + center.y;
		return *this;
	}

	XY& rand_angle(const double & amplitude)
	{
		double angle = urd(gen) * 2 * M_PI;
		x = sin(angle) * amplitude;
		y = cos(angle) * amplitude;
		return *this;
	}

	XY& operator=(const XY &xy_) {
		x = xy_.x;
		y = xy_.y;
		return *this;
	}

	XY operator+(const XY &xy_) {
		return XY(x + xy_.x, y + xy_.y);
	}

	XY operator-(const XY &xy_) {
		return XY(x - xy_.x, y - xy_.y);
	}

	XY operator*(const double &d) {
		return XY(x *d, y *d);
	}

	bool in_box(const XY& xy_, double d) {
		return fabs(x - xy_.x) < d && fabs(y - xy_.y) < d;
	}
};

struct Color {
	double r;
	double g;
	double b;

	Color() : r(1.0), g(1.0), b(1.0) {}
	Color(const double &r_, const double &g_, const double &b_) : r(r_), g(g_), b(b_) {}
	Color(const Color &rgb_) : r(rgb_.r), g(rgb_.g), b(rgb_.b) {}

	Color& rand() {
		r = urd(gen);
		g = urd(gen);
		b = urd(gen);

		return *this;
	}

};

struct Particle {
	XY pos;
	XY vel;
	Color col;
};

struct Scene {
	long width;
	long height;
	double pixel_size;
	XY size;

	std::vector<Particle> particles;
	QueueProcessor qp;

	Scene() : width(0), height(0), pixel_size(0.0)
	{
	}

	~Scene() {
		qp.join();
	}

	void init(long width_, long height_, long particles_count)
	{
		width = width_;
		height = height_;
		pixel_size = 1.0 / (width > height ? width : height);
		size = XY(width * pixel_size, height * pixel_size);
		XY half_size = size * 0.5;

		particles.resize(particles_count);
		for (auto &p : particles) {
			p.pos.rand_pos(half_size, half_size);
			p.vel.rand_angle(0.5);
		}

		qp.start();
	}

	void done()
	{
		qp.stop();
	}

	void step(double time_step)
	{
		for (auto &p : particles) {
			XY new_pos = p.pos + p.vel * time_step;
			p.pos.x += 0.01;

			if (new_pos.x < 0) {
				new_pos.x = 0.01;
				p.vel.x = -p.vel.x;
			}
			if (new_pos.x > size.x) {
				new_pos.x = size.x - 0.01;
				p.vel.x = -p.vel.x;
			}
			if (new_pos.y < 0) {
				new_pos.y = 0.01;
				p.vel.y = -p.vel.y;
			}
			if (new_pos.y > size.y) {
				new_pos.y = size.y - 0.01;
				p.vel.y = -p.vel.y;
			}

			p.pos = new_pos;
		}
	}

	void fill_image_line(SDL_Surface* img, long y)
	{
		XY pos(0.0, y * pixel_size);
		Uint32* pixel = (Uint32*)((Uint8*)(img->pixels) + y * img->pitch);
		SDL_memset((Uint8*)pixel, 0, img->pitch);

		for (long x = 0; x < width; ++x, ++pixel) {
			pos.x = x * pixel_size;
			Color col;
			bool update = false;
			for (const auto &p : particles) {
				if (pos.in_box(p.pos, 0.01)) {
					update = true;
					col = p.col;
					break;
				}
			}
			
			if(update)
				*pixel = SDL_MapRGBA(img->format, lum(col.r), lum(col.g), lum(col.b), 0x00);
		}
	}

	void render(SDL_Surface* img)
	{
		for (long y = 0; y < height; ++y)
			qp.add([=]() {fill_image_line(img, y); });
		qp.wait();
	}
};