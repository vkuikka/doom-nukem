/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/02 18:25:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H
# define RES_X 1000
# define RES_Y 700
# define THREAD_AMOUNT 4
# define NOISE_QUALITY_LIMIT 8
# define SSP_MAX_X 20
# define SSP_MAX_Y 20

# define NOCLIP_SPEED 50.0
# define GRAVITY 12		//	m/s^2
# define JUMP_SPEED 5	//	m/s
# define AIR_ACCEL .3	//	m/s^2
# define MOVE_ACCEL 70	//	m/s^2
# define MOVE_SPEED 10	//	m/s
# define GROUND_FRICTION 5.
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
# define UI_ELEMENT_HEIGHT 14
# define UI_PADDING 2
# define UV_EDITOR_Y_OFFSET UI_ELEMENT_HEIGHT + UI_PADDING * 2

# define UV_PADDING 3

# define SERIALIZE_INITIAL_BUFFER_SIZE 666
# define OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER 10

# include <math.h>
# include <fcntl.h>
# ifdef __APPLE__
#  include <dirent.h>
#  include <sys/syslimits.h>//for PATH_MAX && NAME_MAX
#  include <sys/stat.h>
#  include <arpa/inet.h>
# elif _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <float.h>
#  include <winsock.h>
#  define NAME_MAX 2000
# endif
# include "get_next_line.h"
# include "SDL2/SDL.h"
# include "SDL2/SDL_ttf.h"

# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>

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
	int					index;
	struct s_vert		verts[4];	//vertex coordinates of 3d triangle
	struct s_vec3		v0v1;		//vector between vertices 1 and 0
	struct s_vec3		v0v2;		//vector between vertices 2 and 0
	struct s_vec3		normal;
	int					isquad;
	int					isgrid;
	int					isenemy;
	int					disable_distance_culling;
	int					disable_backface_culling;
	float				opacity;
	float				reflectivity;
	int					shader;
	int					selected;
	struct s_obj		*reflection_obj_all;
	struct s_obj		*reflection_obj_first_bounce;
	struct s_obj		*shadow_faces;
	// int					breakable;
	// int					broken;
	// int					*reflection_culling_mask;
}						t_tri;

typedef struct			s_obj
{
	struct s_tri		*tris;		//array of triangles that make the object
	int					tri_amount;	//amount of triangles
}						t_obj;

typedef struct			s_skybox
{
	struct s_bmp		img;
	struct s_obj		all;
	struct s_obj		visible;
}						t_skybox;

typedef struct			s_camera
{
	t_vec3				up;
	t_vec3				side;
	t_vec3				front;
	t_vec3				pos;
	float				look_side;	//side look angle
	float				look_up;	//up and down look angle
	float				lon;
	float				lat;
	float				fov_y;
	float				fov_x;
}						t_camera;

typedef enum			e_ui_location
{
	UI_UV_SETTINGS = 1,
	UI_UV_EDITOR,
	UI_SERIALIZE,
	UI_DIRECTORY
}						t_ui_location;

typedef enum			e_mouse_location
{
	MOUSE_LOCATION_GAME = 0,
	MOUSE_LOCATION_UI,
	MOUSE_LOCATION_UV_EDITOR,
	MOUSE_LOCATION_GIZMO,
	MOUSE_LOCATION_SELECTION
}						t_mouse_location;

struct					s_level;
typedef struct			s_ui_state
{
	int					ui_max_width;
	int					ui_text_y_pos;
	int					ui_text_x_offset;
	int					ui_text_color;
	char				*text;

	int					mouse_capture;
	int					m1_click;
	int					m1_drag;
	enum e_mouse_location	mouse_location;
	int					is_uv_editor_open;
	float				uv_zoom;
	struct s_vec2		uv_pos;

	int					is_serialize_open;
	char				*save_filename;
	int					text_input_enable;
	int					ssp_visual;

	int					is_directory_open;
	char				*directory;
	char				*extension;
	void				(*open_file)(struct s_level*, char*);
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
	int					blur;
	int					smooth_pixels;
	unsigned			fog_color;
	int					show_quads;
	int					pause_culling_position;
	int					backface_culling;
	int					distance_culling;
	float				render_distance;
	float				fov;

	float				sun_contrast;
	float				direct_shadow_contrast;;
	struct s_vec3		sun_dir;
	float				horizontal_velocity;

	//info
	unsigned			frametime;
	struct s_ui_state	state;
}						t_editor_ui;

