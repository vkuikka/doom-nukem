/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   audio.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsjoberg <lsjoberg@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 21:56:12 by alcohen           #+#    #+#             */
/*   Updated: 2021/07/23 16:02:25 by lsjoberg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	handle_jump_sound(t_level *level)
{
	t_audio	*audio;

	audio = &level->audio;
	if (!audio->played_jump_sound && is_player_in_air(level, PLAYER_HEIGHT))
	{
		Mix_PlayChannel(AUDIO_JUMP_CHANNEL, level->audio.jump, 0);
		audio->played_jump_sound = TRUE;
	}
	if (!is_player_in_air(level, PLAYER_HEIGHT))
	{
		audio->played_jump_sound = FALSE;
	}
}

static void	mix_channel(t_level *level)
{
	if (level->player_ammo)
	{
		Mix_PlayChannel(AUDIO_GUNSHOT_CHANNEL, level->audio.gunshot, 0);
	}
	else
	{
		Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
	}
}

void	handle_audio(t_level *level, t_game_state *game_state)
{
	t_audio		*audio;
	const Uint8	*keys;

	audio = &level->audio;
	keys = SDL_GetKeyboardState(NULL);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
		mix_channel(level);
	else if (level->player_health <= 0)
	{
		Mix_HaltMusic();
		Mix_PlayChannel(AUDIO_DEATH_CHANNEL, level->audio.death, 0);
	}
	if (level->reload_start_time && !audio->played_reload_sound)
	{
		if (!Mix_Playing(AUDIO_RELOAD_CHANNEL))
			Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
		audio->played_reload_sound = TRUE;
	}
	if (level->reload_start_time == 0)
		audio->played_reload_sound = FALSE;
	if (keys[SDL_SCANCODE_SPACE])
		handle_jump_sound(level);
}
