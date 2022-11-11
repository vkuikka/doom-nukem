/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom_nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:50 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/07 15:29:55 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H
# define RES_X 1000
# define RES_Y 750
# define THREAD_AMOUNT 4
# define NOISE_QUALITY_LIMIT 8
# define SSP_MAX_X 20
# define SSP_MAX_Y 20

# define NOCLIP_SPEED 20.0
# define GRAVITY 12		//	m/s^2
# define JUMP_SPEED 5	//	m/s
# define AIR_ACCEL 8	//	m/s^2
# define MOVE_ACCEL 70	//	m/s^2
# define RUN_SPEED 10	//	m/s
# define WALK_SPEED 4	//	m/s
# define CROUCH_SPEED 3	//	m/s
# define GROUND_FRICTION 5.
# define PLAYER_EYE_HEIGHT 1.65
# define PLAYER_HEIGHT_MAGIC 1.3
# define CROUCHED_HEIGHT 1
# define WALL_CLIP_DIST 0.3
# define REFLECTION_DEPTH 3
# define WALKABLE_NORMAL_MIN_Y 0.75
# define DOOR_ACTIVATION_DISTANCE 3.
# define DOOR_LOCATION_INFO_COLOR 0x880088ff
# define DOOR_ACTIVATION_LOCATION_INFO_COLOR 0xcc2288ff
# define LIGHT_LOCATION_INFO_COLOR 0xffdd00ff
# define PERLIN_OFFSET 123
# define SUN_SIZE 0.999	// 0.5 = half of skybox is sun
# define SSP_INITIAL_SIZE 5

# define ENEMY_MOVABLE_HEIGHT_DIFF 3
# define MAX_PROJECTILE_TRAVEL 100
# define PROJECTILE_DAMAGE_DIST 3
# define SPRAY_LINE_PRECISION 2
# define SPRAY_MAX_DIST 15
# define SPRAY_FROM_VIEW_SIZE 0.5
# define FIND_QUADS_FLOAT_ERROR 0.0001
# define RULE_30_SIZE 99

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
# define WIN_LENGTH_SEC 7
# define PLAYER_HEALTH_MAX 100
# define PLAYER_AMMO_MAX 30
# define DEATH_LENGTH_SEC 5
# define DEATH_OVERLAY_COLOR 0xff000088
# define RELOAD_ANIMATION_DURATION 2.0//s
# define ITEM_PICKUP_DIST 1.4//m
# define ITEM_SPAWN_TIME 30//s
# define RADIAL_GRADIENT_RESOLUTION 30

# define AMMO_BOX_TEXT_COLOR 0x037700ff
# define HEALTH_BOX_TEXT_COLOR 0xf76565ff
# define ENEMY_SPAWN_TEXT_COLOR 0x7005fcff

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
# define UI_POST_PROCESS_BLOOM 0xc77dffff
# define UI_POST_PROCESS_SSAO 0x77c7f2ff
# define UI_POST_PROCESS_OTHER 0x33aa33ff
# define UI_POST_PROCESS_DEBUG 0xf76565ff

# define UV_PADDING 3

# define OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER 10

# define AUDIO_GUNSHOT_CHANNEL 0
# define AUDIO_JUMP_CHANNEL 1
# define AUDIO_RELOAD_CHANNEL 2
# define AUDIO_DOOR_CHANNEL 3
# define AUDIO_DEATH_CHANNEL -1

# define SERIALIZE_INITIAL_BUFFER_SIZE 512

# include <math.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/syslimits.h>
# include <sys/stat.h>
# include <arpa/inet.h>
# include <mach-o/dyld.h>
# include <mach-o/getsect.h>
# include "get_next_line.h"
# include "SDL2/SDL.h"
# include "SDL2_ttf/SDL_ttf.h"
# include "SDL2_mixer/SDL_mixer.h"
# include <sys/time.h>
//delete
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>

typedef struct s_audio
{
	float				music_volume;
	float				sound_effect_volume;
	unsigned char		*title_music_mem;
	unsigned char		*game_music_mem;
	unsigned char		*gunshot_mem;
	unsigned char		*jump_mem;
	unsigned char		*reload_mem;
	unsigned char		*death_mem;
	unsigned char		*door_mem;
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
	t_vec3				pos;
	t_vec3				dir;
}						t_ray;

// pos = world position in 3d
// txtr = texture position in 2d
typedef struct s_vert
{
	t_vec3				pos;
	t_vec2				txtr;
	int					selected;
}						t_vert;

typedef enum e_shader
{
	SHADER_NONE = 0,
	SHADER_RULE_30,
	SHADER_PERLIN
}						t_shader;

typedef struct s_perlin_settings
{
	float				move_speed;
	float				speed_diff;
	float				scale;
	float				min;
	float				max;
	int					resolution;
	float				depth;
	float				noise_opacity;
	float				distance;
	float				swirl;
	float				swirl_interval;
	t_vec2				dir;
	int					visualizer;
	t_color_hsl			color_1;
	t_color_hsl			color_2;
}						t_perlin_settings;

struct	s_tri;

// tris = array of triangles that make the object
// tri_amount = amount of triangles
typedef struct s_obj
{
	struct s_tri		*tris;
	int					tri_amount;
	t_bmp				texture;
}						t_obj;

// verts = vertex coordinates of 3d triangle
// v0v1 = vector between vertices 1 and 0
// v0v2 = vector between vertices 2 and 0
typedef struct s_tri
{
	int					index;
	t_vert				verts[4];
	t_vec3				v0v1;
	t_vec3				v0v2;
	t_vec3				normal;
	int					isquad;
	int					isgrid;
	int					isbreakable;
	int					isbroken;
	float				opacity;
	float				reflectivity;
	float				refractivity;
	t_shader			shader;
	int					selected;
	int					opacity_precise;
	t_bmp				*texture;
	t_obj				opacity_obj_all;
	t_obj				reflection_obj_all;
	t_obj				reflection_obj_first_bounce;
	t_obj				shadow_faces;
	t_perlin_settings	*perlin;
}						t_tri;

typedef struct s_uv_parameters
{
	t_vec2				scale;
	t_tri				*tri;
	t_vec2				offset;
	unsigned int		*pixels;
}						t_uv_parameters;

typedef struct s_skybox
{
	t_bmp				img;
	t_obj				all;
	t_obj				visible;
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
	t_door				*door;
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
	MOUSE_LOCATION_GAME_SETTINGS,
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
	GAME_STATE_MAIN_MENU,
	GAME_STATE_EDITOR,
	GAME_STATE_INGAME
}						t_game_state;

