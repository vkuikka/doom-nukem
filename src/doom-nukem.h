/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/15 03:38:37 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H

# define RES_X 800.0
# define RES_Y 600.0
# define THREAD_AMOUNT 50
# define PIXEL_GAP 3	//change to dynamic later

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
	unsigned int		*frame_buffer;
	float				*depth_buffer;
}						t_window;

typedef struct			s_ray		//only position and direction will probably be used
{
	float				pos[3];
	float				dir[3];
}						t_ray;

typedef struct			s_vert
{
	float				pos[3];		//world position in 3d
	float				txtr[2];	//position of the vert in the texture
}						t_vert;

typedef struct			s_ivec3
{
	int					x;
	int					y;
	int					z;
}						t_ivec3;

typedef struct			s_vec3
{
	float				x;
	float				y;
	float				z;
}						t_vec3;

typedef struct			s_tri
{
	struct s_vert		verts[3];	//vertex coordinates of 3d triangle
	float				v0v1[3];	//vector between vertices 1 and 0
	float				v0v2[3];	//vector between vertices 2 and 0
}						t_tri;

typedef struct			s_obj
{
	struct s_tri		*tris;		//array of triangles that make the object
	int					tri_amount;	//amount of triangles
}						t_obj;

typedef struct			s_level
{
	struct s_obj		*obj;		//array of objects in the level
	float				pos[3];		//player position vector
	// float				dir[3];		//vector from player position to indicate ray direction in the middle of screen
	float				look_side;		//using angle instead of direction vector for testing
	float				look_up;
	int					***txtr;	//pointer to the texture as a 2d array of pixel colors
}						t_level;

typedef struct			s_rthread
{
	int					id;
	struct s_level		*level;
	struct s_window		*window;
}						t_rthread;

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
t_level		*rt_test_init_level();
void		fill_pixels(unsigned *grid, int pixel_gap);
void		load_obj(char *filename, t_obj *obj);

#endif
