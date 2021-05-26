/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/26 19:58:22 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H
# define RES_X 1000
# define RES_Y 700
# define THREAD_AMOUNT 8
# define NOISE_QUALITY_LIMIT 8
# define SSP_MAX_X 20
# define SSP_MAX_Y 20

# define NOCLIP_SPEED 50.0
# define GRAVITY 12		//	m/s^2
# define JUMP_SPEED 5	//	m/s
# define AIR_ACCEL .3	//	m/s^2
# define MOVE_ACCEL 70	//	m/s^2
# define RUN_SPEED 10	//	m/s
# define WALK_SPEED 4	//	m/s
# define CROUCH_SPEED 3	//	m/s
# define GROUND_FRICTION 5.
# define PLAYER_HEIGHT 1.75
# define PLAYER_HEIGHT_MAGIC 1.3
# define CROUCHED_HEIGHT 1
# define WALL_CLIP_DIST 0.3
# define REFLECTION_DEPTH 3
# define WALKABLE_NORMAL_MIN_Y 0.75
# define DOOR_ACTIVATION_DISTANCE 3.
# define DOOR_LOCATION_INFO_COLOR 0x880088ff
# define LIGHT_LOCATION_INFO_COLOR 0xffdd00ff

# define ENEMY_MOVABLE_HEIGHT_DIFF 1
# define MAX_PROJECTILE_TRAVEL 100

# define TRUE 1
# define FALSE 0

# define WF_PROJECTILE_COL 0xff4422ff
# define WF_UNSELECTED_COL 0x333333ff
# define WF_SELECTED_COL 0xffaa00ff
# define WF_VERT_COL 0x010101ff
# define WF_NOT_QUAD_WARNING_COL 0x802222ff
# define WF_NORMAL_COL 0xff0000ff
# define WF_VISIBLE_NORMAL_COL 0x00ffffff
# define WF_VISIBLE_COL 0x00ff00ff
# define WF_BACKGROUND_COL 0x99		//1 byte value
# define WF_NORMAL_LEN 0.3

# define HUD_TEXT_COLOR 0xff6666bb
# define HUD_FONT_SIZE 42
# define MAIN_MENU_FONT_SIZE 30
# define MAIN_MENU_BUTTON_AMOUNT 4
# define MAIN_MENU_FONT_BACKGROUND_COLOR 0xffffff55
# define MAIN_MENU_FONT_COLOR 0x000000ff
# define MAIN_MENU_FONT_PADDING_MULTIPLIER 1.5
# define CROSSHAIR_COLOR 0xff0000ff
# define PLAYER_HEALTH_MAX 100
# define PLAYER_AMMO_MAX 30
# define DEATH_LENGTH_SEC 5
# define DEATH_OVERLAY_COLOR 0xff000088
# define VIEWMODEL_FRAMES 6
# define VIEWMODEL_ANIM_FPS 2.0

# define NONFATAL_ERROR_LIFETIME_SECONDS 7.42
# define NONFATAL_ERROR_FADEOUT_TIME_MS 666
# define UI_ERROR_COLOR 0xff000000
# define GIZMO_SCALE_DIVIDER 4
# define UI_FONT_SIZE 13
# define UI_EDITOR_SETTINGS_TEXT_COLOR 0x4444ffff
# define UI_LEVEL_SETTINGS_TEXT_COLOR 0xffffffff
# define UI_INFO_TEXT_COLOR 0xff5500ff
# define UI_FACE_SELECTION_TEXT_COLOR 0xffffffff
# define UI_BACKGROUND_COL 0x222222bb
# define UI_ELEMENT_HEIGHT 14
# define UI_PADDING 2
# define UI_PADDING_4 4
# define UV_EDITOR_Y_OFFSET UI_ELEMENT_HEIGHT + UI_PADDING * 2
# define UI_LEVEL_BAKED_COLOR 0x33aa33ff
# define UI_LEVEL_NOT_BAKED_COLOR 0xcc3333ff

# define UV_PADDING 3