typedef enum e_game_logic_selected
{
	GAME_LOGIC_SELECTED_NONE,
	GAME_LOGIC_SELECTED_SPAWN,
	GAME_LOGIC_SELECTED_WIN,
	GAME_LOGIC_SELECTED_MENU_ANIMATION,
	GAME_LOGIC_SELECTED_AMMO,
	GAME_LOGIC_SELECTED_HEALTH,
	GAME_LOGIC_SELECTED_ENEMY
}						t_glogic_selected;

typedef enum e_main_menu
{
	MAIN_MENU_LOCATION_MAIN,
	MAIN_MENU_LOCATION_SETTINGS,
	MAIN_MENU_LOCATION_SPRAY_SELECT,
	MAIN_MENU_LOCATION_LEVEL_SELECT,
	MAIN_MENU_LOCATION_POST_PROCESS
}						t_main_menu;

typedef enum e_ui_location
{
	UI_LOCATION_MAIN = 0,
	UI_LOCATION_FILE_OPEN,
	UI_LOCATION_FILE_SAVE,
	UI_LOCATION_UV_EDITOR,
	UI_LOCATION_DOOR_EDITOR,
	UI_LOCATION_DOOR_ACTIVATION_BUTTON,
	UI_LOCATION_LIGHT_EDITOR,
	UI_LOCATION_SHADER_EDITOR,
	UI_LOCATION_POST_PROCESS_SETTINGS,
	UI_LOCATION_GAME_SETTINGS,
	UI_LOCATION_ENEMY_AND_DAMAGE_SETTINGS
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
	t_vec2				uv_pos;
	t_glogic_selected	logic_selected;
	int					logic_selected_index;

	char				*save_filename;
	int					text_input_enable;
	int					ssp_visual;
	int					gizmo_active;
	t_vec3				gizmo_pos;
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
	unsigned char		*roboto_data;
	unsigned char		*digital_data;
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
	float				sharpen;
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
	float				bloom_radius;
	float				bloom_intensity;
	float				bloom_limit;
	int					bloom_debug;
	int					ssao_radius;
	float				ssao_intensity;
	float				ssao_light_bias;
	int					ssao_debug;
	int					bake_quality;
	int					bake_blur_radius;

	t_color_hsl			sun_color;
	t_vec3				sun_dir;

	unsigned int		ssp_time;
	unsigned int		cull_time;
	unsigned int		raycast_time;
	unsigned int		post_time;
	unsigned int		raster_time;
	unsigned int		ui_time;
	unsigned int		render_time;
	unsigned int		frame_time;
	unsigned int		total_raycasts;

	int					physics_debug;
	float				horizontal_velocity;
	t_vec2				wishdir;
	t_ui_state			state;
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
	t_vec3				pos;
	float				look_side;
	float				look_up;
}						t_player_pos;

typedef struct s_uv
{
	t_vec2				diff;
	t_vec2				precision;
	t_vec2				coord_uv;
	t_vec2				check;
	t_ivec2				og;
	t_ivec2				coord;
	t_ivec2				inc;
	float				max;
	float				min;
	float				min_x;
	float				min_y;
	float				max_x;
	float				max_y;
}						t_uv;

typedef struct s_obj_animation
{
	t_obj				*keyframes;
	int					keyframe_amount;
	float				duration;
	float				duration_multiplier;
	int					loop;
}						t_obj_animation;

typedef struct s_game_models
{
	t_obj				ammo_pickup_box;
	t_obj				health_pickup_box;
	t_obj				enemy;
	t_obj				enemy_shoot;
	t_obj_animation		enemy_run;
	t_obj_animation		enemy_die;
	t_obj				viewmodel;
	t_obj_animation		reload_animation;
	t_bmp				projectile_texture;
	t_bmp				light_sprite;
	t_bmp				projectile_sprite;
}						t_game_models;

typedef struct s_item_pickup
{
	unsigned int		start_time;
	int					visible;
	t_vec3				pos;
}						t_item_pickup;

typedef struct s_player
{
	t_vec3				vel;
	t_vec3				dir;
	float				move_speed;
	float				height;
	t_vec3				wishdir;
	int					health;
	int					ammo;
}						t_player;

typedef struct s_enemy
{
	t_vec3				spawn_pos;
	unsigned int		dead_start_time;
	t_vec3				pos;
	t_vec3				move_to;
	t_vec3				dir;
	float				dir_rad;
	float				remaining_health;
	float				current_attack_delay;
	unsigned int		move_start_time;
	unsigned int		shoot_start_time;
	int					can_see_player;
}						t_enemy;

typedef struct s_enemy_settings
{
	float				dist_limit;
	float				move_speed;
	float				initial_health;
	float				melee_range;
	float				melee_damage;
	float				attack_frequency;
	float				move_duration;
	float				shoot_duration;
}						t_enemy_settings;

typedef struct s_projectile
{
	t_vec3				dir;
	t_vec3				pos;
	float				speed;
	float				dist;
	float				damage;
	float				scale;
	int					shot_by_player;
}						t_projectile;

typedef struct s_ssao
{
	t_ivec2		upper_bound;
	t_ivec2		lower_bound;
	t_ivec2		kernel_center;
	float		count;
	float		total;
	int			radius;
}				t_ssao;

typedef struct s_game_logic
{
	t_player_pos		spawn_pos;
	t_vec3				win_pos;
	float				win_dist;

	unsigned int		win_start_time;
	unsigned int		death_start_time;
	unsigned int		reload_start_time;
	t_player			player;

	float				item_spin_speed;
	int					enemy_animation_view_index;

	t_projectile		*projectiles;
	int					projectile_amount;
	int					projectile_max;
	t_projectile		enemy_projectile_settings;
	t_projectile		player_projectile_settings;

	t_enemy_settings	enemy_settings;
	t_enemy				*enemies;
	int					enemy_amount;

	t_item_pickup		*health_box;
	int					health_box_amount;
	t_item_pickup		*ammo_box;
	int					ammo_box_amount;
}						t_game_logic;

typedef struct s_camera_path
{
	t_player_pos		*pos;
	int					amount;
	int					loop;
	unsigned int		duration;
	unsigned int		start_time;
}						t_camera_path;

typedef struct s_level
{
	int					render_is_first_pass;
	t_obj				all;
	t_obj				visible;
	int					visible_max;
	t_obj				*ssp;
	int					ssp_max[SSP_MAX_X * SSP_MAX_Y];
	t_bmp				texture;
	t_bmp				normal_map;
	t_bmp				spray;
	unsigned int		*spray_overlay;
	t_color				*baked;
	t_vec2				baked_size;
	t_bake				bake_status;
	float				bake_progress;
	t_skybox			sky;
	t_camera			cam;
	int					level_initialized;
	t_editor_ui			ui;
	t_all_doors			doors;
	t_light				*lights;
	int					light_amount;
	int					selected_light_index;
	t_bmp				main_menu_title;
	t_game_models		game_models;
	t_game_logic		game_logic;
	t_camera_path		main_menu_anim;
	float				world_brightness;
	float				skybox_brightness;
	t_audio				audio;
}						t_level;

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
	t_color				light;
	int					face_index;
	int					reflection_depth;
	t_vec3				normal;
	t_ray				ray;
	t_bmp				*normal_map;
	t_bmp				*texture;
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

