/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/02/20 04:37: 0by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void segv_handler(int sig)
{
	printf ("\n\033[1m\033[31m\tSEGFAULT: %s\n\033[0m", global_seginfo);
    abort();
    (void)sig;
}

float	cast_all(t_ray vec, t_level *l, float *dist_u, float *dist_d, int *index)
{
	int		color;
	float	res = 1000000;

	vec_normalize(&vec.dir);
	for (int j = 0; j < l->allfaces->tri_amount; j++)
	{
		float tmp;
		tmp = cast_face(l->allfaces->tris[j], vec, &color, NULL);
		if (dist_u != NULL)
		{
			if (tmp > 0 && tmp < *dist_d)
				*dist_d = tmp;
			else if (tmp < 0 && tmp > *dist_u)
				*dist_u = tmp;
		}
		else if (tmp > 0 && tmp < res)
		{
			res = tmp;
			if (index)
				*index = j;
		}
	}
	return (res);
}

t_vec3	player_input(int noclip, t_level *level, int in_air, t_vec3 vel)
{
	const Uint8		*keys = SDL_GetKeyboardState(NULL);
	t_vec3			wishdir;
	float			speed;

	wishdir.x = 0;
	wishdir.y = 0;
	wishdir.z = 0;
	speed = 0;
	if (keys[SDL_SCANCODE_LSHIFT] && noclip)
		wishdir.y += 1;
	if (keys[SDL_SCANCODE_SPACE] && (!in_air || noclip))
		wishdir.y -= 1;
	if (keys[SDL_SCANCODE_W])
		wishdir.z += 1;
	if (keys[SDL_SCANCODE_S])
		wishdir.z -= 1;
	if (keys[SDL_SCANCODE_A])
		wishdir.x -= 1;
	if (keys[SDL_SCANCODE_D])
		wishdir.x += 1;

	if (keys[SDL_SCANCODE_LEFT])	//for testing bhop
		level->look_side -= 0.004;
	if (keys[SDL_SCANCODE_RIGHT])
		level->look_side += 0.004;

	if (wishdir.x || wishdir.z)
	{
		rotate_vertex(level->look_side, &wishdir, 0);
		float length = sqrt(wishdir.x * wishdir.x + wishdir.z * wishdir.z);
		wishdir.x /= length;
		wishdir.z /= length;
	}
	if (noclip)
		speed = NOCLIP_SPEED;
	else
		speed = fmax(MOVE_SPEED - (vel.x * wishdir.x + vel.z * wishdir.z), 0);

	wishdir.x *= speed;
	wishdir.z *= speed;
	wishdir.y *= JUMP_SPEED;	//horizontal velocity does not affect vertical velocity

	if (keys[SDL_SCANCODE_LSHIFT] && !noclip)
		vec_mult(&wishdir, 0.5);
	return (wishdir);
}

void	player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level)
{
	t_ray			r;
	float			dist = 0;
	int				index;

	r.pos.x = pos->x;
	r.pos.y = pos->y;
	r.pos.z = pos->z;
	r.dir.x = vel->x;
	r.dir.y = vel->y;
	r.dir.z = vel->z;
	dist = cast_all(r, level, NULL, NULL, &index);
	if (dist > 0 && dist <= vec_length(*vel) + WALL_CLIP_DIST)
	{
		t_vec3	normal;
		vec_cross(&normal, level->allfaces->tris[index].v0v1, level->allfaces->tris[index].v0v2);
		vec_normalize(&normal);
		vec_mult(&normal, vec_dot(*vel, normal));
		t_vec3	clip;
		vec_sub(&clip, *vel, normal);
		vel->x = clip.x;
		vel->y = clip.y;
		vel->z = clip.z;
	}
}

