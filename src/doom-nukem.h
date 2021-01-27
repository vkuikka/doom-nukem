/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/26 02:28:34 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H

# define RES_X 800.0
# define RES_Y 600.0
# define THREAD_AMOUNT 20

# define MOVE_SPEED 0.1
# define NOCLIP_SPEED 0.8

# define WALL_CLIP_DIST 0.3

# include <math.h>
# include <fcntl.h>
# include <sys/time.h>
# include "get_next_line.h"
# include "SDL2/SDL.h"

# include <stdlib.h>
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

typedef struct			s_vec2
{
	float				x;
	float				y;
}						t_vec2;

typedef struct			s_tri
{
	struct s_vert		verts[4];	//vertex coordinates of 3d triangle
	float				v0v1[3];	//vector between vertices 1 and 0
	float				v0v2[3];	//vector between vertices 2 and 0
	int					isquad;
}						t_tri;

typedef struct			s_obj
{
	struct s_tri		*tris;		//array of triangles that make the object
	int					tri_amount;	//amount of triangles
	int					*backface_culling_mask;
	int					*distance_culling_mask;
}						t_obj;

typedef struct			s_level
{
	struct s_obj		*obj;		//array of objects in the level
	float				pos[3];		//player position vector
	// float				dir[3];		//vector from player position to indicate ray direction in the middle of screen
	float				look_side;		//using angle instead of direction vector for testing
	float				look_up;
	int					*txtr;		//pointer to the texture as a 2d array of pixel colors
	int					quality;
	unsigned			fog_color;
}						t_level;

typedef struct			s_rthread
{
	int					id;
	struct s_level		*level;
	struct s_window		*window;
	struct s_bmp		*img;
}						t_rthread;

typedef struct __attribute__((__packed__))	s_bmp_fileheader {
	unsigned char							fileMarker1;
	unsigned char							fileMarker2;
	unsigned int							bfSize;
	uint16_t								unused1;
	uint16_t								unused2;
	unsigned int							imageDataOffset;
}											t_bmp_fileheader;

typedef struct __attribute__((__packed__))	s_bmp_infoheader {
	unsigned int							biSize;
	int										width;
	int										height;
	uint16_t								planes;
	uint16_t								bitPix;
	unsigned int							biCompression;
	unsigned int							biSizeImage;
	int										biXPelsPerMeter;
	int										biYPelsPerMeter;
	unsigned int							biClrUsed;
	unsigned int							biClrImportant;
}											t_bmp_infoheader;

typedef struct __attribute__((__packed__))	s_bmp_image {
	unsigned char							b;
	unsigned char							g;
	unsigned char							r;
	unsigned char							a;
}											t_bmp_image;

typedef struct								s_bmp
{
	int										width;
	int										height;
	int										*image;
}											t_bmp;

void		vec_normalize(float vec1[3]);						//makes vector length 1
float		vec_dot(float ve1[3], float ve2[3]);				//dot product of two vectors
float		vec_length(float vec[3]);
void		vec_sub(float res[3], float ve1[3], float ve2[3]);
void		vec_add(float res[3], float ve1[3], float ve2[3]);
void		vec_cross(float res[3], float u[3], float v[3]);
void		vec_rot(float res[3], float ve1[3], float ang);		//rotates atound y axis
int			vec_cmp(float ve1[3], float ve2[3]);
void		vec_avg(float res[3], float ve1[3], float ve2[3]);
void		vec_copy(float res[3], float ve[3]);
void		vec2_copy(float res[2], float ve[2]);

void		init_window(t_window **window);

void		draw_line(int line[4], t_window *window);

int			render(void *t);
float		cast_face(t_tri t, t_ray ray, int *col, t_bmp *img);
t_level		*init_level();

void		fill_pixels(unsigned *grid, int pixel_gap);
void		load_obj(char *filename, t_obj *obj);
t_bmp		bmp_read(char *str);

void		culling(t_level *level, int *visible, t_obj *culled);
void		find_quads(t_obj *obj);

int			face_color(float u, float v, t_tri t, t_bmp *img);
// int			skybox_color(t_ray r, t_bmp *img);

#endif
