#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vidd/config.h>

#include <vidd/vidd.h>
#include <vidd/display.h>
#include <vidd/commands.h>
#include <vidd/getch.h>
#include <vidd/fuzzy_find.h>

static void _vidd_load_copy(struct vidd_client* client) { vidd_load_copy(); }

#define VIDD_MULTI_KEY_BIND_START(n, k) \
	void n (struct vidd_client* client) \
	{ \
		uint32_t nkey = getch(false); \
		switch (nkey) \
		{
#define VIDD_MULTI_KEY_BIND_OPTION(k, f) \
		case k: \
		{ \
			f(client); \
		} break;
#define VIDD_MULTI_KEY_BIND_OPTION_DEFAULT(f) \
		default: \
		{ \
			f(client, nkey); \
		} break;
#define VIDD_MULTI_KEY_BIND_END(n) \
		} \
	}

VIDD_MULTI_KEY_BIND_START(vidd_delete_object_multi_key, 'a')
	VIDD_MULTI_KEY_BIND_OPTION('w', vidd_delete_word)
VIDD_MULTI_KEY_BIND_END(vidd_delete_object_multi_key)

VIDD_MULTI_KEY_BIND_START(vidd_multi_key_delete, 'd')
	VIDD_MULTI_KEY_BIND_OPTION('d', vidd_delete_line)
	VIDD_MULTI_KEY_BIND_OPTION('j', vidd_delete_two_lines_down)
	VIDD_MULTI_KEY_BIND_OPTION('k', vidd_delete_two_lines_up)
	VIDD_MULTI_KEY_BIND_OPTION('a', vidd_delete_object_multi_key)
	VIDD_MULTI_KEY_BIND_OPTION_DEFAULT(vidd_delete_movement)
VIDD_MULTI_KEY_BIND_END(vidd_multi_key_delete)

VIDD_MULTI_KEY_BIND_START(vidd_multi_key_copy, 'y')
	VIDD_MULTI_KEY_BIND_OPTION('y', vidd_copy_line)
VIDD_MULTI_KEY_BIND_END(vidd_multi_key_copy)

VIDD_MULTI_KEY_BIND_START(vidd_multi_key_space, ' ')
	VIDD_MULTI_KEY_BIND_OPTION('1', vidd_switch_to_tab1)
	VIDD_MULTI_KEY_BIND_OPTION('2', vidd_switch_to_tab2)
	VIDD_MULTI_KEY_BIND_OPTION('3', vidd_switch_to_tab3)
	VIDD_MULTI_KEY_BIND_OPTION('4', vidd_switch_to_tab4)
	VIDD_MULTI_KEY_BIND_OPTION('5', vidd_switch_to_tab5)
	VIDD_MULTI_KEY_BIND_OPTION('6', vidd_switch_to_tab6)
	VIDD_MULTI_KEY_BIND_OPTION('7', vidd_switch_to_tab7)
	VIDD_MULTI_KEY_BIND_OPTION('8', vidd_switch_to_tab8)
	VIDD_MULTI_KEY_BIND_OPTION('9', vidd_switch_to_tab9)
	VIDD_MULTI_KEY_BIND_OPTION('r', vidd_run_line)
	VIDD_MULTI_KEY_BIND_OPTION('e', vidd_fuzzy_find_edit)
	VIDD_MULTI_KEY_BIND_OPTION('o', vidd_fuzzy_find_open)
	VIDD_MULTI_KEY_BIND_OPTION('f', vidd_fuzzy_find_float)
	VIDD_MULTI_KEY_BIND_OPTION('m', vidd_man_word)
	VIDD_MULTI_KEY_BIND_OPTION('y', _vidd_load_copy)
	VIDD_MULTI_KEY_BIND_OPTION('c', vidd_next_layout)
VIDD_MULTI_KEY_BIND_END(vidd_multi_key_space)

static void vidd_multi_key_delete_then_insert(struct vidd_client* client)
{
	vidd_multi_key_delete(client);
	vidd_enter_insert_mode(client);
}

