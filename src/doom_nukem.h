/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom_nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/17 18:37:12 by vkuikka          ###   ########.fr       */
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
# define AIR_ACCEL 8	//	m/s^2
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
# define DOOR_ACTIVATION_LOCATION_INFO_COLOR 0xcc2288ff
# define LIGHT_LOCATION_INFO_COLOR 0xffdd00ff

# define ENEMY_MOVABLE_HEIGHT_DIFF 1
# define MAX_PROJECTILE_TRAVEL 100
# define PROJECTILE_DAMAGE_DIST 3
# define SPRAY_LINE_PRECISION 2
# define SPRAY_MAX_DIST 15
# define SPRAY_FROM_VIEW_SIZE 0.5
# define FIND_QUADS_FLOAT_ERROR 0.0001

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
# define WF_BACKGROUND_COL 0x999999ff
# define WF_NORMAL_LEN 0.3

# define SSP_VISUAL_CHESSBOARD_1 0x66666644
# define SSP_VISUAL_CHESSBOARD_2 0x66666688
# define HUD_TEXT_COLOR 0xff6666bb
# define HUD_FONT_SIZE 42
# define HUD_GAME_EVENT_FONT_SIZE 130
# define HUD_GAME_EVENT_TEXT_COLOR 0xffffffff
# define CYCLE_RGB_LOOP_FPS 0.4
# define MAIN_MENU_FONT_SIZE 30
# define MAIN_MENU_BUTTON_AMOUNT 4
# define MAIN_MENU_FONT_BACKGROUND_COLOR 0xffffff55
# define MAIN_MENU_FONT_COLOR 0xff8a00ff
# define MAIN_MENU_FONT_PADDING_MULTIPLIER 1.5
# define CROSSHAIR_COLOR 0xff0000ff
# define INITIAL_LEVEL_WIN_DIST 3
# define WIN_LENGTH_SEC 15
# define PLAYER_HEALTH_MAX 100
# define PLAYER_AMMO_MAX 30
# define DEATH_LENGTH_SEC 5
# define DEATH_OVERLAY_COLOR 0xff000088
# define VIEWMODEL_FRAMES 10
# define VIEWMODEL_ANIM_FPS 2.0

# define NONFATAL_ERROR_LIFETIME_SECONDS 7.42
# define NONFATAL_ERROR_FADEOUT_TIME_MS 666
# define UI_ERROR_COLOR 0xff000000
# define GIZMO_SCALE_DIVIDER 4
# define UI_SLIDER_WIDTH 100
# define UI_SLIDER_BUTTON_WIDTH 5
# define UI_SLIDER_BUTTON_HEIGHT 12
# define UI_FONT_SIZE 13
# define UI_DIRECTORY_FOLDER_COLOR 0xfff7c4ff
# define UI_DIRECTORY_FILE_WANTED_COLOR 0x77c7f2ff
# define UI_DIRECTORY_FILE_OTHER_COLOR 0x8c8c8cff
# define UI_LEVEL_SETTINGS_TEXT_COLOR 0xccb0f5ff
# define UI_INFO_TEXT_COLOR 0xff0000ff
# define UI_BACKGROUND_COL 0x222222bb
# define UI_ELEMENT_HEIGHT 16
# define UI_PADDING 2
# define UI_PADDING_4 4
# define UI_LEVEL_BAKED_COLOR 0x33aa33ff
# define UI_LEVEL_BAKING_COLOR 0xccaa33ff
# define UI_LEVEL_NOT_BAKED_COLOR 0xcc3333ff
# define UI_SHADER_SETTINGS 0xc77dffff

# define UV_PADDING 3

# define SERIALIZE_INITIAL_BUFFER_SIZE 512
# define OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER 10

# define AUDIO_GUNSHOT_CHANNEL 0
# define AUDIO_JUMP_CHANNEL 1
# define AUDIO_RELOAD_CHANNEL 2
# define AUDIO_DOOR_CHANNEL 3
# define AUDIO_DEATH_CHANNEL -1

# include <math.h>
# include <fcntl.h>
# ifdef __APPLE__
#  include <dirent.h>
#  include <sys/syslimits.h>//for PATH_MAX && NAME_MAX
#  include <sys/stat.h>
#  include <arpa/inet.h>
#  include <mach-o/dyld.h> //for _NSGetExecutablePath
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