typedef struct			s_level
{
	// struct s_obj		*all_objs;	//(if want to add multiple objects) array of objects in the level
	struct s_obj		all;		//all faces
	struct s_obj		visible;	//visible faces
	struct s_obj		*ssp;		//screen space partition
	struct s_bmp		texture;
	struct s_bmp		normal_map;
	struct s_skybox		sky;
	struct s_camera		cam;
	struct s_editor_ui	ui;
	int					shadow_color;
}						t_level;

typedef struct			s_rthread
{
	int					id;
	struct s_level		*level;
	struct s_window		*window;
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

typedef struct __attribute__((__packed__))	s_bmp_pixel_32 {
	unsigned char							b;
	unsigned char							g;
	unsigned char							r;
	unsigned char							a;
}											t_bmp_pixel_32;

typedef struct __attribute__((__packed__))	s_bmp_pixel_24 {
	unsigned char							b;
	unsigned char							g;
	unsigned char							r;
}											t_bmp_pixel_24;

typedef struct			s_cast_result
{
	float				u;
	float				v;
	float				dist;
	int					color;
	int					face_index;
	int					reflection_depth;
	struct s_vec3		normal;
	struct s_ray		ray;
	struct s_bmp		*normal_map;
	struct s_bmp		*texture;
}						t_cast_result;

typedef struct			s_buffer
{
	void				*data;
	int					next;
	size_t				size;
}						t_buffer;

void		vec_normalize(t_vec3 *vec);						//makes vector length 1
float		vec_dot(t_vec3 ve1, t_vec3 ve2);			//dot product of two vectors
float		vec_length(t_vec3 vec);
void		vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void		vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v);
void		vec_rot(t_vec3 *res, t_vec3 ve1, float ang);
int			vec_cmp(t_vec3 ve1, t_vec3 ve2);
void		vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
float		vec_angle(t_vec3 v1, t_vec3 v2);
void		vec_mult(t_vec3 *res, float mult);
void		vec_div(t_vec3 *res, float div);
float		vec2_length(t_vec2 vec);
void		vec2_avg(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void		vec2_sub(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void		vec2_add(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);

void		init_window(t_window **window);
void		init_level(t_level **level);

void		screen_space_partition(t_level *level);
void		init_screen_space_partition(t_level *level);
int			get_ssp_index(int x, int y);
int			get_ssp_coordinate(int coord, int horizontal);

int			raycast(void *t);
float		cast_face(t_tri t, t_ray ray, t_cast_result *res);
void		fill_pixels(unsigned *grid, int pixel_gap, int blur, int smooth);
unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade);
void		face_color(float u, float v, t_tri t, t_cast_result *res);

void		wireframe(t_window *window, t_level *level);
void		camera_offset(t_vec3 *vertex, t_camera *cam);

void		load_obj(char *filename, t_obj *obj);
t_bmp		bmp_read(char *str);

void		culling(t_level *level);
int			occlusion_culling(t_tri tri, t_level *level);
void		init_reflection_culling(t_level *level);
void		reflection_culling(t_level *level, int i);
void		free_reflection_culling(t_level *level);
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
void		file_save(char *str, char *extension, void (*f)(t_level*, char*));
void		path_up_dir(char *path);
void		go_in_dir(char *path, char *folder);
void		text_input(char *str, t_level *level);

void		uv_editor(t_level *level, t_window *window);
void		enable_uv_editor(t_level *level);
void		disable_uv_editor(t_level *level);

void		player_movement(t_level *level);

void		enemies_update_physics(t_level *level);
void		enemies_update_sprites(t_level *level);

int			fog(int color, float dist, unsigned fog_color, t_level *level);
int			skybox(t_bmp *img, t_obj *obj, t_ray r);

void		opacity(t_cast_result *res, t_level *l, t_obj *obj);
void		shadow(t_level *l, t_vec3 normal, t_cast_result *res);

void		reflection(t_cast_result *res, t_level *l, t_obj *obj);

unsigned	wave_shader(t_vec3 mod, t_vec3 *normal, unsigned col1, unsigned col2);

void		select_face(t_camera *cam, t_level *level, int x, int y);

void		save_level(t_level *level);
void		open_level(t_level *level, char *filename);

void		cast_all_color(t_ray r, t_level *l, t_obj *obj, t_cast_result *res);
int			cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri);
int			cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri);

void		fix_uv_overlap(t_level *level);
int			tri_uv_intersect(t_tri t1, t_tri t2);
int			point_in_tri(t_vec2 pt, t_vec2 v1, t_vec2 v2, t_vec2 v3);

#endif