struct vidd_keybind vidd_normal_mode_keybinds[] = {
	[0 ... 510]={vidd_void, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('c')]={(void(*)(struct vidd_client*))vidd_client_quit, VIDD_ACTION_MOVEMENT},
	[':']={vidd_enter_command_mode, VIDD_ACTION_MOVEMENT},
	['/']={vidd_enter_find_next_mode, VIDD_ACTION_MOVEMENT},
	['?']={vidd_enter_find_prev_mode, VIDD_ACTION_MOVEMENT},
	['>']={vidd_indent, VIDD_ACTION_EDIT},
	['<']={vidd_deindent, VIDD_ACTION_EDIT},
	['v']={vidd_enter_select_mode, VIDD_ACTION_MOVEMENT},
	['V']={vidd_enter_line_select_mode, VIDD_ACTION_MOVEMENT},
	['i']={vidd_enter_insert_mode, VIDD_ACTION_EDIT},
	['a']={vidd_enter_insert_mode_right, VIDD_ACTION_EDIT},
	['o']={vidd_insert_on_new_line, VIDD_ACTION_EDIT},
	['O']={vidd_insert_on_new_line_up, VIDD_ACTION_EDIT},
	['x']={vidd_delete, VIDD_ACTION_EDIT},
	[KEY_DELETE]={vidd_delete, VIDD_ACTION_EDIT},
	['X']={vidd_backspace, VIDD_ACTION_EDIT},
	['c']={vidd_multi_key_delete_then_insert, VIDD_ACTION_EDIT},
	[' ']={vidd_multi_key_space, VIDD_ACTION_MOVEMENT},
	['d']={vidd_multi_key_delete, VIDD_ACTION_EDIT},
	['D']={vidd_delete_line, VIDD_ACTION_EDIT},
	['r']={vidd_replace_character, VIDD_ACTION_EDIT},
	['R']={vidd_enter_replace_mode, VIDD_ACTION_MOVEMENT},
	['w']={vidd_move_next_word, VIDD_ACTION_MOVEMENT},
	['e']={vidd_move_next_word_end, VIDD_ACTION_MOVEMENT},
	['b']={vidd_move_prev_word, VIDD_ACTION_MOVEMENT},
	['f']={vidd_find_next_char, VIDD_ACTION_MOVEMENT},
	['F']={vidd_find_prev_char, VIDD_ACTION_MOVEMENT},
	['}']={vidd_goto_next_paragraph, VIDD_ACTION_MOVEMENT},
	['{']={vidd_goto_prev_paragraph, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('n')]={vidd_toggle_comment, VIDD_ACTION_EDIT},

	[KEY_LEFT]={vidd_move_left, VIDD_ACTION_MOVEMENT},
	[KEY_DOWN]={vidd_move_down, VIDD_ACTION_MOVEMENT},
	[KEY_UP]={vidd_move_up, VIDD_ACTION_MOVEMENT},
	[KEY_RIGHT]={vidd_move_right, VIDD_ACTION_MOVEMENT},
	['h']={vidd_move_left, VIDD_ACTION_MOVEMENT},
	['j']={vidd_move_down, VIDD_ACTION_MOVEMENT},
	['k']={vidd_move_up, VIDD_ACTION_MOVEMENT},
	['l']={vidd_move_right, VIDD_ACTION_MOVEMENT},
	['H']={vidd_view_move_left, VIDD_ACTION_MOVEMENT},
	['L']={vidd_view_move_right, VIDD_ACTION_MOVEMENT},
	['K']={vidd_view_move_up, VIDD_ACTION_MOVEMENT},
	['J']={vidd_view_move_down, VIDD_ACTION_MOVEMENT},

