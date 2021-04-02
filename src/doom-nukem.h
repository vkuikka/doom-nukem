/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/01 15:18:09 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H
# define RES_X 800
# define RES_Y 600
# define TICKRATE 128
# define THREAD_AMOUNT 8
# define NOISE_QUALITY_LIMIT 8

# define NOCLIP_SPEED 20.0 / TICKRATE
# define MOVE_SPEED 7.0 / TICKRATE
# define JUMP_SPEED 5.0 / TICKRATE
# define PLAYER_HEIGHT 1.75
# define WALL_CLIP_DIST 0.3
# define REFLECTION_DEPTH 3

# define TRUE 1
# define FALSE 0

# define WF_UNSELECTED_COL 0x333333ff
# define WF_SELECTED_COL 0xffaa00ff
# define WF_NOT_QUAD_WARNING_COL 0x802222ff
# define WF_NORMAL_COL 0xff0000ff
# define WF_VISIBLE_NORMAL_COL 0x00ffffff
# define WF_VISIBLE_COL 0x00ff00ff
# define WF_BACKGROUND_COL 0x99		//1 byte value

# define UI_FONT_SIZE 13
# define UI_EDITOR_SETTINGS_TEXT_COLOR 0x4444ffff
# define UI_LEVEL_SETTINGS_TEXT_COLOR 0xffffffff
# define UI_INFO_TEXT_COLOR 0xff5500ff
# define UI_FACE_SELECTION_TEXT_COLOR 0xffffffff
# define UI_BACKGROUND_COL 0x222222bb

# ifndef FLT_MAX
#  define FLT_MAX 3.40282347E+38
# endif

# include <math.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/syslimits.h>//for PATH_MAX && NAME_MAX
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

typedef struct			s_ivec2
{
	int					x;
	int					y;
}						t_ivec2;

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
	int					isenemy;
	float				opacity;
	float				reflectivity;
	// int					breakable;
	// int					broken;
	// int					*reflection_culling_mask;
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
	// struct s_obj		*all_objs;	//(if want to add multiple objects) array of objects in the level
	struct s_obj		all;		//all faces
	struct s_obj		visible;	//visible faces
	struct s_obj		*ssp;		//screen space partition
	struct s_vec3		pos;		//player position
	struct s_skybox		sky;
	float				look_side;	//side look angle
	float				look_up;	//up and down look angle
	struct s_editor_ui	*ui;
	int					shadow_color;
}						t_level;

typedef struct			s_ui_state
{
	int					ui_max_width;
	int					ui_text_y_pos;
	int					ui_text_x_offset;
	int					ui_text_color;
	int					m1down;
	char				*text;

	int					is_directory_open;
	char				*directory;
	char				*extension;
	void				(*open_file)(t_level*, char*);
}						t_ui_state;

typedef struct			s_editor_ui
{
	int					editor_active;
	int					noclip;
	int					wireframe;
	int					wireframe_on_top;
	int					raycast_quality;
	int					wireframe_culling_visual;
	int					fog;
	unsigned			fog_color;
	int					show_quads;
	int					pause_culling_position;
	int					backface_culling;
	int					distance_culling;
	float				render_distance;

	float				sun_contrast;
	float				direct_shadow_contrast;;
	struct s_vec3		sun_dir;

	//info
	float				physhz;
	unsigned			frametime;
	struct s_ui_state	state;
}						t_editor_ui;

typedef struct			s_physthread
{
	float				hz;
	struct s_level		*level;
	struct s_vec3		pos;
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
typedef struct			s_cast_result
{
	float				dist;
	int					color;
	float				transparent_dist;
	int					transparent_color;
}						t_cast_result;

void		vec_normalize(t_vec3 *vec);						//makes vector length 1
float		vec_dot(t_vec3 ve1, t_vec3 ve2);			//dot product of two vectors
float		vec_length(t_vec3 vec);
void		vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v);
void		vec_rot(t_vec3 *res, t_vec3 ve1, float ang);
int			vec_cmp(t_vec3 ve1, t_vec3 ve2);
void		vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_copy(t_vec3 *res, t_vec3 vec);
void		vec2_copy(t_vec2 *res, t_vec2 vec);
float		vec_angle(t_vec3 v1, t_vec3 v2);
void		vec_mult(t_vec3 *res, float mult);
void		vec_div(t_vec3 *res, float div);

void		init_window(t_window **window);
t_level		*init_level(void);

int			raycast(void *t);
float		cast_face(t_tri t, t_ray ray, int *col, t_bmp *img);
void		fill_pixels(unsigned *grid, int pixel_gap);
unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade, unsigned r1);
int			face_color(float u, float v, t_tri t, t_bmp *img);

void		wireframe(t_window *window, t_level *level);

void		load_obj(char *filename, t_obj *obj);
t_bmp		bmp_read(char *str);

void		culling(t_level *level);
void		find_quads(t_obj *obj);

void		rotate_vertex(float angle, t_vec3 *vertex, int axis);
void		rot_cam(t_vec3 *cam, const float lon, const float lat);

void		init_ui(t_window *window, t_level *level);
void		ui_render(t_level *level);
void		ui_config(t_level *level);
void		set_text_color(int color);
void		text(char *text);
void		button(int *var, char *text);
void		int_slider(int *var, char *str, int min, int max);
void		float_slider(float *var, char *str, float min, float max);
int			call(char *str, void (*f)(t_level*), t_level *level);
void		file_browser(char *str, char *extension, void (*f)(t_level*, char*));
void		path_up_dir(char *path);

void		init_physics(t_level *level);
void		physics_sync(t_level *level, t_physthread *get_data);
void		player_movement(t_vec3 *pos, t_level *level);
int			physics(void *data_pointer);

void		enemies_update_physics(t_level *level);
void		enemies_update_sprites(t_level *level);

int			fog(int color, float dist, unsigned fog_color, t_level *level);
int			skybox(t_level l, t_ray r);

void		transparency(t_ray r, t_obj *obj, t_bmp *texture, t_cast_result *res);
float		shadow(t_ray r, t_rthread *t, t_tri hit);
int			reflection(t_ray *r, t_rthread *t, t_tri hit, int depth);
float		wave_shader(t_vec3 mod);

void		select_face(t_level *level, int x, int y);

#endif
