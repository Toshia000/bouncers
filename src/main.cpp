#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Minimum speed of a bouncer
static constexpr bn::fixed MIN_SPEED = 1;
// Maximum speed of a bouncer
static constexpr bn::fixed MAX_SPEED = 3;


// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
public:
    bn::sprite_ptr sprite;
    bn::fixed x_speed;
    bn::fixed y_speed;
    bn::random rnd;

    Bouncer(bn::fixed x_start, bn::fixed y_start) :
        sprite(bn::sprite_items::dot.create_sprite(x_start, y_start)),
        x_speed(rnd.get_fixed(MIN_SPEED, MAX_SPEED)),
        y_speed(rnd.get_fixed(MIN_SPEED, MAX_SPEED)) {
    }

    void update()
    {
        bn::fixed x = sprite.x();
        bn::fixed y = sprite.y();

        // Update x and y positions by adding speed
        x += x_speed;
        y += y_speed;

        // If we've gone off the screen on the right
        if (x > MAX_X)
        {
            // Snap back to screen and reverse direction
            x = MAX_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X)
        {
            // Snap back to screen and reverse direction
            x = MIN_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the bottom
        if (y > MAX_Y)
        {
            // Snap back to screen and reverse direction
            y = MAX_Y;
            y_speed *= -1;
        }
        // If we've gone off the screen on the top
        if (y < MIN_Y)
        {
            // Snap back to screen and reverse direction
            y = MIN_Y;
            y_speed *= -1;
        }

        sprite.set_x(x);
        sprite.set_y(y);
    }

private:
};

bn::fixed average_x(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (Bouncer bouncer : bouncers)
    {
        x_sum += bouncer.sprite.x();
    }

    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (bouncers.size() > 0)
    {
        x_average /= bouncers.size();
    }

    return x_average;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers, bn::fixed start_x, bn::fixed start_y)
{
    // Only add if we're below the maximum
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer(start_x, start_y));
    }
}

int main()
{
    bn::core::init();

    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    bn::fixed y_spawn = 0;

    while (true)
    {
        // if A is pressed add a new bouncer
        if (bn::keypad::a_pressed())
        {
            add_bouncer(bouncers, 0, y_spawn);
        }

        // if B is pressed print the average to the console
        if(bn::keypad::b_pressed()) {
            bn::fixed x_average =  average_x(bouncers);
            BN_LOG("Average x: ", x_average);
        }

        for (Bouncer &b : bouncers)
        {
            b.update();
        }

        y_spawn += 1;

        if (y_spawn > MAX_Y)
        {
            y_spawn = MIN_Y;
        }

        bn::core::update();
    }
}