	['n']={vidd_repeat_last_find, VIDD_ACTION_MOVEMENT},
	['N']={vidd_repeat_last_find_reverse, VIDD_ACTION_MOVEMENT},
	['*']={vidd_find_next_word_under_cursor, VIDD_ACTION_MOVEMENT},
	['#']={vidd_find_prev_word_under_cursor, VIDD_ACTION_MOVEMENT},
	['M']={vidd_set_marker, VIDD_ACTION_MOVEMENT},
	['m']={vidd_goto_marker, VIDD_ACTION_MOVEMENT},
	['z']={vidd_view_center, VIDD_ACTION_MOVEMENT},
	['Z']={vidd_cursor_center, VIDD_ACTION_MOVEMENT},
	['g']={vidd_goto_top, VIDD_ACTION_MOVEMENT},
	['G']={vidd_goto_bottom, VIDD_ACTION_MOVEMENT},
	['y']={vidd_multi_key_copy, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('y')]={vidd_copy_to_clipboard, VIDD_ACTION_MOVEMENT},
	['p']={vidd_paste, VIDD_ACTION_EDIT},
	['$']={vidd_move_to_line_end, VIDD_ACTION_MOVEMENT},
	['0']={vidd_move_to_line_start, VIDD_ACTION_MOVEMENT},
	['^']={vidd_move_to_line_start, VIDD_ACTION_MOVEMENT},
	['q']={vidd_macro_toggle, VIDD_ACTION_MOVEMENT},
	['@']={vidd_macro_run, VIDD_ACTION_MOVEMENT},
	[KEY_RETURN]={vidd_swap, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL_BACKSLASH]={vidd_duplicate, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('j')]={vidd_client_next, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('k')]={vidd_client_prev, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('h')]={vidd_decrease_master_size, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('l')]={vidd_increase_master_size, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('w')]={vidd_enter_window_move_mode, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('i')]={vidd_increase_master_count, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('d')]={vidd_decrease_master_count, VIDD_ACTION_MOVEMENT},
	[27]={vidd_enter_normal_mode, VIDD_ACTION_MOVEMENT},
	[255]={vidd_toggle_drawing, VIDD_ACTION_MOVEMENT},
	[253]={vidd_save_spot, VIDD_ACTION_MOVEMENT},
	[254]={vidd_load_spot, VIDD_ACTION_MOVEMENT},
	[443]={vidd_toggle_drawing, VIDD_ACTION_MOVEMENT},
	[0]={vidd_floating_toggle, VIDD_ACTION_MOVEMENT},
	['~']={vidd_toggle_case, VIDD_ACTION_MOVEMENT},
	['`']={vidd_run_make, VIDD_ACTION_MOVEMENT},
};
int vidd_normal_mode_keybinds_length = sizeof(vidd_normal_mode_keybinds) / sizeof(vidd_normal_mode_keybinds[0]);

struct vidd_keybind vidd_window_move_mode_keybinds[] = {
	[0 ... 510]={0, VIDD_ACTION_MOVEMENT},
	[KEY_ESCAPE]={vidd_enter_normal_mode, VIDD_ACTION_MOVEMENT},
	[KEY_CTRL('w')]={vidd_enter_normal_mode, VIDD_ACTION_MOVEMENT},
	['j']={vidd_move_floating_down, VIDD_ACTION_MOVEMENT},
	['k']={vidd_move_floating_up, VIDD_ACTION_MOVEMENT},
	['l']={vidd_move_floating_right, VIDD_ACTION_MOVEMENT},
	['h']={vidd_move_floating_left, VIDD_ACTION_MOVEMENT},
	['o']={vidd_floating_increase_width, VIDD_ACTION_MOVEMENT},
	['y']={vidd_floating_decrease_width, VIDD_ACTION_MOVEMENT},
	['u']={vidd_floating_increase_height, VIDD_ACTION_MOVEMENT},
	['i']={vidd_floating_decrease_height, VIDD_ACTION_MOVEMENT},
};
int vidd_window_move_mode_keybinds_length = sizeof(vidd_window_move_mode_keybinds) / sizeof(vidd_window_move_mode_keybinds[0]);

