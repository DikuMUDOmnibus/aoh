/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"


bool  check_social      args (( CHAR_DATA *ch, char *command, char *argument ));
bool  check_xsocial     args (( CHAR_DATA *ch, char *command, char *argument ));
void  make_preg         args (( CHAR_DATA *mother, CHAR_DATA *father ));
int   can_interpret     args (( CHAR_DATA *ch, int cmd ));

int can_interpret(CHAR_DATA *ch, int cmd)
{
  bool cando = FALSE;

  if (IS_NPC(ch)) return 0;

  /* immortals can always */
  if (ch->level == MAX_LEVEL) return 1;

  /* checking trust */
  if (cmd_table[cmd].level > get_trust(ch)) return 0;

  /* common commands */
  if (cmd_table[cmd].race == 0 && cmd_table[cmd].powertype == 0)
    cando = TRUE;

  /* class specific commands */
  if (cmd_table[cmd].race > 0 && cmd_table[cmd].powertype == 0 && ch->class == cmd_table[cmd].race)
  {
    if (cmd_table[cmd].powertype > 0 && ch->pcdata->powers[cmd_table[cmd].powertype] < cmd_table[cmd].powerlevel)
      cando = FALSE;
    else
      cando = TRUE;
  }

  if (!cando) return 0;
  if (ch->position < cmd_table[cmd].position) return -1; 

  return 1;
}

void do_racecommands( CHAR_DATA *ch, char *argument )
{
  int i, cmd, foundpower = 0;
  char buf[MAX_STRING_LENGTH];
  bool displayed;

  if (ch->class == 0 || IS_NPC(ch))
  {
    send_to_char("You don't have any special powers.\n\r\n\r", ch);
    return;
  }

  sprintf(buf, "%s\n\r", get_dystopia_banner("Powers", 76));
  send_to_char(buf, ch);

  i = 0;
  displayed = FALSE;
  for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
  {
    /* check for class */
    if (!IS_CLASS(ch, cmd_table[cmd].race))
      continue;

    /* check to see if the player has actually learned the power */
    if (cmd_table[cmd].powertype > 0 && ch->pcdata->powers[cmd_table[cmd].powertype] < cmd_table[cmd].powerlevel)
      continue;

    strcpy(buf, "");

    /* In the first line we also print the class name */
    if (i == 0)
    {
      switch (ch->class)
      {
//        case CLASS_DEMON : sprintf(buf, " %15s : ", "Demon Powers"); break;
      }
    }

    /* 4 powers per line, and then indent */
    else if (i % 4 == 0)
    {
      sprintf(buf, "\n\r                   ");
    }

    i++;
    send_to_char(buf, ch);
    send_to_char(cmd_table[cmd].name,ch);
    send_to_char(" ",ch);
    displayed = TRUE;
    foundpower++;
  }

  if (displayed) send_to_char("\n\r",ch);
  if (foundpower == 0)
  {
    send_to_char("\n\r You do not yet possess any powers.\n\r\n\r", ch);
  }
  sprintf(buf, "%s\n\r", get_dystopia_banner("", 76));
  send_to_char(buf, ch);
  return;
}
	
/*
 * Command logging types.
 */
#define LOG_NORMAL	0  // Doesn't log unless the player is logged
#define LOG_ALWAYS	1  // Will always log this command (both file and logchannel)
#define LOG_NEVER	2  // Will never log this command, no matter what
#define LOG_SEMI        3  // Will always log this command (file only)

/*
 * Log-all switch.
 */
char *  last_command    = NULL;
bool    arena_open	= FALSE;
bool    arena_base      = FALSE;
bool	arenaopen	= FALSE;
bool    ragnarok        = FALSE;
bool	fLogAll		= FALSE;
bool    extra_log	= FALSE;
bool    cmd_done        = TRUE;
bool    arena2          = TRUE;
bool    arena2death     = FALSE;
int     exptimer        = 0;
int     qpstimer        = 0;
int     players_logged  = 0;
int     thread_count    = 0;
int     top_playerid    = 0;
int     ragnarok_cost   = 3000;
int     ragnarok_timer  = PULSE_AREA * 60;
int     pulse_arena     = PULSE_AREA * 60;
int     players_decap   = 0;
int     players_gstolen = 0;
int	iDelete		= 0;
int     total_output    = 0;
int     mudinfo[MUDINFO_MAX];
int     ccenter[CCENTER_MAX];
bool    check_disabled (const struct cmd_type *command);
bool    double_exp          = FALSE;
bool    double_fightstance   = FALSE;
bool    double_coins 		= FALSE;


DISABLED_DATA *disabled_first;
BAN_DATA *ban_list;
BAN_DATA *newbieban_list;

