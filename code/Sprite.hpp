/*
 * SPRITE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#ifndef SPRITE_HEADER
#define SPRITE_HEADER

    #include <memory>
    #include <basics/Canvas>
    #include <basics/Atlas>
    #include <basics/Vector>

    namespace example
    {

        using basics::Canvas;
        using basics::Size2f;
        using basics::Point2f;
        using basics::Vector2f;
        using basics::Atlas;

        class Sprite
        {

        protected:

            const Atlas::Slice * texture;        ///< Textura en la que está la imagen del sprite.

            Point2f  position;                   ///< Posición del sprite (normalmente en coordenadas virtuales).
            float    radius;
            float    scale;                      ///< Escala el tamaño del sprite. Por defecto es 1.
            float    angle;
            float    angular_speed;
            Vector2f linear_speed;               ///< Velocidad a la que se mueve el sprite. Usar el valor por defecto (0,0) para dejarlo quieto.
            bool     visible;                    ///< Indica si el sprite se debe actualizar y dibujar o no. Por defecto es true.
            bool     is_player_ship;
            bool     is_bullet;

        public:

            /**
             * Inicializa una nueva instancia de Sprite.
             * @param texture Puntero a la textura en la que está su imagen. No debe ser nullptr.
             */
            Sprite(const Atlas::Slice * texture);

            /**
             * Destructor virtual para facilitar heredar de esta clase si fuese necesario.
             */
            virtual ~Sprite() = default;


            float ship_impulse = 50;        ///< Fuerza de impulso que recibirá la nave al avanzar

        public:

            // Getters (con nombres autoexplicativos):

            const float    & get_radius         () const { return radius;          }
            const float    & get_angle          () const { return angle;           }
            const Point2f  & get_position       () const { return position;        }
            const float    & get_position_x     () const { return position[0];     }
            const float    & get_position_y     () const { return position[1];     }
            const Vector2f & get_linear_speed   () const { return linear_speed;    }
            const float    & get_linear_speed_x () const { return linear_speed[0]; }
            const float    & get_linear_speed_y () const { return linear_speed[1]; }
            const float    & get_angular_speed  () const { return angular_speed;   }
            const bool       is_visible         () const { return  visible;        }
            const Atlas::Slice get_texture()             { return *texture;        }
            bool is_not_visible                 () const { return !visible;        }

        public:

            // Setters (con nombres autoexplicativos):

            void set_player_ship  (const bool is_the_player)        { is_player_ship = is_the_player; }
            void set_is_bullet    (const bool _is_bullet)           { is_bullet =_is_bullet;          }
            void set_texture      (const Atlas::Slice *new_texture) { texture = new_texture;          }
            void set_position     (const Point2f & new_position)    { position = new_position;        }

            void set_scale              (float new_scale)            { scale = new_scale;         }
            void set_angle              (float new_angle)            { angle = new_angle;         }
            void set_linear_speed       (const Vector2f & new_speed) { linear_speed = new_speed;  }
            void set_angular_speed      (const float & new_speed)    { angular_speed = new_speed; }

            void set_position_x (const float & new_position_x)
                                { position.coordinates.x () = new_position_x; }

            void set_position_y (const float & new_position_y)
                                { position.coordinates.y () = new_position_y; }


        public:

            /**
             * Hace que el sprite no se actualice ni se dibuje.
             */
            void hide ()
            {
                visible = false;
            }

            /**
             * Hace que el sprite se actualice ni se dibuje.
             */
            void show ()
            {
                visible = true;
            }


        public:

            /**
             * Comprueba si el área envolvente circular de este sprite se solapa con la de otro.
             * @param other Referencia al otro sprite.
             * @return true si las áreas se solapan o false en caso contrario.
             */
            bool intersects (const Sprite & other);

            /**
             * Comprueba si un punto está dentro del sprite.
             * Se puede usar para, por ejemplo, comprobar si el punto donde el usuario ha tocado la
             * pantalla está dentro del sprite.
             * @param point Referencia al punto que se comprobará.
             * @return true si el punto está dentro o false si está fuera.
             */
            bool contains (const Point2f & point);



        public:

            /**
             * Actualiza la posición del sprite automáticamente en función de su velocidad, pero
             * solo cuando es visible.
             * @param time Fracción de tiempo que se debe avanzar.
             */
            virtual void update (float time);

            /**
             * Dibuja la imagen del sprite automáticamente, pero solo cuando es visible.
             * @param canvas Referencia al Canvas que se debe usar para dibujar la imagen.
             */
            virtual void render (Canvas & canvas);

        };

    }

#endif