typedef struct s_blur
{
	t_ivec2		upper_bound;
	t_ivec2		lower_bound;
	t_color		*pixels;
	t_color		*buff;
	t_ivec2		size;
	float		intensity;
	int			radius;
	int			quality;
	int			skip_zeroes;
}				t_blur;

typedef struct s_window
{
	SDL_Renderer		*SDLrenderer;
	SDL_Window			*SDLwindow;
	SDL_Texture			*texture;
	unsigned int		*frame_buffer;
	float				*depth_buffer;
	t_color				*brightness_buffer;
	t_vec3				*pixel_pos_buffer;
	SDL_Texture			*raster_texture;
	unsigned int		*raster_texture_pixels;
	SDL_Texture			*text_texture;
	SDL_Texture			*ui_texture;
	unsigned int		*ui_texture_pixels;
	unsigned int		*post_process_buf;
}						t_window;

typedef struct s_rthread
{
	int					id;
	t_level				*level;
	t_window			*window;
}						t_rthread;

void			visible_request_merge(t_level *level, int amount);
void			merge_mesh(t_level *level, t_obj *obj, t_vec3 pos,
					t_vec2 rotation);
void			merge_enemies_runtime(t_level *level);
void			merge_enemies_editor(t_level *level, float rot);
void			merge_pickups(t_level *level, float rot);
void			merge_game_models(t_level *level, t_game_state game_state);
t_vec2			viewmodel_sway(t_level *level);
void			merge_viewmodel(t_level *level);
void			div_every_uv(t_level *l);
void			coloring_loop(t_tri *t1, t_bmp *img, t_uv *v);
void			check_and_color(t_tri *t1, t_bmp *img, t_uv *v);
void			copy_uv2(t_tri *t1, t_uv *v, t_bmp *img);
void			copy_uv(t_tri *t1, t_uv	*v, t_bmp *img);
void			move_uv_y(t_tri *t1, t_level *l, t_uv *v, int i);
void			clear_intersection(t_tri *t1, t_level *l, t_uv *v, int i);
void			baked_state(t_level *l, t_uv *v);
void			loop_state(t_level *l, t_uv *v);
void			move_uv(t_tri *t1, int t1_index, t_level *l, t_uv *v);
void			fix_uv_overlap(t_level *level);
float			find_angle(t_vec3 v1, t_vec3 v2);
void			turn_sprite(t_tri *tri, t_vec3 look_at);
void			merge_sprite(t_level *level, t_vec3 pos, t_bmp *texture);
void			door_start_animate(t_door *door);
float			dist_to_door_activation(t_level *level, t_door *door);
void			door_activate(t_level *level);
void			door_move_vert(t_level *level, t_door *door, float time,
					t_ivec2 v);
void			door_animate(t_level *level);
float			gradient_function(t_vec2 diff, t_vec2 mid, float div,
					t_vec2 i);
int				init_gradient(float *arr);
float			radial_gradient(t_ivec2 pixel, t_ivec2 lower_bound,
					float diameter);
t_color			*get_buff(t_color *set);
unsigned int	color_to_int(t_color color);
t_color			int_to_color(unsigned int color);
void			color_cpy(t_color *c1, t_color *c2);
void			color_add(t_color *c1, t_color *c2);
void			bloom_apply(t_window *window, t_color *buff, t_level *level);
int				bloom_init(void *data_pointer);
int				buffer_alloc(t_color **buff);
void			bloom(t_level *level, t_window *window);
char			*get_current_directory(void);
void			go_in_dir(char *path, char *folder);
void			path_up_dir(char *path);
void			loop_directory(char *directory, void *funcdata, void (*f)(int,
						char *, void *));
void			pixel_put(int x, int y, unsigned int color,
					unsigned int *texture);
t_vec3			move2z(t_vec3 *p1, t_vec3 *p2);
int				z_clip_line(t_vec3 *start, t_vec3 *stop);
void			print_line(t_vec3 start, t_vec3 stop, unsigned int color,
					unsigned int *texture);
void			rotate_vertex(float angle, t_vec3 *vertex, int axis);
void			pixel_put_force(int x, int y, int color, unsigned int *texture);
void			put_vertex(t_vec3 vertex, int color, unsigned int *texture);
void			camera_offset(t_vec3 *vertex, t_camera *cam);
void			put_normal(unsigned int *texture, t_level *level, t_tri tri,
					unsigned int color);
t_vec3			wireframe_render_line(t_obj *obj, t_ivec3 i, t_level *level,
					unsigned int *texture);
void			render_wireframe(unsigned int *texture, t_level *level,
					t_obj *obj, int is_visible);
void			wireframe(unsigned int *texture, t_level *level);
void			set_uv_vert(char **file, int i, t_vec2 *vert);
t_vec2			*load_uvs(char **file);
void			set_vert(char **file, int i, t_vec3 *vert);
t_vec3			*load_verts(char **file);
void			read_uv_indices(char *str, t_ivec3 *uv_index, int *j, int x);
int				read_indices(char *str, t_ivec3 *tex_index,
					t_ivec3 *uv_index);
int				set_tri(char *str, t_vec3 *verts, t_vec2 *uvs, t_tri *tri);
int				obj_set_all_tris_res(t_vec3 *verts, t_vec2 *uvs, int res);
int				obj_set_all_tris(char **file, t_obj *obj);
int				obj_get_face_amount(char **file);
void			tri_optimize(t_tri *tri);
int				load_obj_internal(char **file, t_obj *obj);
int				load_obj(char *filename, t_obj *obj);
void			load_obj_from_memory(unsigned char *data, t_obj *obj);
SDL_Texture		*empty_texture(SDL_Renderer *renderer);
void			init_fonts(t_editor_ui *ui);
void			init_animation(t_level *level);
void			init_textures(t_level *level);
void			init_embedded(t_level *level);
void			projectile_default(t_projectile *projectile);
void			init_enemy_settings(t_enemy_settings *enemy);
void			level_default_settings(t_level *level);
void			create_default_level(t_level *level);
void			check_buffers(t_window *window);
void			init_window_struct(t_window **window);
void			init_window(t_window **window);
void			init_audio_effects(t_level *l);
void			init_audio(t_level *l);
float			sign(t_vec2 p1, t_vec2 p2, t_vec2 p3);
int				point_in_tri(t_vec2 pt, t_vec2 v1, t_vec2 v2, t_vec2 v3);
int				line_orientation(t_vec2 p, t_vec2 q, t_vec2 r);
int				line_intersect(t_vec2 p1, t_vec2 q1, t_vec2 p2, t_vec2 q2);
void			calc_quad_uv(t_tri *tri);
int				check_uv_edge(int i, int j, t_tri t1, t_tri t2);
int				tri_uv_intersect(t_tri t1, t_tri t2);
void			player_reload(t_level *level);
void			player_shoot(t_level *level);
void			game_finished(t_level *level, t_game_state *game_state,
					float time);
