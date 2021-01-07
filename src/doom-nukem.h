/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/07 10:44:26 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H

# define RES_X 800.0
# define RES_Y 600.0
# define THREAD_AMOUNT 100

# include <math.h>
# include <fcntl.h>
# include <sys/time.h>
# include <pthread.h>
# include "get_next_line.h"
# include "SDL2/SDL.h"

# include <stdio.h>

typedef struct			s_window
{
	SDL_Renderer		*SDLrenderer;
	SDL_Window			*SDLwindow;
	SDL_Texture			*texture;
	unsigned			*pixels;
}						t_window;

typedef struct			s_ray		//only position and direction will probably be used
{
	float				pos[3];
	float				dir[3];
	// float				normal[3];	//normal of intersection point
	// float				reflect[3];	//reflected ray direction from intersection point
	// int					obj_index;	//prevent ray from intersecting the object it is cast from in bounces
}						t_ray;

typedef struct			s_vert
{
	float				pos[3];		//world position in 3d
	float				txtr[2];	//position of the vert in the texture
}						t_vert;

typedef struct			s_tri
{
	struct s_vert		verts[3];	//vertex coordinates of 3d triangle
	float				v0v1[3];	//vector between vertexes 1 and 0
	float				v0v2[3];	//vector between vertexes 2 and 0
}						t_tri;

typedef struct			s_obj
{
	struct s_tri		tris[3];	//triangles that make up the object
	int					***txtr;	//pointer to the texture as a 2d array of pixel colors
}						t_obj;

typedef struct			s_world
{
	struct s_objects	*obj;			//array of objects in the level
	float				player_pos[3];
	float				player_dir[3];	//vector from player position to indicate ray direction in the middle of screen?????????
}						t_world;

typedef struct			s_rthread
{
	int					id;
	struct s_player		*player;
	struct s_window		*window;
}						t_rthread;

typedef struct			s_player
{
	float				posz;
	float				posx;
	float				angle;
}						t_player;

void		vec_normalize(float vec1[3]);						//makes vector length 1
void		vec_normalize_two(float vec1[3], float vec2[3]);	//longest vector length will be 1 and other will be same scale
float		vec_dot(float ve1[3], float ve2[3]);				//dot product of two vectors
float		vec_length(float vec[3]);
void		vec_sub(float res[3], float ve1[3], float ve2[3]);
void		vec_cross(float res[3], float u[3], float v[3]);
void		vec_rot(float res[3], float ve1[3], float ang);		//rotates atound y axis

void		init_window(t_window **window);

void		draw_line(int line[4], t_window *window);

void		*rt_test(void *t);

#endif
