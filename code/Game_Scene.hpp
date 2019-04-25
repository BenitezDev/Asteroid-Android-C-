/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

    #include <map>
    #include <list>
    #include <memory>
    #include <vector>

    #include <basics/Canvas>
    #include <basics/Id>
    #include <basics/Scene>
    #include <basics/Atlas>
    #include <basics/Timer>

    #include "Sprite.hpp"

    namespace example
    {

        using basics::Id;
        using basics::Timer;
        using basics::Canvas;
        using basics::Texture_2D;

        class Game_Scene : public basics::Scene
        {

            // Este typedefs pueden ayudar a hacer el código más compacto y claro:

            typedef basics::Graphics_Context::Accessor Context;

            /**
             * Representa el estado de la escena en su conjunto.
             */
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };

            /**
             * Representa el estado del juego cuando el estado de la escena es RUNNING.
             */
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
                END_GAME,
                PAUSE
            };


            static constexpr unsigned number_of_buttons       = 5 ;
            static constexpr unsigned number_of_bullets_pool  = 20;
            static constexpr unsigned number_of_big_asteroids = 2 ;

            // En el caso de querer que haya mas asteroides o mas tipos de asteroides,
            // hay que sumarselos a esta variable

            unsigned num_asteroids = number_of_big_asteroids ;


        private:

            State          state;                               ///< Estado de la escena.
            Gameplay_State gameplay;                            ///< Estado del juego cuando la escena está RUNNING.
            bool           suspended;                           ///< true cuando la escena está en segundo plano y viceversa.

            unsigned       canvas_width;                        ///< Ancho de la resolución virtual usada para dibujar.
            unsigned       canvas_height;                       ///< Alto  de la resolución virtual usada para dibujar.

            Timer          timer;                               ///< Cronómetro usado para medir intervalos de tiempo


            std::shared_ptr < basics::Atlas > atlas;             ///< Referencia al atlas que se va a utilizar
            std::shared_ptr <    Sprite     > player_ship;       ///< Shared_ptr a Sprite que guarda al jugador

            std::vector < std::shared_ptr <Sprite> >  ui_buttons   ; ///< Vector que guarda todos los botones
            std::vector < std::shared_ptr <Sprite> >  big_asteroids; ///< Vector que guardan todos los asteroides grandes
            std::vector < std::shared_ptr <Sprite> >  bullet_pool  ; ///< Vector que guarda el pool de balas

        public:

            /**
             * Solo inicializa los atributos que deben estar inicializados la primera vez, cuando se
             * crea la escena desde cero.
             */
            Game_Scene();

            /**
             * Este método lo llama Director para conocer la resolución virtual con la que está
             * trabajando la escena.
             * @return Tamaño en coordenadas virtuales que está usando la escena.
             */
            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            /**
             * Aquí se inicializan los atributos que deben restablecerse cada vez que se inicia la escena.
             * @return
             */
            bool initialize () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a segundo plano.
             */
            void suspend () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a primer plano.
             */
            void resume () override;

            /**
             * Este método se invoca automáticamente una vez por fotograma cuando se acumulan
             * eventos dirigidos a la escena.
             */
            void handle (basics::Event & event) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * actualize su estado.
             */
            void update (float time) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * dibuje su contenido.
             */
            void render (Context & context) override;

        private:

            /**
             * Carga los sprites y calcula el aspect ratio real
             */
            void load_atlas ();

            /**
             * Prepara los asteroides y el pool de balas
             */
            void create_sprites ();

            /**
             * Actualiza la escena para ser jugada
             */
            void start_playing ();

            /**
             * Bucle principal de la escena
             * @param time deltatime de la escena
             */
            void run_simulation (float time);

            /**
             * Se dibujan todos los sprites que conforman la escena
             * @param canvas
             */
            void render_scene (Canvas & canvas);

            /**
             * Prepara la escena para ser jugada. Se ejecuta cuando se carga por primera vez
             *  y cuando el jugador muere o completa el juego
             */
            void prepare_scene();

            /**
             * Prepara la nave del jugador.
             */
            void create_player_ship();

            /**
             * Crea los botones de la UI
             */
            void create_ui();

            /**
             * Renderiza todos los botones de la UI
             * @param canvas
             */
            void render_ui (Canvas & canvas);

            /**
             * Renderiza todos los asteroides activos de la escena
             * @param canvas
             */
            void render_asteroids (Canvas & canvas);

            /**
             * Aplica el efecto que proceda a cada boton. Cambia el sprite cuando se tocan los botones
             * @param touch_location Posición donde el jugador ha pulsado
             */
            void check_ui_touch(Point2f touch_location);

            /**
             * Dispara la primera bala "desactivada" del pool con un Cooldown de 0.3 segundos
             */
            void shoot();

            /**
             * Decrementa en 1 la cantidad de asteroides "activos" de la escena
             */
            void destroy_asteroid() { num_asteroids--; }

            /**
             * Comprueba si la bala ha salido de los bordes de la pantalla para "desactivarla"
             * Si están activas, las actualiza
             * @param time deltatime
             */
            void update_bullets_visibility(float time);

            /**
             * Resetea la Transformation del canvas
             * @param canvas
             */
            void reset_canvas(Canvas & canvas);

            /**
             * Actualiza las posiciones del sprite si ha traspasado los bordes de la pantalla
             * @param sprite que se desea actualizar la posicion
             */
            void wrap_coordinates(Sprite &sprite);

        };

    }

#endif