int				pick_up_pick_ups(t_level *level, t_item_pickup *pickups,
					int amount);
void			reset_pick_ups(t_level *level);
void			game_logic_win_lose(t_level *level, t_game_state *game_state);
void			game_logic(t_level *level, t_game_state *game_state);
void			rot_cam(t_vec3 *cam, const float lon, const float lat);
void			trace_bounce(t_cast_result *res, t_obj *obj, t_level *l);
void			add_lightness(t_level *l, t_color light, t_cast_result *res);
void			raytrace(t_cast_result *res, t_obj *obj, t_level *l);
int				cast_loop(t_obj *obj, t_cast_result *res);
void			cast_all_color(t_level *l, t_obj *obj, t_cast_result *res,
					int apply_fog);
t_ray			ray_set(t_camera *cam, t_ivec2 xy);
void			cast_result_set(t_cast_result *res, t_level *level);
void			set_render_result(t_window *window, t_cast_result res,
					unsigned int i);
int				raycast(t_level *level, t_window *window, int thread_id);
int				init_raycast(void *data_pointer);
int				get_ssp(t_ivec2 pixel);
int				get_ssp_coordinate(int coord, int horizontal);
void			ssp_edge_vectors(t_vec3 result[2], t_camera c, float px,
					int horizontal);
int				horizontal(t_tri tri, int x, t_camera cam, int side);
int				vertical(t_tri tri, int x, t_camera cam, int side);
void			partition_range(int (*side_comparison)(t_tri, int, t_camera,
						int), t_tri tri, t_camera cam, float bounds[3]);
void			find_partition(int (*side_comparison)(t_tri, int, t_camera,
						int), t_tri tri, t_camera cam, float bounds[3]);
void			ssp_set_tri(t_tri tri, t_level *level, int x, int y);
void			find_ssp_index(t_tri tri, t_level *level);
void			ssp_set_all(t_level *level, t_tri tri);
void			screen_space_partition(t_level *level);
void			init_screen_space_partition(t_level *level);
char			*get_current_directory(void);
void			go_in_dir(char *path, char *folder);
void			path_up_dir(char *path);
void			loop_directory(char *directory, void *data, void (*f)(int,
						char *, void *));
void			set_fourth_vertex(t_tri *a);
void			set_mirrored_dir(t_tri *a, int not_shared_index);
int				has_2_shared_vertices(t_tri a, t_tri b, int *not_shared);
int				is_mirrored_set(t_tri *a, t_tri *b);
void			find_quads(t_obj *obj);
int				cull_behind_occlusion(t_vec3 dir, t_vec3 pos, t_tri tri);
int				is_value_close(float a, float b, float range);
float			area(t_vec3 *a, t_vec3 *b, t_vec3 *c);
int				vec3_point_in_tri(t_vec3 *p, t_vec3 *a, t_vec3 *b,
					t_vec3 *c);
int				vec3_point_in_face(t_vec3 *p, t_tri *face);
int				is_bface_in_aface(t_tri a, t_tri b, t_level *level);
int				occlusion_culling(t_tri tri, t_level *level);
void			deselect_all_faces(t_level *level);
void			find_closest_mouse(t_vec3 *vert, int *i, int *k,
					t_ivec2 *mouse);
void			update_closest_vertex(t_level *level);
void			raycast_face_selection(t_ray vec, t_level *level);
void			select_face(t_camera *cam, t_level *level);
void			toggle_selection_all(t_level *level);
void			make_bounds(t_blur *blur, t_ivec2 p);
float			add_to_buffer(t_blur b, t_ivec2 i, unsigned int coord);
void			box_avg(t_ivec2 p, t_blur blur);
void			box_blur(t_blur blur, int thread);
void			texture_minmax(t_vec2 *min, t_vec2 *max, t_tri tri);
void			normalize_data(t_vec3 *uvw);
void			get_uv(t_vec3 *uvw, t_ivec2 ipoint, t_tri tri, t_bmp *txtr);
t_vec3			uv_to_3d(t_tri tri, t_bmp *txtr, t_ivec2 point);
void			wrap_coords(int *x, int *y, int max_x, int max_y);
void			bake_pixel(t_level *l, t_ivec2 wrap, t_ivec2 i,
					t_cast_result *res);
int				point_in_face(t_tri tri, t_vec2 point);
void			fill_area(t_level *l, t_ivec2 start, t_color c, int res);
void			bake_area(t_level *l, t_cast_result *res, t_ivec2 i);
void			bake_face(t_cast_result *res, t_level *l);
void			blur_bake(t_level *level);
int				bake(void *d);
void			clear_bake(t_level *level);
void			start_bake(t_level *level);
void			wait_threads(t_level *level,
					SDL_Thread *threads[THREAD_AMOUNT]);
void			create_threads(t_level *level, t_window *window);
void			render_raycast(t_window *window, t_level *level,
					t_game_state *game_state);
void			spray_point(t_level *l, t_vec2 texture, t_vec2 steps,
					t_tri *tri);
void			spray_line(t_level *l, t_vec2 line[2], t_tri *tri,
					t_vec2 axis_step);
void			square_step(t_vec2 square[4], t_level *l, t_tri *tri,
					float step);
void			draw_square(t_level *l, t_vec2 square[4], t_tri *tri);
t_vec2			uv_to_2d(t_tri tri, t_vec2 uv, int isquad);
int				cast_uv(t_tri t, t_ray ray, t_vec2 *uv);
int				raycast_face_pos(t_ray *r, t_obj *object);
t_vec2			corner_cam_diff(int corner, t_camera *cam);
void			move_cam(t_level *l, t_camera *cam, int hit, t_ray r);
void			rot_to_corner(t_ray *r, t_camera cam, t_vec2 cam_diff);
int				face_in_front(t_camera *cam, t_level *level);
void			spray(t_camera cam, t_level *level);
void			input_player_movement(t_vec3 *wishdir, const Uint8 *keys);
void			uv_linear_zoom(t_level *level, const Uint8 *keys);
void			input_uv(t_level *level, const Uint8 *keys);
void			player_input(t_level *level, t_player *player);
int				player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level,
					float height);