void	player_movement(t_vec3 *pos, t_level *level)
{
	static t_vec3	vel = {0, 0, 0};
	static int		noclip = 1;
	static float	asd = 0;
	t_ray			r;
	float			dist;
	int				in_air;

	if (level == NULL)
	{
		noclip = noclip ? 0 : 1;
		return;
	}
	r.pos.x = pos->x;
	r.pos.y = pos->y;
	r.pos.z = pos->z;
	r.dir.x = 0;
	r.dir.y = 1;
	r.dir.z = 0;
	dist = cast_all(r, level, NULL, NULL, NULL);
	if (dist > 0 && dist <= PLAYER_HEIGHT && !noclip)
	{
		in_air = 0;
		if (dist < PLAYER_HEIGHT)
			pos->y -= PLAYER_HEIGHT - dist;
	}
	else
		in_air = 1;
	t_vec3 wishdir = player_input(noclip, level, in_air, vel);
	vel.y = fmax(fmin(vel.y, 0.5), -0.5);

	if (noclip)
	{
		pos->x += wishdir.x;
		pos->y += wishdir.y;
		pos->z += wishdir.z;
		vel.x = 0;
		vel.y = 0;
		vel.z = 0;
		return ;
	}
	if (vel.x || vel.y || vel.z)
		player_collision(&vel, pos, level);
	pos->x += vel.x;
	pos->y += vel.y;
	pos->z += vel.z;

	if ((wishdir.x || wishdir.y || wishdir.z)) //&& sqrtf(vel.x * vel.x + vel.z * vel.z) < MAX_SPEED)
	{
		vel.x += wishdir.x;
		vel.y += wishdir.y;
		vel.z += wishdir.z;
	}
	else if (!in_air && wishdir.x == 0 && wishdir.z == 0)
	{
		vel.x *= 0.9;
		vel.z *= 0.9;
	}
	if (in_air)
		vel.y += 0.002;		//gravity
	else if (vel.y > 0)
		vel.y = 0;
}

int				is_tri_side(t_tri tri, t_ray c)
{
	t_vec3   end;

	int amount = tri.isquad ? 4 : 3;
	for (int i = 0; i < amount; i++)
	{
		vec_sub(&end, tri.verts[i].pos, c.pos);
		if (vec_dot(end, c.dir) <= 0)
			return (0);
	}
	return (1);
}

void			split_obj(t_obj *culled, t_level *level, int *faces_left, int *faces_right)
{
	int		right_amount = 0;
	int		left_amount = 0;
	t_ray	right;
	t_ray	left;

	left.pos.x = level->pos.x;
	left.pos.y = level->pos.y;
	left.pos.z = level->pos.z;
	vec_rot(&left.dir, (t_vec3){0,0,1}, level->look_side - (M_PI / 2));
	right.pos.x = level->pos.x;
	right.pos.y = level->pos.y;
	right.pos.z = level->pos.z;
	vec_rot(&right.dir, (t_vec3){0,0,1}, level->look_side + (M_PI / 2));
	for (int i = 0; i < culled->tri_amount; i++)
	{
		if (culled[0].tris[i].isgrid)
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			culled[1].tris[right_amount] = culled[0].tris[i];
			left_amount++;
			right_amount++;
		}
		else if (is_tri_side(culled[0].tris[i], left))
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			left_amount++;
		}
		else if (is_tri_side(culled[0].tris[i], right))
		{
			culled[1].tris[right_amount] = culled[0].tris[i];
			right_amount++;
		}
		else
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			culled[1].tris[right_amount] = culled[0].tris[i];
			left_amount++;
			right_amount++;
		}
	}
	culled[0].tri_amount = left_amount;
	culled[1].tri_amount = right_amount;
	(*faces_left) = left_amount;
	(*faces_right) = right_amount;
}