# define SERIALIZE_INITIAL_BUFFER_SIZE 666
# define OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER 10

# define AUDIO_VOLUME_INIT MIX_MAX_VOLUME / 10
# define AUDIO_MUSIC "Audio/Music/str4E.ogg"
# define AUDIO_GUNSHOT "Audio/SoundEffects/gunshot.wav"
# define AUDIO_GUNSHOT_CHANNEL 0
# define AUDIO_JUMP "Audio/SoundEffects/jump.wav"
# define AUDIO_JUMP_CHANNEL 1
# define AUDIO_RELOAD "Audio/SoundEffects/reload.wav"
# define AUDIO_RELOAD_CHANNEL 2
# define AUDIO_DOOR "Audio/SoundEffects/door.wav"
# define AUDIO_DOOR_CHANNEL 3
# define AUDIO_DEATH "Audio/SoundEffects/osrsDeath.wav"
# define AUDIO_DEATH_CHANNEL -1

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
# include "SDL2_ttf/SDL_ttf.h"
# include "SDL2_mixer/SDL_mixer.h"

# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>

typedef struct			s_audio
{
	float				music_volume;
	float				sound_effect_volume;
	Mix_Music			*music;
	Mix_Chunk			*gunshot;
	Mix_Chunk			*jump;
	Mix_Chunk			*reload;
	Mix_Chunk			*death;
	Mix_Chunk			*door;
}						t_audio;

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

typedef struct			s_rect
{
	int					x;
	int					y;
	int					w;
	int					h;
}						t_rect;

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

typedef struct			s_uv_parameters
{
	struct s_vec2		scale;
	struct s_tri		*tri;
	struct s_vec2		offset;
	unsigned			*pixels;
}						t_uv_parameters;

typedef struct			s_projectile
{
	struct s_vec3		dir;
	float				speed;
	float				dist;
	float				damage;
}						t_projectile;

typedef struct			s_enemy
{
	struct s_vec3		dir;
	struct s_vec2		projectile_uv[3];
	float				move_speed;
	float				dist_limit;
	float				initial_health;
	float				remaining_health;
	float				attack_range;
	float				attack_frequency;
	float				projectile_speed;	//	0 = no projectile
	float				projectile_scale;
	float				attack_damage;
	float				current_attack_delay;
}						t_enemy;