int				is_player_in_air(t_level *level, float height);
void			noclip(t_level *level, t_vec3 *wishdir, float delta_time);
void			rotate_wishdir(t_level *level, t_vec3 *wishdir);
int				vertical_movement(t_vec3 *wishdir, t_vec3 *vel,
					float delta_time, int in_air);
void			air_movement(t_vec3 *wishdir, t_vec3 *vel, float delta_time);
void			horizontal_movement_no_input(t_vec3 *vel, float delta_time);
void			horizontal_movement(t_vec3 *wishdir, t_vec3 *vel,
					float delta_time, float movespeed);
void			apply_velocity(t_vec3 vel, float h, t_level *level,
					float delta_time);
void			movement_physics(t_level *level, float delta_time,
					t_player *player);
void			player_movement(t_level *level);
float			cast_all(t_ray vec, t_level *level, int *index);
void			grid_check(float *val, int isgrid);
float			start_cast(t_tri t, t_vec3 *pvec, t_ray ray,
					t_cast_result *res);
float			cast_face(t_tri t, t_ray ray, t_cast_result *res);
int				noise2(int x, int y);
float			lin_inter(float x, float y, float s);
float			smooth_inter(float x, float y, float s);
float			noise2d(float x, float y);
float			perlin_noise(float x, float y, float freq, int depth);
void			perlin_init(t_tri *t);
float			stretch_value(float perlin, float min, float max);
float			noise_opacity(t_perlin_settings p, float perlin,
					t_cast_result res);
float			swirl(t_vec3 pos, float time, t_perlin_settings p);
float			noise_swirl(float time, t_vec3 pos, t_perlin_settings p);
float			noise_move(float time, t_vec3 pos, t_perlin_settings p);
float			perlin_opacity(t_vec3 *pos, float perlin, t_level *l,
					t_perlin_settings p);
unsigned int	depth_grid_visualizer(t_vec3 pos, t_level *level,
					t_cast_result *res);
unsigned int	shader_perlin(t_vec3 pos, t_level *level, t_cast_result *res);
int				cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri);
int				cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri);
int				frustrum_culling(t_vec3 side_normal[4], t_vec3 pos,
					t_tri tri);
void			distance_minmax(float *min, float *max, t_tri tri, t_vec3 pos);
int				distance_culling(t_tri tri, t_vec3 player,
					float render_distance);
int				backface_culling(t_vec3 pos, t_tri tri);
int				reflection_backface(t_tri t1, t_tri t2);
void			calculate_side_normals(t_vec3 normal[4], t_vec3 corner[4]);
void			face_border_normals(t_vec3 v[4], t_vec3 dir, t_tri target);
void			shadow_face_culling(t_level *level, int i);
void			vertex_directions(t_vec3 corner[4], t_level *l, int i,
					t_vec3 pos);
t_vec3			camera_to_reflection(t_level *level, int i);
void			reflection_culling_first_bounce(t_level *level, int i);
int				face_distance_culling(t_tri t1, t_tri t2, t_level *level);
void			reflection_culling(t_level *level, int i);
void			opacity_culling(t_level *level, int i, t_obj *obj);
void			free_culling(t_level *level);
void			init_culling(t_level *level);
void			calculate_corner_vectors(t_vec3 corner[4], t_camera *cam);
void			skybox_culling(t_level *level, t_camera *cam,
					t_vec3 side_normals[4]);
void			static_culling(t_level *l);
void			cull_visible(t_level *l, t_vec3 side_normals[4]);
void			culling(t_level *l);
int				obj_check_polygon_equality(t_obj *a, t_obj *b);
int				load_animation(char *get_filename,
					t_obj_animation *animation, int amount, float duration);
void			obj_copy(t_obj *target, t_obj *source);
t_obj			get_animation_target(t_obj_animation *animation);
void			tri_lerp(t_tri *target, t_tri *a, t_tri *b, float t);
void			interpolate_tris(t_obj *target, t_obj_animation *animation,
					int node_amount, float time);
void			play_animation(t_obj *target, t_obj_animation *animation,
					unsigned int start_time);
float			pixel_dot_product(t_ivec2 i, t_ivec2 mid, t_window *win);
void			ssao_kernel_iter(t_ssao *ssao, t_window *win, t_ivec2 i,
					float br);
void			ssao_bounds(t_ssao *ssao);
float			ssao_avg(t_ssao ssao, t_level *level);
float			surrounding_diff(t_ssao ssao, t_level *level, t_window *win);
void			ssao_color(t_window *win, t_level *level, float amount,
					t_ivec2 coords);
void			ssao_calculate(t_window *win, t_level *level, int thread_id);
int				ssao_thread(void *data_pointer);
void			ssao(t_window *window, t_level *level);
void			typing_input_backspace(t_level *level);
void			typing_input(t_level *level, SDL_Event event);
void			set_mouse_input_location_3d_space(t_level *level);
void			get_mouse_coordinate(t_level *level, int *x, int *y);
void			set_mouse_input_location(t_level *level,
					t_game_state game_state);
void			mouse_input(t_level *level, SDL_Event event);
void			toggle_mouse_capture(t_level *level, t_window *window,
					t_game_state *game_state);
void			ui_go_back(t_level *level, t_game_state *game_state);
void			keyboard_input(t_window *window, t_level *level,
					SDL_Event event, t_game_state *game_state);
void			read_input(t_window *window, t_level *level,
					t_game_state *game_state);
void			create_projectile(t_game_logic *logic, t_vec3 pos, t_vec3 dir,
					t_projectile get);
void			remove_projectile(t_game_logic *logic, int i);
int				projectile_collision_dynamic(t_projectile *projectile,
					t_level *level);
int				projectile_collision_static(t_projectile *projectile,
					t_level *level, t_vec3 next_pos);
void			projectiles_update(t_level *level);
void			free_file2d(char **file);
char			**file2d_internal(char *file, long size);
char			**file2d(char *filename);
char			**file2d_from_memory(unsigned char *data);
unsigned char	*read_embedded_file(char *sector_name);
unsigned long	get_embedded_size(char *sector_name);
void			update_camera(t_level *l);
void			render_raster(t_window *window, t_level *level);
void			render_ui(t_window *window, t_level *level,
					t_game_state *game_state);
void			raycast_finish(t_window *window, t_level *level);
void			render(t_window *window, t_level *level,
					t_game_state *game_state);
void			tick_forward(t_level *level, t_game_state *game_state);
void			viewmodel(t_window *window, t_level *level,
					t_game_state *game_state);
void			dnukem(t_window *window, t_level *level,
					t_game_state game_state);
int				main(int argc, char **argv);
void			enemy_turn(t_enemy *enemy);
void			enemy_vision(t_enemy *enemy, t_level *level,
					t_enemy_settings *settings);
