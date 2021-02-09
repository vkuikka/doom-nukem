/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/02/09 19:35:20 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H
# define RES_X 800.0
# define RES_Y 600.0
# define THREAD_AMOUNT 20
# define TARGETFPS 35
# define TICKRATE 128

# define MOVE_SPEED 10.0 / TICKRATE
# define NOCLIP_SPEED 30.0 / TICKRATE

# define WALL_CLIP_DIST 0.3

#ifndef FLT_MAX
#define FLT_MAX 3.40282347E+38
#endif

# include <math.h>
# include <fcntl.h>
# include <sys/time.h>
# include "get_next_line.h"
# include "SDL2/SDL.h"
# include "SDL2/SDL_ttf.h"

# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
char		*global_seginfo;

typedef struct			s_bmp
{
	int					width;
	int					height;
	int					*image;
}						t_bmp;

typedef struct			s_window
{
	SDL_Renderer		*SDLrenderer;
	SDL_Window			*SDLwindow;
	SDL_Texture			*texture;
	unsigned int		*frame_buffer;
	float				*depth_buffer;
}						t_window;

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

typedef struct			s_ray
{
	struct s_vec3		pos;
	struct s_vec3		dir;
}						t_ray;

typedef struct			s_vert
{
	struct s_vec3		pos;		//world position in 3d
	struct s_vec2		txtr;		//texture position in 2d
	int					selected;
}						t_vert;

typedef struct			s_tri
{
	struct s_vert		verts[4];	//vertex coordinates of 3d triangle
	struct s_vec3		v0v1;		//vector between vertices 1 and 0
	struct s_vec3		v0v2;		//vector between vertices 2 and 0
	int					isquad;
	int					isgrid;
}						t_tri;

typedef struct			s_obj
{
	struct s_tri		*tris;		//array of triangles that make the object
	int					tri_amount;	//amount of triangles
	int					*backface_culling_mask;
	int					*distance_culling_mask;
}						t_obj;

typedef struct			s_skybox
{
	struct s_bmp		img;
	struct s_obj		obj;
}						t_skybox;

typedef struct			s_level
{
	struct s_obj		*obj;		//array of objects in the level
	struct s_obj		*allfaces;
	struct s_vec3		pos;		//player position
	struct s_skybox		sky;
	float				look_side;	//side look angle
	float				look_up;	//up and down look angle
	int					*txtr;		//pointer to the level texture
	int					quality;
	int					enable_fog;
	unsigned			fog_color;
}						t_level;

typedef struct			s_physthread
{
	float				*hz;
	struct s_level		*level;
	struct s_vec3		*pos;
}						t_physthread;

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

void		vec_normalize(t_vec3 *vec);						//makes vector length 1
float		vec_dot(t_vec3 ve1, t_vec3 ve2);			//dot product of two vectors
float		vec_length(t_vec3 vec);
void		vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v);
void		vec_rot(t_vec3 *res, t_vec3 ve1, float ang);		//rotates atound y axis
int			vec_cmp(t_vec3 ve1, t_vec3 ve2);
void		vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_copy(t_vec3 *res, t_vec3 vec);
void		vec2_copy(t_vec2 *res, t_vec2 vec);
float		vec_angle(t_vec3 v1, t_vec3 v2);
void		vec_mult(t_vec3 *res, int mult);

void		init_window(t_window **window);
t_level		*init_level();

int			render(void *t);
float		cast_face(t_tri t, t_ray ray, int *col, t_bmp *img);
void		fill_pixels(unsigned *grid, int pixel_gap);
unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade, unsigned r1);
int			face_color(float u, float v, t_tri t, t_bmp *img);

void		wireframe(t_window *window, t_level *level);

void		load_obj(char *filename, t_obj *obj);
t_bmp		bmp_read(char *str);

void		culling(t_level *level, int *visible, t_obj *culled);
void		find_quads(t_obj *obj);

void		rotate_vertex(float angle, t_vec3 *vertex, int axis);
void		rot_cam(t_vec3 *cam, const float lon, const float lat);

void		init_buttons(t_window *window);
void		draw_buttons(void);
void		button(int *var, char *text);

#endif