typedef struct			s_tri
{
	int					index;
	struct s_vert		verts[4];	//vertex coordinates of 3d triangle
	struct s_vec3		v0v1;		//vector between vertices 1 and 0
	struct s_vec3		v0v2;		//vector between vertices 2 and 0
	struct s_vec3		normal;
	int					isenemy;
	struct s_enemy		*enemy;
	int					isprojectile;
	struct s_projectile	*projectile;
	int					isquad;
	int					isgrid;
	int					disable_distance_culling;
	int					disable_backface_culling;
	float				opacity;
	float				reflectivity;
	float				refractivity;
	int					shader;
	int					selected;
	struct s_obj		*opacity_obj_all;
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

typedef struct			s_door
{
	int					indice_amount;
	int					*indices;
	int					*isquad;
	t_vec3				**pos1;
	t_vec3				**pos2;
	float				transition_time;
	unsigned			transition_start_time;
	int					transition_direction;
}						t_door;

typedef struct			s_all_doors
{
	struct s_door		*door;
	int					door_amount;
	int					selected_index;
}						t_all_doors;

typedef enum			e_mouse_location
{
	MOUSE_LOCATION_GAME = 0,
	MOUSE_LOCATION_UI,
	MOUSE_LOCATION_UV_EDITOR,
	MOUSE_LOCATION_GIZMO_X,
	MOUSE_LOCATION_GIZMO_Y,
	MOUSE_LOCATION_GIZMO_Z,
	MOUSE_LOCATION_MAIN_MENU,
	MOUSE_LOCATION_LIGHT_EDITOR,
	MOUSE_LOCATION_SELECTION
}						t_mouse_location;

typedef enum			e_game_state
{
	GAME_STATE_MAIN_MENU = 0,
	GAME_STATE_EDITOR,
	GAME_STATE_INGAME,
	GAME_STATE_DEAD
}						t_game_state;

typedef enum			e_ui_location
{
	UI_LOCATION_MAIN = 0,
	UI_LOCATION_FILE_OPEN,
	UI_LOCATION_FILE_SAVE,
	UI_LOCATION_UV_EDITOR,
	UI_LOCATION_SETTINGS,
	UI_LOCATION_DOOR_EDITOR,
	UI_LOCATION_LIGHT_EDITOR
}						t_ui_location;

struct					s_level;
typedef struct			s_ui_state
{
	enum e_ui_location	ui_location;
	int					ui_max_width;
	int					ui_text_y_pos;
	int					ui_text_x_offset;
	int					ui_text_color;
	char				*text;

	char				**error_message;
	unsigned			*error_start_time;
	int					error_amount;

	int					mouse_capture;
	int					m1_click;
	int					m1_drag;
	enum e_mouse_location	mouse_location;
	float				uv_zoom;
	struct s_vec2		uv_pos;

	char				*save_filename;
	int					text_input_enable;
	int					ssp_visual;

	char				*directory;
	char				*extension;
	void				(*open_file)(struct s_level*, char*);
}						t_ui_state;

typedef struct			s_editor_ui
{
	int					editor_active;
	int					noclip;
	int					vertex_select_mode;
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
	unsigned			ssp;
	unsigned			cull;
	unsigned			render;
	unsigned			frametime;
	struct s_ui_state	state;
}						t_editor_ui;

typedef struct			s_light
{
	t_vec3				pos;
	float				radius;
	float				brightness;
}						t_light;

typedef struct			s_player_pos
{
	struct s_vec3		pos;
	float				look_side;
	float				look_up;
}						t_player_pos;

typedef struct			s_level
{
	// struct s_obj		*all_objs;	//(if want to add multiple objects) array of objects in the level
	struct s_obj		all;		//all faces
	struct s_obj		visible;	//visible faces
	struct s_obj		*ssp;		//screen space partition
	struct s_bmp		texture;
	struct s_bmp		normal_map;
	struct s_bmp		spray;
	struct s_bmp		baked;
	int					is_baked;
	struct s_skybox		sky;
	struct s_camera		cam;
	struct s_editor_ui	ui;
	struct s_all_doors	doors;
	struct s_light		*lights;
	int					light_amount;
	int					selected_light_index;
	struct s_enemy		player;
	int					shadow_color;
	int					player_health;
	int					player_ammo;
	struct s_player_pos	spawn_pos;
	struct s_bmp		main_menu_title;
	struct s_player_pos	main_menu_pos1;
	struct s_player_pos	main_menu_pos2;
	unsigned			main_menu_anim_time;
	unsigned			main_menu_anim_start_time;
	unsigned			death_start_time;
	struct s_vec3		player_vel;
	unsigned			reload_start_time;
	int					viewmodel_index;
	struct s_bmp		viewmodel[VIEWMODEL_FRAMES];
	float				brightness;
	struct s_audio		audio;
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
	struct s_bmp		*baked;
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
float		cast_all(t_ray vec, t_level *level, float *dist_u, float *dist_d, int *index);
void		fill_pixels(unsigned *grid, int pixel_gap, int blur, int smooth);
unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade, unsigned alpha);
void		face_color(float u, float v, t_tri t, t_cast_result *res);
void		wireframe(t_window *window, t_level *level);
void		camera_offset(t_vec3 *vertex, t_camera *cam);
SDL_Color	get_sdl_color(unsigned color);

void		load_obj(char *filename, t_obj *obj);
t_bmp		bmp_read(char *str);

void		culling(t_level *level);
int			occlusion_culling(t_tri tri, t_level *level);
void		init_culling(t_level *level);
void		free_culling(t_level *level);
void		reflection_culling(t_level *level, int i);
void		find_quads(t_obj *obj);
void		set_fourth_vertex(t_tri *a);

void		rotate_vertex(float angle, t_vec3 *vertex, int axis);
void		rot_cam(t_vec3 *cam, const float lon, const float lat);

void		init_enemy(t_tri *face);
void		init_ui(t_window *window, t_level *level);
void		init_player(t_enemy *player);
void		init_audio(t_level *level);
void		ui_render(t_level *level);
void		ui_config(t_level *level);
void		set_text_color(int color);
void		text(char *text);
int			button(int *var, char *text);
void		int_slider(int *var, char *str, int min, int max);
void		float_slider(float *var, char *str, float min, float max);
int			call(char *str, void (*f)(t_level*), t_level *level);
void		file_browser(char *str, char *extension, void (*f)(t_level*, char*));
void		file_save(char *str, char *extension, void (*f)(t_level*, char*));
void		path_up_dir(char *path);
void		go_in_dir(char *path, char *folder);
void		text_input(char *str, t_level *level);
void		find_closest_mouse(t_vec3 *vert, int *i, int *k);
void		render_text(char *text, t_window *window, t_ivec2 *pos, SDL_Texture *get_texture);

void		main_menu(t_level *level, t_window *window, t_game_state *game_state);
void		main_menu_move_background(t_level *level);
void		hud(t_level *level, t_window *window, t_game_state game_state);
void		create_projectile(t_level *level, t_vec3 pos, t_vec3 dir, t_enemy *enemy);

void		uv_editor(t_level *level, t_window *window);
void		enable_uv_editor(t_level *level);
void		obj_editor(t_level *level, t_window *window);
void		obj_editor_input(t_level *level);

void		player_movement(t_level *level, t_game_state game_state);

void		enemies_update_physics(t_level *level);
void		enemies_update_sprites(t_level *level);

int			fog(int color, float dist, unsigned fog_color, t_level *level);
int			skybox(t_bmp *img, t_obj *obj, t_ray r);

void		opacity(t_cast_result *res, t_level *l, t_obj *obj, float opacity);
void		shadow(t_level *l, t_vec3 normal, t_cast_result *res);

void		reflection(t_cast_result *res, t_level *l, t_obj *obj);

unsigned	wave_shader(t_vec3 mod, t_vec3 *normal, unsigned col1, unsigned col2);

void		select_face(t_camera *cam, t_level *level, int x, int y);
void		deselect_all_faces(t_level *level);

void		save_level(t_level *level);
void		open_level(t_level *level, char *filename);

void		cast_all_color(t_ray r, t_level *l, t_obj *obj, t_cast_result *res);
int			cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri);
int			cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri);

