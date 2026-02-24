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

// Add constants for vertical movement
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
public:
    bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
    bn::fixed x_speed = BASE_SPEED;
    bn::fixed y_speed = BASE_SPEED;

    // Constructor to set random speeds
    Bouncer(bn::random &random)
    {
        x_speed = random.get_fixed(-3, 3);
        y_speed = random.get_fixed(-3, 3);

        // changed random speeds
        if (x_speed == 0)
            x_speed = 2;
        if (y_speed == 0)
            y_speed = 2;
    }

    void update()
    {
        bn::fixed x = sprite.x();
        bn::fixed y = sprite.y();

        // Update x and y position by adding speed
        x += x_speed;
        y += y_speed;

        // If we've gone off the screen on the right
        if (x > MAX_X)
        {
            x = MAX_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X)
        {
            x = MIN_X;
            x_speed *= -1;
        }

        // Bounce vertically
        if (y > MAX_Y)
        {
            y = MAX_Y;
            y_speed *= -1;
        }
        if (y < MIN_Y)
        {
            y = MIN_Y;
            y_speed *= -1;
        }

        sprite.set_position(x, y);
    }
};

bn::fixed average_x(const bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    bn::fixed x_sum = 0;

    for (const Bouncer &bouncer : bouncers)
    {
        x_sum += bouncer.sprite.x();
    }

    if (bouncers.size() > 0)
    {
        return x_sum / bouncers.size();
    }

    return 0;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers, bn::random &random)
{
    // Only add if we're below the maximum
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer(random));
    }
}

int main()
{
    bn::core::init();

    // Sprites and x speeds of bouncers
    // Items with the same index correspond to each other

    //  color backdrop (blue)
    bn::backdrop::set_color(bn::color(0, 0, 15));

    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    // Random generator instance
    bn::random random;

    while (true)
    {
        // if A is pressed add a new bouncer
        if (bn::keypad::a_pressed())
        {
            add_bouncer(bouncers, random);
        }

        if (bn::keypad::b_pressed())
        {
            BN_LOG("Average x: ", average_x(bouncers));
        }

        // for each bouncer
        for (Bouncer &bouncer : bouncers)
        {
            bouncer.update();
        }

        bn::core::update();
    }
}