pthread_mutex_t        memory_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    /* Name           Function         Min Position Level    Log   Class, DiscLevel, DiscName */

    /* commone movement commands */
    { "north",        do_north,        POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "east",         do_east,         POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "south",        do_south,        POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "west",         do_west,         POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "up",           do_up,           POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "down",         do_down,         POS_STANDING,  0,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "cast",           do_cast,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 0, 0 },
    { "call",           do_call,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "consider",	do_consider,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "crack",		do_crack,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "diagnose",	do_diagnose,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "dismount",	do_dismount,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "enter",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "exits",		do_exits,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "get",		do_get,		POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "inventory",	do_inventory,   POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "combatswitch",	do_combatswitch,POS_FIGHTING,    0,  LOG_NORMAL, 0, 0, 0 },
    { "look",		do_look,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "ls",		do_look,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "mount",		do_mount,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "order",		do_order,	POS_SITTING,	 1,  LOG_ALWAYS, 0, 0, 0 },
    { "rest",		do_rest,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "reimb",          do_reimb,       POS_MEDITATING, 12,  LOG_NORMAL, 0, 0, 0 },   
    { "sit",		do_sit,	        POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "tell",		do_tell,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "whisper",	do_whisper,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "wield",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,	 4,  LOG_NORMAL, 0, 0, 0 },
    { "version",	do_version,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "linkdead",	do_linkdead,    POS_DEAD,        7,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Informational commands.
     */
    { "affects",        do_affects,     POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "dcredits",       do_dcredits,    POS_DEAD,  	 0,  LOG_NORMAL, 0, 0, 0 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "examine",	do_examine,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "reply",          do_reply,       POS_MEDITATING,  0,  LOG_NORMAL, 0, 0, 0 },
    { "autostance",	do_autostance,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "mastery",        do_mastery,     POS_STANDING,    3,  LOG_SEMI,   0, 0, 0 },
    { "expcalc",        do_exp,  	POS_FIGHTING,    1,  LOG_NORMAL, 0, 0, 0 },
    { "pkpowers",       do_pkpowers, 	POS_STANDING,    0,  LOG_NORMAL, 0, 0, 0 }, 
    { "gensteal",       do_gensteal, 	POS_STANDING,    3,  LOG_NORMAL, 0, 0, 0 },
    { "level",          do_level,  	POS_FIGHTING,    0,  LOG_NORMAL, 0, 0, 0 },
    { "selfdelete",     do_selfdelete,  POS_STANDING,    2,  LOG_NEVER,  0, 0, 0 },
    { "score",		do_score,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "skill",		do_skill,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "spells",		do_spell,	POS_MEDITATING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "socials",	do_socials,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "time",		do_time,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "typo",		do_typo,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "who",		do_who,		POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "wizlist",	do_wizlist,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "xemot",		do_huh,		POS_DEAD, 	 1,  LOG_NORMAL, 0, 0, 0 },
    { "xemote",		do_xemote,	POS_SITTING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "xsocial",	do_huh,		POS_DEAD, 	 1,  LOG_NORMAL, 0, 0, 0 },
    { "xsocials",	do_xsocials,	POS_DEAD, 	 1,  LOG_NORMAL, 0, 0, 0 },
    { "group",          do_group,       POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },


    /*
     * Fortress
     */
    { "garrison",       do_garrison,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 },
    { "hire",		do_hire, 	   POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Guilds
     */
    { "raise",          do_raise,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Dark Hand
    { "perfectsight",   do_perfectsight,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Dark Hand
    { "knightshield",   do_knightshield,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Crusaders
    { "curse",          do_faeriecurse,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Damned
    { "guardianbless",  do_guardianbless,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Guardian
    { "beastform",      do_beastform,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Beast
    { "purification",   do_purification,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Beast
    { "vineforge",      do_vineforge,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Druid
    { "grecall",        do_grecall,        POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // All






    /*
     * Professions
     */
    { "bash",          do_bash,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Knight
    { "layonhands",    do_layonhands,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Knight
    { "rage",          do_rage,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Barbarian
    { "eyegouge",      do_eyegouge,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Barbarian
    { "dirtkick",      do_dirtkick,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Thief
    { "impale",        do_impale,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Assassin
    { "sandman",       do_sandman,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Assassin
    { "darkblaze",     do_darkblaze,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Monk
    { "blink",         do_blink,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Soldier
    { "sharpen",       do_sharpen,       POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 }, // Mercenary
    { "landscape",     do_landscape,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0}, // Cleric
    { "thorns",        do_thorns,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0}, // Cleric




    /*
     * Xrakisis
     */
    { "smith",             do_smith,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "mix",               do_mix,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "alloys",            do_alloys,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "forge",             do_forge,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "materials",         do_materials,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "destroy",           do_destroy,         POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "calc",              do_calc,        POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "toggle",            do_toggle,        POS_DEAD,        10,  LOG_NORMAL, 0, 0, 0 },
    { "gainlevel",         do_gainlevel,        POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "selfsphere",        do_selfsphere,       POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "newbiepack",        do_newbiepack,       POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "proficiencies",     do_proficiencies,    POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "selfrace",          do_raceself,    POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "racepowers",     do_racepowers, POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "ascension",     do_ascension, POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "convert",           do_convert, POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "fightstyle",        do_fightstyle,        POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "sell",              do_sell,              POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "powers",              do_powers,              POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "wizlist",		do_wizlist,	POS_DEAD,	 	 0,  LOG_NORMAL, 0,0,0 },
    { "spheresrese",                do_resetsphere,     POS_DEAD,                0,  LOG_NORMAL, 0,0,0 },
    { "spheresreset",                do_resetspheres,     POS_DEAD,                0,  LOG_NORMAL, 0,0,0 },
    { "truesight",                do_truesight,     POS_DEAD,                0,  LOG_NORMAL, 0,0,0 },




// Race Powers -Xrakisis
    { "thunderhammer",      do_thunderhammer, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "jawlock",            do_jawlock, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "tunnel",             do_tunnel, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "mistform",           do_mistform, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "weaponenhance",      do_weaponenhance, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "armorenhance",       do_armorenhance, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "flameblade",         do_flameblade, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "goblingrenade",      do_goblingrenade, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "airshiptravel",      do_airshiptravel, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "superberserk",       do_superberserk, POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },
    { "infravision",        do_elveninfra, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "songs",              do_songs, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "elfsing",            do_elfsing, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "lessons",            do_lessons, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },
    { "tremble",            do_tremble, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "pirate",             do_pirate, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "regrow",             do_regrow, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "toughskin",          do_toughskin, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "fly",                do_fly, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "darkness",           do_darkness, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "drowfire",           do_drowfire, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "demonform",          do_demonform, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "gfavor",             do_gfavor, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "earthpunch",         do_earthpunch, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "chant",              do_chant, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "gore",               do_gore, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },
    { "stoneshape",               do_stoneshape, POS_FIGHTING,                 3,  LOG_NORMAL, 0,0,0 },

   { "jutsus",             do_jutsus, POS_STANDING,                 3,  LOG_NORMAL, 0,0,0 },


// wokanist
    { "elemshift",         do_elemshift,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "gasbreath",         do_gbreath,          POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "naturesfury",       do_naturesfury,      POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },

// chaos
    { "chaossurge",        do_chaossurge,       POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "chaosgate",         do_chaosgate,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "chaosmagic",        do_chaosmagic,       POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "chaosshield",       do_chaosshield,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "chaos",             do_chaos,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// combat 1
    { "leech",             do_leech,            POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "bladejump",         do_bladejump,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "ironclaw",          do_ironclaw,         POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "kakusu",            do_kakusu,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "fightdance",        do_fightdance,       POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// combat 2
    { "superberserk",      do_superberserk,     POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "bomuzite",          do_bomuzite,         POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "shouldercharge",    do_shouldercharge,   POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// combat 3
    { "entomb",            do_entomb,           POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "spikes",            do_spikes,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "unnerve",           do_unnerve,          POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "dtrick",            do_dtrick,           POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },


// combat 4
    { "desanc",            do_desanc,           POS_FIGHTING,        3,  LOG_NORMAL, 0, 0, 0 },
    { "shatter",           do_shatter,          POS_FIGHTING,        3,  LOG_NORMAL, 0, 0, 0 },
    { "enrage",            do_enrage,           POS_STANDING,        3,  LOG_NORMAL, 0, 0, 0 },


// Aeromancer
    { "disperse",          do_disperse,         POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "enfold",            do_enfold,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "evaporate",         do_evaporate,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Enchantment
    { "weaponenhance",      do_weaponenhance,   POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "armorenhance",       do_armorenhance,    POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "caust",              do_caust,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "immolate",           do_immolate,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "freezeweapon",       do_wfreeze,         POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "poisonblade",        do_poisonblade,     POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Enhancement
    { "nightsight",         do_nightsight,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "graft",              do_graft,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "darktendrils",       do_darktendrils,    POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Healing
    { "heal",               do_heal,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "regenerate",         do_regenerate,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "regrow",             do_regrow,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Hydromancer
    { "hydro",              do_hydro,           POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "cbreath",            do_cbreath,         POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "frosthands",         do_frost,           POS_STANDING,    3,  LOG_NORMAL, 0, 0, 0 },


// Geomancer
    { "solidify",           do_solidify,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "rockslide",          do_rockslide,       POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "earthshatter",       do_earthshatter,    POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "earthswallow",       do_earthswallow,    POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Transmuter
    { "zuloform",           do_zuloform,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "change",             do_change,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "dragonform",         do_dragonform,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "mask",               do_mask,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "camouflage",         do_camouflage,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "bonemod",            do_bonemod,         POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Illusionist
    { "clones",             do_illusionaryclone,POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "formillusion",       do_formillusion,    POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "shadowplane",        do_shadowplane,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "shadowsight",        do_shadowsight,     POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "grab",               do_grab,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Moon
    { "moonbeam",           do_moonbeam,        POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "moongate",           do_moongate,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Diviner
    { "scry",               do_scry,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "readaura",           do_readaura,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "gleam",              do_gleam,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "reveal",             do_reveal,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Abjurer
    { "dimensiondodge",     do_dimedodge,        POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "holyaura",           do_holyaura,         POS_STANDING,    3,  LOG_NORMAL, 0, 0, 0 },
    { "shield",             do_shield,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


// Necromancer
    { "creepingdoom",       do_creepingdoom,    POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "powerword",          do_powerword,       POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "darksummon",         do_darksummon,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "fleshportal",        do_fleshportal,     POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Pyromancer
    { "flameblade",         do_flameblade,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "firebreath",         do_fbreath,         POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "magma",              do_magma,           POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "enflame",            do_enflame,         POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Animal
    { "sclaws",             do_sclaws,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "claws",              do_claws,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "fangs",              do_fangs,           POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "rend",               do_rend,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "talons",             do_talons,          POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "burrow",             do_burrow,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },

// Animal 2
    { "quills",             do_quills,          POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "roar",               do_roar,            POS_FIGHTING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "flameclaws",         do_flameclaws,      POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },
    { "skin",               do_skin,            POS_STANDING,        0,  LOG_NORMAL, 0, 0, 0 },


    /* Religion commands */

    { "worship",     do_worship,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "forsake",     do_forsake,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "setavatar",  do_setavatar,  POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "setdecap",   do_setdecap,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "setlogout",  do_setlogout,  POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "setlogin",   do_setlogin,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "settie",     do_settie,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "relset",      do_relset,      POS_DEAD,      12,  LOG_NORMAL, 0,0,0},
    { "setrank",     do_setrank,     POS_DEAD,      12,  LOG_NORMAL, 0,0,0},
    { "relbuy",      do_relbuy,      POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "darmor",      do_darmor,      POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "gzone",       do_gzone,       POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "holyword",    do_holyword,    POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "areaaffects", do_areaaffects, POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "thorns",      do_thorns,      POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "rpurge",      do_rpurge,      POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "deja",        do_deja,        POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "holysymbol",  do_holysymbol,  POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "badmoon",     do_badmoon,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "areaheal",    do_areaheal,    POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "relpowers",   do_relpowers,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "hdesire",     do_hdesire,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "dscry",       do_dscry,       POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "pestilence",  do_pestilence,  POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "banishment",  do_banishment,  POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "starsight",   do_starsight,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "gaia",        do_gaia,        POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "dbattle",     do_dbattle,     POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "callwild",    do_callwild,    POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "madness",     do_madness,     POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "gfury",       do_gfury,       POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "subvert",     do_subvert,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "rebirth",     do_rebirth,     POS_STANDING,   3,  LOG_NORMAL, 0,0,0},
    { "supernova",   do_supernova,   POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},
    { "farmor",      do_farmor,      POS_FIGHTING,   3,  LOG_NORMAL, 0,0,0},



    /*
     * Configuration commands.
     */

    { "alignment",	do_alignment,   POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "alias",          do_alias,       POS_STANDING,    2,  LOG_SEMI,   0, 0, 0 },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "vt100",          do_vt100,       POS_DEAD,        1,  LOG_NORMAL, 0, 0, 0 },
    { "compres",        do_compres,     POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "compress",       do_compress,    POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "description",	do_description, POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "password",	do_password,    POS_DEAD,	 0,  LOG_NEVER,  0, 0, 0 },
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "ansi",		do_ansi,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "autoexit",	do_autoexit,    POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "autoloot",	do_autoloot,    POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "autosac",	do_autosac,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "map",            do_map,         POS_DEAD,        1,  LOG_NORMAL, 0, 0, 0 },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },  
    { "brief1",	        do_brief1,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "brief2",         do_brief2,      POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "brief3",         do_brief3,      POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "brief4",         do_brief4,      POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "brief5",         do_brief5,      POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "brief6",         do_brief6,      POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "cprompt",	do_cprompt,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "prompt",		do_prompt,	POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "sound",          do_sound,       POS_DEAD,        1,  LOG_NORMAL, 0, 0, 0 },
    { "music",          do_music,       POS_DEAD,        1,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Communication commands.
     */

    { "channels",       do_channels,     POS_DEAD,       0,  LOG_NORMAL, 0, 0, 0 },
    { "flame",		do_flame,        POS_DEAD,       0,  LOG_NORMAL, 0, 0, 0 },
    { "chat",		do_chat,	 POS_DEAD,       0,  LOG_NORMAL, 0, 0, 0 },
    { "roleplay",       do_roleplay,     POS_DEAD,       2,  LOG_NORMAL, 0, 0, 0 },
    { ".",	        do_chat,	 POS_DEAD,       2,  LOG_NORMAL, 0, 0, 0 },
    { "emote",		do_xemote,	 POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { ",",		do_xemote,	 POS_SITTING,    0,  LOG_NORMAL, 0, 0, 0 },
    { "gtell",		do_gtell,	 POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { ";",		do_gtell,	 POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "sing",		do_sing,	 POS_SLEEPING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "note",		do_note,	 POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "board",		do_board,	 POS_DEAD,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "pose",		do_emote,	 POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "say",		do_say,		 POS_MEDITATING, 0,  LOG_NORMAL, 0, 0, 0 },
    { "'",		do_say,		 POS_MEDITATING, 0,  LOG_NORMAL, 0, 0, 0 },
    { "yell",		do_yell,	 POS_SITTING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "powers",         do_racecommands, POS_FIGHTING,   1,  LOG_NORMAL, 0, 0, 0 },
    { "bet",            do_bet,          POS_DEAD,       2,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Object manipulation commands.
     */

    { "trigger",        do_trigger,     POS_FIGHTING,    3,  LOG_NORMAL, 0, 0, 0 },
    { "brandish",	do_brandish,    POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "close",		do_close,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "draw",		do_draw,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "drink",		do_drink,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "drop",		do_drop,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "eat",		do_eat,	        POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "empty",		do_empty,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "fill",		do_fill,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "give",		do_give,	POS_SITTING,	 0,  LOG_SEMI,   0, 0, 0 },
    { "gift",           do_gift,        POS_STANDING,    0,  LOG_NEVER,  0, 0, 0 },
    { "hold",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "lock",		do_lock,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "open",		do_open,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "pick",		do_pick,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "put",		do_put,	        POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "quaff",		do_quaff,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "recite",		do_recite,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "remove",		do_remove,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "remort",            do_remort,              POS_DEAD,                 3,  LOG_NORMAL, 0,0,0 },

    { "removealias",    do_removealias, POS_STANDING,    2,  LOG_NORMAL, 0, 0, 0 },
    { "sheath",		do_sheath,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "take",		do_get,	        POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "throw",		do_throw,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "sacrifice",	do_sacrifice,	POS_SITTING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "unlock",		do_unlock,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "wear",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "zap",		do_zap,     	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "junk",           do_sacrifice,   POS_STANDING,    1,  LOG_NORMAL, 0, 0, 0 },

    /*
     * Combat commands.
     */

    { "generation",     do_generation,  POS_STANDING,   12,  LOG_ALWAYS, 0, 0, 0 },
    { "backstab",	do_backstab,    POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "bs",		do_backstab,    POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "hurl",		do_hurl,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "punch",		do_punch,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "stance",		do_stance,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },

                    /* PK POWERS */

    { "eaglesight",       do_pkscry,    POS_STANDING,   3,  LOG_NORMAL, 0, 0, 0 },
    { "lifesense",	  do_pkaura,	POS_STANDING,   3,  LOG_NORMAL, 0, 0, 0 },
    { "sanctum",	  do_pkheal,    POS_STANDING,   3,  LOG_NORMAL, 0, 0, 0 },
    { "calltoarms",	  do_pkcall,	POS_FIGHTING,   3,  LOG_NORMAL, 0, 0, 0 },
    { "ironmind",	  do_pkmind,    POS_STANDING,   3,  LOG_NORMAL, 0, 0, 0 },
    { "objectscry",       do_pkobjscry, POS_STANDING,   3,  LOG_NORMAL, 0, 0, 0 },


    /*
     * Miscellaneous commands.
     */
    { "accep",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "accept",		do_accept,	POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "artifact",	do_artifact,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "artisteal",      do_artisteal,   POS_STANDING,    3,  LOG_NORMAL, 0, 0, 0 },
    { "birth",		do_birth,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "blindfold",	do_blindfold,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "breaku",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "breakup",	do_breakup,	POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "claim",		do_claim,	POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "consen",		do_huh,		POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "consent",	do_consent,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "finger",		do_finger,	POS_SITTING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "follow",		do_follow,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "gag",		do_gag,		POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "hide",		do_hide,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "home",		do_home,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "hunt",		do_hunt,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "locate",		do_locate,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "propos",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "propose",	do_propose,	POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "qui",		do_qui,		POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0 },
    { "quit",		do_quit,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "escape",		do_escape,	POS_DEAD,	 3,  LOG_NORMAL, 0, 0, 0 },
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "rent",		do_rent,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "safe",		do_safe,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "save",		do_save,	POS_DEAD, 	 0,  LOG_NORMAL, 0, 0, 0 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "smother",	do_smother,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "scan",		do_scan,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "spy",		do_spy,		POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "nosummon",	do_nosummon,	POS_DEAD, 	 1,  LOG_NORMAL, 0, 0, 0 },
    { "tie",		do_tie,		POS_STANDING,	 3,  LOG_NORMAL, 0, 0, 0 },
    { "token",		do_token,	POS_STANDING,	 2,  LOG_NORMAL, 0, 0, 0 },
    { "track",		do_track,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "train",		do_train,       POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "untie",		do_untie,	POS_STANDING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "visible",	do_visible,	POS_SLEEPING,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "where",		do_where,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "info",		do_info,	POS_DEAD,       12,  LOG_NORMAL, 0, 0, 0 },
    { "tick",		do_tick,        POS_DEAD,       12,  LOG_ALWAYS, 0, 0, 0 },
    { "resetarea",	do_resetarea,   POS_DEAD,       10,  LOG_ALWAYS, 0, 0, 0 },
    { "ban",		do_ban, 	POS_DEAD,	12,  LOG_ALWAYS, 0, 0, 0 },
    { "transfer",       do_transfer,    POS_DEAD,        7,  LOG_NORMAL, 0, 0, 0 },
    { "afk",      	do_afk,         POS_STANDING,    3,  LOG_NORMAL, 0, 0, 0 },
    { "hedit",		do_hedit,	POS_STANDING,   10,  LOG_NORMAL, 0, 0, 0 },
    { "freeze",		do_freeze,      POS_DEAD,        9,  LOG_ALWAYS, 0, 0, 0 },  
    { "bitchslap", 	do_freeze,      POS_DEAD,        9,  LOG_ALWAYS, 0, 0, 0 },
    { "prefix",		do_prefix,	POS_DEAD,       10,  LOG_NORMAL, 0, 0, 0 },
    { "hset",		do_hset,        POS_DEAD,       12,  LOG_ALWAYS, 0, 0, 0 },
    { "hlist",		do_hlist,	POS_DEAD,       12,  LOG_ALWAYS, 0, 0, 0 },
    { "flex",		do_flex,	POS_SITTING,	 0,  LOG_NORMAL, 0, 0, 0 },
    { "newbie",		do_newbie,	POS_DEAD,	 1,  LOG_NORMAL, 0, 0, 0 },
    { "relevel",	do_relevel,	POS_DEAD,   	 0,  LOG_ALWAYS, 0, 0, 0 },
    { "upkeep",		do_upkeep,	POS_DEAD,        3,  LOG_NORMAL, 0, 0, 0 },
    { "healme",		do_healme,	POS_STANDING,    2,  LOG_NORMAL, 0, 0, 0 },
    { "bounty", 	do_bounty, 	POS_STANDING, 	 2,  LOG_NORMAL, 0, 0, 0 },
    { "bountylist",	do_bountylist, 	POS_FIGHTING, 	 2,  LOG_NORMAL, 0, 0, 0 },

     /* quest stuff */
    { "createbase",     do_createbase,  POS_DEAD,       12,  LOG_NORMAL, 0, 0, 0 },
    { "showquest",      do_showquest,   POS_DEAD,        1,  LOG_NORMAL, 0, 0, 0 },
    { "qgain",          do_qgain,       POS_STANDING,    1,  LOG_NORMAL, 0, 0, 0 },
    { "qcomplete",      do_qcomplete,   POS_STANDING,    1,  LOG_NORMAL, 0, 0, 0 },

     /* extra description commands */
    { "pull",         do_pull,        POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },
    { "push",         do_push,        POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },
    { "press",        do_press,       POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },
    { "touch",        do_touch,       POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },

     /* shopping commands */
    { "shop",         do_shop,        POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },
    { "buy",          do_buy,         POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },
    { "list",         do_list,        POS_STANDING, 1, LOG_NORMAL, 0, 0, 0 },

     /* alias and other nifty commands */
    { "showalias",    do_showalias,   POS_STANDING, 2, LOG_NORMAL, 0, 0, 0 },
    { "history",      do_history,     POS_DEAD,     1, LOG_NORMAL, 0, 0, 0 },
    { "changes",      do_changes,     POS_DEAD,     1, LOG_NORMAL, 0, 0, 0 },
    { "news",         do_changes,     POS_DEAD,     1, LOG_NORMAL, 0, 0, 0 },

    /*
     * more commands.
     */

    { "timer",          do_timer,        POS_FIGHTING,     2,  LOG_NORMAL, 0, 0, 0 },
    { "ragnarok",       do_ragnarok,     POS_STANDING,     3,  LOG_NORMAL, 0, 0, 0 },
    { "showsilence",    do_showsilence,  POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "pathfind",       do_pathfind,     POS_DEAD,        11,  LOG_NORMAL, 0, 0, 0 },
    { "addchange",      do_addchange,    POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "advance",        do_advance,      POS_STANDING,    10,  LOG_ALWAYS, 0, 0, 0 },
    { "delchange",      do_delchange,    POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "displayvotes",   do_displayvotes, POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "vote",           do_vote,         POS_STANDING,     3,  LOG_NORMAL, 0, 0, 0 },
    { "addpoll",        do_addpoll,      POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "polledit",       do_polledit,     POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "stoppoll",       do_stoppoll,     POS_STANDING,    12,  LOG_NORMAL, 0, 0, 0 },
    { "ccenter",        do_ccenter,      POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "showcomp",       do_showcompress, POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 }, 
    { "superlog",	do_superlog,	 POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "trust",		do_trust,	 POS_DEAD,	  11,  LOG_ALWAYS, 0, 0, 0 },
    { "allow",		do_allow,	 POS_DEAD,	  11,  LOG_ALWAYS, 0, 0, 0 },
    { "bind",		do_bind,	 POS_DEAD,	  10,  LOG_ALWAYS, 0, 0, 0 },
    { "clearstats",	do_clearstats,	 POS_STANDING,	   0,  LOG_NORMAL, 0, 0, 0 },
    { "create",		do_create,	 POS_STANDING,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "deny",		do_deny,	 POS_DEAD,	  10,  LOG_ALWAYS, 0, 0, 0 },
    { "disable",	do_disable,	 POS_DEAD,	  11,  LOG_ALWAYS, 0, 0, 0 },
    { "disconnect",	do_disconnect,	 POS_DEAD,	  10,  LOG_NEVER,  0, 0, 0 },
    { "divorce",	do_divorce,	 POS_DEAD,	   9,  LOG_ALWAYS, 0, 0, 0 },
    { "freeze",		do_freeze,	 POS_DEAD,	   9,  LOG_ALWAYS, 0, 0, 0 },
    { "marry",		do_marry,	 POS_DEAD,	   9,  LOG_ALWAYS, 0, 0, 0 },
    { "paradox",	do_paradox,	 POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "bully",          do_bully,        POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "noset",          do_noset,        POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "copyover",	do_copyover,	 POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "cprime",         do_cprime,       POS_DEAD,        11,  LOG_ALWAYS, 0, 0, 0 },
    { "shutdow",	do_shutdow,	 POS_DEAD,	  12,  LOG_NORMAL, 0, 0, 0 },
    { "shutdown",	do_shutdown,     POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "users",		do_users,	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "omni",		do_omni,	 POS_DEAD,	  10,  LOG_NORMAL, 0, 0, 0 },
    { "wizlock",	do_wizlock,	 POS_DEAD,	  11,  LOG_ALWAYS, 0, 0, 0 },
    { "closemud",	do_closemud,	 POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "force",		do_force,	 POS_DEAD,	   9,  LOG_ALWAYS, 0, 0, 0 },
    { "asperson",       do_asperson,     POS_DEAD,        12,  LOG_ALWAYS, 0, 0, 0 },
    { "offline",        do_offline,      POS_DEAD,        12,  LOG_ALWAYS, 0, 0, 0 },
    { "pfile",          do_pfile,        POS_DEAD,         7,  LOG_ALWAYS, 0, 0, 0 },
    { "exlist",		do_exlist,	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "mload",		do_mload,	 POS_DEAD,	   7,  LOG_ALWAYS, 0, 0, 0 },
    { "undeny",         do_undeny,  	 POS_DEAD,        12,  LOG_ALWAYS, 0, 0, 0 },
    { "mset",		do_mset,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "multicheck",	do_multicheck,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "resetpasswd",	do_resetpassword,POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "oclone",		do_oclone,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "oload",		do_oload,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "oset",		do_oset,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "otransfer",	do_otransfer,    POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "pload",		do_pload,	 POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "preturn",	do_preturn,	 POS_DEAD,	   2,  LOG_NORMAL, 0, 0, 0 },
    { "pset",		do_pset,	 POS_DEAD,	  10,  LOG_ALWAYS, 0, 0, 0 },
    { "pstat",          do_pstat,        POS_DEAD,        10,  LOG_ALWAYS, 0, 0, 0 },
    { "purge",		do_purge,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "release",	do_release,	 POS_DEAD,	   9,  LOG_ALWAYS, 0, 0, 0 },
    { "restore",	do_restore,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "rset",		do_rset,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "silence",	do_silence,	 POS_DEAD,	   9,  LOG_NORMAL, 0, 0, 0 },
    { "sla",		do_sla,		 POS_DEAD,	  10,  LOG_NORMAL, 0, 0, 0 },
    { "slay",		do_slay,	 POS_DEAD,	  10,  LOG_ALWAYS, 0, 0, 0 },
    { "decapitate",	do_decapitate,   POS_STANDING,	   3,  LOG_NORMAL, 0, 0, 0 },
    { "sset",		do_sset,	 POS_DEAD,	  10,  LOG_ALWAYS, 0, 0, 0 },
    { "transfer",	do_transfer,     POS_DEAD,	   7,  LOG_ALWAYS, 0, 0, 0 },
    { "transport",	do_transport,    POS_DEAD,	   1,  LOG_NORMAL, 0, 0, 0 },
    { "at",		do_at,	         POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "bamfin",		do_bamfin,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "bamfout",	do_bamfout,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "echo",		do_echo,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "goto",		do_goto,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "holylight",	do_holylight,    POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "invis",		do_invis,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "log",		do_log,	         POS_DEAD,	  12,  LOG_ALWAYS, 0, 0, 0 },
    { "logstat",        do_logstat,      POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "logstatclear",   do_logstatclear, POS_DEAD,        12,  LOG_NORMAL, 0, 0, 0 },
    { "memusage",	do_memory, 	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "mfind",		do_mfind,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "mstat",		do_mstat,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "mwhere",		do_mwhere,	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "ofind",		do_ofind,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "ostat",		do_ostat,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "peace",		do_peace,	 POS_DEAD,	  11,  LOG_NORMAL, 0, 0, 0 },
    { "recho",		do_recho,	 POS_DEAD,	   8,  LOG_ALWAYS, 0, 0, 0 },
    { "rstat",		do_rstat,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { "slookup",	do_slookup,	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "snoop",		do_snoop,	 POS_DEAD,	   8,  LOG_NORMAL, 0, 0, 0 },
    { "immune",		do_immune,	 POS_DEAD,	   1,  LOG_NORMAL, 0, 0, 0 },
    { "immtalk",	do_immtalk,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },
    { ":",		do_immtalk,	 POS_DEAD,	   7,  LOG_NORMAL, 0, 0, 0 },

    { "leader",       do_leader,        POS_STANDING,      2,  LOG_NORMAL, 0, 0, 0 },
    { "leaderclear",  do_leaderclear,   POS_DEAD,          7,  LOG_NORMAL, 0, 0, 0 },

    /* newbie banning and unbanning */
    { "newban",       do_newban,        POS_DEAD,         12,  LOG_NORMAL, 0, 0, 0 },
    { "newallow",     do_newallow,      POS_DEAD,         12,  LOG_NORMAL, 0, 0, 0 },

    /* Arena stuff - Jobo */

    { "resign",       do_resign,        POS_STANDING,   3, LOG_NORMAL, 0, 0, 0 },
    { "arenastats",   do_arenastats,    POS_STANDING,   2, LOG_NORMAL, 0, 0, 0 },
    { "arenaset",     do_arenaset,      POS_DEAD,       7, LOG_NORMAL, 0, 0, 0 },
    { "arenajoin",    do_arenajoin,     POS_STANDING,   3, LOG_NORMAL, 0, 0, 0 },
    { "arenaopen",    do_openthearena,  POS_DEAD,      12, LOG_NORMAL, 0, 0, 0 },

    { "fortressstats", do_fortressstats, POS_STANDING, 2, LOG_NORMAL, 0, 0, 0 },
    { "challenge",     do_challenge,     POS_STANDING, 3, LOG_NORMAL, 0, 0, 0 },
    { "decline",       do_decline,       POS_STANDING, 3, LOG_NORMAL, 0, 0, 0 },
    { "agree",         do_accept2,       POS_STANDING, 3, LOG_NORMAL, 0, 0, 0 },

    { "ignore",  do_ignore,  POS_DEAD,     2, LOG_NORMAL, 0,0,0 },
    { "policy",  do_policy,  POS_STANDING, 2, LOG_NEVER,  0,0,0 },
    { "favor",   do_favor,   POS_STANDING, 2, LOG_NORMAL, 0,0,0 },

    { "setavatar",   do_setavatar,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0 },
    { "setdecap",    do_setdecap,    POS_STANDING,   3,  LOG_NORMAL, 0,0,0 },
    { "setlogout",   do_setlogout,   POS_STANDING,   3,  LOG_NORMAL, 0,0,0 },
    { "setlogin",    do_setlogin,    POS_STANDING,   3,  LOG_NORMAL, 0,0,0 },
    { "settie",      do_settie,      POS_STANDING,   3,  LOG_NORMAL, 0,0,0 },


    /*
     * OLC 1.1b and Social Edit
     */
    { "aedit",          do_aedit,       POS_DEAD,   11,  LOG_NORMAL, 0, 0, 0 },
    { "redit",          do_redit,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "oedit",          do_oedit,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "medit",          do_medit,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "asave",          do_asave,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "alist",          do_alist,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "resets",         do_resets,      POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },
    { "sedit",          do_sedit,       POS_DEAD,    8,  LOG_NORMAL, 0, 0, 0 },

    { "relearn",        do_relearn,     POS_DEAD,    1,  LOG_NORMAL, 0, 0, 0 },

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0,0,0 }
};


/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  char argu[MAX_STRING_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  char command[MAX_STRING_LENGTH];
  char logline[MAX_STRING_LENGTH];
  char cmd_copy[MAX_INPUT_LENGTH] ;
  bool found, foundstar = FALSE;
  int cmd, trust, star = 0;
  sh_int col = 0;

  /* due to the way threads has been implanted */
  if (!ch) return;

  /* One of these days I'll have to figure out why
   * a player ends up in a room that doesn't exists
   * every few weeks....
   */
  if (!ch->in_room)
  {
    sprintf(buf, "Interpret: %s not in any room.", ch->name);
    bug(buf, 0);
    return;
  }

  sprintf(argu, "%s %s", arg, one_argument(argument, arg));

  /*
   * Strip leading spaces.
   */
  while ( isspace(*argument) )
    argument++;
  if (argument[0] == '\0')
    return;

  strcpy(cmd_copy, argument);

  /*
   * Implement freeze command.
   */
  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE))
  {
    send_to_char( "You can't do anything while crying like a bitch!\n\r", ch );
    return;
  }

  /* check for AFK */
  if (!IS_NPC(ch) && IS_SET(ch->extra, EXTRA_AFK) && str_cmp(argument, "afk"))
    do_afk(ch,"");

  /*
   * Grab the command word.
   * Special parsing so ' can be a command,
   * also no spaces needed after punctuation.
   */
  strcpy( logline, argument );
  if (!isalpha(argument[0]) && !isdigit(argument[0]))
  {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while (isspace(*argument))
      argument++;
  }
  else
  {
    argument = one_argument( argument, command );
  }


 /*
  * List all valid commands
  */
  if (command[strlen(command) - 1] == '*')
  {
    command[strlen(command) - 1] = '\0';

    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
      if ((!str_prefix(command, cmd_table[cmd].name) || strlen(command) == 0) && can_interpret(ch, cmd))
      {
        foundstar = TRUE;
        star++;
        sprintf( buf, "%-15s", cmd_table[cmd].name );
        send_to_char( buf, ch );
        if ( ++col % 5 == 0 )
          send_to_char( "\n\r", ch );
      }
    }
    if (col % 5 != 0 && foundstar)
      send_to_char("\n\r", ch);

    if (foundstar && star != 0)
    {
      sprintf(buf, "\n%d command%s found.\n\r", star, (star > 1) ? "s" : "");
      stc(buf, ch);
    }
    if (!foundstar)
    {
      stc("No commands found.\n\r", ch);
    }
    return;
  }

  /*
   * Alias system, beta version by Jobo.
   */
  if (!IS_NPC(ch))
  {
    ALIAS_DATA *ali;

    for (ali = ch->pcdata->alias; ali; ali = ali->next)
    {
      if (!str_cmp(command, ali->short_n))
      {
        sprintf(buf, "%s %s", ali->long_n, argument);
        interpret(ch, buf);
        return;
      }
    }
  }

  /*
   * Look for command in command table.
   */
  found = FALSE;
  trust = get_trust( ch );
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    if (command[0] == cmd_table[cmd].name[0] && !str_prefix(command, cmd_table[cmd].name) && cmd_table[cmd].level <= trust)
    {
      if (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
      {
        if      (!str_cmp( cmd_table[cmd].name, "say"     )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "'"       )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "immtalk" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, ":"       )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "chat"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "."       )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "look"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "save"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "exits"   )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "emote"   )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "tell"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "order"   )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "who"     )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "where"   )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "relevel" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "safe"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "scan"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "spy"     )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "score"   )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "save"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "inventory" ))  found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "oreturn" ))    found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "truesight" ))  found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "cast" ))       found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "quit" ) && !IS_NPC(ch) && ch->pcdata->obj_vnum != 0) found = TRUE;
        else
        {
          send_to_char( "Not without a body!\n\r", ch );
          return;
        }
      }
      else if (IS_EXTRA(ch,TIED_UP) )
      {
        if      (!str_cmp( cmd_table[cmd].name, "say"  )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "'"    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "chat" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "."    )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "yell" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "shout")) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "look" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "call" )) found = TRUE; 
        else if (!str_cmp( cmd_table[cmd].name, "save" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "exits")) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "inventory" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "tell" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "restore" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "order" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "who" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "where" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "relevel" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "safe" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "scan" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "spy"  )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "sleep" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "wake" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "upkeep" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "score" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "immune" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "consent" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "report" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "goto" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "flex" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "wake" )) found = TRUE;
        else if (!str_cmp( cmd_table[cmd].name, "drink" )) found = TRUE;
        else
        {
          send_to_char( "Not while tied up.\n\r", ch );
          if (ch->position > POS_STUNNED)
            act("$n strains against $s bonds.",ch,NULL,NULL,TO_ROOM);
          return;
        }
      }
      found = TRUE;
      break;
    }
  }

  /*
   * Log and snoop.
   */
  if ( cmd_table[cmd].log == LOG_NEVER )
    strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

  if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_LOG))
      || fLogAll
      || cmd_table[cmd].log == LOG_ALWAYS )
  {
    if (!IS_CREATOR(ch) && !IS_NPC(ch))
    {
      sprintf( log_buf, "Log %s: %s", ch->pcdata->switchname, logline );
      log_string( log_buf );
    }
  }
  if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
  {
    write_to_buffer( ch->desc->snoop_by, "% ",    2 );
    write_to_buffer( ch->desc->snoop_by, logline, 0 );
    write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
  }

  if (ch->desc != NULL)
    write_to_buffer(ch->desc,"\n\r",2);	
  if (!found )
  {
    /*
     * Look for command in socials table.
     */
    if ( !check_social( ch, command, argument ) )
    {
      if ( !check_xsocial( ch, command, argument ) )
        send_to_char( "Huh?\n\r", ch );
  sprintf( buf,"#R%s#n is not a recognized command please type 'commands' to list them.\n\r",command );
        send_to_char (buf, ch);
    }
    return;
  }
  else /* a normal valid command.. check if it is disabled */
  if (check_disabled (&cmd_table[cmd]))
  {
    send_to_char ("This command has been temporarily disabled.\n\r", ch );
    return;
  }

  /*
   * Character not in position for command?
   */
  if ( ch->position < cmd_table[cmd].position )
  {
    switch( ch->position )
    {
      case POS_DEAD:
        send_to_char( "Lie still; you are DEAD.\n\r", ch );
        break;
      case POS_MORTAL:
      case POS_INCAP:
      {
        send_to_char( "You are hurt far too bad for that.\n\r", ch );
        break;
      }
      case POS_STUNNED:
      {
        send_to_char( "You are too stunned to do that.\n\r", ch );
        break;
      }
      case POS_SLEEPING:
        send_to_char( "In your dreams, or what?\n\r", ch );
        break;
      case POS_MEDITATING:
      case POS_SITTING:
      case POS_RESTING:
        send_to_char( "Nah... You feel too relaxed...\n\r", ch);
        break;
      case POS_FIGHTING:
        send_to_char( "No way!  You are still fighting!\n\r", ch);
        break;
    }
    return;
  }

  /*
   * Dispatch the command.
   */

  if (last_command != NULL) free_string(last_command);
  sprintf(log_buf, "%s %s BY %s", cmd_table[cmd].name,argument,ch->name); 
  last_command = str_dup(log_buf);

  cmd_done = FALSE;
  (*cmd_table[cmd].do_fun)(ch, argument);
  cmd_done = TRUE;

  tail_chain( );
  return;
}


/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, char *argument)
{
    char *command_string;
    
    /* copy the string */
    command_string = str_dup(argument);
    
    /* dispatch the command */
    (*do_fun) (ch, command_string);
    
    /* free the string */
    free_string(command_string);
}


bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }


    if ( !found )
	return FALSE;

    switch ( ch->position )
    {

    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;

    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim) )
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( social_table[cmd].others_found,
		    victim, NULL, ch, TO_NOTVICT );
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		break;
	    }
	}
    }
    return TRUE;
}