void	action_loop(t_window *window, t_level *l, t_bmp *bmp, t_obj *culled, int *faces_left, int *faces_right, int rendermode)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	if (rendermode != 2)
	{
		global_seginfo = "split_obj\n";
		split_obj(culled, l, faces_left, faces_right);
		l->obj = culled;
	}
	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	if (rendermode == 2)
	{
		global_seginfo = "wireframe\n";
		wireframe(window, l);
	}
	else
	{
		i = 0;
		if (!(thread_data = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
			ft_error("memory allocation failed\n");
		global_seginfo = "render\n";
		while (i < THREAD_AMOUNT)
		{
			if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = l;
			thread_data[i]->window = window;
			thread_data[i]->img = bmp;
			threads[i] = SDL_CreateThread(render, "asd", (void*)thread_data[i]);
			i++;
		}
		i = 0;
		while (i < THREAD_AMOUNT)
		{
			int thread_returnvalue;
			SDL_WaitThread(threads[i], &thread_returnvalue);
			free(thread_data[i]);
			i++;
		}
		free(thread_data);
		global_seginfo = "fill_pixels\n";
		fill_pixels(window->frame_buffer, l->quality);
	}

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	draw_buttons();
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int		get_fps(int i)
{
	struct timeval	time;
	static long		s[2];
	static int		frames[2];
	static int		fps[2];

	frames[i]++;
	gettimeofday(&time, NULL);
	if (s[i] != time.tv_sec)
	{
		s[i] = time.tv_sec;
		fps[i] = frames[i];
		frames[i] = 0;
	}
	return (fps[i]);
}


float		get_hz(void)
{
	static float oldTime = 0;
	float newTime = SDL_GetTicks();
	float dt = newTime - oldTime;
	float fps = 1000.0f / dt;
	oldTime = newTime;
	return (fps);
}
float		avghz(float hz)
{
	struct timeval	time;
	static long		s;
	static int		frames;
	static float	avghz = 0;
	static float	tmp = 0;

	frames++;
	gettimeofday(&time, NULL);
	tmp += hz;
	if (s != time.tv_sec)
	{
		s = time.tv_sec;
		avghz = tmp /= frames;
		frames = 0;
		tmp = 0;
	}
	return (avghz);
}

#include <time.h>
#include <assert.h>

#if defined(_WIN32)

#include <windows.h>

#elif defined(__unix__) || defined(__linux) || defined(__linux__) || defined(__ANDROID__) || defined(__EPOC32__) || defined(__QNX__)

#include <time.h>

#elif defined(__APPLE__)

#include <sys/time.h>

#endif

unsigned long getTimeInNanoseconds(void) {
#if defined(_WIN32)
    LARGE_INTEGER freq;
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    QueryPerformanceFrequency(&freq);
    assert(freq.LowPart != 0 || freq.HighPart != 0);

    if (count.QuadPart < MAXLONGLONG / 1000000) {
        assert(freq.QuadPart != 0);
        return count.QuadPart * 1000000 / freq.QuadPart;
    } else {
        assert(freq.QuadPart >= 1000000);
        return count.QuadPart / (freq.QuadPart / 1000000);
    }

#elif defined(__unix__) || defined(__linux) || defined(__linux__) || defined(__ANDROID__) || defined(__QNX__)
    struct timespec currTime;
    clock_gettime(CLOCK_MONOTONIC, &currTime);
    return (uint64_t)currTime.tv_sec * 1000000 + ((uint64_t)currTime.tv_nsec / 1000);

#elif defined(__EPOC32__)
    struct timespec currTime;
    /* Symbian supports only realtime clock for clock_gettime. */
    clock_gettime(CLOCK_REALTIME, &currTime);
    return (uint64_t)currTime.tv_sec * 1000000 + ((uint64_t)currTime.tv_nsec / 1000);

#elif defined(__APPLE__)
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    return (uint64_t)currTime.tv_sec * 1000000 + (uint64_t)currTime.tv_usec;

#else
#error "Not implemented for target OS"
#endif
}

int			physics(void *data_pointer)
{
	t_physthread	*data = data_pointer;
	unsigned long	start;
	unsigned long	target = 1000000 / TICKRATE;

	while (1)
	{
		start = getTimeInNanoseconds();
		global_seginfo = "player_movement\n";
		player_movement(data->pos, data->level);// sometimes gets only visible faces?...
		SDL_Delay(5);
		*data->hz = avghz(get_hz());
		while (getTimeInNanoseconds() - start < target)
			;
	}
	return (0);
}

int		did_move(t_level *level)
{
	static int		last_side = 0;
	static int		last_up = 0;
	static t_vec3	pos;
	int				res = 0;

	if (last_side != level->look_side && last_up != level->look_up && !vec_cmp(pos, level->pos))
		res = 1;
	last_side = level->look_side;
	last_up = level->look_up;
	pos = level->pos;
	return (res);
}

void	set_quality(int frametime, t_level *level, int currentfps)
{
	static int	lastfps = 0;
	static int	s = 0;
	const int	pause_time = 2;//seconds until can improve quality
	int			minfps = TARGETFPS - 10;
	int		maxfps = TARGETFPS + 10;

	if (lastfps != currentfps)
		s++;
	lastfps = currentfps;
	if (frametime > 1000 / minfps)
		level->quality += 2;
	else if (s == pause_time && did_move(level) && frametime < 1000 / maxfps)
		level->quality -= 2;
	did_move(level);//update statics
	if (s == pause_time)
		s = 0;
	if (level->quality < 0)
		level->quality = 1;
	else if (level->quality > 13)
		level->quality = 13;
}

int			main(int argc, char **argv)
{
	SDL_Event	event;
	t_window	*window;
	t_level		*level;
	unsigned	frametime;
	t_bmp		bmp;
	int			rendermode;//0 raycast all, 1 raycast culled, 2 wireframe
	int			relmouse;
	t_obj		*culled;
	t_physthread	physicsdata;
	t_vec3		pos;

#if __APPLE__
	struct sigaction act;
	act.sa_handler = segv_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, NULL);
#endif

	relmouse = 0;
	rendermode = 1;
	global_seginfo = "bmp_read\n";
	bmp = bmp_read("out.bmp");
	global_seginfo = "init_level\n";
	level = init_level();
	level->quality = 1;
	global_seginfo = "init_window\n";
	init_window(&window);
	init_buttons(window);
	if (!(culled = (t_obj*)malloc(sizeof(t_obj) * 2)))
		ft_error("memory allocation failed\n");
	if (!(culled[0].tris = (t_tri*)malloc(sizeof(t_tri) * level->obj->tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(culled[1].tris = (t_tri*)malloc(sizeof(t_tri) * level->obj->tri_amount)))
		ft_error("memory allocation failed\n");
	for (int i = 0; i < level->obj->tri_amount; i++)
	{
		culled[0].tris[i] = level->obj[0].tris[i];
		culled[1].tris[i] = level->obj[0].tris[i];
	}
	culled[0].tri_amount = level->obj[0].tri_amount;
	culled[1].tri_amount = level->obj[0].tri_amount;
	level->allfaces = level->obj;
	physicsdata.level = level;
	float physhz = 0;
	physicsdata.hz = &physhz;
	physicsdata.pos = &pos;
	pos.x = level->pos.x;
	pos.y = level->pos.y;
	pos.z = level->pos.z;
	SDL_CreateThread(physics, "physics", (void*)&physicsdata);
	while (1)
	{
		frametime = SDL_GetTicks();
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return (0);
			else if (event.type == SDL_MOUSEMOTION && relmouse)
			{
				level->look_side += (float)event.motion.xrel / 600;
				level->look_up -= (float)event.motion.yrel / 600;
			}
			/*else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				level->look_side += 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				level->look_side -= 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP)
				level->look_up += 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				level->look_up -= 0.1;*/
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_N)
				player_movement(NULL, NULL);
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_M)
			{
				rendermode = rendermode == 2 ? 0 : rendermode + 1;
				level->quality = 1;
			}
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_TAB)
			{
				relmouse = relmouse ? 0 : 1;
				if (relmouse)
					SDL_SetRelativeMouseMode(SDL_TRUE);
				else
				{
					SDL_SetRelativeMouseMode(SDL_FALSE);
					SDL_WarpMouseInWindow(window->SDLwindow, RES_X / 2.0, RES_Y / 2.0);
				}
			}
		}
		level->pos = pos;
		t_obj *tmp = level->obj;
		int faces_visible = level->obj->tri_amount;
		int faces_left = culled[0].tri_amount;
		int faces_right = culled[0].tri_amount;
		if (rendermode == 1)
		{
			faces_visible = 0;
			culling(level, &faces_visible, culled);
		}
		else
		{
			for (int i = 0; i < level->obj->tri_amount; i++)
				culled[0].tris[i] = level->obj[0].tris[i];
			culled[0].tri_amount = level->obj[0].tri_amount;
		}
		button(&level->enable_fog, "fog");
		button(&rendermode, "culling");
		action_loop(window, level, &bmp, culled, &faces_left, &faces_right, rendermode);
		level->obj = tmp;

		frametime = SDL_GetTicks() - frametime;
		//printf("time: %d ms\n", frametime);
		char buf[50];
		int fps = get_fps(0);
		set_quality(frametime, level, fps);
		sprintf(buf, "%.2fhz %dfps %d(%dL %dR)faces quality: %d", physhz, fps, faces_visible, faces_left, faces_right, level->quality);
		SDL_SetWindowTitle(window->SDLwindow, buf);

		//SDL_Delay(2);
		//if (frametime < 100)
		//	usleep(10000);
	}
}