void			enemy_attack(t_enemy *enemy, t_level *level);
void			enemy_move(t_enemy *enemy, t_level *level);
void			enemy_state_machine(t_enemy *enemy, t_level *level);
void			bmp_set(t_bmp *img, int *tmp);
int				bmp_error_check(t_bmp_fileheader fh, t_bmp_infoheader ih);
t_bmp			bmp_read(char *filename);
t_bmp			bmp_read_from_memory(unsigned char *data);
void			transform_quad(t_tri *tri, t_vec3 dir);
void			move_selected(t_level *l, t_vec3 dir);
void			obj_editor_input_move(t_level *level, t_vec3 move_amount,
					t_vec3 avg, int selected_vert_amount);
void			obj_editor_input(t_level *level, t_vec3 move_amount);
void			set_new_face_pos(t_tri *tri, t_vec3 avg, float scale);
void			set_new_face(t_tri *tri, t_vec3 pos, t_vec3 dir, float scale);
void			add_face(t_level *level);
void			remove_tri(t_level *level, int i);
void			remove_faces(t_level *level);
void			set_obj(t_level *level, char *filename);
void			set_texture(t_level *level, char *filename);
void			set_normal_map(t_level *level, char *filename);
void			set_skybox(t_level *level, char *filename);
void			set_spray(t_level *level, char *filename);
void			set_win_pos(t_level *level);
void			set_spawn_pos(t_level *level);
void			delete_enemy(t_level *level);
void			add_enemy(t_level *level);
void			enemy_spawn(t_enemy *enemy, t_enemy_settings *settings);
void			spawn_enemies(t_level *level);
void			enemies_update(t_level *level);
float			opacity_full_check(t_cast_result *res, t_level *l, t_obj *obj,
					float opacity);
void			opacity(t_cast_result *res, t_level *l, t_obj *obj,
					float opacity);
t_color			sunlight(t_level *l, t_cast_result *res, t_color light);
void			color_set(t_color *col, float value);
t_color			lights(t_level *l, t_cast_result *res);
void			reflection(t_cast_result *res, t_level *l, t_obj *obj);
char			*init_rule30(void);
unsigned int	shader_rule30(t_vec3 pos);
void			handle_jump_sound(t_level *level);
void			reload_audio(t_level *level);
void			handle_audio(t_level *level);
void			draw_camera_path(char *str, t_camera_path *path,
					unsigned int *texture, t_level *level);
void			cam_lerp(t_camera *cam, t_player_pos a, t_player_pos b,
					float f);
t_vec3			vec_bezier_middle(t_vec3 a, t_vec3 b, t_vec3 c, float f);
float			bezier_float_middle(float a, float b, float c, float f);
void			camera_path_bezier(t_camera_path *path, t_camera *cam,
					float time);
void			camera_path_delete_pos(t_camera_path *path, int index);
void			camera_path_add_pos(t_camera_path *path, t_camera c);
float			path_get_percentage(t_camera_path *path);
void			camera_path_set(t_camera_path *path, t_camera *cam);
float			lerp(float a, float b, float f);
t_vec3			vec_interpolate(t_vec3 a, t_vec3 b, float f);
void			vec2_normalize(t_vec2 *vec);
void			vec_normalize(t_vec3 *vec);
float			vec_length(t_vec3 vec);
float			vec2_length(t_vec2 vec);
float			vec_dot(t_vec3 ve1, t_vec3 ve2);
int				vec_cmp(t_vec3 ve1, t_vec3 ve2);
void			vec2_avg(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void			vec2_add(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec2_mult(t_vec2 *res, float mult);
void			vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void			vec2_sub(t_vec2 *res, t_vec2 ve1, t_vec2 ve2);
void			vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2);
void			vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v);
void			vec_rot(t_vec3 *res, t_vec3 ve1, float ang);
float			vec_angle(t_vec3 v1, t_vec3 v2);
void			vec_mult(t_vec3 *res, float mult);
void			vec_div(t_vec3 *res, float div);
float			vec_dist(t_vec3 ve1, t_vec3 ve2);
unsigned int	crossfade(unsigned int color1, unsigned int color2,
					unsigned int fade, unsigned int alpha);
unsigned int	brightness(unsigned int color1, t_color new);
t_color			get_skybox_brightness(t_level *l);
void			skybox_reset_result(t_level *l, t_cast_result *res);
float			skybox_sun_fade(t_color *col, t_cast_result *res, t_level *l);
void			skybox(t_level *l, t_cast_result *res);
void			fog(unsigned int *color, float dist, unsigned int fog_color,
					t_level *level);
void			blur_pixels(unsigned int *color, int gap);
int				smooth_color_kernel(unsigned int *pixels, int gap, int x,
					int y);
int				smooth_color(unsigned int *pixels, int gap, int x, int y);
void			fill_one_pixel(unsigned int *grid, int gap, t_ivec2 coord,
					int s);
void			fill_pixels(unsigned int *grid, int gap, int smooth);
unsigned int	vec_to_color(t_vec3 color);
t_vec3			color_to_vec(unsigned int color);
unsigned int	sharpen_kernel(unsigned int *buf, int i, float center,
					float neighbor);
void			sharpen(unsigned int *pixels, unsigned int *buf, float amount);
void			post_process(t_window *window, t_level *level);
SDL_Color		get_sdl_color(unsigned int color);
t_vec3			get_normal(int vec);
void			wrap_coords_inverted(int *x, int *y, int max_x, int max_y);
void			normal_map(float u, float v, t_tri t, t_cast_result *res);
void			baked_color(t_cast_result *res, int x, int y);
void			face_color(float u, float v, t_tri t, t_cast_result *res);
unsigned int	set_lightness(unsigned int color, float b);
unsigned int	set_saturation(unsigned int color, float s);
void			hsl_update_color(t_color_hsl *c);
void			uv_pixel_put(int x, int y, int color, unsigned int *texture);
float			get_texture_scale(t_bmp *img);
void			uv_print_line(t_vec2 start, t_vec2 stop, t_ivec2 color,
					unsigned int *pixels);
void			put_uv_vertex(t_vec2 vertex, int color, unsigned int *pixels);
t_vec2			*find_closest_to_mouse(t_level *level, t_vec2 *txtr,
					t_vec2 *screen_pos, t_ivec2 *mouse);
void			update_uv_closest_vertex(t_level *level, float image_scale,
					t_ivec2 offset, t_ivec2 mouse);
void			set_fourth_vertex_uv(t_tri *a);
int				get_uv_wf_line_settings(int k, t_vec2 *start, t_vec2 *stop,
					t_uv_parameters param);
t_ivec2			get_uv_wf_line_color(int k, int next);
void			draw_face_uv(t_level *level, t_uv_parameters param,
					t_ivec2 mouse);