bool check_xsocial( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *partner = NULL;
    int cmd;
    int stage;
    int amount;
    bool is_ok = FALSE;
    bool found = FALSE;
    bool one = FALSE;
    bool two = FALSE;

    if (IS_NPC(ch)) return FALSE;

    for ( cmd = 0; xsocial_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == xsocial_table[cmd].name[0]
	&&   !str_prefix( command, xsocial_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    switch ( ch->position )
    {

    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return TRUE;
    }
    if (IS_NPC(victim))
    {
	send_to_char("You can only perform xsocials on players.\n\r",ch);
	return TRUE;
    }

	if( !str_cmp(ch->pcdata->switchname, "") )
	{
		victim->pcdata->partner = ch;
	}
  else if (IS_SET(victim->extra, TIED_UP))
         { victim->pcdata->partner = ch;
         }

    
	if (IS_EXTRA(ch, TIED_UP))
    {
	send_to_char("You wiggle and strain but the ropes only tighten.\n\r",ch);
        act("$n strains helplessly against $m bonds.",ch,NULL,NULL,TO_ROOM);
	return FALSE;
    }
    else if ( arg[0] == '\0' )
    {
	act( xsocial_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
	act( xsocial_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( victim == ch )
    {
	act( xsocial_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM );
	act( xsocial_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR );
    }
    else
    {
	if (xsocial_table[cmd].gender == SEX_MALE && ch->sex != SEX_MALE)
	{
	    send_to_char("Only men can perform this type of social.\n\r",ch);
	}
	else if (xsocial_table[cmd].gender == SEX_FEMALE && ch->sex != SEX_FEMALE)
	{
	    send_to_char("Only women can perform this type of social.\n\r",ch);
	}
	else if (xsocial_table[cmd].gender == SEX_MALE && victim->sex != SEX_FEMALE)
	{
	    send_to_char("You can only perform this social on a woman.\n\r",ch);
	}
	else if (xsocial_table[cmd].gender == SEX_FEMALE && victim->sex != SEX_MALE)
	{
	    send_to_char("You can only perform this social on a man.\n\r",ch);
	}
	else if (xsocial_table[cmd].gender == 3 && ch->sex != SEX_FEMALE)
	{
		stc( "Only females may preform this command.\n\r",ch);
	}
	else if (xsocial_table[cmd].gender == 3 && victim->sex != SEX_FEMALE)
	{
		stc( "You can only preform this command on a female.\n\r",ch);
	}
	else if (((partner = victim->pcdata->partner) == NULL || partner != ch))
  	{
         	stc("You cannot perform an xsocial on someone without their consent.\n\r",ch);
	 }
	else if (xsocial_table[cmd].stage == 0 && ch->pcdata->stage[0] < 1
	    && ch->pcdata->stage[2] > 0 && ch->sex == 5)
	    send_to_char("You have not yet recovered from last time!\n\r",ch);
	else if (xsocial_table[cmd].stage == 0 && victim->pcdata->stage[0] < 1
	    && victim->pcdata->stage[2] > 0 && victim->sex == 5)
	    send_to_char("They have not yet recovered from last time!\n\r",ch);
	else if (xsocial_table[cmd].stage > 0 && ch->pcdata->stage[0] < 100)
	    send_to_char("You are not sufficiently aroused.\n\r",ch);
	else if (xsocial_table[cmd].stage > 0 && victim->pcdata->stage[0] < 100)
	    send_to_char("They are not sufficiently aroused.\n\r",ch);
	else if (xsocial_table[cmd].stage > 1 && ch->pcdata->stage[1] < 1)
	    send_to_char("You are not in the right position.\n\r",ch);
	else if (xsocial_table[cmd].stage > 1 && victim->pcdata->stage[1] < 1)
	    send_to_char("They are not in the right position.\n\r",ch);
	else
	{
	    act(xsocial_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT);
	    act(xsocial_table[cmd].char_found,    ch, NULL, victim, TO_CHAR   );
	    act(xsocial_table[cmd].vict_found,    ch, NULL, victim, TO_VICT   );
	    if (xsocial_table[cmd].chance)
	    {
		if (ch->sex == SEX_FEMALE && 
		    !IS_EXTRA(ch, EXTRA_PREGNANT) && number_range(1,3) == 1) 
		make_preg(ch,victim);
		else if (victim->sex == SEX_FEMALE && 
		    !IS_EXTRA(victim, EXTRA_PREGNANT) && 
		    number_range(1,3) == 1) 
		make_preg(victim,ch);
	    }
	    if (!str_prefix(xsocial_table[cmd].name,"x-tie"))
	    {
		SET_BIT(victim->extra, TIED_UP);
	    }
	    if (!str_prefix(xsocial_table[cmd].name,"x-gag"))
	    {
		SET_BIT(victim->extra, GAGGED);
	    }
	    if (!str_prefix(xsocial_table[cmd].name,"x-blindfold"))
	    {
		SET_BIT(victim->extra, BLINDFOLDED);
	    }
	    if (xsocial_table[cmd].stage == 1)
	    {
		ch->pcdata->stage[1] = xsocial_table[cmd].position;
		victim->pcdata->stage[1] = xsocial_table[cmd].position;
		if (!IS_SET(ch->extra, EXTRA_DONE))
		{
		    SET_BIT(ch->extra, EXTRA_DONE);
		    if (ch->sex == SEX_FEMALE)
		    {
			act("You feel $n bleed as you enter $m.",ch,NULL,victim,TO_VICT);
			act("You feel yourself bleed as $N enters you.",ch,NULL,victim,TO_CHAR);
			ch->in_room->blood += 1;
		    }
		}
		if (!IS_SET(victim->extra, EXTRA_DONE))
		{
		    SET_BIT(victim->extra, EXTRA_DONE);
		    if (victim->sex == SEX_FEMALE)
		    {
			act("You feel $N bleed as you enter $M.",ch,NULL,victim,TO_CHAR);
			act("You feel yourself bleed as $n enters you.",ch,NULL,victim,TO_VICT);
			ch->in_room->blood += 1;
		    }
		}
		stage = 2;
	    }
	    else stage = xsocial_table[cmd].stage;
	    if (stage == 2) amount = ch->pcdata->stage[1];
		else amount = 100;
	    if (xsocial_table[cmd].self > 0)
	    {
		is_ok = FALSE;
		if (ch->pcdata->stage[stage] >= amount) is_ok = TRUE;
		ch->pcdata->stage[stage] += xsocial_table[cmd].self;
		if (!is_ok && ch->pcdata->stage[stage] >= amount) 
		{
		    stage_update(ch,victim,stage,xsocial_table[cmd].name);
		    one = TRUE;
		}
	    }
	    if (xsocial_table[cmd].other > 0)
	    {
		is_ok = FALSE;
		if (victim->pcdata->stage[stage] >= amount) is_ok = TRUE;
		victim->pcdata->stage[stage] += xsocial_table[cmd].other;
		if (!is_ok && victim->pcdata->stage[stage] >= amount) 
		{
		    stage_update(victim,ch,stage,xsocial_table[cmd].name);
		    two = TRUE;
		}
	    }
	    if ( one && two )
	    {
		    ch->pcdata->stage[0] = 0;
		    victim->pcdata->stage[0] = 0;
		if (!IS_EXTRA(ch, EXTRA_EXP))
		{
		    send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",ch);
		    SET_BIT(ch->extra, EXTRA_EXP);
		    ch->exp += 100000;
		}
		if (!IS_EXTRA(victim, EXTRA_EXP))
		{
		    send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",victim);
		    SET_BIT(victim->extra, EXTRA_EXP);
		    victim->exp += 100000;
		}
	    }
	}
    }
    return TRUE;
}

void stage_update( CHAR_DATA *ch, CHAR_DATA *victim, int stage,char *argument )
{
    if (IS_NPC(ch) || IS_NPC(victim)) return;
    if (stage == 0)
    {
	if (ch->sex == SEX_MALE)
	{
	    send_to_char("You get a boner.\n\r",ch);
	    act("You feel $n get a boner.",ch,NULL,victim,TO_VICT);
	    return;
	}
	else if (ch->sex == SEX_FEMALE)
	{
	    send_to_char("You get wet.\n\r",ch);
	    act("You feel $n get wet.",ch,NULL,victim,TO_VICT);
	    return;
	}
    }
    else if (stage == 2)
    {
	if (ch->sex == SEX_MALE)
	{
	    if( str_cmp(argument,"xm-cum")   && str_cmp(argument,"xm-facial") && str_cmp(argument,"xm-canal") &&
		str_cmp(argument,"xm-canal") && str_cmp(argument,"xm-cbreasts") && str_cmp(argument,"xm-chair") &&
		str_cmp(argument,"xm-chand") && str_cmp(argument,"xm-cstomach") && str_cmp(argument,"xf-chands") &&
		str_cmp(argument,"xf-cbreasts") )
	    {
		act("You grit your teeth as you shoot your creamy load inside of $M.",ch,NULL,victim,TO_CHAR);
		act("$n grits his teeth as he shoots his load inside of you.",ch,NULL,victim,TO_VICT);
		act("$n grits his teeth as he shoots a load of cum inside of $N.",ch,NULL,victim,TO_NOTVICT);
	    }
	    ch->pcdata->genes[8] += 1;
	    victim->pcdata->genes[8] += 1;
	    save_char_obj(ch);
	    save_char_obj(victim);
	    if (victim->pcdata->stage[2] < 1 || victim->pcdata->stage[2] >= 250)
	    {
		ch->pcdata->stage[2] = 0;
		if (ch->pcdata->stage[0] >= 200) ch->pcdata->stage[0] -= 100;
	    }
	    else ch->pcdata->stage[2] = 200;
	    if (victim->sex == SEX_FEMALE && 
		!IS_EXTRA(victim, EXTRA_PREGNANT) && number_percent() <= 8) 
	    make_preg(victim,ch);
	    return;
	}
	else if (ch->sex == SEX_FEMALE)
	{
	    if( str_cmp(argument,"xf-cum") && str_cmp(argument,"xf-cface") )
	    {
		act("You whimper as you cum.",ch,NULL,victim,TO_CHAR);
		act("$n whimpers as $e cums.",ch,NULL,victim,TO_ROOM);
	    }
	    if (victim->pcdata->stage[2] < 1 || victim->pcdata->stage[2] >= 250)
	    {
		ch->pcdata->stage[2] = 0;
		if (ch->pcdata->stage[0] >= 200) ch->pcdata->stage[0] -= 100;
	    }
	    else ch->pcdata->stage[2] = 200;
	    return;
	}
    }
    return;
}

void make_preg( CHAR_DATA *mother, CHAR_DATA *father )
{
    char *strtime;
    char buf [MAX_STRING_LENGTH];

    if (IS_NPC(mother) || IS_NPC(father)) return;

  strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    free_string(mother->pcdata->conception);
    mother->pcdata->conception = str_dup(strtime);
    sprintf(buf,"%s", father->name);
    free_string(mother->pcdata->cparents);
    mother->pcdata->cparents = str_dup(buf);
    SET_BIT(mother->extra, EXTRA_PREGNANT);
    mother->pcdata->genes[0] = (mother->max_hit + father->max_hit) * 0.5;
    mother->pcdata->genes[1] = (mother->max_mana + father->max_mana) * 0.5;
    mother->pcdata->genes[2] = (mother->max_move + father->max_move) * 0.5;
    if (IS_IMMUNE(mother, IMM_SLASH) && IS_IMMUNE(father, IMM_SLASH))
	SET_BIT(mother->pcdata->genes[3], IMM_SLASH);
    if (IS_IMMUNE(mother, IMM_STAB) && IS_IMMUNE(father, IMM_STAB))
	SET_BIT(mother->pcdata->genes[3], IMM_STAB);
    if (IS_IMMUNE(mother, IMM_SMASH) && IS_IMMUNE(father, IMM_SMASH))
	SET_BIT(mother->pcdata->genes[3], IMM_SMASH);
    if (IS_IMMUNE(mother, IMM_ANIMAL) && IS_IMMUNE(father, IMM_ANIMAL))
	SET_BIT(mother->pcdata->genes[3], IMM_ANIMAL);
    if (IS_IMMUNE(mother, IMM_MISC) && IS_IMMUNE(father, IMM_MISC))
	SET_BIT(mother->pcdata->genes[3], IMM_MISC);
    if (IS_IMMUNE(mother, IMM_CHARM) && IS_IMMUNE(father, IMM_CHARM))
	SET_BIT(mother->pcdata->genes[3], IMM_CHARM);
    if (IS_IMMUNE(mother, IMM_HEAT) && IS_IMMUNE(father, IMM_HEAT))
	SET_BIT(mother->pcdata->genes[3], IMM_HEAT);
    if (IS_IMMUNE(mother, IMM_COLD) && IS_IMMUNE(father, IMM_COLD))
	SET_BIT(mother->pcdata->genes[3], IMM_COLD);
    if (IS_IMMUNE(mother, IMM_LIGHTNING) && IS_IMMUNE(father, IMM_LIGHTNING))
	SET_BIT(mother->pcdata->genes[3], IMM_LIGHTNING);
    if (IS_IMMUNE(mother, IMM_ACID) && IS_IMMUNE(father, IMM_ACID))
	SET_BIT(mother->pcdata->genes[3], IMM_ACID);
    if (IS_IMMUNE(mother, IMM_HURL) && IS_IMMUNE(father, IMM_HURL))
	SET_BIT(mother->pcdata->genes[3], IMM_HURL);
    if (IS_IMMUNE(mother, IMM_BACKSTAB) && IS_IMMUNE(father, IMM_BACKSTAB))
	SET_BIT(mother->pcdata->genes[3], IMM_BACKSTAB);
    if (IS_IMMUNE(mother, IMM_KICK) && IS_IMMUNE(father, IMM_KICK))
	SET_BIT(mother->pcdata->genes[3], IMM_KICK);
    if (IS_IMMUNE(mother, IMM_DISARM) && IS_IMMUNE(father, IMM_DISARM))
	SET_BIT(mother->pcdata->genes[3], IMM_DISARM);
    if (IS_IMMUNE(mother, IMM_STEAL) && IS_IMMUNE(father, IMM_STEAL))
	SET_BIT(mother->pcdata->genes[3], IMM_STEAL);
    if (IS_IMMUNE(mother, IMM_SLEEP) && IS_IMMUNE(father, IMM_SLEEP))
	SET_BIT(mother->pcdata->genes[3], IMM_SLEEP);
    if (IS_IMMUNE(mother, IMM_DRAIN) && IS_IMMUNE(father, IMM_DRAIN))
	SET_BIT(mother->pcdata->genes[3], IMM_DRAIN);
    mother->pcdata->genes[4] = number_range(1,2);
    return;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    for ( ; *arg != '\0'; arg++ )
    {
	if ( !isdigit(*arg) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

/* Syntax is:
disable - shows disabled commands
disable <command> - toggles disable status of command
*/

void do_disable (CHAR_DATA *ch, char *argument)
{
	int i;
	DISABLED_DATA *p,*q;
	char buf[100];
	
	if (IS_NPC(ch))
	{
		send_to_char ("RETURN first.\n\r",ch);
		return;
	}
	
	if (!argument[0]) /* Nothing specified. Show disabled commands. */
	{
		if (!disabled_first) /* Any disabled at all ? */
		{
			send_to_char ("There are no commands disabled.\n\r",ch);
			return;
		}

		send_to_char ("Disabled commands:\n\r"
		              "Command      Level   Disabled by\n\r",ch);
		                
		for (p = disabled_first; p; p = p->next)
		{
			sprintf (buf, "%-12s %5d   %-12s\n\r",p->command->name, p->level, p->disabled_by);
			send_to_char (buf,ch);
		}
		return;
	}
	
	/* command given */

	/* First check if it is one of the disabled commands */
	for (p = disabled_first; p ; p = p->next)
		if (!str_cmp(argument, p->command->name))
			break;
			
	if (p) /* this command is disabled */
	{
	/* Optional: The level of the imm to enable the command must equal or exceed level
	   of the one that disabled it */
	
		if (get_trust(ch) < p->level)
		{
			send_to_char ("This command was disabled by a higher power.\n\r",ch);
			return;
		}
		
		/* Remove */
		
		if (disabled_first == p) /* node to be removed == head ? */
			disabled_first = p->next;
		else /* Find the node before this one */
		{
			for (q = disabled_first; q->next != p; q = q->next); /* empty for */
			q->next = p->next;
		}
		
		free_string (p->disabled_by); /* free name of disabler */
		free_mem (p,sizeof(DISABLED_DATA)); /* free node */
		
		save_disabled(); /* save to disk */
		send_to_char ("Command enabled.\n\r",ch);
	}
	else /* not a disabled command, check if that command exists */
	{
		/* IQ test */
		if (!str_cmp(argument,"disable"))
		{
			send_to_char ("You cannot disable the disable command.\n\r",ch);
			return;
		}

		/* Search for the command */
		for (i = 0; cmd_table[i].name[0] != '\0'; i++)
			if (!str_cmp(cmd_table[i].name, argument))
				break;

		/* Found? */				
		if (cmd_table[i].name[0] == '\0')
		{
			send_to_char ("No such command.\n\r",ch);
			return;
		}

		/* Can the imm use this command at all ? */				
		if (cmd_table[i].level > get_trust(ch))
		{
			send_to_char ("You dot have access to that command; you cannot disable it.\n\r",ch);
			return;
		}
		
		/* Disable the command */
		
		p = alloc_mem (sizeof(DISABLED_DATA));

		p->command = &cmd_table[i];
		p->disabled_by = str_dup (ch->pcdata->switchname); /* save name of disabler */
		p->level = get_trust(ch); /* save trust */
		p->next = disabled_first;
		disabled_first = p; /* add before the current first element */
		
		send_to_char ("Command disabled.\n\r",ch);
		save_disabled(); /* save to disk */
	}
}

/* Check if that command is disabled 
   Note that we check for equivalence of the do_fun pointers; this means
   that disabling 'chat' will also disable the '.' command
*/   
bool check_disabled (const struct cmd_type *command)
{
	DISABLED_DATA *p;
	
	for (p = disabled_first; p ; p = p->next)
		if (p->command->do_fun == command->do_fun)
			return TRUE;

	return FALSE;
}

/* Load disabled commands */
void load_disabled()
{
	FILE *fp;
	DISABLED_DATA *p;
	char *name;
	int i;
	
	disabled_first = NULL;
	
	fp = fopen (DISABLED_FILE, "r");
	
	if (!fp) /* No disabled file.. no disabled commands : */
		return;
		
	name = fread_word (fp);
	
	while (str_cmp(name, END_MARKER)) /* as long as name is NOT END_MARKER :) */
	{
		/* Find the command in the table */
		for (i = 0; cmd_table[i].name[0] ; i++)
			if (!str_cmp(cmd_table[i].name, name))
				break;
				
		if (!cmd_table[i].name[0]) /* command does not exist? */
		{
			bug ("Skipping uknown command in " DISABLED_FILE " file.",0);
			fread_number(fp); /* level */
			fread_word(fp); /* disabled_by */
		}
		else /* add new disabled command */
		{
			p = alloc_mem(sizeof(DISABLED_DATA));
			p->command = &cmd_table[i];
			p->level = fread_number(fp);
			p->disabled_by = str_dup(fread_word(fp)); 
			p->next = disabled_first;
			
			disabled_first = p;

		}
		
		name = fread_word(fp);
	}

	fclose (fp);		
}

/* Save disabled commands */
void save_disabled()
{
	FILE *fp;
	DISABLED_DATA *p;
	
	if (!disabled_first) /* delete file if no commands are disabled */
	{
		unlink (DISABLED_FILE);
		return;
	}
	
	fp = fopen (DISABLED_FILE, "w");
	
	if (!fp)
	{
		bug ("Could not open " DISABLED_FILE " for writing",0);
		return;
	}
	
	for (p = disabled_first; p ; p = p->next)
		fprintf (fp, "%s %d %s\n", p->command->name, p->level, p->disabled_by);
		
	fprintf (fp, "%s\n",END_MARKER);
		
	fclose (fp);
}