typedef struct s_audio
{
	float				music_volume;
	float				sound_effect_volume;
	Mix_Music			*title_music;
	Mix_Music			*game_music;
	Mix_Chunk			*gunshot;
	Mix_Chunk			*jump;
	int					played_jump_sound;
	Mix_Chunk			*reload;
	int					played_reload_sound;
	Mix_Chunk			*death;
	Mix_Chunk			*door;
}						t_audio;

typedef struct s_bmp
{
	int					width;
	int					height;
	int					*image;
}						t_bmp;

typedef struct s_window
{
	SDL_Renderer		*SDLrenderer;
	SDL_Window			*SDLwindow;
	SDL_Texture			*texture;//rename to frame_buffer
	unsigned int		*frame_buffer;//rename to frame_buffer_pixels
	float				*depth_buffer;
	SDL_Texture			*raster_texture;
	unsigned int		*raster_texture_pixels;
	SDL_Texture			*text_texture;
	SDL_Texture			*ui_texture;
	unsigned int		*ui_texture_pixels;
	unsigned int		*buf;
}						t_window;

typedef struct s_rect
{
	int					x;
	int					y;
	int					w;
	int					h;
}						t_rect;

typedef struct s_ivec3
{
	int					x;
	int					y;
	int					z;
}						t_ivec3;

typedef struct s_ivec2
{
	int					x;
	int					y;
}						t_ivec2;

//hue        0, 1
//saturation 0, 1
//lightness -1, 1
typedef struct s_color_hsl
{
	float				hue;
	float				saturation;
	float				lightness;
	unsigned int		rgb_hue;
	int					color;
	float				r;
	float				g;
	float				b;
}						t_color_hsl;

typedef struct s_color
{
	float				r;
	float				g;
	float				b;
}						t_color;

typedef struct s_vec3
{
	float				x;
	float				y;
	float				z;
}						t_vec3;

typedef struct s_vec2
{
	float				x;
	float				y;
}						t_vec2;

typedef struct s_ray
{
	struct s_vec3		pos;
	struct s_vec3		dir;
}						t_ray;

// pos = world position in 3d
// txtr = texture position in 2d
typedef struct s_vert
{
	struct s_vec3		pos;
	struct s_vec2		txtr;
	int					selected;
}						t_vert;

typedef struct s_uv_parameters
{
	struct s_vec2		scale;
	struct s_tri		*tri;
	struct s_vec2		offset;
	unsigned int		*pixels;
}						t_uv_parameters;

typedef struct s_perlin_settings
{
	float				move_speed;
	float				speed_diff;
	float				scale;
	float				min;
	float				max;
	int					depth;
	int					visualizer;
	t_color_hsl			color_1;
	t_color_hsl			color_2;
}						t_perlin_settings;

typedef struct s_projectile
{
	struct s_vec3		dir;
	float				speed;
	float				dist;
	float				damage;
}						t_projectile;

// projectile_speed = 0 -> no projectile
typedef struct s_enemy
{
	struct s_vec3		dir;
	struct s_vec2		projectile_uv[3];
	float				move_speed;
	float				dist_limit;
	float				initial_health;
	float				remaining_health;
	float				attack_range;
	float				attack_frequency;
	float				projectile_speed;
	float				projectile_scale;
	float				attack_damage;
	float				current_attack_delay;
}						t_enemy;

// tris = array of triangles that make the object
// tri_amount = amount of triangles
typedef struct s_obj
{
	struct s_tri		*tris;
	int					tri_amount;
}						t_obj;

// verts = vertex coordinates of 3d triangle
// v0v1 = vector between vertices 1 and 0
// v0v2 = vector between vertices 2 and 0
typedef struct s_tri
{
	int					index;
	struct s_vert		verts[4];
	struct s_vec3		v0v1;
	struct s_vec3		v0v2;
	struct s_vec3		normal;
	int					isenemy;
	struct s_enemy		*enemy;
	int					isprojectile;
	struct s_projectile	*projectile;
	int					isquad;
	int					isgrid;
	int					isbreakable;
	int					disable_distance_culling;
	int					disable_backface_culling;
	float				opacity;
	float				reflectivity;
	float				refractivity;
	int					shader;
	int					selected;
	int					opacity_precise;
	t_obj				opacity_obj_all;
	t_obj				reflection_obj_all;
	t_obj				reflection_obj_first_bounce;
	t_obj				shadow_faces;
	t_perlin_settings	*perlin;
}						t_tri;