void			uv_wireframe_selected(t_level *level, t_uv_parameters param,
					t_ivec2 *mouse);
void			uv_wireframe(t_level *level, t_ivec2 offset,
					unsigned int *pixels, float image_scale);
void			draw_texture(t_level *level, unsigned int *pixels,
					float image_scale, t_ivec2 offset);
void			uv_editor(t_level *level, unsigned int *pixels);
void			enable_uv_editor(t_level *level);
int				get_fps(void);
int				get_selected_amount(t_level *level);
void			copy_tri_settings(t_tri *a, t_tri *b);
void			ui_config_projectile_settings(t_projectile *projectile);
void			ui_config_enemy_settings(t_enemy_settings *enemy);
void			ui_enemy_and_damage_settings(t_level *level);
void			ui_config_face_perlin_settings(t_perlin_settings *p);
void			ui_config_face_perlin(t_perlin_settings *perlin,
					t_level *level);
void			ui_confing_face_render_settings(t_tri *tri, t_level *level);
void			ui_confing_face_settings(t_level *level, int selected_amount,
					t_tri *tri);
void			ui_config_selected_faces(t_level *level);
void			make_fileopen_call(t_level *level, char *file);
void			ui_loop_directory_callback(int isdir, char *name, void *data);
void			ui_render_directory_header(t_level *level);
void			ui_render_directory(t_level *level);
int				nothing_selected(t_level *level);
void			ui_render_info(t_editor_ui *ui, t_level *level);
void			center_screen_print_line(t_vec2 dir, unsigned int color);
void			ui_physics_info(t_editor_ui *ui, t_level *level);
void			ui_post_process_go_back(t_level *level);
void			ui_post_process_settings(t_level *level);
void			ui_render_settings(t_level *level);
void			ui_settings_volume(t_level *level);
void			ui_settings(t_level *level);
void			ui_door_settings(t_level *level);
void			ui_door_editor(t_level *level);
void			ui_single_light_settings(t_level *level);
void			ui_level_light_settings(t_level *level);
void			ui_light_editor_bake(t_level *level);
void			bake_buttons(t_level *level);
void			ui_light_editor(t_level *level);
void			ui_game_settings_delete_selected(t_level *level);
void			ui_animation_settings(t_level *level);
void			ui_game_settings(t_level *level);
void			choose_shader(t_tri *tri);
void			ui_shader_settings(t_level *level);
void			ui_level_settings(t_level *level);
void			ui_editor(t_level *level);
void			ui_baking(t_level *level);
int				editor_select(t_level *level);
void			select_editor_ui(t_level *level);
void			ui_main_menu(t_window *window, t_level *level,
					t_game_state *game_state);
void			reset_ui_state(t_ui_state *state, t_level *level);
void			ui(t_window *window, t_level *level, t_game_state *game_state);
t_level			*get_level(t_level *get_level);
t_window		*get_window(t_window *get_window);
t_ui_state		*get_ui_state(t_ui_state *get_state);
int				edit_slider_var(float *unit, t_ui_state *state);
int				edit_call_var(t_ui_state *state, t_rect rect);
int				edit_button_var(int *var, t_ui_state *state);
t_rect			render_call(t_ivec2 *size, t_ui_state *state);
void			render_button(unsigned int *texture, int *var, int dy,
					int color);
void			render_slider_button(unsigned int *texture, float pos, int dy,
					int color);
void			render_color_slider(t_window *window, float pos, int dy,
					unsigned int *colors);
void			render_slider(unsigned int *texture, float pos, int dy,
					int color);
void			text(char *str);
int				button(int *var, char *str);
float			clamp(float var, float min, float max);
int				int_slider(int *var, char *str, int min, int max);
int				float_slider(float *var, char *str, float min, float max);
void			generate_color_slider_saturation(unsigned int *res, int color);
void			generate_color_slider_lightness(unsigned int *res, int color);
int				color_slider(t_color_hsl *var, char *str);
void			file_save(char *str, char *extension, void (*f)(t_level *,
						char *));
void			file_browser(char *str, char *extension, void (*f)(t_level *,
						char *));
void			text_input(char *str, t_level *level);
int				call(char *str, void (*f)(t_level *));
void			main_menu_text_background(t_rect rect, unsigned int *pixels);
void			main_menu_title(t_bmp *img, unsigned int *pixels);
int				mouse_collision(t_rect rect, t_ivec2 mouse);
int				main_menu_button_text(char *text, int index, t_level *level,
					unsigned int *pixels);
void			fix_faces(t_level *level);
void			main_menu_buttons_level(t_game_state *game_state,
					int *state_changed, t_level *level, unsigned int *pixels);
void			main_menu_buttons_other(t_game_state *game_state,
					int *state_changed, t_level *level, unsigned int *pixels);
void			main_menu(t_level *level, unsigned int *pixels,
					t_game_state *game_state);
unsigned int	noise(unsigned int color, float probability, float amount);
unsigned int	black_and_white(unsigned int color, float amount);
unsigned int	chroma_px(t_bmp *img, int x, int y, int x_amount);
void			chromatic_abberation(unsigned int *pixels, unsigned int *buf,
					int amount);
void			fake_analog_signal(t_bmp *img, unsigned int *pixels,
					float amount);
unsigned int	cycle_rgb(unsigned int time);
void			pixel_put_hud(int x, int y, int color, unsigned int *texture);
void			death_overlay(unsigned int *pixels);
void			crosshair(unsigned int *pixels, int x, int y);
void			ingame_hud(t_level *level, unsigned int *pixels);
void			hud(t_level *level, unsigned int *pixels);
void			button_pixel_put(int x, int y, int color,
					unsigned int *texture);
void			set_text_color(int color);
SDL_Color		get_text_color(void);
t_ivec2			render_text_internal(char *str, t_window *window,
					TTF_Font *font, t_ivec2 pos);
t_ivec2			render_text(char *str, int x, int y);
void			render_text_3d(char *str, t_vec3 pos, unsigned int color,
					t_level *level);
void			render_ssp_visual_background(unsigned int *texture);
void			ssp_visual_text_text(t_level *level, int max_tris, int x,
					int y);
void			render_ssp_visual_text(t_level *level);
void			ui_render_background(t_window *window, t_level *level);
void			init_ui_state(t_level *level);
void			init_ui_settings_post_process(t_editor_ui *ui);
void			init_ui_settings(t_editor_ui *ui);
void			init_color_slider(t_level *level);
void			init_ui(t_window *window, t_level *level);
void			gizmo_pixel_put(int x, int y, int color, unsigned int *texture);
void			gizmo_print_line(t_vec3 start, t_vec3 stop, int color,
					unsigned int *pixels);
