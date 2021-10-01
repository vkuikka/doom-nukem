/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   audio.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 21:56:12 by alcohen           #+#    #+#             */
/*   Updated: 2021/09/25 15:59:27 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	handle_jump_sound(t_level *level)
{
	const Uint8	*keys;
	t_audio		*audio;

	keys = SDL_GetKeyboardState(NULL);
	if (!keys[SDL_SCANCODE_SPACE])
		return ;
	audio = &level->audio;
	if (!audio->played_jump_sound && is_player_in_air(level, PLAYER_EYE_HEIGHT))
	{
		Mix_PlayChannel(AUDIO_JUMP_CHANNEL, level->audio.jump, 0);
		audio->played_jump_sound = TRUE;
	}
	if (!is_player_in_air(level, PLAYER_EYE_HEIGHT))
	{
		audio->played_jump_sound = FALSE;
	}
}

void	handle_audio(t_level *level, t_game_state *game_state)
{
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
	{
		if (level->game_logic.player.ammo)
			Mix_PlayChannel(AUDIO_GUNSHOT_CHANNEL, level->audio.gunshot, 0);
		else
			Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
	}
	else if (level->game_logic.player.health <= 0)
	{
		Mix_HaltMusic();
		Mix_PlayChannel(AUDIO_DEATH_CHANNEL, level->audio.death, 0);
	}
	if (level->game_logic.reload_start_time
		&& !level->audio.played_reload_sound)
	{
		if (!Mix_Playing(AUDIO_RELOAD_CHANNEL))
			Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
		level->audio.played_reload_sound = TRUE;
	}
	if (level->game_logic.reload_start_time == 0)
		level->audio.played_reload_sound = FALSE;
	handle_jump_sound(level);
}