struct vidd_keybind vidd_select_mode_keybinds[] = {
	[0 ... 510]={0, VIDD_ACTION_MOVEMENT},
	['x']={vidd_delete_selection, VIDD_ACTION_EDIT},
	['d']={vidd_delete_selection, VIDD_ACTION_EDIT},
	['y']={vidd_selection_copy, VIDD_ACTION_MOVEMENT},
	['s']={vidd_selection_swap_cursor, VIDD_ACTION_MOVEMENT},
	['>']={vidd_selection_indent, VIDD_ACTION_EDIT},
	['<']={vidd_selection_deindent, VIDD_ACTION_EDIT},
	[KEY_CTRL('n')]={vidd_selection_toggle_comment, VIDD_ACTION_EDIT},
};
int vidd_select_mode_keybinds_length = sizeof(vidd_select_mode_keybinds) / sizeof(vidd_select_mode_keybinds[0]);

struct vidd_keybind vidd_insert_mode_keybinds[] = {
	[0 ... 510]={vidd_insert_char, VIDD_ACTION_EDIT},
	[KEY_CTRL('c')]={vidd_enter_normal_mode, VIDD_ACTION_MOVEMENT},
	[KEY_BACKSPACE]={vidd_backspace, VIDD_ACTION_EDIT},
	[KEY_RETURN]={vidd_split_at_cursor, VIDD_ACTION_EDIT},
	[KEY_DELETE]={vidd_delete, VIDD_ACTION_EDIT},
	[KEY_CTRL_BACKSPACE]={vidd_delete, VIDD_ACTION_EDIT},
	[KEY_LEFT]={vidd_move_left, VIDD_ACTION_MOVEMENT},
	[KEY_DOWN]={vidd_move_down, VIDD_ACTION_MOVEMENT},
	[KEY_UP]={vidd_move_up, VIDD_ACTION_MOVEMENT},
	[KEY_RIGHT]={vidd_move_right, VIDD_ACTION_MOVEMENT},
	[KEY_ESCAPE]={vidd_exit_insert_mode, VIDD_ACTION_MOVEMENT},
};
int vidd_insert_mode_keybinds_length = sizeof(vidd_insert_mode_keybinds) / sizeof(vidd_insert_mode_keybinds[0]);

struct command vidd_commands[] = {
	{ "w", vidd_write },
	{ "wa", vidd_write_all },
	{ "q", vidd_client_quit },
	{ "q!", vidd_client_force_quit },
	{ "qa", vidd_exit_all },
	{ "qa!", vidd_force_exit_all },
	{ "wq", vidd_write_quit },
	{ "waqa", vidd_write_quit_all },
	{ "e", vidd_edit },
	{ "o", vidd_open },
	{ "!o", vidd_run_command_in_open },
	{ "f", vidd_open_in_floating_window },
	{ "!f", vidd_run_command_in_floating_window },
	{ "!", vidd_run_command },
	{ "syntax", vidd_load_syntax },
	{ "set", vidd_set },
	{ "man", vidd_man },
};
int vidd_commands_length = sizeof(vidd_commands) / sizeof(vidd_commands[0]);

void vidd_run_normal_mode_keybind(struct vidd_client* client)
{
	if (vidd_normal_mode_keybinds[client->key].type == VIDD_ACTION_EDIT)
	{
		if (client->readOnly)
		{
			vidd_show_error(client, "read only file");
			return;
		}
		client->unsavedChanges = true;
		vidd_set_status(client);
	}
	vidd_normal_mode_keybinds[client->key].func(client);
}

void vidd_run_line_select_mode_keybind(struct vidd_client* client)
{
	if (vidd_select_mode_keybinds[client->key].func)
	{
		if (vidd_select_mode_keybinds[client->key].type == VIDD_ACTION_EDIT)
		{
			if (client->readOnly)
			{
				vidd_show_error(client, "read only file");
				return;
			}
			client->unsavedChanges = true;
			vidd_set_status(client);
		}
		vidd_select_mode_keybinds[client->key].func(client);
	}
	else vidd_run_normal_mode_keybind(client);
}