t_vec3			vec_sub_return(t_vec3 ve1, t_vec3 ve2);
void			put_gizmo_box(t_vec3 vertex, int color, unsigned int *pixels);
void			gizmo_render(t_level *level, unsigned int *pixels);
int				is_near(int a, int b, int range);
t_vec3			calc_move_screen_space(int index, int amount);
void			gizmo_call_move(t_level *level, t_vec3 move_amount);
void			gizmo_set_mouse_location(t_level *level, int deltax, int deltay,
					int drag_direction);
void			gizmo_move_amount(t_level *level, t_ivec2 mouse,
					int drag_direction);
void			gizmo_drag(t_level *level, t_vec3 x, t_vec3 y, t_vec3 z);
void			gizmo(t_level *level);
void			light_put_text(t_level *level);
void			select_light(t_level *level);
void			move_light(t_level *level, t_vec3 move_amount);
void			delete_all_lights(t_level *level);
void			delete_light(t_level *level);
void			add_light(t_level *level);
void			enable_light_editor(t_level *level);
void			nonfatal_error(char *message);
void			ui_remove_expired_nonfatal_start_times(t_level *level);
void			ui_remove_expired_nonfatal_errors(t_level *level);
int				ui_nonfatal_get_fade(t_level *level, int i);
void			ui_render_nonfatal_errors(t_level *level);
float			obj_find_lowest_point(t_obj *obj);
int				obj_pos_set_to_floor(t_vec3 *vec, t_obj *obj,
					t_level *level);
void			add_health_box(t_level *level);
void			add_ammo_box(t_level *level);
void			delete_health_box(t_level *level);
void			delete_ammo_box(t_level *level);
void			move_selected_vec(t_level *level, t_vec3 move_amount,
					t_vec3 *vec);
void			game_logic_move_selected(t_level *level, t_vec3 move);
int				check_if_3d_closer(t_vec3 vert, float *dist, t_level *level);
void			game_logic_select_nearest_to_mouse_item(t_level *level,
					float *dist);
void			game_logic_select_nearest_to_mouse_not_item(t_level *level,
					float *dist);
void			game_logic_select_nearest_to_mouse(t_level *level);
void			game_logic_put_info(t_level *level, unsigned int *texture);
void			select_door(int index, t_level *level);
void			deselect_door(t_level *level);
void			find_selected_door_index(t_level *level);
void			door_find_avg(t_level *level);
void			door_put_text(t_level *level);
void			door_activation_move(t_level *level, t_vec3 move_amount);
void			delete_all_doors(t_level *level);
void			delete_door(t_level *level);
t_door			*alloc_new_door(t_level *level);
void			add_new_door(t_level *level);
void			set_door_pos(t_level *level, int is_pos2);
void			set_door_pos_1(t_level *level);
void			set_door_pos_2(t_level *level);
void			enable_door_editor(t_level *level);
void			serialize_vec2(t_vec2 vec, t_buffer *buf);
void			serialize_vec3(t_vec3 vec, t_buffer *buf);
void			serialize_color(t_color_hsl color, t_buffer *buf);
void			serialize_settings(t_level *level, t_buffer *buf);
void			serialize_vert(t_vert *vert, t_buffer *buf);
void			serialize_enemy_settings(t_enemy_settings *enemy,
					t_buffer *buf);
void			serialize_projectile(t_projectile *projectile, t_buffer *buf);
void			serialize_player_pos(t_player_pos *pos, t_buffer *buf);
void			serialize_perlin_settings(t_perlin_settings *p, t_buffer *buf);
void			serialize_tri(t_tri *tri, t_buffer *buf);
void			serialize_obj(t_obj *obj, t_buffer *buf);
void			serialize_bmp(t_bmp *bmp, t_buffer *buf);
void			serialize_string(char *str, t_buffer *buf);
void			serialize_door(t_door *door, t_buffer *buf);
void			serialize_doors(t_level *level, t_buffer *buf);
void			serialize_lights(t_level *level, t_buffer *buf);
void			serialize_level_images(t_level *level, t_buffer *buf);
void			serialize_pickups(t_level *level, t_buffer *buf);
void			serialize_enemies(t_level *level, t_buffer *buf);
void			serialize_menu_anim(t_level *level, t_buffer *buf);
void			serialize_level(t_level *level, t_buffer *buf);
float			ntoh_float(float value);
void			deserialize_float(float *x, t_buffer *buf);
void			deserialize_int(int *x, t_buffer *buf);
void			save_file(t_level *level, t_buffer *buf);
void			open_file(char *filename, t_buffer *buf);
void			save_file(t_level *level, t_buffer *buf);
void			open_file(char *filename, t_buffer *buf);
void			open_level(t_level *level, char *filename);
void			deserialize_vec2(t_vec2 *vec, t_buffer *buf);
void			deserialize_vec3(t_vec3 *vec, t_buffer *buf);
void			deserialize_color(t_color_hsl *color, t_buffer *buf);
void			deserialize_settings(t_level *level, t_buffer *buf);
void			deserialize_vert(t_vert *vert, t_buffer *buf);
void			deserialize_enemy_settings(t_enemy_settings *enemy,
					t_buffer *buf);
void			deserialize_projectile(t_projectile *projectile, t_buffer *buf);
void			deserialize_player_pos(t_player_pos *pos, t_buffer *buf);
void			deserialize_perlin_settings(t_perlin_settings *p,
					t_buffer *buf);
void			deserialize_tri(t_tri *tri, t_buffer *buf);
void			deserialize_obj(t_obj *obj, t_buffer *buf);
void			deserialize_bmp(t_bmp *bmp, t_buffer *buf);
char			*deserialize_string(int len, t_buffer *buf);
void			malloc_door(t_door *door);
void			deserialize_door_pos(t_door *door, t_buffer *buf,
					int door_index);
void			deserialize_door(t_door *door, t_buffer *buf);
void			free_doors(t_level *level);
void			deserialize_doors(t_level *level, t_buffer *buf);
void			deserialize_lights(t_level *level, t_buffer *buf);
void			deserialize_level_images(t_level *level, t_buffer *buf);
void			deserialize_health_pickups(t_level *level, t_buffer *buf);
void			deserialize_ammo_pickups(t_level *level, t_buffer *buf);
void			deserialize_pickups(t_level *level, t_buffer *buf);
void			deserialize_enemies(t_level *level, t_buffer *buf);
void			deserialize_menu_anim(t_level *level, t_buffer *buf);
int				check_file_header(t_buffer *buf);
void			deserialize_level(t_level *level, t_buffer *buf);
void			reserve_space(t_buffer *buf, size_t bytes);
float			hton_float(float value);
void			serialize_float(float x, t_buffer *buf);
void			serialize_int(int x, t_buffer *buf);
void			save_level(t_level *level);
#endif
