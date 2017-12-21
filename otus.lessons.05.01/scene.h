#pragma once

#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include <SDL2/SDL.h>

#include "global.h"
#include "queue_processor.h"

Uint8 lum(const double &d)
{
    return d < 0.0 ? 0 : d > 1.0 ? 255 : (Uint8)(255 * d);
}
double inv(const double &d)
{
    return d < 1e-4 ? 1e4 : 1.0/d;
}
double sqr(const double &d)
{
    return d*d;
}
double tri(const double &d)
{
    return d*d*d;
}
double sgn(const double &d)
{
    return d < 0.0 ? -1.0 : d > 0.0 ? 1.0 : 0.0;
}
double loop(double a)
{
	return (sin(a) + 1.0) * 0.5;
}

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

    XY& operator=(const XY &xy_)
    {
        x = xy_.x;
        y = xy_.y;
        return *this;
    }

    XY operator+(const XY &xy_) const
    {
        return XY(x + xy_.x, y + xy_.y);
    }

    XY& operator+=(const XY &xy_)
    {
        x += xy_.x;
        y += xy_.y;
        return *this;
    }

    XY operator-(const XY &xy_) const
    {
        return XY(x - xy_.x, y - xy_.y);
    }

    XY operator*(const XY &xy_) const
    {
        return XY(x * xy_.x, y * xy_.y);
    }

    XY operator*(const double &d) const
    {
        return XY(x *d, y *d);
    }
    XY& operator*=(const double &d)
    {
        x *= d;
        y *= d;
        return *this;
    }

    bool in_box(const XY& xy_, double d) const
    {
        return fabs(x - xy_.x) < d && fabs(y - xy_.y) < d;
    }

    double dist2(const XY& xy_) const
    {
        return sqr(x - xy_.x) + sqr(y - xy_.y);
    }

    double dist(const XY& xy_) const
    {
        return sqrt(sqr(x - xy_.x) + sqr(y - xy_.y));
    }

    double dist() const
    {
        return sqrt(sqr(x) + sqr(y));
    }

    XY vec(const XY& xy_) const
    {
        return (*this) * inv(dist(xy_));
    }
};

struct Color {
    double r;
    double g;
    double b;

    Color() : r(1.0), g(1.0), b(1.0) {}
    Color(const double &d) : r(d), g(d), b(d) {}
    Color(const double &r_, const double &g_, const double &b_) : r(r_), g(g_), b(b_) {}
    Color(const Color &col) : r(col.r), g(col.g), b(col.b) {}

    Color& rand()
    {
        r = urd(gen);
        g = urd(gen);
        b = urd(gen);

        return *this;
    }

    Color operator*(const double &d) const
    {
        return Color(r *d, g *d, b*d);
    }

    Color& operator*=(const double &d)
    {
        r *= d;
        g *= d;
        b *= d;
        return *this;
    }

    Color operator+(const Color &col_) const
    {
        return Color(r +col_.r, g +col_.g, b+col_.b);
    }

