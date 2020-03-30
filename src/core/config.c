#include "config.h"

#include "core/file.h"
#include "core/log.h"

#include <stdio.h>
#include <string.h>

#define MAX_LINE 100

static const char *INI_FILENAME = "augustus.ini";

// Keep this in the same order as the config_keys in config.h
static const char *ini_keys[] = {
    "gameplay_fix_immigration",
    "gameplay_fix_100y_ghosts",
    "gameplay_fix_editor_events",
    "ui_sidebar_info",
    "ui_show_intro_video",
    "ui_smooth_scrolling",
    "ui_walker_waypoints",
    "ui_visual_feedback_on_delete",
    "ui_show_water_structure_range",
    "ui_show_construction_size",
    "gameplay_change_grandfestival",
    "gameplay_change_jealous_gods",
    "gameplay_change_global_labour",
    "gameplay_change_school_walkers",
    "gameplay_change_retire_at_60",
    "gameplay_change_fixed_workers",
    "gameplay_enable_extra_forts",
    "gameplay_wolves_block",
    "gameplay_dynamic_granaries",
    "gameplay_houses_stockpile_more",
    "gameplay_buyers_dont_distribute",
    "gameplay_change_immediate_delete",
    "gameplay_change_getting_granaries_go_offroad",
    "gameplay_change_tower_sentries_go_offroad",
};

static const char *ini_string_keys[] = {
    "ui_language_dir"
};

static int values[CONFIG_MAX_ENTRIES];
static char string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX];

int config_get(config_key key)
{
    return values[key];
}

void config_set(config_key key, int value)
{
    values[key] = value;
}

const char *config_get_string(config_string_key key)
{
    return string_values[key];
}

void config_set_string(config_string_key key, const char *value)
{
    if (!value) {
        string_values[key][0] = 0;
    } else {
        strncpy(string_values[key], value, CONFIG_STRING_VALUE_MAX - 1);
    }
}

void config_set_defaults(void)
{
    values[CONFIG_GP_FIX_IMMIGRATION_BUG] = 0;
    values[CONFIG_GP_FIX_100_YEAR_GHOSTS] = 0;
    values[CONFIG_UI_WALKER_WAYPOINTS] = 0;
    values[CONFIG_GP_FIX_EDITOR_EVENTS] = 0;
    values[CONFIG_UI_SIDEBAR_INFO] = 0;
    values[CONFIG_UI_SMOOTH_SCROLLING] = 0;
    values[CONFIG_GP_CH_GRANDFESTIVAL] = 0;
    values[CONFIG_GP_CH_JEALOUS_GODS] = 0;
    values[CONFIG_GP_CH_GLOBAL_LABOUR] = 0;
    values[CONFIG_GP_CH_SCHOOL_WALKERS] = 0;
    values[CONFIG_GP_CH_RETIRE_AT_60] = 0;
    values[CONFIG_GP_CH_FIXED_WORKERS] = 0;
    values[CONFIG_GP_CH_EXTRA_FORTS] = 0;
    values[CONFIG_GP_CH_WOLVES_BLOCK] = 0;
    values[CONFIG_GP_CH_DYNAMIC_GRANARIES] = 0;
    values[CONFIG_GP_CH_MORE_STOCKPILE] = 0;
    values[CONFIG_GP_CH_NO_BUYER_DISTRIBUTION] = 0;
    values[CONFIG_GP_CH_IMMEDIATELY_DELETE_BUILDINGS] = 0;
    values[CONFIG_GP_CH_GETTING_GRANARIES_GO_OFFROAD] = 0;
    values[CONFIG_GP_CH_TOWER_SENTRIES_GO_OFFROAD] = 0;
    values[CONFIG_UI_SHOW_INTRO_VIDEO] = 0;
    values[CONFIG_UI_VISUAL_FEEDBACK_ON_DELETE] = 0;
    values[CONFIG_UI_SHOW_WATER_STRUCTURE_RANGE] = 0;
    values[CONFIG_UI_SHOW_CONSTRUCTION_SIZE] = 0;
    string_values[CONFIG_STRING_UI_LANGUAGE_DIR][0] = 0;
}

void config_load(void)
{
    config_set_defaults();
    FILE *fp = file_open(INI_FILENAME, "rt");
    if (!fp) {
        return;
    }
    char line_buffer[MAX_LINE];
    char *line;
    while ((line = fgets(line_buffer, MAX_LINE, fp))) {
        // Remove newline from string
        int last = strlen(line) - 1;
        while (last >= 0 && (line[last] == '\n' || line[last] == '\r')) {
            line[last] = 0;
            last--;
        }
        char *equals = strchr(line, '=');
        if (equals) {
            *equals = 0;
            for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
                if (strcmp(ini_keys[i], line) == 0) {
                    int value = atoi(&equals[1]);
                    log_info("Config key", ini_keys[i], value);
                    values[i] = value;
                    break;
                }
            }
            for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
                if (strcmp(ini_string_keys[i], line) == 0) {
                    const char *value = &equals[1];
                    log_info("Config key", ini_string_keys[i], 0);
                    log_info("Config value", value, 0);
                    strncpy(string_values[i], value, CONFIG_STRING_VALUE_MAX);
                    break;
                }
            }
        }
    }
    file_close(fp);
}

void config_save(void)
{
    FILE *fp = file_open(INI_FILENAME, "wt");
    if (!fp) {
        log_error("Unable to write configuration file", INI_FILENAME, 0);
        return;
    }
    for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%d\n", ini_keys[i], values[i]);
    }
    for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%s\n", ini_string_keys[i], string_values[i]);
    }
    file_close(fp);
}
