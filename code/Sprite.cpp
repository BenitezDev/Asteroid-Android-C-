/*
 * SPRITE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Sprite.hpp"

using namespace basics;

namespace example
{

    Sprite::Sprite(const Atlas::Slice * texture)
    :
        texture (texture)
    {
        radius         = (texture->width + texture->height) * .5f;
        position       = { 0.f, 0.f };
        scale          = 1.f;
        linear_speed   = { 0.f, 0.f };
        angular_speed  = 0.f;
        visible        = true;
        is_player_ship = false;
    }

    void Sprite::update(float time)
    {
        if (visible)
        {

            // Filtra para que solo se ejecute si es el sprite de la nave
            // ya que este se mueve con un impulso y no con un movimiento constante

            if(is_player_ship)
            {

                if (ship_impulse < 0)
                {
                    ship_impulse = 0;
                }

                angle += angular_speed * time;

                Vector2f force =
                        Vector2f{std::cos(angle), -std::sin(angle)} * ship_impulse * time;

                position.coordinates.x() += force.coordinates.x();
                position.coordinates.y() += force.coordinates.y();

            }

            // Como la nave es la unica que se mueve con aceleracion, aqui solo entran los Sprites
            // que se mueven con velocidad constante

            else
            {

                angle += angular_speed * time;

                Vector2f displacement = linear_speed * time;

                position.coordinates.x() += displacement.coordinates.x();
                position.coordinates.y() += displacement.coordinates.y();

            }
        }
    }


    bool Sprite::intersects (const Sprite & other)
    {

        if(other.is_visible())
        {

            float sqr_distance_between_circles =
                    (other.position[0] - position[0]) * (other.position[0] - position[0]) +
                    (other.position[1] - position[1]) * (other.position[1] - position[1]);

            return  sqr_distance_between_circles <= (other.radius+radius)*(other.radius+radius);

        }
        else
            return false;

    }

    bool Sprite::contains (const Point2f & point)
    {

        return (
                    (point[0] - position[0]) * (point[0] - position[0])
                     +
                    (point[1] - position[1]) * (point[1] - position[1])
               )< radius * radius;

    }

    void Sprite::render(basics::Canvas &canvas)
    {
        if (visible)
        {
            if(is_player_ship || is_bullet)
            {
                canvas.set_transform(rotate_then_translate_2d(80-angle,
                                                              Vector2f{
                                                                      position[0],
                                                                      position[1]}));

                canvas.fill_rectangle
                        (
                                {0.f, 0.f},
                                {texture->width*scale, texture->height*scale},
                                texture
                        );

                canvas.set_transform(basics::Transformation2f());
            }
            else
            {

                canvas.set_transform(rotate_then_translate_2d(angle,
                                                              Vector2f{ position[0],
                                                                        position[1]}));

                canvas.fill_rectangle
                        (
                                {0.f, 0.f},
                                {texture->width*scale, texture->height*scale},
                                texture
                        );

                canvas.set_transform(basics::Transformation2f());
            }
        }
    }


}