void vidd_run_select_mode_keybind(struct vidd_client* client)
{
	if (vidd_select_mode_keybinds[client->key].func)
	{
		if (vidd_select_mode_keybinds[client->key].type == VIDD_ACTION_EDIT)
		{
			if (client->readOnly)
			{
				vidd_show_error(client, "read only file");
				return;
			}
			client->unsavedChanges = true;
			vidd_set_status(client);
		}
		vidd_select_mode_keybinds[client->key].func(client);
	}
	else vidd_run_normal_mode_keybind(client);
}

void vidd_run_insert_mode_keybind(struct vidd_client* client)
{
	vidd_insert_mode_keybinds[client->key].func(client);
	if (vidd_insert_mode_keybinds[client->key].type == VIDD_ACTION_EDIT)
	{
		client->unsavedChanges = true;
		vidd_set_status(client);
	}
}

void vidd_run_replace_mode_keybind(struct vidd_client* client)
{
	if (client->key == 27)
	{
		client->mode = VIDD_MODE_NORMAL;
		vidd_move_left(client);
		vidd_set_status(client);
	}
	else
	{
		client->cursor.y->buffer.data[client->cursor.x++] = client->key;
		client->unsavedChanges = true;
		vidd_redraw(client);
	}
}

void vidd_run_find_mode_keybind(struct vidd_client* client)
{
	if (client->key == 27)
	{
		buffer_clear(&command_input);
		cursor_restore();
		client->mode = VIDD_MODE_NORMAL;
		vidd_set_status(client);
	}
	else if (client->key == 127)
	{
		if (command_input.length == 0) return;
		cursor_move(-1, 0);
		printf(" ");
		cursor_move(-1, 0);
		buffer_pop(&command_input);
	}
	else if (client->key == KEY_RETURN)
	{
		buffer_copy(&client->last_find, &command_input);
		char* word = command_input.data;

		cursor_restore();

		if (client->mode == VIDD_MODE_FIND)
			vidd_find_next_word(client, word, strlen(word));
		else vidd_find_prev_word(client, word, strlen(word));

		vidd_mode_swap(client, 1);
		vidd_redraw(client);
		//vidd_set_status(client);

		buffer_clear(&command_input);
	}
	else
	{
		buffer_push(&command_input, client->key);
		printf("%c", client->key);
	}
}

#include <vidd/mode_command.h>

void vidd_run_command_mode_keybind(struct vidd_client* client)
{
	vidd_command_mode_interrupt(client, client->key);
}

void vidd_run_window_move_mode_keybind(struct vidd_client* client)
{
	if (vidd_window_move_mode_keybinds[client->key].func)
		vidd_window_move_mode_keybinds[client->key].func(client);
}

void(*vidd_editor_keybinds[])(struct vidd_client*) = {
	[VIDD_MODE_NORMAL]=vidd_run_normal_mode_keybind,
	[VIDD_MODE_INSERT]=vidd_run_insert_mode_keybind,
	[VIDD_MODE_SELECT]=vidd_run_select_mode_keybind,
	[VIDD_MODE_LINE_SELECT]=vidd_run_line_select_mode_keybind,
	[VIDD_MODE_COMMAND]=vidd_run_command_mode_keybind,
	[VIDD_MODE_FIND]=vidd_run_find_mode_keybind,
	[VIDD_MODE_FIND_REVERSE]=vidd_run_find_mode_keybind,
	[VIDD_MODE_REPLACE]=vidd_run_replace_mode_keybind,
	[VIDD_MODE_WINDOW_MOVE]=vidd_run_window_move_mode_keybind,
};
int vidd_editor_keybinds_length = sizeof(vidd_editor_keybinds) / sizeof(vidd_editor_keybinds[0]);

#endif