    Color& operator+=(const Color &col_)
    {
        r += col_.r;
        g += col_.g;
        b += col_.b;
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
	double timer;

    std::vector<Particle> particles;
    std::vector<Particle> particles_tmp;
    QueueProcessor qp;

    Scene() : width(0), height(0), pixel_size(0.0), size(0, 0), timer(0)
    {
    }

    ~Scene()
    {
        qp.join();
    }

    void init(long width_, long height_, long particles_count)
    {
        width = width_;
        height = height_;
        pixel_size = 1.0 / (width > height ? width : height);
        size = XY(width * pixel_size, height * pixel_size);
        XY half_size = size * 0.5;
		timer = 0; 

        double a = 0;
        double dp = 2 * M_PI / 3;
        double da = 2 * M_PI / particles_count;
        particles.resize(particles_count);
		double x = 0;
        for (auto &p : particles) {
            p.pos.rand_pos(half_size, half_size);
//            p.vel.rand_angle(0.5);
            p.col = Color(
                (sin(a)+1)*0.5,
                (sin(a + dp)+1)*0.5,
                (sin(a + dp * 2)+1)*0.5
            );
            a += da;
        }
        particles_tmp = particles;

        qp.start();
    }

    void done()
    {
        qp.stop();
    }

    XY edge_force(const Particle& p_)
    {
        const XY &pos = p_.pos;
        return XY(inv(pos.x + 1) - inv(size.x - pos.x + 1), inv(pos.y + 1) - inv(size.y - pos.y + 1));
    }

    XY friction_force(const Particle& p_)
    {
        const XY &vel = p_.vel;
        return XY(-vel.x * 0.5, -vel.y * 0.5);
    }

    XY multi_particle_force(const Particle& p_)
    {
        XY multi_force(0);
        for (auto &p : particles) {
            if(&p == &p_)
                continue;

            double id = inv(p_.pos.dist(p.pos));
            XY vec = (p_.pos - p.pos) * id;

            multi_force += vec * (sqr(id) );

        }
        return multi_force * 0.01 * inv(particles.size());
    }

    XY force(const Particle& p)
    {
        // return edge_force(p) + friction_force(p);
        return edge_force(p) + friction_force(p) + multi_particle_force(p);
    }

    void step_particle(double time_step, const Particle& p_in, Particle& p_out)
    {
        XY pos_step = p_in.vel;
        XY vel_step = force(p_in);

        p_out.pos = p_in.pos + pos_step * time_step;
        p_out.vel = p_in.vel + vel_step * time_step;

        if (p_out.pos.x < 0.0) {
            p_out.pos.x = -p_out.pos.x;
            p_out.vel.x = -p_out.vel.x;
        }
        if (p_out.pos.x > size.x) {
            p_out.pos.x = 2.0*size.x - p_out.pos.x;
            p_out.vel.x = -p_out.vel.x;
        }
        if (p_out.pos.y < 0.0) {
            p_out.pos.y = -p_out.pos.y;
            p_out.vel.y = -p_out.vel.y;
        }
        if (p_out.pos.y > size.y) {
            p_out.pos.y = 2.0*size.y - p_out.pos.y;
            p_out.vel.y = -p_out.vel.y;
        }
        if(p_out.vel.dist() < 0.01)
            p_out.vel.rand_angle(0.5);
    }

    void step(double time_step)
    {
        auto p_in = particles.cbegin();
        auto p_out = particles_tmp.begin();
        while(p_in != particles.cend()) {
            qp.add([=]() {
                step_particle(time_step, *p_in, *p_out);
            });
            ++p_in;
            ++p_out;
        }
        qp.wait();
        std::swap(particles, particles_tmp);
    }

    double potential(const double &d)
    {
        return (inv(sqr(d+0.1)) - inv(d))*1e-4;
    }

    void fill_image_line(SDL_Surface* img, long y)
    {
        XY pos(0.0, y * pixel_size);
        Uint32* pixel = (Uint32*)((Uint8*)(img->pixels) + y * img->pitch);
        // SDL_memset((Uint8*)pixel, 0, img->pitch);

        double ipc = inv(sqr(sqr(particles.size())));

        for (long x = 0; x < width; ++x, ++pixel, pos.x += pixel_size) {
            Color col(0);
            double aid = 0;
            for (const auto &p : particles) {
                double id = inv(pos.dist(p.pos)*2);
                aid += inv(sqr((10 - id)*0.5)) + id*2;
                col += p.col * id;
            }
            col *=  aid * inv((particles.size())) * 0.01;

            *pixel = SDL_MapRGBA(img->format, lum(col.r), lum(col.g), lum(col.b), 0x00);
        }
    }

    void render(SDL_Surface* img)
    {
        for (long y = 0; y < height; ++y)
            qp.add([=]() {
            fill_image_line(img, y);
        });
        qp.wait();
    }

    void process(SDL_Surface* img, double time_step)
    {
        for (long y = 0; y < height; ++y)
            qp.add([=]() {
            fill_image_line(img, y);
        });

        auto p_in = particles.cbegin();
        auto p_out = particles_tmp.begin();
        while(p_in != particles.cend()) {
            qp.add([=]() {
                step_particle(time_step, *p_in, *p_out);
            });
            ++p_in;
            ++p_out;
        }

		timer += time_step;
        qp.wait();
        std::swap(particles, particles_tmp);
    }
};