typedef struct s_skybox
{
	struct s_bmp		img;
	struct s_obj		all;
	struct s_obj		visible;
}						t_skybox;

// look_side = angle for looking to the side
// look_up = angle for looking up and down
typedef struct s_camera
{
	t_vec3				up;
	t_vec3				side;
	t_vec3				front;
	t_vec3				pos;
	float				look_side;
	float				look_up;
	float				lon;
	float				lat;
	float				fov_y;
	float				fov_x;
}						t_camera;

typedef struct s_door
{
	int					indice_amount;
	int					*indices;
	int					*isquad;
	t_vec3				**pos1;
	t_vec3				**pos2;
	int					is_activation_pos_active;
	t_vec3				avg;
	t_vec3				activation_pos;
	float				transition_time;
	unsigned int		transition_start_time;
	int					transition_direction;
}						t_door;

typedef struct s_all_doors
{
	struct s_door		*door;
	int					door_amount;
	int					selected_index;
}						t_all_doors;

typedef enum e_mouse_loc
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
}						t_mouse_loc;

typedef enum e_bake
{
	BAKE_BAKED = 0,
	BAKE_BAKING,
	BAKE_NOT_BAKED,
}						t_bake;

typedef enum e_game_state
{
	GAME_STATE_MAIN_MENU = 0,
	GAME_STATE_EDITOR,
	GAME_STATE_INGAME,
	GAME_STATE_DEAD,
	GAME_STATE_WIN
}						t_game_state;

typedef enum e_main_menu
{
	MAIN_MENU_LOCATION_MAIN,
	MAIN_MENU_LOCATION_SETTINGS,
	MAIN_MENU_LOCATION_SPRAY_SELECT,
	MAIN_MENU_LOCATION_LEVEL_SELECT
}						t_main_menu;

typedef enum e_ui_location
{
	UI_LOCATION_MAIN = 0,
	UI_LOCATION_FILE_OPEN,
	UI_LOCATION_FILE_SAVE,
	UI_LOCATION_UV_EDITOR,
	UI_LOCATION_GAME_SETTINGS,
	UI_LOCATION_DOOR_EDITOR,
	UI_LOCATION_DOOR_ACTIVATION_BUTTON,
	UI_LOCATION_LIGHT_EDITOR
}						t_ui_location;

struct	s_level;
typedef struct s_ui_state
{
	TTF_Font			*current_font;
	t_ui_location		ui_location;
	int					ui_max_width;
	int					ui_text_y_pos;
	int					ui_text_x_offset;
	int					ui_text_color;

	char				**error_message;
	unsigned int		*error_start_time;
	int					error_amount;

	int					mouse_capture;
	t_ivec2				mouse;
	int					m1_click;
	int					m1_drag;
	enum e_mouse_loc	mouse_location;
	float				uv_zoom;
	struct s_vec2		uv_pos;

	char				*save_filename;
	int					text_input_enable;
	int					ssp_visual;
	int					gizmo_active;
	struct s_vec3		gizmo_pos;
	float				gizmo_dist_from_screen;
	unsigned int		*color_slider_hue_colors;

	char				*directory;
	int					find_dir;
	int					find_extension;
	char				*extension;
	void				(*open_file)(struct s_level *, char *);
}						t_ui_state;

typedef struct s_editor_ui
{
	t_main_menu			main_menu;
	TTF_Font			*editor_font;
	TTF_Font			*hud_font;
	TTF_Font			*win_lose_font;
	TTF_Font			*main_menu_font;
	int					editor_active;
	int					noclip;
	int					vertex_select_mode;
	int					wireframe;
	int					wireframe_on_top;
	int					raycast_quality;
	int					wireframe_culling_visual;
	int					fog;
	int					blur;
	int					chromatic_abberation;
	int					smooth_pixels;
	t_color_hsl			fog_color;
	int					show_quads;
	int					pause_culling_position;
	int					backface_culling;
	int					occlusion_culling;
	int					distance_culling;
	float				render_distance;
	float				fov;
	int					raytracing;
	int					spray_from_view;
	float				spray_size;
	int					normal_map_disabled;

	t_color_hsl			sun_color;
	struct s_vec3		sun_dir;
	float				horizontal_velocity;

	unsigned int		ssp_time;
	unsigned int		cull_time;
	unsigned int		raycast_time;
	unsigned int		raster_time;
	unsigned int		ui_time;
	unsigned int		render_time;
	unsigned int		frame_time;
	unsigned int		total_raycasts;
	struct s_ui_state	state;
}						t_editor_ui;

