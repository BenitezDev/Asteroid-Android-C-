/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Game_Scene.hpp"

#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Id>

using namespace basics;
using namespace std;

namespace example
{

    // ---------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        canvas_width  = 1280;
        canvas_height =  720;

        // load_atlas() carga los sprites y calcula el aspect ratio real

        load_atlas();


        // Se inicia la semilla del generador de números aleatorios:

        srand (unsigned(time(nullptr)));

        // Se inicializan otros atributos:

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------
    // Algunos atributos se inicializan en este método en lugar de hacerlo en el constructor porque
    // este método puede ser llamado más veces para restablecer el estado de la escena y el constructor
    // solo se invoca una vez.

    bool Game_Scene::initialize ()
    {
        state     = LOADING;
        suspended = true;
        gameplay  = UNINITIALIZED;

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        suspended = true;               // Se marca que la escena ha pasado a primer plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::resume ()
    {
        suspended = false;              // Se marca que la escena ha pasado a segundo plano
    }

    // -----------------------------------------HANDLE----------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)               // Se descartan los eventos cuando la escena está LOADING
        {

            // Se empieza a jugar cuando el usuario toca la pantalla por primera vez
            // o cuando termina el juego (victoria o derrota)
            if (gameplay == WAITING_TO_START || gameplay == END_GAME)
            {
                start_playing ();
            }
            else switch (event.id)
            {
                case ID(touch-started):     // El usuario toca la pantalla
                case ID(touch-moved):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    // comprueba si hay pulsación

                    check_ui_touch({x,y});

                    break;
                }

                case ID(touch-ended):       // El usuario deja de tocar la pantalla
                {
                    // "Suelta" los botones
                    check_ui_touch({-100,-100});
                    break;
                }
            }
        }
    }

    // -----------------------------------------UPDATE----------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING: prepare_scene(); break;
            case RUNNING: run_simulation (time); break;
            case ERROR:   break;
        }
    }

    // -----------------------------------------RENDER----------------------------------------------

    void Game_Scene::render (Context & context)
    {
        if (!suspended)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                switch (state)
                {
                    case LOADING: reset_canvas (*canvas); break;
                    case RUNNING: render_scene (*canvas); break;
                    case ERROR:   break;
                }
            }
        }
    }



    // ----------------------------PREPARACIÓN DE LA ESCENA-----------------------------------------

    // Carga los sprites y calcula el aspect ratio real

    void Game_Scene::load_atlas()
    {

        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            float real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();

            canvas_width  = unsigned ( canvas_height * real_aspect_ratio );

            atlas.reset (new Atlas("game-scene/AsteroidsSpriteSheet.sprites", context));

            state = atlas->good () ? RUNNING : ERROR;
        }

    }

    // Prepara la escena para ser jugada. Se ejecuta cuando se carga por primera vez
    // y cuando el jugador muere o completa el juego

    void Game_Scene::prepare_scene()
    {

        create_sprites();

        create_player_ship();

        create_ui();

        timer.reset();

        // Suma de todos los asteroides de la escena
        // se debe cambiar si se meten más asteroides

        num_asteroids = number_of_big_asteroids;

        state = RUNNING;
        gameplay = PLAYING;
    }

    // Prepara los vectors<> de los asteroides y del pool de balas

    void Game_Scene::create_sprites ()
    {

        big_asteroids.resize (number_of_big_asteroids);

        // PROBLEMA: al tener que pasarle un ID para crear los sprites. No he podido hacer este paso
        // en un bucle ya que no podía pasarle un string en el ID(string).
        // Al querer hacer que cada asteroide saliese desde un lado de la pantalla con una
        // dirección random, me ha costado hacer un bucle para ello.
        // No es la mejor forma de hacerlo

        // Asteroide 1:

        big_asteroids[0].reset(new Sprite(atlas->get_slice (ID(Big_Asteroid_1))));

        big_asteroids[0]->set_position({rand () % int(canvas_width*0.4),
                                        rand () % int(canvas_height) - 20});

        big_asteroids[0]->set_angular_speed(1.f);

        switch (rand()% 4)
        {
            case 0:
                big_asteroids[0]->set_linear_speed( { rand()%101 + 50.f    , rand()%101 + 50.f});
                break;
            case 1:
                big_asteroids[0]->set_linear_speed( { -(rand()%101 + 50.f) , rand()%101 + 50.f});
                break;
            case 2:
                big_asteroids[0]->set_linear_speed( { rand()%101 + 50.f    , -(rand()%101 + 50.f)});
                break;
            case 3:
                big_asteroids[0]->set_linear_speed( { -(rand()%101 + 50.f) , -(rand()%101 + 50.f)});
                break;
        }

        // Asteroide 2:

        big_asteroids[1].reset(new Sprite(atlas->get_slice (ID(Big_Asteroid_2))));

        big_asteroids[1]->set_position({rand () % int ((canvas_width - (canvas_width*0.6f + 1 )) + (canvas_width*0.6f)),
                                        rand () % int(canvas_height) - 20});

        big_asteroids[1]->set_angular_speed(1.f);

        switch (rand()% 4)
        {
            case 0:
                big_asteroids[1]->set_linear_speed({ rand()%101 + 50.f    , rand()%101 + 50.f});
                break;
            case 1:
                big_asteroids[1]->set_linear_speed({ -(rand()%101 + 50.f) , rand()%101 + 50.f});
                break;
            case 2:
                big_asteroids[1]->set_linear_speed({ rand()%101 + 50.f    , -(rand()%101 + 50.f)});
                break;
            case 3:
                big_asteroids[1]->set_linear_speed({ -(rand()%101 + 50.f) , -(rand()%101 + 50.f)});
                break;
        }


        // Se prepara el pool de balas

        bullet_pool.resize(number_of_bullets_pool);

        for(auto & bullet : bullet_pool)
        {
            bullet.reset (new Sprite(atlas->get_slice (ID(Shoot))));
            bullet->hide();
            bullet->set_is_bullet(true);
        }

    }

    // Actualiza la escena para ser jugada

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
        prepare_scene();
    }

    // Prepara la nave del jugador.

    void Game_Scene::create_player_ship()
    {
        player_ship.reset(new Sprite(atlas->get_slice(ID(Ship))));

        player_ship->set_position({canvas_width*0.5f, canvas_height*0.5f});

        player_ship->set_angle(0.f);

        player_ship->set_angular_speed(0.f);

        // Hace que se mueva por impulsos y no por una velocidad constante

        player_ship->set_player_ship(true);

    }

    // He tratado de arreglar que los botones se queden rotados tras la partida con esta función
    // No ha funcionado

    void Game_Scene::reset_canvas(basics::Canvas &canvas)
    {
        canvas.set_transform(basics::Transformation2f());
    }

    // Crea los botones

    void Game_Scene::create_ui()
    {

        ui_buttons.resize(number_of_buttons);

        // Izquierda
        ui_buttons[0].reset (new Sprite(atlas->get_slice (ID(UI_Left_Black))));
        ui_buttons[0]->set_position({150,100});
        ui_buttons[0]->set_scale(1.5f);
        ui_buttons[0]->set_angle(0);

        // Derecha
        ui_buttons[1].reset (new Sprite(atlas->get_slice (ID(UI_Right_Black))));
        ui_buttons[1]->set_position({300,100});
        ui_buttons[1]->set_scale(1.5f);
        ui_buttons[1]->set_angle(0);

        // Propulsor
        ui_buttons[2].reset (new Sprite(atlas->get_slice (ID(UI_Up_Black))));
        ui_buttons[2]->set_position({canvas_width-400,100});
        ui_buttons[2]->set_scale(1.5f);
        ui_buttons[2]->set_angle(0);

        // Disparo
        ui_buttons[3].reset (new Sprite(atlas->get_slice (ID(UI_Fire_Black))));
        ui_buttons[3]->set_position({canvas_width-250,200});
        ui_buttons[3]->set_scale(1.5f);
        ui_buttons[3]->set_angle(0);

        // Pausa
        ui_buttons[4].reset (new Sprite(atlas->get_slice (ID(UI_Pause_Black))));
        ui_buttons[4]->set_position({canvas_width-250,canvas_height-50});
        ui_buttons[4]->set_scale(1.5f);
        ui_buttons[4]->set_angle(0);

    }

    // -----------------------------GESTIONAR LAS PULSACIONES---------------------------------------

    // Aplica el efecto que proceda a cada boton. Cambia el sprite cuando se tocan los botones

    void Game_Scene::check_ui_touch(basics::Point2f touch_location)
    {

        // Esta función tampoco la he podico hacer en un bucle ya que no podia pasar como ID un string

        // Izquierda

        if (ui_buttons[0]->contains(touch_location))
        {
            ui_buttons[0]->set_texture(atlas->get_slice (ID(UI_Left_Black)));

            // Se filtra que el juego no este en pausa

            if(gameplay == PLAYING)     player_ship -> set_angular_speed(-3.f);
        }
        else
        {
            // Si no esta pulsafo el boton se le pone el sprite en blanco

            ui_buttons[0]->set_texture(atlas->get_slice (ID(UI_Left_White)));

            player_ship -> set_angular_speed(0.f);
        }

        // Derecha

        if (ui_buttons[1]->contains(touch_location) )
        {
            ui_buttons[1]->set_texture(atlas->get_slice (ID(UI_Right_Black)));

            if(gameplay == PLAYING)     player_ship -> set_angular_speed(3.f);
        }
        else
        {
            ui_buttons[1]->set_texture(atlas->get_slice (ID(UI_Right_White)));

            player_ship -> set_angular_speed(0.f);
        }

        // Propulsor

        if (ui_buttons[2]->contains(touch_location) )
        {
            ui_buttons[2]->set_texture(atlas->get_slice (ID(UI_Up_Black)));

            // Se le aplica un impulso a la nave si el juego esta en PLAYING
            if(gameplay == PLAYING)     player_ship->ship_impulse += 40;
        }
        else
        {
            ui_buttons[2]->set_texture(atlas->get_slice (ID(UI_Up_White)));
        }

        // Disparar

        if (ui_buttons[3]->contains(touch_location) )
        {
            ui_buttons[3]->set_texture(atlas->get_slice (ID(UI_Fire_Black)));

            // Dispara un proyectil

            if(gameplay == PLAYING)     shoot();
        }
        else
        {
            ui_buttons[3]->set_texture(atlas->get_slice (ID(UI_Fire_White)));
        }


        // Pausa

        if (ui_buttons[4]->contains(touch_location) )
        {
            ui_buttons[4]->set_texture(atlas->get_slice (ID(UI_Pause_Black)));

            // Alterna entre estado en pausa o estado jugando
            if(gameplay == PAUSE) gameplay = PLAYING;
            else gameplay = PAUSE;

        }
        else
        {
            ui_buttons[4]->set_texture(atlas->get_slice (ID(UI_Pause_White)));
        }

    }
    // ------------------------------------SUB-UPDATES----------------------------------------------

    // Principal bucle update de la escena

    void Game_Scene::run_simulation (float time)
    {

        if(gameplay == PLAYING)
        {

            // Se decrementa el impulso con el tiempo al jugador. Como si fuera una Fuerza de Rozamiento

            player_ship->ship_impulse -= 20.f;

            player_ship->update(time);

            // Se comprueba que el jugador no haya rebosado los limites de la pantalla. Se teletrasporta
            // a las coordenadas correspondientes si se sale de la pantalla

            wrap_coordinates(*player_ship);

            // Actualiza la posicion de todos los asteroides

            for (auto & asteroid : big_asteroids)
            {
                asteroid->update(time);
                wrap_coordinates(*asteroid);
            }

            // Se comprueba la colision de la nave con los asteroides.

            for (auto & asteroid : big_asteroids)
            {

                if (player_ship->intersects(*asteroid))
                {
                    // Esto hará que salga una pantalla en rojo (por haber perdido) y se esperará a
                    // que se pulse la pantalla para volver a jugar

                    gameplay = WAITING_TO_START;
                }

            }

            // Se actualizan la visibilidad de las balas

            update_bullets_visibility(time);

            // Se comprueban las colisiones entre las balas y los asteroides

            for (auto & bullet : bullet_pool)
            {
                for(auto & asteroid : big_asteroids)
                {
                    if(bullet->intersects(*asteroid))
                    {

                        // Se "desactiva" la bala

                        asteroid->hide();

                        // Decrementa en 1 los asteroides activos en la escena

                        destroy_asteroid();

                        // Se comprueba que quedan asteroides en la escena

                        if(num_asteroids <= 0 )
                        {

                            // Si se han destruido todos los asteroides. Se cambia el estado del
                            // gameplay para hacer que salga una pantalla verde (vistoria) al jugador.
                            // Luego se espera a que pulse la pantalla para empezar una nueva partida

                            gameplay = END_GAME;
                        }
                    }
                }

            }

        }

    }

    // Comprueba si la bala ha salido de los bordes de la pantalla para "desactivarla"
    // Si están activas, las actualiza

    void Game_Scene::update_bullets_visibility(float time)
    {

        for(auto & bullet : bullet_pool)
        {

            // Se actualizan solo las balas que esten "activas"

            if(bullet->is_visible())
            {
                bullet->update(time);
            }

            // Se "desactivan" aquellas balas que hayan salido de los bordes de la pantalla

            if(bullet->get_position_x() < 0 || bullet->get_position_x() > canvas_width)
                bullet->hide();
            if(bullet->get_position_y() < 0 || bullet->get_position_y() > canvas_height)
                bullet->hide();
        }

    }

    // ------------------------------------SUB-RENDERS----------------------------------------------

    // Se dibujan todos los sprites que conforman la escena

    void Game_Scene::render_scene (Canvas & canvas)
    {

        // Si la partida ha terminado y el jugador ha perdido, se pinta la pantalla en rojo

        if(gameplay == WAITING_TO_START)
        {
            canvas.set_color(1,0,0);
            canvas.fill_rectangle({ 0.f, 0.f} ,{(float)canvas_width, (float)canvas_height});
        }
        else

        // Si la partida ha terminado y el jugador ha ganado, se pinta la pantalla en verde

        if( gameplay == END_GAME )
        {
            canvas.set_color(0,1,0);
            canvas.fill_rectangle({ 0.f, 0.f} ,{(float)canvas_width, (float)canvas_height});
        }

        else
        {
            // Se pinta un fondo negro
            canvas.set_color(0,0,0);
            canvas.fill_rectangle({ 0.f, 0.f} ,{(float)canvas_width, (float)canvas_height});

            // Se mandan a renderizar los elementos de la escena
            player_ship->render(canvas);
            render_ui(canvas);
            render_asteroids(canvas);
            for(auto & bullet : bullet_pool)
                bullet->render(canvas);
        }
    }

    // Manda renderizar a todos los botones de la escena

    void Game_Scene::render_ui(basics::Canvas &canvas)
    {

        for( auto & button : ui_buttons)
        {
            button->render(canvas);
        }

    }

    // Manda renderizar a todos los asteroides de la escena
    void Game_Scene::render_asteroids(basics::Canvas &canvas)
    {

        for(auto & asteroid: big_asteroids)
        {
            asteroid->render(canvas);
        }

    }

    // ---------------------------------------------------------------------------------------------

    // Actualiza las posiciones del sprite si ha traspasado los bordes de la pantalla

    void Game_Scene::wrap_coordinates(example::Sprite &sprite)
    {
        if(sprite.get_position_x() < 0.0f)
            sprite.set_position_x(sprite.get_position_x()+(float)canvas_width);
        if(sprite.get_position_x() >= canvas_width)
            sprite.set_position_x(sprite.get_position_x()-(float)canvas_width);
        if(sprite.get_position_y() < 0.0f)
            sprite.set_position_y(sprite.get_position_y()+(float)canvas_height);
        if(sprite.get_position_y() >= canvas_height)
            sprite.set_position_y(sprite.get_position_y()-(float)canvas_height);
    }

    // Dispara la primera bala "desactivada" del pool con un Cooldown de 0.3 segundos

    void Game_Scene::shoot()
    {

        for (auto &bullet : bullet_pool)
        {

            if (!bullet->is_visible() && timer.get_elapsed_seconds() > 0.3f)
            {

                bullet->set_position(player_ship->get_position());
                bullet->set_angle(player_ship->get_angle());
                bullet->set_angular_speed(0);

                bullet->set_linear_speed({300 * std::cos(player_ship->get_angle()),
                                          -300 * std::sin(player_ship->get_angle())});

                bullet->show();

                timer.reset();
                break;
            }
        }

    }

}
