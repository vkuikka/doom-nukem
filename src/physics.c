#include "doom-nukem.h"

float	        cast_all(t_ray vec, t_level *l, float *dist_u, float *dist_d, int *index)
{
	float	res = FLT_MAX;

	vec_normalize(&vec.dir);
	for (int j = 0; j < l->allfaces->tri_amount; j++)
	{
		float tmp;
		tmp = cast_face(l->allfaces->tris[j], vec, NULL, NULL);
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

t_vec3	        player_input(int noclip, t_level *level, int in_air, t_vec3 vel)
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

void	        player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level)
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

void	        player_movement(t_vec3 *pos, t_level *level)
{
	static t_vec3	vel = {0, 0, 0};
	static int		noclip = TRUE;
	static float	asd = 0;
	t_ray			r;
	float			dist;
	int				in_air;

	global_seginfo = "player_movement\n";
	if (level == NULL)
	{
		noclip = noclip ? FALSE : TRUE;
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
		in_air = FALSE;
		if (dist < PLAYER_HEIGHT)
			pos->y -= PLAYER_HEIGHT - dist;
	}
	else
		in_air = TRUE;
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


float	        get_hz(void)
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

unsigned long   getTimeInNanoseconds(void) {
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

int     	    physics(void *data_pointer)
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