typedef struct s_light
{
	t_vec3				pos;
	t_color_hsl			color;
	float				radius;
	float				power;
}						t_light;

typedef struct s_player_pos
{
	struct s_vec3		pos;
	float				look_side;
	float				look_up;
}						t_player_pos;

typedef struct s_level
{
	struct s_obj		all;
	struct s_obj		visible;
	struct s_obj		*ssp;
	struct s_bmp		texture;
	struct s_bmp		normal_map;
	struct s_bmp		spray;
	unsigned int		*spray_overlay;
	t_color				*baked;
	t_vec2				baked_size;
	t_bake				bake_status;
	float				bake_progress;
	struct s_skybox		sky;
	struct s_camera		cam;
	int					level_initialized;
	struct s_editor_ui	ui;
	struct s_all_doors	doors;
	struct s_light		*lights;
	int					light_amount;
	int					selected_light_index;
	struct s_enemy		player;
	int					player_health;
	int					player_ammo;
	struct s_player_pos	spawn_pos;
	struct s_bmp		main_menu_title;
	struct s_player_pos	main_menu_pos1;
	struct s_player_pos	main_menu_pos2;
	struct s_vec3		win_pos;
	float				win_dist;
	unsigned int		win_start_time;
	unsigned int		main_menu_anim_time;
	unsigned int		main_menu_anim_start_time;
	unsigned int		death_start_time;
	struct s_vec3		player_vel;
	unsigned int		reload_start_time;
	int					viewmodel_index;
	struct s_bmp		viewmodel[VIEWMODEL_FRAMES];
	float				world_brightness;
	float				skybox_brightness;
	struct s_audio		audio;
}						t_level;

typedef struct s_rthread
{
	int					id;
	struct s_level		*level;
	struct s_window		*window;
}						t_rthread;

typedef struct __attribute__((__packed__)) s_bmp_fileheader
{
	char		fileMarker1;
	char		fileMarker2;
	int16_t		bfSize;
	int16_t		bfFill1;
	int16_t		bfReserved1;
	int16_t		bfReserved2;
	int16_t		bfOffBits;
	int16_t		bfFill2;
}						t_bmp_fileheader;

typedef struct __attribute__((__packed__)) s_bmp_infoheader
{
	int					biSize;
	int					width;
	int					height;
	int16_t				planes;
	int16_t				bitPix;
	int					biCompression;
	int					biSizeImage;
	int					biXPelsPerMeter;
	int					biYPelsPerMeter;
	int					biClrUsed;
	int					biClrImportant;
}						t_bmp_infoheader;

typedef struct s_cast_result
{
	t_vec2				uv;
	float				dist;
	int					raytracing;
	unsigned int		color;
	int					face_index;
	int					reflection_depth;
	struct s_vec3		normal;
	struct s_ray		ray;
	struct s_bmp		*normal_map;
	struct s_bmp		*texture;
	t_color				*baked;
	unsigned int		*spray_overlay;
	int					raycast_amount;
}						t_cast_result;

typedef struct s_buffer
{
	void				*data;
	int					next;
	size_t				size;
}						t_buffer;