void		fix_uv_overlap(t_level *level);
int			tri_uv_intersect(t_tri t1, t_tri t2);
int			point_in_tri(t_vec2 pt, t_vec2 v1, t_vec2 v2, t_vec2 v3);
void		toggle_selection_all(t_level *level);
void		add_face(t_level *level);
void		remove_faces(t_level *level);
void		nonfatal_error(t_level *level, char *message);
t_ivec2		put_text(char *text, t_window *window, SDL_Texture *texture, t_ivec2 pos);
void		set_new_face(t_level *level, t_vec3 pos, t_vec3 dir, float scale);

t_vec3		vec_interpolate(t_vec3 a, t_vec3 b, float f);
float		lerp(float a, float b, float f);

void		door_animate(t_level *level);
void		door_put_text(t_window *window, t_level *level);
void		add_new_door(t_level *level);
void		delete_door(t_level *level);
void		door_activate(t_level *level);
void		set_door_pos_1(t_level *level);
void		set_door_pos_2(t_level *level);
void		enable_door_editor(t_level *level);
void		find_selected_door_index(t_level *level);
void		lights(t_level *l, t_vec3 pos, unsigned *color);
unsigned	brightness(unsigned color1, float brightness, unsigned alpha);
int			nothing_selected(t_level *level);
void		light_put_text(t_window *window, t_level *level);
void		enable_light_editor(t_level *level);
void		add_light(t_level *level);
void		select_light(t_level *level, int x, int y);
void		delete_light(t_level *level);
void		bake(t_level *l);

#endif