void			vec_normalize(t_vec3 *vec);
float			vec_dot(t_vec3 ve1, t_vec3 ve2);
float			vec_length(t_vec3 vec);
void			vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void			vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void			vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v);
void			vec_rot(t_vec3 *res, t_vec3 ve1, float ang);
int				vec_cmp(t_vec3 ve1, t_vec3 ve2);
void			vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
float			vec_angle(t_vec3 v1, t_vec3 v2);
void			vec_mult(t_vec3 *res, float mult);
void			vec_div(t_vec3 *res, float div);
t_vec3			vec_interpolate(t_vec3 a, t_vec3 b, float f);
float			lerp(float a, float b, float f);
float			vec2_length(t_vec2 vec);
void			vec2_avg(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec2_sub(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec2_add(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec2_mult(t_vec2 *res, float mult);
float			clamp(float var, float min, float max);

void			init_window(t_window **window);
void			init_embedded(t_level *level);
void			create_default_level(t_level *level);

void			screen_space_partition(t_level *level);
void			init_screen_space_partition(t_level *level);
int				get_ssp(t_ivec2 pixel);
int				get_ssp_coordinate(int coord, int horizontal);

void			read_input(t_window *window, t_level *level,
					t_game_state *game_state);
void			game_logic(t_level *level, t_game_state *game_state);
int				init_raycast(void *t);
float			cast_face(t_tri t, t_ray ray, t_cast_result *res);
float			cast_all(t_ray vec, t_level *level, int *index);
void			fill_pixels(unsigned int *grid, int pixel_gap,
					int blur, int smooth);
unsigned int	crossfade(unsigned int color1, unsigned int color2,
					unsigned int fade, unsigned int alpha);
unsigned int	set_lightness(unsigned int color, float b);
unsigned int	set_saturation(unsigned int color, float s);
void			hsl_update_color(t_color_hsl *c);
void			face_color(float u, float v, t_tri t,
					t_cast_result *res);
void			print_line(t_vec3 start, t_vec3 stop, unsigned int color,
					unsigned int *texture);
void			wireframe(unsigned int *texture, t_level *level);
void			camera_offset(t_vec3 *vertex, t_camera *cam);
SDL_Color		get_sdl_color(unsigned int color);

char			**file2d(char *filename);
char			**file2d_from_memory(unsigned char *data, unsigned int size);
void			free_file2d(char **file);
int				load_obj(char *filename, t_obj *obj);
void			load_obj_from_memory(unsigned char *data,
					unsigned int size, t_obj *obj);
t_bmp			bmp_read(char *str);
t_bmp			bmp_read_from_memory(unsigned char *data,
					unsigned int size);

void			culling(t_level *level);
int				occlusion_culling(t_tri tri, t_level *level);
void			init_culling(t_level *level);
void			free_culling(t_level *level);
void			static_culling(t_level *l);
void			reflection_culling(t_level *level, int i);
void			find_quads(t_obj *obj);
void			set_fourth_vertex(t_tri *a);

void			rotate_vertex(float angle, t_vec3 *vertex, int axis);
void			rot_cam(t_vec3 *cam, const float lon, const float lat);

char			*get_current_directory(void);
void			path_up_dir(char *path);
void			go_in_dir(char *path, char *folder);
void			loop_directory(char *directory, void *data,
					void (*f)(int, char *, void *));

void			init_enemy(t_tri *face);
void			init_ui(t_window *window, t_level *level);
void			init_player(t_enemy *player);
void			init_audio(t_level *level);
void			ui_render(t_window *window, t_level *level);
void			set_text_color(int color);
void			text(char *text);
int				button(int *var, char *text);
int				int_slider(int *var, char *str, int min, int max);
int				float_slider(float *var, char *str, float min,
					float max);
int				color_slider(t_color_hsl *var, char *str);
int				call(char *str, void (*f)(t_level *));
void			file_browser(char *str, char *extension,
					void (*f)(t_level *, char *));
void			file_save(char *str, char *extension,
					void (*f)(t_level *, char *));
void			text_input(char *str, t_level *level);
void			find_closest_mouse(t_vec3 *vert, int *i, int *k,
					t_ivec2 *mouse);
int				mouse_collision(t_rect rect, t_ivec2 mouse);

void			main_menu(t_level *level, unsigned int *pixels,
					t_game_state *game_state);
void			main_menu_move_background(t_level *level);
void			hud(t_level *level, unsigned int *pixels,
					t_game_state game_state);
void			fake_analog_signal(t_bmp *img, unsigned int *pixels,
					float amount);
void			chromatic_abberation(unsigned int *pixels,
					unsigned int *buf, int amount);
void			create_projectile(t_level *level, t_vec3 pos,
					t_vec3 dir, t_enemy *enemy);

void			uv_editor(t_level *level, unsigned int *pixels);
void			enable_uv_editor(t_level *level);
void			gizmo_render(t_level *level, unsigned int *pixels);
void			gizmo(t_level *level);
void			obj_editor_input(t_level *level, t_vec3 move_amount);

void			player_movement(t_level *level);

void			enemies_update_physics(t_level *level);
void			enemies_update_sprites(t_level *level);

void			fog(unsigned int *color, float dist, unsigned int fog_color,
					t_level *level);
int				skybox(t_level *l, t_cast_result res);

void			opacity(t_cast_result *res, t_level *l, t_obj *obj,
					float opacity);
void			reflection(t_cast_result *res, t_level *l, t_obj *obj);
unsigned int	shader_wave(t_vec3 mod, t_vec3 *normal,
					unsigned int col1, unsigned int col2);
unsigned int	shader_rule30(t_vec3 pos);
unsigned int	shader_test(t_vec3 pos, t_level *level, t_cast_result *res);
t_color			sunlight(t_level *l, t_cast_result *res, t_color light);

void			select_face(t_camera *cam, t_level *level,
					int x, int y);
void			deselect_all_faces(t_level *level);

void			save_level(t_level *level);
void			open_level(t_level *level, char *filename);

void			cast_all_color(t_level *l, t_obj *obj,
					t_cast_result *res, int apply_fog);
int				cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri);
int				cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri);

void			fix_uv_overlap(t_level *level);
int				tri_uv_intersect(t_tri t1, t_tri t2);
int				point_in_tri(t_vec2 pt,
					t_vec2 v1, t_vec2 v2, t_vec2 v3);
void			toggle_selection_all(t_level *level);
void			add_face(t_level *level);
void			remove_faces(t_level *level);
t_ivec2			put_text(char *text, t_window *window,
					SDL_Texture *texture, t_ivec2 pos);
void			set_new_face(t_level *level, t_vec3 pos, t_vec3 dir,
					float scale);

void			game_logic_put_info(t_level *level, unsigned int *texture);
void			door_animate(t_level *level);
void			door_put_text(t_level *level);
void			add_new_door(t_level *level);
void			delete_door(t_level *level);
void			door_activate(t_level *level);
void			set_door_pos_1(t_level *level);
void			set_door_pos_2(t_level *level);
void			enable_door_editor(t_level *level);
void			find_selected_door_index(t_level *level);
void			door_activation_move(t_level *level,
					t_vec3 move_amount);
t_color			lights(t_level *l, t_cast_result *res, t_vec3 normal);
unsigned int	brightness(unsigned int color1, t_color new);
int				nothing_selected(t_level *level);
void			light_put_text(t_level *level);
void			enable_light_editor(t_level *level);
void			add_light(t_level *level);
void			move_light(t_level *level, t_vec3 move_amount);
void			select_light(t_level *level, int x, int y);
void			delete_light(t_level *level);
void			delete_all_lights(t_level *level);
void			set_fourth_vertex_uv(t_tri *a);
void			start_bake(t_level *level);
t_vec3			get_normal(int vec);
void			handle_audio(t_level *level, t_game_state *game_state);
int				is_player_in_air(t_level *level, float height);
void			spray(t_camera cam, t_level *level);
void			set_obj(t_level *level, char *filename);
void			set_texture(t_level *level, char *filename);
void			set_normal_map(t_level *level, char *filename);
void			set_skybox(t_level *level, char *filename);
void			set_spray(t_level *level, char *filename);
void			set_win_pos(t_level *level);
void			set_spawn_pos(t_level *level);
void			set_menu_pos_1(t_level *level);
void			set_menu_pos_2(t_level *level);
void			nonfatal_error(char *message);
void			ui_render_nonfatal_errors(t_level *level);
t_ui_state		*get_ui_state(t_ui_state *get_state);
t_window		*get_window(t_window *get_window);
t_level			*get_level(t_level *get_level);
t_ivec2			render_text(char *text, int x, int y);
void			render_text_3d(char *str, t_vec3 pos,
					unsigned int color, t_level *level);
void			button_pixel_put(int x, int y, int color,
					unsigned int *texture);
void			ui(t_window *window, t_level *level, t_game_state *game_state);
void			render_ssp_visual_background(unsigned int *texture);
void			render_ssp_visual_text(t_level *level);
void			ui_render_background(t_window *window, t_level *level);
void			perlin_init(t_tri *tri);

#endif
