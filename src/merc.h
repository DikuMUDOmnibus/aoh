/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
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

#ifndef _MERC_HEADER_
#define _MERC_HEADER_

#include <stdlib.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <pthread.h>

#include "vnums.h"
#include "jobo_quest.h"
#include "jobo_comm.h"
#include "kav_soundex.h"            // KaVir's soundex code

#include "xrakisis.h"
#include "races.h"
#include "changes.h"
#include "shinobi.h"
#include "profession.h"
#include "guild.h"




/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_QUEST_FUN( fun )        void fun( )
#define DECLARE_SHOP_FUN( fun )         void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_QUEST_FUN( fun )        QUEST_FUN fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_SHOP_FUN( fun )         SHOP_FUN  fun
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;

#define unix
#else
typedef short int sh_int;
typedef unsigned char bool;
#endif

/* mccp: support bits */

#include <zlib.h>
#define TELOPT_COMPRESS 85
#define COMPRESS_BUF_SIZE 16384

/*
 * Structure types.
 */
typedef struct area_affect          AREA_AFFECT;
typedef struct affect_data          AFFECT_DATA;
typedef struct affect2_data         AFFECT2_DATA;
typedef struct area_data            AREA_DATA;
typedef struct ban_data             BAN_DATA;
typedef struct char_data            CHAR_DATA;
typedef struct alias_data           ALIAS_DATA;
typedef struct quest_data           QUEST_DATA;
typedef struct history_data         HISTORY_DATA;
typedef struct change_data          CHANGE_DATA;
typedef struct vote_data            VOTE_DATA;
typedef struct poll_data            POLL_DATA;
typedef struct l_board              LEADER_BOARD;
typedef struct map_type             MAP_TYPE;
typedef struct editor_data          EDITOR_DATA;
typedef struct dummy_arg            DUMMY_ARG;
typedef struct descriptor_data      DESCRIPTOR_DATA;
typedef struct exit_data            EXIT_DATA;
typedef struct extra_descr_data     EXTRA_DESCR_DATA;
typedef struct help_data            HELP_DATA;
typedef struct kill_data            KILL_DATA;
typedef struct mob_index_data       MOB_INDEX_DATA;
typedef struct war_data             WAR_DATA;
typedef struct note_data            NOTE_DATA;
typedef struct obj_data             OBJ_DATA;
typedef struct obj_index_data       OBJ_INDEX_DATA;
typedef struct pc_data              PC_DATA;
typedef struct reset_data           RESET_DATA;
typedef struct room_index_data      ROOM_INDEX_DATA;
typedef struct time_info_data       TIME_INFO_DATA;
typedef struct weather_data         WEATHER_DATA;
typedef struct disabled_data        DISABLED_DATA;
typedef struct religion_table       RELIGION_TABLE;

struct disabled_data
{
  DISABLED_DATA         * next;          /* pointer to next node          */
  struct cmd_type const * command;       /* pointer to the command struct */
  char                  * disabled_by;   /* name of disabler              */
  sh_int                  level;         /* level of disabler             */
};

extern DISABLED_DATA *disabled_first;

/*
 * Function types.
 */
typedef void DO_FUN    args (( CHAR_DATA * ch, char *argument ));
typedef bool SPEC_FUN  args (( CHAR_DATA * ch, char *argument ));
typedef void QUEST_FUN args (( CHAR_DATA * questmaster, CHAR_DATA * ch, char *argument ));
typedef void SPELL_FUN args (( int sn, int level, CHAR_DATA * ch, void *vo ));
typedef void SHOP_FUN  args (( CHAR_DATA * shopkeeper, CHAR_DATA * ch, char *argument ));

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 8192
#define MAX_INPUT_LENGTH	  400
#define MSL			 8192 // added for ease, -Xrak
#define MIL			  400 // same

/* 
 * Rotains Gobal Procedures
 */
void  clear_stats           args (( CHAR_DATA * ch ));
void  improve_wpn           args (( CHAR_DATA * ch, int dtype, int right_hand ));
void  improve_stance        args (( CHAR_DATA * ch ));
void  skillstance           args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  show_spell            args (( CHAR_DATA * ch, int dtype ));
void  crack_head            args (( CHAR_DATA * ch, OBJ_DATA * obj, char *argument ));
void  critical_hit          args (( CHAR_DATA * ch, CHAR_DATA * victim, int dt, int dam ));
void  take_item             args (( CHAR_DATA * ch, OBJ_DATA * obj ));
void  raw_kill              args (( CHAR_DATA * victim ));
void  trip                  args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  disarm                args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  make_corpse           args (( CHAR_DATA * ch ));
void  one_hit               args (( CHAR_DATA * ch, CHAR_DATA * victim, int dt, int handtype ));
void  special_hurl          args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  make_part             args (( CHAR_DATA * ch, char *argument ));
void  behead                args (( CHAR_DATA * victim ));
void  paradox               args (( CHAR_DATA * ch ));

/* 
 * Godwars Game Parameters
 * By Rotain
 */
#define SKILL_ADEPT               100
#define SKILL_THAC0_32             18
#define SKILL_THAC0_00              6
#define MAX_DISCIPLINES		   44
#define MUDNAME                    "Age of Heroes II"
#define MAX_KINGDOM                 5
#define MAX_RELIGIONS               5

#define OBJ_VNUM_HOLYSYMBOL	2220
#define ROOM_VNUM_REL_HALL1	2002
#define ROOM_VNUM_REL_HALL2	2002
#define ROOM_VNUM_REL_HALL3	2002
/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_KNUDE                 500
#define MAX_MATERIAL		   20  // Custom, increment with table
#define MAX_CHANGE                 15
#define MAX_HISTORY                15
#define MAX_VOTE_OPTIONS	    5
#define MAX_IGNORE                 10
#define MAX_ALIAS                  30
#define CURRENT_REVISION            0
#define MAX_SKILL		  117
#define MAX_SPELL		   72
#define MAX_LEVEL		   12
#define MAX_TOP_PLAYERS	           20
#define MAX_CLASS		    4
#define MAX_STATS       5
#define STAT_STR        0
#define STAT_INT        1
#define STAT_WIS        2
#define STAT_DEX        3
#define STAT_CON        4


/* Size of the map and depth of recursion to undertake */
#define MAPX     10
#define MAPY      8
#define MAXDEPTH  2

#define LEVEL_HERO		   (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 5)
#define LEVEL_MORTAL		   (MAX_LEVEL - 10)
#define LEVEL_AVATAR		   (MAX_LEVEL - 9)
#define LEVEL_BUILDER		   (MAX_LEVEL - 5)
#define LEVEL_QUESTMAKER	   (MAX_LEVEL - 4)
#define LEVEL_ENFORCER		   (MAX_LEVEL - 3)
#define LEVEL_JUDGE		   (MAX_LEVEL - 2)
#define LEVEL_HIGHJUDGE		   (MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL)

#define PULSE_THIRTY              (30 * PULSE_PER_SECOND)
#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_DB_DUMP             (1800* PULSE_PER_SECOND ) /* 30 minutes */
#define PULSE_EMBRACE             ( 4 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_PLAYERS	 	  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (30 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)
#define PULSE_WW                  ( 4 * PULSE_PER_SECOND)
#define PULSE_ARENA		  (120 * PULSE_AREA)  /* 120 minutes */
#define PULSE_RAGNAROK            ( 15 * PULSE_AREA)

#define MAX_STAT 1

/* IMM bits for mobs */
#define IMM2_SUMMON              (A)
#define IMM2_CHARM               (B)
#define IMM2_MAGIC               (C)
#define IMM2_WEAPON              (D)
#define IMM2_BASH                (E)
#define IMM2_PIERCE              (F) // done -xrak
#define IMM2_SLASH               (G) // done -xrak
#define IMM2_FIRE                (H)
#define IMM2_COLD                (I)
#define IMM2_LIGHTNING           (J)
#define IMM2_ACID                (K)
#define IMM2_POISON              (L)
#define IMM2_NEGATIVE            (M)
#define IMM2_HOLY                (N)
#define IMM2_ENERGY              (O)
#define IMM2_MENTAL              (P)
#define IMM2_DISEASE             (Q)
#define IMM2_DROWNING            (R)
#define IMM2_LIGHT		(S)
#define IMM2_SLEEP		(T)
#define IMM2_WOOD                (X)
#define IMM2_SILVER              (Y)
#define IMM2_IRON                (Z)

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH		(O)
#define ASSIST_ALL       	(P)
#define ASSIST_ALIGN	        (Q)
#define ASSIST_RACE    	     	(R)
#define ASSIST_PLAYERS      	(S)
#define ASSIST_GUARD        	(T)
#define ASSIST_VNUM		(U)

/* Flags for wpn resistance checks */
#define WPN_UNARM     0
#define WPN_LONGSWORD         1
#define WPN_SPEAR             2
#define WPN_BROADSWORD        3
#define WPN_WHIP              4
#define WPN_STAFF             5
#define WPN_KNIFE             6
#define WPN_WARHAMMER         7
#define WPN_MACE              8
#define WPN_AXE               9
#define WPN_TWOHANDED         10
#define WPN_SHORTSWORD        11
#define WPN_BASTARDSWORD      12
#define WPN_FLAIL     13
#define WPN_MORNINGSTAR   14
#define WPN_POLEARM   15
#define WPN_CLUB      16
#define WPN_SICKLE    17
#define WPN_DAGGER    18
#define MAX_WPN               19




/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SLEEP		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)
 

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SLEEP		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)




/*
 * Let's just keep it down here - Jobo
 */
#include "board.h"

/*
 * Site ban structure.
 */
struct ban_data
{
  BAN_DATA *next;
  char *name;
  char *reason;
};

/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct time_info_data
{
  int hour;
  int day;
  int month;
  int year;
};

struct weather_data
{
  int mmhg;
  int change;
  int sky;
  int sunlight;
};

/*
 * Directions.
 * Used in #ROOMS.
 */
typedef enum
{
  DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_UP, DIR_DOWN,
  DIR_NORTHEAST, DIR_NORTHWEST, DIR_SOUTHEAST, DIR_SOUTHWEST,
  DIR_SOMEWHERE
}
dir_types;

/*
 * Stable prompt ala Erwins howto.
 */
#define VT_SAVECURSOR            "\e7"   /* Save cursor and attrib         */
#define VT_RESTORECURSOR         "\e8"   /* Restore cursor pos and attribs */
#define VT_SETWIN_CLEAR          "\e[r"  /* Clear scrollable window size   */
#define VT_CLEAR_SCREEN          "\e[2J" /* Clear screen                   */
#define VT_CLEAR_LINE            "\e[2K" /* Clear this whole line          */
#define VT_RESET_TERMINAL        "\ec"

#define SUB_NORTH DIR_NORTH
#define SUB_EAST  DIR_EAST
#define SUB_SOUTH DIR_SOUTH
#define SUB_WEST  DIR_WEST
#define SUB_UP    DIR_UP
#define SUB_DOWN  DIR_DOWN
#define SUB_NE    DIR_NORTHEAST
#define SUB_NW    DIR_NORTHWEST
#define SUB_SE    DIR_SOUTHEAST
#define SUB_SW    DIR_SOUTHWEST

/*
 * threaded status - Jobo
 */
#define STATUS_LOOKUP       0   // New Descriptor, in lookup pr. default.
#define STATUS_DONE         1   // The lookup is done.
#define STATUS_WAIT         2   // Closed while in thread.
#define STATUS_CLOSED       3   // Closed, ready to be recycled.

/*
 * Extra Descr bits - Jobo
 */
#define ED_TYPE_NONE        0
#define ED_TYPE_PULL        1
#define ED_TYPE_PRESS       2
#define ED_TYPE_PUSH        3
#define ED_TYPE_TOUCH       4

#define ED_ACTION_NONE      0
#define ED_ACTION_TELEPORT  1
#define ED_ACTION_OBJECT    2
#define ED_ACTION_SPELL     3
#define ED_ACTION_ELEVATOR  4


/* 
 * mob triggers - Jobo
 */
#define MTRIG_SAY         0
#define MTRIG_LOCATE      1
#define MTRIG_SCRY        2
#define MTRIG_HUNTER      4


/*
 * Mudinfo Bits
 */
#define MUDINFO_UPDATED      0
#define MUDINFO_MCCP_USERS   1
#define MUDINFO_OTHER_USERS  2
#define MUDINFO_PEAK_USERS   3
#define MUDINFO_MBYTE        4
#define MUDINFO_BYTE         5
#define MUDINFO_DATA_PEAK    6
#define MUDINFO_MSP_USERS    7
#define MUDINFO_MBYTE_S      8
#define MUDINFO_BYTE_S       9
#define MUDINFO_MAX         10

/*
 * Taking care of the control center
 */
#define CCENTER_MIN_EXP     0   /* default 4K        */
#define CCENTER_MAX_EXP     1   /* default 3 mill    */
#define CCENTER_EXP_LEVEL   2   /* default 100 %     */
#define CCENTER_QPS_LEVEL   3   /* default 100 %     */
#define CCENTER_MAX         4

/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_SEX			 6
#define CON_GET_NEW_RACE		 7
#define CON_GET_NEW_PROF		 8
#define CON_GET_NEW_GUILD		 9
#define CON_GET_NEW_CLASS		 10

#define CON_GET_NEW_ANSI                 11
#define CON_READ_MOTD			 12
#define CON_NOT_PLAYING			 13
#define CON_EDITING			 14
#define CON_COPYOVER_RECOVER             15
#define CON_PFILE                        25

/*
 * Character substates
 */
typedef enum
{
  SUB_NONE, SUB_PAUSE, SUB_PERSONAL_DESC, SUB_OBJ_SHORT, SUB_OBJ_LONG,
  SUB_OBJ_EXTRA, SUB_MOB_LONG, SUB_MOB_DESC, SUB_ROOM_DESC, SUB_ROOM_EXTRA,
  SUB_ROOM_EXIT_DESC, SUB_WRITING_NOTE, SUB_MPROG_EDIT, SUB_HELP_EDIT,
  SUB_WRITING_MAP, SUB_PERSONAL_BIO, SUB_REPEATCMD, SUB_RESTRICTED,
  SUB_DEITYDESC,
  /* timer types ONLY below this point */
  SUB_TIMER_DO_ABORT = 128, SUB_TIMER_CANT_ABORT
}
char_substates;

/*
 * Timer macros.
 */

#define TIMER(ch, tmr)		((ch)->tick_timer[(tmr)])
#define SET_TIMER(ch, tmr, tm)	((ch)->tick_timer[(tmr)] = (tm))
#define ADD_TIMER(ch, tmr, tm)  ((ch)->tick_timer[(tmr)] += (tm))
#define SUB_TIMER(ch, tmr, tm)  ((ch)->tick_timer[(tmr)] -= (tm))
#define TIME_UP(ch, tmr)	((ch)->tick_timer[(tmr)] == 0 ? TRUE : FALSE)

#define RTIMER(room, rtmr)	     ((room)->tick_timer[(rtmr)])
#define SET_RTIMER(room, rtmr, rtm)  ((room)->tick_timer[(rtmr)] = (rtm))
#define ADD_RTIMER(room, rtmr, rtm)  ((room)->tick_timer[(rtmr)] += (rtm))
#define SUB_RTIMER(room, rtmr, rtm)  ((room)->tick_timer[(rtmr)] -= (rtm))
#define RTIME_UP(room, rtmr)	     ((room)->tick_timer[(rtmr)] == 0 ? TRUE : FALSE)

/* player timers */
#define TIMER_CHALLENGE          1
#define MAX_TIMER		 11
#define TIMER_SUPERNOVA		 3
#define TIMER_BADMOON		 4
#define TIMER_HDESIRE		 5
#define TIMER_PESTILENCE	 6
#define TIMER_MADNESS	 	 7
#define TIMER_CALLWILD	 	 8
#define TIMER_GFURY		 9
#define TIMER_DEJAVU		 10


/* room timers */
// #define RTIMER_SILENCE           1
// #define MAX_RTIMER		 2

#define TIMER_ENTOMB             0
#define TIMER_THIRD_ARM_GROWING	 1
#define TIMER_FOURTH_ARM_GROWING 2
#define TIMER_THIRD_ARM_GOING	 3
#define TIMER_FOURTH_ARM_GOING   4


/*
#define RTIMER_LIFE_VORTEX	1
#define RTIMER_DEATH_VORTEX	2
#define RTIMER_GLYPH_PROTECTION	3
#define RTIMER_HIDE_ROOM	4
#define RTIMER_SWARM_BEES	5
#define RTIMER_SWARM_RATS	6
#define RTIMER_SWARM_BATS	7
#define RTIMER_GHOST_LIGHT	8
#define RTIMER_NEXUS_FLAME	9
#define RTIMER_NEXUS_WATER	10
#define RTIMER_NEXUS_AIR	11
#define RTIMER_NEXUS_EARTH	12
#define RTIMER_NEXUS_ENTROPY	13*/
#define RTIMER_WALL_NORTH	0
#define RTIMER_WALL_EAST	1
#define RTIMER_WALL_SOUTH	2
#define RTIMER_WALL_WEST	3
#define RTIMER_WALL_UP		4
#define RTIMER_WALL_DOWN	5
/*#define RTIMER_DISCORD		6
#define RTIMER_DARK_ROOM	7*/
#define RTIMER_SILENCE		6
#define RTIMER_FIRE		7
#define RTIMER_WATER		8
#define RTIMER_WIND		9
#define RTIMER_EARTH		10
#define RTIMER_HOLY		11
#define RTIMER_SAFE		12
#define MAX_RTIMER		13


/*
 * Needed for threads - Jobo
 */
struct dummy_arg
{
  DUMMY_ARG *next;
  DESCRIPTOR_DATA *d;
  char *buf;
  sh_int status;
};

/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
  DESCRIPTOR_DATA       *   next;
  DESCRIPTOR_DATA       *   snoop_by;
  CHAR_DATA             *   character;
  char                  *   host;
  char                  *   showstr_head;           /* From ENVY code to compile */
  char                  *   showstr_point;          /* From ENVY code to compile */
  char                  *   outbuf;
  char                      inbuf[4 * MAX_INPUT_LENGTH];
  char                      incomm[MAX_INPUT_LENGTH];
  char                      inlast[MAX_INPUT_LENGTH];
  char                 **   pString;                /* OLC */
  sh_int                    descriptor;
  sh_int                    connected;
  sh_int                    lookup_status;
  bool                      fcommand;
  int                       repeat;
  int                       outsize;
  int                       outtop;
  int                       editor;                 /* OLC */
  void                  *   pEdit;                  /* OLC */
  z_stream              *   out_compress;           /* MCCP */
  unsigned char         *   out_compress_buf;       /* MCCP */
};

/*
 * Attribute bonus structures.
 */
struct str_app_type
{
  sh_int tohit;
  sh_int todam;
  sh_int carry;
  sh_int wield;
};

struct int_app_type
{
  sh_int learn;
};

struct wis_app_type
{
  sh_int practice;
};

struct dex_app_type
{
  sh_int defensive;
};

struct con_app_type
{
  sh_int hitp;
  sh_int shock;
};

/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4

/*
 * Help table types.
 */
struct help_data
{
  HELP_DATA *next;
  HELP_DATA *prev;
  AREA_DATA *area;
  sh_int level;
  char *keyword;
  char *text;
};

/*
 * Data structure for notes.
 */
struct note_data
{
  NOTE_DATA *next;
  char *sender;
  char *date;
  char *to_list;
  char *subject;
  char *text;
  time_t date_stamp;
  time_t expire;
};


/*
 * An affect for areas
 */
struct area_affect
{
  AREA_AFFECT *next;
  sh_int       religion;
  sh_int       duration;
  sh_int       type;
  sh_int       level;
};




/*
 * An affect.
 */
struct affect_data
{
  AFFECT_DATA *next;
  sh_int religion;
  sh_int type;
  sh_int duration;
  sh_int location;
  sh_int modifier;
  int bitvector;
};

struct  affect2_data
{
    AFFECT2_DATA *      next;
    sh_int              type;
    sh_int              duration;
    sh_int              location;
    sh_int              modifier;
    int                 bitvector;
};


/*
 * A quest
 */
struct quest_data
{
  QUEST_DATA *next;             // next quest player has
  sh_int time;                  // in mud hours
  sh_int type;                  // the type of quest
  int giver;                    // the questmasters vnum
  int vnums[4];                 // vnum of questor
};

/*
 * History data used for storing messages;
 */
struct history_data
{
  HISTORY_DATA *next;
  char *message;
};

/*
 * An alias
 */
struct alias_data
{
  ALIAS_DATA *next;
  char *short_n;
  char *long_n;
};

/* Structure for the map itself */
struct map_type
{
  char tegn;                    /* Character to print at this map coord */
  int vnum;                     /* Room this coord represents */
  int depth;                    /* Recursive depth this coord was found at */
  int info;
  bool can_see;
};

/*
 * one single change
 */
struct change_data
{
  CHANGE_DATA *next;
  CHANGE_DATA *prev;
  char *imm;
  char *text;
  char *date;
};


typedef struct heap_data
{
  sh_int             iVertice;
  ROOM_INDEX_DATA  * knude[MAX_KNUDE];
} HEAP;

/*
 * one poll
 */
struct poll_data
{
  POLL_DATA *next;
  VOTE_DATA *votes;
  time_t expire;
  char *name;
  char *options[MAX_VOTE_OPTIONS];
  int vcount[MAX_VOTE_OPTIONS];
};

/*
 * one vote
 */
struct vote_data
{
  VOTE_DATA *next;
  char *pname;
  char *phost;
  int choice;
};

/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
  sh_int number;
  sh_int killed;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/* ASCII conversions -- used so we can have letters in this file */

#define A                       1
#define B                       2
#define C                       4
#define D                       8
#define E                       16
#define F                       32
#define G                       64
#define H                       128
#define I                       256
#define J                       512
#define K                       1024
#define L                       2048
#define M                       4096
#define N                       8192
#define O                       16384
#define P                       32768
#define Q                       65536
#define R                       131072
#define S                       262144
#define T                       524288
#define U                       1048576
#define V                       2097152
#define W                       4194304
#define X                       8388608
#define Y                       16777216
#define Z                       33554432
#define aa                      67108864  /* doubled due to conflicts */
#define bb                      134217728
#define cc                      268435456
#define ddd                     536870912
#define ee                      1073741824
#define ff			2147483648
/*
 * Bits for 'affected_by'.
 * Used in #MOBILES. flags2
 */

#define AFF_TOTALBLIND		(D)
#define AFF_SPIRITGUARD		(E)

// flag3
#define AFF3_BLINK_1ST_RD	(A)
#define AFF3_BLINK_2ND_RD	(B)

/*
 * Immunities, for players.  KaVir.
*/
#define IMM_SLASH             1       /* Resistance to slash, slice.          */
#define IMM_STAB              2       /* Resistance to stab, pierce.          */
#define IMM_SMASH             4       /* Resistance to blast, pound, crush.   */
#define IMM_ANIMAL            8       /* Resistance to bite, claw.            */
#define IMM_MISC             16       /* Resistance to grep, suck, whip.      */
#define IMM_CHARM            32       /* Immune to charm spell.               */
#define IMM_HEAT             64       /* Immune to fire/heat spells.          */
#define IMM_COLD            128       /* Immune to frost/cold spells.         */
#define IMM_LIGHTNING       256       /* Immune to lightning spells.          */
#define IMM_ACID            512       /* Immune to acid spells.               */
#define IMM_SUMMON         1024       /* Immune to being summoned.            */
#define IMM_SHIELDED       2048       /* For Obfuscate. Block scry, etc.      */
#define IMM_HURL           4096       /* Cannot be hurled.                    */
#define IMM_BACKSTAB       8192       /* Cannot be backstabbed.               */
#define IMM_KICK          16384       /* Cannot be kicked.                    */
#define IMM_DISARM        32768       /* Cannot be disarmed.                  */
#define IMM_STEAL         65536       /* Cannot have stuff stolen.            */
#define IMM_SLEEP        131072       /* Immune to sleep spell.               */
#define IMM_DRAIN        262144       /* Immune to energy drain.              */
#define IMM_TRANSPORT    524288       /* Objects can't be transported to you. */
#define IMM_TRAVEL      1048576       /* Immune to class travelling powers    */
#define IMM_SHIELD2     2097152



/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC                  (A) /* Auto set for mobs    */
#define ACT_SENTINEL                (B) /* Stays in one room    */
#define ACT_SCAVENGER               (C) /* Picks up objects     */
#define ACT_AGGRESSIVE              (D) /* Attacks PC's         */
#define ACT_STAY_AREA               (E) /* Won't leave area     */
#define ACT_WIMPY                   (F) /* Flees when hurt      */
#define ACT_FISH                    (G) /* Can only swim        */
#define ACT_NOSWIM                  (H) /* cannot enter water   */
#define ACT_MOUNT                   (J) /* Can be mounted       */
#define ACT_NOPARTS                 (K) /* Dead = no body parts */
#define ACT_NOEXP                   (L) /* No exp for killing   */
#define ACT_PROTOTYPE               (M)
#define ACT_NOAUTOKILL              (N)
#define ACT_NOTRAVEL		    (O)
/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_SHADOWPLANE		     64 /* Creatures in shadow plane - KaVir */
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_FLAMING		   2048   /* For burning creatures - KaVir */
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_ETHEREAL		  16384 /* For ethereal creatures - KaVir */
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_CHARM		 131072
#define AFF_FLYING		 262144
#define AFF_PASS_DOOR		 524288
#define AFF_POLYMORPH		1048576 /* For polymorphed creatures - KaVir */
#define AFF_SHADOWSIGHT		2097152 /* Can see between planes - KaVir */
#define AFF_PROTECT_GOOD        4194304
#define AFF_PEACE               8388608
#define AFF_DARK_VISION		16777216
#define AFF_SWIM		33554432
#define AFF_REGENERATION	67108864 
#define AFF_HOLYAURA		134217728
#define AFF_DROWFIRE		268435456


/* affected_by 2 */
#define EXTRA_BLINKY		1
#define AFF2_BLINK_1ST_RD	2
#define AFF2_BLINK_2ND_RD       4
#define AFF2_BALOR              8
#define AFF2_FARMOR             16
#define AFF2_THORNS             32
#define AFF2_DARMOR             64
#define AFF2_FROZEN             128
#define AFF2_EXPMOD             256





/*
 * Bits for 'itemaffect'.
 * Used in #MOBILES.
 */
#define ITEMA_CHAOSSHIELD             1
#define ITEMA_ARTIFACT                2
#define ITEMA_REGENERATE              4
#define ITEMA_SPEED                   8
#define ITEMA_VORPAL                 16
#define ITEMA_RESISTANCE             32
#define ITEMA_VISION                 64
#define ITEMA_STALKER               128
#define ITEMA_VANISH                256
#define ITEMA_ILLUSIONS		    512
#define ITEMA_HOLYSYMBOL	   1024


/* 
 * Foreground Colours 
 */
#define NO_COLOUR       "\e[0m"     /*   Resets Colour  */
#define GREY            "\e[1;30m"
#define D_WHITE         "\e[0;37m"
#define WHITE           "\e[1;37m"
#define D_RED           "\e[0;31m"
#define L_RED           "\e[1;31m"
#define D_GREEN         "\e[0;32m"
#define L_GREEN         "\e[1;32m"
#define D_BLUE          "\e[0;34m"
#define L_BLUE          "\e[1;34m"
#define BROWN           "\e[0;33m"
#define YELLOW          "\e[1;33m"
#define MAGENTA         "\e[0;35m"
#define PINK            "\e[1;35m"
#define D_CYAN          "\e[0;36m"
#define L_CYAN          "\e[1;36m"
#define NORMAL          "\e[0;37m"
/*
 * Ansi Affects
 */
#define UNDERLINE       "\e[4m"
#define INVERTED        "\e[7m"
#define FLASHING        "\e[5m"
/*
 * Background Colours 
 */
#define BG_RED          "\e[1;41m"
#define BG_GREEN        "\e[1;42m"
#define BG_BROWN        "\e[1;43m"
#define BG_BLUE         "\e[1;44m"
#define BG_PURPLE       "\e[1;45m"
#define BG_CYAN         "\e[1;46m"
#define BG_WHITE        "\e[1;47m"
#define BG_CLEAR        "\e[1;47m\e[0;32m" 

#define ADD_COLOUR(_player, _str, _col)				\
{								\
  char swh_temp[255];						\
  if (!IS_NPC(_player) && IS_SET(_player->act, PLR_ANSI))	\
  {								\
     swh_temp[0] = '\0';					\
     strcpy(swh_temp, _col);					\
     strcat(swh_temp, _str);					\
     strcat(swh_temp, NORMAL);					\
     strcpy(_str, swh_temp);					\
  }								\
}

#define SCALE_COLS 4

#define COL_SCALE(_swh_str,_swh_ch,_swh_curr,_swh_max) \
	ADD_COLOUR(_swh_ch, _swh_str, \
	(_swh_curr < 1) ? L_RED : \
	(_swh_curr < _swh_max) ? \
	scale[(SCALE_COLS * _swh_curr) / ((_swh_max > 0) \
	? _swh_max : 1)] : L_CYAN)

extern char *scale[SCALE_COLS];

/*
 * Bits for 'polymorph'.
 * Used for players.
 */
#define POLY_BAT		      1
#define POLY_WOLF		      2
#define POLY_MIST		      4
#define POLY_SERPENT		      8
#define POLY_RAVEN		     16
#define POLY_FISH		     32
#define POLY_FROG		     64
#define POLY_ZULOFORM               128
#define POLY_SHIFT                  256
#define POLY_SPIDER		    512
#define POLY_DRAGON		   1024

/*
 * Score.
 */
#define SCORE_TOTAL_XP		      0
#define SCORE_HIGH_XP		      1
#define SCORE_TOTAL_LEVEL	      2
#define SCORE_HIGH_LEVEL	      3
#define SCORE_QUEST		      4
#define SCORE_NUM_QUEST		      5

/*
 * Damcap values.
 */
#define DAM_CAP		      0
#define DAM_CHANGE	      1

/* return values for check_imm */
#define IS_NORMAL               0
#define IS_DIMMUNE              1
#define IS_RESISTANT            2
#define IS_VULNERABLE           3

/*
 * Mounts
 */
#define IS_ON_FOOT		      0
#define IS_MOUNT		      1
#define IS_RIDING		      2
#define IS_CARRIED		      4
#define IS_CARRYING		      8

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      6
#define ITEM_ARMOR		      7
#define ITEM_POTION		      8
#define ITEM_FURNITURE		      9
#define ITEM_TRASH		     10
#define ITEM_CONTAINER		     11
#define ITEM_DRINK_CON		     12
#define ITEM_KEY		     13
#define ITEM_FOOD		     14
#define ITEM_MONEY		     15
#define ITEM_BOAT		     16
#define ITEM_CORPSE_NPC		     17
#define ITEM_CORPSE_PC		     18
#define ITEM_FOUNTAIN		     19
#define ITEM_PILL		     20
#define ITEM_PORTAL		     21
#define ITEM_HOLYSYMBOL		     22
#define ITEM_STAKE		     24
#define ITEM_QUEST		     25
#define ITEM_WALL                    26
#define ITEM_DTOKEN		     27
#define ITEM_HEAD		     28
#define ITEM_COOKINGPOT   	     29
#define	ITEM_FAITHTOKEN		     30
#define ITEM_GRENADE		     31

/*
 * Weapon Stats
 */

#define WEAPON_FLAMING          (A)
#define WEAPON_FROST            (B)
#define WEAPON_VAMPIRIC         (C)
#define WEAPON_SHARP            (D)
#define WEAPON_VORPAL           (E)
#define WEAPON_TWO_HANDS        (F)
#define WEAPON_SHOCKING         (G)
#define WEAPON_POISON           (H)
#define WEAPON_SUNBLADE         (I)
#define WEAPON_DRAGONLANCE      (J)
#define WEAPON_SILVER           (K)
#define WEAPON_RUNE_FORCE_BOLT  (L)
#define WEAPON_RUNE_SMITE_EVIL  (M)
#define WEAPON_RUNE_BLAZE       (N)
#define WEAPON_RUNE_LIGHTNING   (O)
#define WEAPON_RUNE_DANCING     (P)
#define WEAPON_ELE_FLAME        (Q)
#define WEAPON_ELE_WATER        (R)
#define WEAPON_ELE_EARTH        (S)
#define WEAPON_ELE_AIR          (T)

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_THROWN		      4
#define ITEM_KEEP		      8
#define ITEM_VANISH		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_LOYAL		  16384
#define ITEM_SHADOWPLANE	  32768
#define ITEM_PROTOTYPE		    (L) // 2048, same as anti_neutral.. FIX FIX

#define ITEM_NOCLAIM             131072
#define ITEM_NOLOCATE            262144

/* Item extra flags II, the return of the item flags!  */
#define ITEM_ARM		  (A)
#define ITEM_DAEMONSEED		  (D)

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_FACE		  32768
#define ITEM_WEAR_SPECIAL	  65536
#define ITEM_WEAR_BODYART 	 131072
#define ITEM_WEAR_MEDAL          262144
#define ITEM_WEAR_FLOAT          524288

/*
 * Apply types (for quest affects).
 * Used in #OBJECTS.
 */
#define QUEST_STR		      1
#define QUEST_DEX		      2
#define QUEST_INT		      4
#define QUEST_WIS		      8
#define QUEST_CON		     16
#define QUEST_HITROLL		     32
#define QUEST_DAMROLL		     64
#define QUEST_HIT		    128
#define QUEST_MANA		    256
#define QUEST_MOVE		    512
#define QUEST_AC		   1024
#define QUEST_ENCHANTED		   2048
#define QUEST_SPELLPROOF	   4096
#define QUEST_ARTIFACT		   8192
#define QUEST_IMPROVED		  16384
#define QUEST_PRIZE      	  32768
#define QUEST_RELIC               65536
#define QUEST_RITUAL		 131072  
#define QUEST_WWPOISON		 262144
#define QUEST_FLAMEBLADE	 524288
#define QUEST_SHINOBI		1048576
#define QUEST_GIANTSTONE	2097152
#define QUEST_SHARPEN		4194304
#define QUEST_VINEFORGE		8388608

// #define R                       131072
// #define S                       262144
// #define T                       524288
// #define U                       1048576


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_POLY		     25

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_OTRANS		      2
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_PRIVATE		     16
#define ROOM_SAFE		     32
#define ROOM_NO_RECALL		     64
#define ROOM_NO_TELEPORT	    128
#define ROOM_TOTAL_DARKNESS         256
#define ROOM_BLADE_BARRIER          512
#define ROOM_ARENA                 1024
#define ROOM_FLAMING               2048
#define ROOM_SILENCE               4096
#define ROOM_ASTRAL		   8192
#define ROOM_PROTOTYPE		  16384
#define ROOM_ORDER		  32768
#define ROOM_LIGHT                65536

/*
 * Shinobi Room Flags
 * Used by the final 5 powers.
 */

#define ROOM_SFIRE		1
#define ROOM_SWATER		2
#define ROOM_SWIND		4
#define ROOM_SHOLY		8
#define ROOM_SEARTH		16
#define ROOM_SSAFE		32




/*
 * Room text flags (KaVir).
 * Used in #ROOMS.
 */
#define RT_LIGHTS		      1     /* Toggles lights on/off */
#define RT_SAY			      2     /* Use this if no others powers */
#define RT_ENTER		      4
#define RT_CAST			      8
#define RT_THROWOUT		     16   /* Erm...can't remember ;) */
#define RT_OBJECT		     32     /* Creates an object */
#define RT_MOBILE		     64     /* Creates a mobile */
#define RT_LIGHT		    128     /* Lights on ONLY */
#define RT_DARK			    256     /* Lights off ONLY */
#define RT_OPEN_LIFT		    512 /* Open lift */
#define RT_CLOSE_LIFT		   1024 /* Close lift */
#define RT_MOVE_LIFT		   2048 /* Move lift */
#define RT_SPELL		   4096     /* Cast a spell */
#define RT_PORTAL		   8192     /* Creates a one-way portal */
#define RT_TELEPORT		  16384   /* Teleport player to room */
#define RT_ACTION		  32768
#define RT_BLANK_1		  65536
#define RT_BLANK_2		 131072
#define RT_TEXT                  262144 /* Send a block of text to char */
#define RT_RETURN		1048576     /* Perform once */
#define RT_PERSONAL		2097152   /* Only shows message to char */
#define RT_TIMER		4194304     /* Sets object timer to 1 tick */

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5

/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_PICKPROOF		     32
#define EX_NOPASS                    64
#define EX_EASY                     128
#define EX_HARD                     256
#define EX_INFURIATING              512
#define EX_NOCLOSE                 1024
#define EX_NOLOCK                  2048
#define EX_ICE_WALL                4096
#define EX_FIRE_WALL               8192
#define EX_SWORD_WALL             16384
#define EX_PRISMATIC_WALL         32768
#define EX_IRON_WALL              65536
#define EX_MUSHROOM_WALL         131072
#define EX_CALTROP_WALL          262144
#define EX_ASH_WALL              524288
#define EX_WARDING              1048576
#define EX_SHIN_WALL		2097152
#define MAX_EXFLAG		      21
#define MAX_WALL		       9

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11

/*
 * Equipment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_THIRD		     18
#define WEAR_FOURTH		     19
#define WEAR_FACE		     20
#define WEAR_SCABBARD_L		     21
#define WEAR_SCABBARD_R		     22
#define WEAR_SPECIAL                 23
#define WEAR_FLOAT                   24
#define WEAR_MEDAL                   25
#define WEAR_BODYART                 26
#define MAX_WEAR		     27

/*
 * Locations for damage.
 */
#define LOC_HEAD		      0
#define LOC_BODY		      1
#define LOC_ARM_L		      2
#define LOC_ARM_R		      3
#define LOC_LEG_L		      4
#define LOC_LEG_R		      5

/*
 * For Head
 */
#define LOST_EYE_L		       1
#define LOST_EYE_R		       2
#define LOST_EAR_L		       4
#define LOST_EAR_R		       8
#define LOST_NOSE		      16
#define BROKEN_NOSE		      32
#define BROKEN_JAW		      64
#define BROKEN_SKULL		     128
#define LOST_HEAD		     256
#define LOST_TOOTH_1		     512  /* These should be added..... */
#define LOST_TOOTH_2		    1024  /* ...together to caculate... */
#define LOST_TOOTH_4		    2048  /* ...the total number of.... */
#define LOST_TOOTH_8		    4096  /* ...teeth lost.  Total..... */
#define LOST_TOOTH_16		    8192  /* ...possible is 31 teeth.   */
#define LOST_TONGUE		   16384

/*
 * For Body
 */
#define BROKEN_RIBS_1		       1  /* Remember there are a total */
#define BROKEN_RIBS_2		       2  /* of 12 pairs of ribs in the */
#define BROKEN_RIBS_4		       4  /* human body, so not all of  */
#define BROKEN_RIBS_8		       8  /* these bits should be set   */
#define BROKEN_RIBS_16		      16  /* at the same time.          */
#define BROKEN_SPINE		      32
#define BROKEN_NECK		      64
#define CUT_THROAT		     128
#define CUT_STOMACH		     256
#define CUT_CHEST		     512

/*
 * For Arms
 */
#define BROKEN_ARM		       1
#define LOST_ARM		       2
#define LOST_HAND		       4
#define LOST_FINGER_I		       8  /* Index finger */
#define LOST_FINGER_M		      16  /* Middle finger */
#define LOST_FINGER_R		      32  /* Ring finger */
#define LOST_FINGER_L		      64  /* Little finger */
#define LOST_THUMB		     128
#define BROKEN_FINGER_I		     256  /* Index finger */
#define BROKEN_FINGER_M		     512  /* Middle finger */
#define BROKEN_FINGER_R		    1024  /* Ring finger */
#define BROKEN_FINGER_L		    2048  /* Little finger */
#define BROKEN_THUMB		    4096

/*
 * For Legs
 */
#define BROKEN_LEG		       1
#define LOST_LEG		       2
#define LOST_FOOT		       4
#define LOST_TOE_A		       8
#define LOST_TOE_B		      16
#define LOST_TOE_C		      32
#define LOST_TOE_D		      64  /* Smallest toe */
#define LOST_TOE_BIG		     128
#define BROKEN_TOE_A		     256
#define BROKEN_TOE_B		     512
#define BROKEN_TOE_C		    1024
#define BROKEN_TOE_D		    2048  /* Smallest toe */
#define BROKEN_TOE_BIG		    4096

/*
 * For Bleeding
 */
#define BLEEDING_HEAD		       1
#define BLEEDING_THROAT		       2
#define BLEEDING_ARM_L		       4
#define BLEEDING_ARM_R		       8
#define BLEEDING_HAND_L		      16
#define BLEEDING_HAND_R		      32
#define BLEEDING_LEG_L		      64
#define BLEEDING_LEG_R		     128
#define BLEEDING_FOOT_L		     256
#define BLEEDING_FOOT_R		     512

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_MEDITATING		      5
#define POS_SITTING		      6
#define POS_RESTING		      7
#define POS_FIGHTING                  8
#define POS_STANDING                  9

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC                1   /* Don't EVER set.  */
#define PLR_SOUND                 2
#define PLR_LEFTHAND              4
#define PLR_AUTOEXIT              8
#define PLR_AUTOLOOT             16
#define PLR_AUTOSAC              32
#define PLR_AMBI                128
#define PLR_FREEZE              256
#define PLR_MAP                 512
#define PLR_PROMPT             1024
#define PLR_TELNET_GA          2048
#define PLR_HOLYLIGHT          4096
#define PLR_WIZINVIS           8192
#define PLR_ANSI              16384
#define	PLR_SILENCE           32768
#define PLR_RIGHTHAND        131072
#define PLR_MUSIC            262144
#define PLR_LOG              524288
#define PLR_DENY            1048576
#define PLR_BRIEF1          2097152
#define PLR_BRIEF2          4194304
#define PLR_BRIEF3          8388608
#define PLR_BRIEF4         16777216
#define PLR_BRIEF5         33554432
#define PLR_BRIEF6         67108864

/* New bits for playrs */
#define NEW_IRONMIND           1
#define NEW_DARKNESS           2
#define THIRD_HAND             4
#define FOURTH_HAND            8
#define NEW_MASTERY           16
#define NEW_SCLAWS	      32
#define NEW_MONKFLAME	      64
#define NEW_QUILLS           128
#define NEW_REND             256
#define NEW_SKIN	     512
#define NEW_FIGHTDANCE      1024
#define NEW_DARKTENDRILS    2048
#define NEW_TOUGHSKIN	    4096
#define NEW_MONKADAM	    8192
#define NEW_CUBEFORM       16384
#define NEW_WASH	   32768
#define NEW_VIUNEMTO	  131072
#define NEW_HIDE	  266144
#define NEW_CHAGI         524288
#define NEW_JAWLOCK      1048576
#define NEW_VISION       2097152
/*
 * JFLAGS : ch->pcdata->jflags
 */
#define JFLAG_SETDECAP                1
#define JFLAG_SETLOGIN                2
#define JFLAG_SETLOGOUT               4
#define JFLAG_SETAVATAR               8
#define JFLAG_BULLY                  16
#define JFLAG_SETTIE                 32
#define JFLAG_NOSET                  64
#define JFLAG_SS1                   128
#define JFLAG_SS2                   256
#define JFLAG_SS3                   512
#define JFLAG_SS4                  1024
#define JFLAG_SS5                  2048
#define JFLAG_MADNESS              4096
#define JFLAG_POLICY               8192
#define JFLAG_POLICE              16384

/*
 * TEMPFLAGS : ch->pcdata->tempflag
 */
#define TEMP_ARTIFACT                 1
#define TEMP_VT100                    2
#define TEMP_AGGRESSIVE               4
#define TEMP_EDGE                     8

/*
 * RELPOWERS : ch->pcdata->rel_powers
 */
#define REL_DARMOR                    1
#define REL_HOLYWORD                  2
#define REL_GZONE                     4
#define REL_GAIA                      8
#define REL_THORNS                   16
#define REL_CALLWILD                 32
#define REL_DEJA                     64
#define REL_PESTILENCE              128
#define REL_MADNESS                 256
#define REL_AREAHEAL                512
#define REL_BANISHMENT             1024
#define REL_STARSIGHT              2048
#define REL_RPURGE                 4096
#define REL_DBATTLE                8192
#define REL_GFURY                 16384
#define REL_BADMOON               32768
#define REL_HDESIRE               65536
#define REL_DSCRY                131072
#define REL_SUBVERT              262144
#define REL_REBIRTH              524288
#define REL_CHAOS               1048576
#define REL_NOVA                2097152
#define REL_FARMOR              4194304

#define RELIGION_LOA                  1
#define RELIGION_STARS                2
#define RELIGION_ONE                  3
#define RELIGION_WILD                 4
#define RELIGION_GAHR                 5

/*
 * Religion Area Affects
 */
#define AREA_AFF_GZONE                1
#define AREA_AFF_THORNS               2
#define AREA_AFF_PESTILENCE           3
#define AREA_AFF_BADMOON              4
#define AREA_AFF_GAIA                 5
#define AREA_AFF_CHAOS                6

/*
 * special flags
 */

#define SPC_CHOSE_RELIGION      1       /* To set when new religion must be chosen */
#define SPC_HAS_CHOSEN_RELIGION 2       /* Have chosen the religion */


/*
 * EXTRA bits for players. (KaVir)
 */
#define EXTRA_NEWPASS		      1
#define EXTRA_OSWITCH		      2
#define TIED_UP			      4
#define GAGGED			      8
#define BLINDFOLDED		     16
#define EXTRA_DONE		     32
#define EXTRA_EXP		     64
#define EXTRA_PREGNANT		    128
#define EXTRA_LABOUR		    256
#define EXTRA_BORN		    512
#define EXTRA_PROMPT		   1024
#define EXTRA_MARRIED		   2048
#define EXTRA_AFK        	   4096
#define EXTRA_CALL_ALL		   8192
#define EXTRA_DRAGON		  16384
#define PINNEDWALL 		  32768

/*
 * Stances for combat
 */
#define STANCE_NONE		     -1
#define STANCE_NORMAL		      0
#define STANCE_VIPER		      1
#define STANCE_CRANE		      2
#define STANCE_CRAB		      3
#define STANCE_MONGOOSE		      4
#define STANCE_BULL		      5
#define STANCE_MANTIS		      6
#define STANCE_DRAGON		      7
#define STANCE_TIGER		      8
#define STANCE_MONKEY		      9
#define STANCE_SWALLOW		     10
#define STANCE_WOLF                  11
#define STANCE_AUTOSTANCE	     12
#define STANCE_SS1		     13
#define STANCE_SS2                   14
#define STANCE_SS3                   15
#define STANCE_SS4                   16
#define STANCE_SS5                   17

/*
 *  Bits for superstances
 */
#define STANCEPOWER_DODGE              1  /* more dodge */
#define STANCEPOWER_PARRY              2  /* more parry */
#define STANCEPOWER_SPEED              4  /* 2 extra attack */
#define STANCEPOWER_BYPASS             8  /* bypass dodge/parry */
#define STANCEPOWER_DAMAGE_1          16  /* lesser increase damage and chance to hit */
#define STANCEPOWER_DAMAGE_2          32  /* greater increase damage and chance to hit */
#define STANCEPOWER_DAMAGE_3          64  /* supreme increase damage and chance to hit */
#define STANCEPOWER_RESIST_1         128  /* lesser resist damage */
#define STANCEPOWER_RESIST_2         256  /* greater resist damage */
#define STANCEPOWER_RESIST_3         512  /* supreme resist damage */
#define STANCEPOWER_DAMCAP_1        1024  /* lesser damcap bonus */
#define STANCEPOWER_DAMCAP_2        2048  /* greater damcap bonus */
#define STANCEPOWER_DAMCAP_3        4096  /* supreme damcap bonus */
#define STANCEPOWER_REV_DAMCAP_1    8192  /* lesser damcap penalty for opponent */
#define STANCEPOWER_REV_DAMCAP_2   16384  /* greater damcap penalty for opponent */
#define STANCEPOWER_REV_DAMCAP_3   32768  /* supreme damcap penalty for opponent */

/*
 * Channel bits.
 */

#define	CHANNEL_CHAT		      2
#define CHANNEL_CLASS                 4
#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_MUSIC		     16
#define CHANNEL_DKTALK	             32
#define CHANNEL_ROLEPLAY             64
#define	CHANNEL_YELL		    128
#define	CHANNEL_LOG		    256
#define	CHANNEL_INFO		    512
#define	CHANNEL_FLAME		   1024
#define	CHANNEL_TELL		   2048
#define CHANNEL_NEWBIE		   4096

struct l_board
{
  char   * pk_name;
  char   * time_name;
  char   * quest_name;
  char   * mobkills_name;
  char   * pkscore_name;
  char   * arena_name;
  int      arena_number;
  int      pk_number;
  int      time_number;
  int      quest_number;
  int      mobkills_number;
  int      pkscore_number;
};

struct religion_table
{
  char * name;
  char * truename;
  char * channel;
  char * decapmessage;
  char * rankname[5];
};


/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
  MOB_INDEX_DATA *     next;
  SPEC_FUN       *     spec_fun;
  QUEST_FUN      *     quest_fun;
  SHOP_FUN       *     shop_fun;
  CHAR_DATA      *     mount;
  CHAR_DATA      *     wizard;
  AREA_DATA      *     area;              /* OLC */
  char           *     player_name;
  char           *     short_descr;
  char           *     long_descr;
  char           *     description;
  int		       material;
  int                  loc_hp[7];
  int                  vnum;
  sh_int               count;
  sh_int               killed;
  sh_int               sex;
  sh_int               mounted;
  sh_int               home;
  sh_int               natural_attack;
  int                  level;
  int                  immune;
  int                  polyaff;
  int                  vampaff;
  int                  itemaffect;
  int                  form;
  int                  act;
  int                  extra;
  int                  affected_by;
  int                  affected_by2;
  int                  alignment;
    sh_int		race;
  /* values 0-2 in OLC */
  int                  toughness;
  int                  dam_modifier;
  int                  extra_attack;
};

struct editor_data
{
  sh_int   numlines;
  sh_int   on_line;
  sh_int   size;
  char     line[49][81];
};

/*
 * One character (PC or NPC).
 */
struct char_data
{
  CHAR_DATA        *  next;
  CHAR_DATA        *  prev_in_room;
  CHAR_DATA        *  next_in_room;
  CHAR_DATA        *  master;
  CHAR_DATA        *  leader;
  CHAR_DATA        *  fighting;
  CHAR_DATA        *  reply;
  CHAR_DATA        *  mount;
  CHAR_DATA        *  challenger;
  SPEC_FUN         *  spec_fun;
  QUEST_FUN        *  quest_fun;
  SHOP_FUN         *  shop_fun;
  MOB_INDEX_DATA   *  pIndexData;
  DESCRIPTOR_DATA  *  desc;
  AFFECT_DATA      *  affected;
  AFFECT2_DATA     *  affected2;
  OBJ_DATA         *  carrying;
  ROOM_INDEX_DATA  *  in_room;
  ROOM_INDEX_DATA  *  was_in_room;
  PC_DATA          *  pcdata;
  DO_FUN           *  last_cmd;
  DO_FUN           *  prev_cmd;
  char             *  hunting;
  char             *  name;
  char             *  pload;
  char             *  short_descr;
  char             *  long_descr;
  char             *  description;
  char             *  morph;
  char             *  createtime;
  char             *  lasttime;
  char             *  lasthost;
  char             *  prompt;
  char             *  cprompt;
  char             *  prefix;
  int		      material;
  int                 c_material;
//  int		      alloy[20];
  time_t              logon;
  time_t              save_time;
  bool                deathmatch;
  bool                tracking;
  sh_int              sex;
  sh_int              amount_attacks_recieved;
  sh_int              amount_attacks_dealt;
  sh_int              decay_pulse;
  sh_int              hunt_pointer;
  sh_int              hunt_playerid;
  sh_int              generation;
  sh_int              tick_timer[MAX_TIMER];
  sh_int              loc_hp[7];
  sh_int              wpn[19];  
  sh_int              spl[8];   
  sh_int              cmbt[8];   
  sh_int              mounted;
  sh_int              level; 
  sh_int              trust; 
  sh_int              timer; 
  sh_int              wait;
  sh_int	      race;
  sh_int	      size;
  int                 amount_damage_recieved;
  int                 amount_damage_dealt;
  int                 class;
  int                 immune;
  int                 polyaff;
  int                 fight_timer;
  int                 itemaffect;
  int                 form;
  int                 flag2;
  int                 stance[24];
  int		      pcstyle[4];
  int                 home;
  int                 played;
  int                 pkill;
  int                 pdeath;
  int                 mkill;
  int                 mdeath;
  int                 hit;
  int                 max_hit;
  int                 mana;
  int                 max_mana;
  int                 move;
  int                 max_move;
  double              exp;
  int                 act;
  int                 extra;
  int                 newbits;
  int                 special;
  int                 affected_by;
  int                 affected_by2;
  int                 position;
  int                 practice;
  int                 carry_weight;
  int                 carry_number;
  int                 saving_throw;
  int                 alignment;
  int                 hitroll;
  int                 damroll;
  int                 armor;
  int                 deaf;
  int                 damcap[2];

    /* stats */
    sh_int		perm_stat[MAX_STATS];
    sh_int		mod_stat[MAX_STATS];


  /* BUILD INTERFACE */
  void             *  dest_buf;
  EDITOR_DATA      *  editor;
  sh_int              substate;
  int                 pagelen;

  CHAR_DATA *	        viunemto;

  int     chakra;
  int     immune2;
  int     res;
  int     vuln;
  int     pcsphere;
  int	  pRank;
  int     pcRace;
  int	  pcRaceLevel;
  int	  pcpractice;
  int 	  pcwpnprof;
  int	  pcmaxwpnprof;
  int	  pcsphereammount;
  int	  pcgold;
  int	  pcsilver;
  int	  pccopper;
  int     pcremort;
  int     pcsize;
  int	  pcLegacy;
  int	  profession;
  int     guild;
  int     rawmaterial[50];
  int	  alloy[20];

  
};

/*
 * Data which only PC's have.
 */
struct pc_data
{
  PC_DATA      *next;
  CHAR_DATA    *familiar;
  CHAR_DATA    *partner;
  CHAR_DATA    *propose;
  CHAR_DATA    *pfile;
  OBJ_DATA     *chobj;
  OBJ_DATA     *memorised;
  BOARD_DATA   *board;
  NOTE_DATA    *in_progress;
  ALIAS_DATA   *alias;
  QUEST_DATA   *quests;
  AREA_DATA    *prev_area;
  HISTORY_DATA *history;
  char         *last_decap[2];
  char         *retaliation;
  char         *pwd;
  char         *bamfin;
  char         *bamfout;
  char         *title;
  char         *conception;
  char         *parents;
  char         *cparents;
  char         *marriage;
  char         *switchname;
  char         *decapmessage;
  char         *tiemessage;
  char         *loginmessage;
  char         *logoutmessage;
  char         *avatarmessage;
  char         *last_global;
  time_t        last_note[MAX_BOARD];                                
  time_t        denied;
  int           rel_powers;
  sh_int        religion;
  sh_int        relrank;
  sh_int        current_faith;
  sh_int        faith;
  sh_int        revision;
  sh_int        alias_count;
  sh_int        vt100_size;
  sh_int        mean_paradox_counter;
  sh_int        safe_counter;
  sh_int        perm_str;
  sh_int        perm_int;
  sh_int        perm_wis;
  sh_int        perm_dex;   
  sh_int        perm_con;
  sh_int        mod_str;
  sh_int        mod_int; 
  sh_int        mod_wis;
  sh_int        mod_dex;   
  sh_int        mod_con;  
  sh_int        time_tick;
  sh_int        mortal;
  sh_int        stage[3]; 
  sh_int        condition[3];
  sh_int        learned[MAX_SKILL];
  sh_int        followers;
  int           betting_amount;
  int           betting_char;
  int           ignore[MAX_IGNORE];
  int           playerid;
  int           tempflag;
  int           damreduct;
  int           damreductdec;
  int           jflags;
  int           questsrun;
  int           questtotal;
  int           quest;
  int           pagelen;
  int           sit_safe;
  int           powers[20];
  sh_int              rank;
  int           score[6];
  int           genes[10];
  int           obj_vnum;
  int           awins;
  int           alosses;
  int           security;
  int           bounty;
  int           stats[12];
        int       songs[11];
        sh_int    kingdom;
	sh_int	  kingrank;

  int pc_eyes;
  int pc_hair;
  int pc_looks;
  int pc_build;

  

};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		19

struct liq_type
{
  char *liq_name;
  char *liq_color;
  sh_int liq_affect[3];
};

/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
  EXTRA_DESCR_DATA *next;       /* Next in list                     */
  EXTRA_DESCR_DATA *prev;       /* Previous           */
  char *keyword;                /* Keyword in look/examine          */
  char *description;            /* What to see                      */
  char *buffer1;                /* special action buffer 1          */
  char *buffer2;                /* special action buffer 2          */
  sh_int type;                  /* activation type                  */
  sh_int vnum;                  /* action related vnum              */
  sh_int action;                /* action type                      */
};

/*
 * Prototype for an object.
 */
struct obj_index_data
{
  OBJ_INDEX_DATA      * next;
  EXTRA_DESCR_DATA    * extra_descr;
  EXTRA_DESCR_DATA    * first_extradesc;
  EXTRA_DESCR_DATA    * last_extradesc;
  AFFECT_DATA         * first_affect;
  AFFECT_DATA         * last_affect;
  AFFECT_DATA         * affected;
  AREA_DATA           * area;
  char                * name;
  char                * short_descr;
  char                * description;
  int			material;
  int                   c_material;
  int		  	alloy;
  sh_int                item_type;
  sh_int                count;
  sh_int                weight;
  sh_int                weapflags;
  int                   vnum;
  int                   extra_flags;
  int                   wear_flags;
  int                   cost;
  int                   value[4];
};

/*
 * One object.
 */
struct obj_data
{
  OBJ_DATA            *  next;
  OBJ_DATA            *  next_content;
  OBJ_DATA            *  contains;
  OBJ_DATA            *  in_obj;
  CHAR_DATA           *  carried_by;
  CHAR_DATA           *  chobj;
  EXTRA_DESCR_DATA    *  extra_descr;
  EXTRA_DESCR_DATA    *  first_extradesc;
  EXTRA_DESCR_DATA    *  last_extradesc;
  AFFECT_DATA         *  first_affect;
  AFFECT_DATA         *  last_affect;
  AFFECT_DATA         *  affected;
  OBJ_INDEX_DATA      *  pIndexData;
  ROOM_INDEX_DATA     *  in_room;
  char                *  name;
  char                *  short_descr;
  char                *  description;
  char                *  questmaker;
  char                *  questowner;
  int			 material; // By Xrak
  int                    c_material; // By Xrak
  int			 alloy;
  sh_int                 points;
  int                    item_type;
  int                    ownerid;
  int                    extra_flags;
  int                    extra_flags2;
  int                    wear_flags;
  int                    wear_loc;
  int                    weight;
  int                    weapflags;
  int                    condition;
  int                    toughness;
  int                    resistance;
  int                    quest;
  int                    cost;
  int                    level;
  int                    timer;
  int                    value[4];
};

/*
 * Exit data.
 */
struct exit_data
{
  EXIT_DATA        * prev;          /* previous exit in linked list  */
  EXIT_DATA        * next;          /* next exit in linked list      */
  EXIT_DATA        * rexit;         /* Reverse exit pointer          */
  ROOM_INDEX_DATA  * to_room;       /* Pointer to destination room   */
  char             * keyword;       /* Keywords for exit or door     */
  char             * description;   /* Description of exit           */
  int                vnum;          /* Vnum of room exit leads to    */
  int                rvnum;         /* Vnum of room in opposite dir  */
  int                exit_info;     /* door states & other flags     */
  int                key;           /* Key vnum                      */
  sh_int             vdir;          /* 0,5 N\E\S\W\U\D shit          */
  int                rs_flags;      /* OLC                           */
  int                orig_door;     /* OLC                           */
  bool               color;         /* pathfinding                   */
};

/*
 * Room text checking data.
 */
typedef struct roomtext_data
{
  int type;
  int power;
  int mob;
  char *input;
  char *output;
  char *choutput;
  char *name;
  struct roomtext_data *next;
}
ROOMTEXT_DATA;

/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
  RESET_DATA *next;
  char command;
  int arg1;
  int arg2;
  int arg3;
};

/*
 * Area definition.
 */
struct area_data
{
  AREA_DATA    *  next;
  AREA_AFFECT  *  affects;
  char         *  name;
  char         *  filename;
  char         *  builders;
  char         *  music;
  sh_int          age;
  int             security;
  int             lvnum;
  int             uvnum;
  int             vnum;
  int             cvnum;
  int             area_flags;
  int             areabits;
};

/*
 * Room type.
 */
struct room_index_data
{
  ROOM_INDEX_DATA   * next;
  ROOM_INDEX_DATA   * next_room;
  ROOM_INDEX_DATA   *   next_update;   // Shinobi
  CHAR_DATA         * people;
  CHAR_DATA         * first_person;
  CHAR_DATA         * last_person;
  OBJ_DATA          * contents;
  OBJ_DATA          * first_content;
  OBJ_DATA          * last_content;
  EXTRA_DESCR_DATA  * extra_descr;
  EXTRA_DESCR_DATA  * first_extradesc;
  EXTRA_DESCR_DATA  * last_extradesc;
  AREA_DATA         * area;
  EXIT_DATA         * exit[6];
  EXIT_DATA         * first_exit;
  EXIT_DATA         * last_exit;
  ROOMTEXT_DATA     * roomtext;
  RESET_DATA        * reset_first;      /* OLC */
  RESET_DATA        * reset_last;       /* OLC */
  char              * track[5];
  char              * name;
  char              * description;
  int                 vnum;
  int                 room_flags;
  sh_int              light;
  sh_int              blood;
  sh_int              track_dir[5];
  sh_int              sector_type;
  sh_int              tick_timer[MAX_RTIMER];
  sh_int              heap_index;
  sh_int              steps;
  bool                visited;

    int			shinobi;
};

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000

/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4

#define TAR_OBJ_CHAR_DEF            5
#define TAR_OBJ_CHAR_OFF            6
#define TAR_OBJ_ROOM		    7
#define TAR_EXIT		    8
#define TAR_CHAR_WORLD		    9

#define TARGET_CHAR                 0
#define TARGET_OBJ                  1
#define TARGET_ROOM                 2
#define TARGET_NONE                 3

#define PURPLE_MAGIC		    0
#define RED_MAGIC		    1
#define BLUE_MAGIC		    2
#define GREEN_MAGIC		    3
#define YELLOW_MAGIC		    4

/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
  char *name;                   /* Name of skill    */
  sh_int skill_level;           /* Level needed by class  */
  SPELL_FUN *spell_fun;         /* Spell pointer (for spells) */
  sh_int target;                /* Legal targets    */
  sh_int minimum_position;      /* Position for caster / user */
  sh_int *pgsn;                 /* Pointer to associated gsn  */
  sh_int slot;                  /* Slot for #OBJECT loading */
  sh_int min_mana;              /* Minimum mana used    */
  sh_int beats;                 /* Waiting time after use */
  char *noun_damage;            /* Damage message   */
  char *msg_off;                /* Wear off message   */
};

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern sh_int gsn_thorns;
extern sh_int gsn_faith;
extern sh_int gsn_gfury;
extern sh_int gsn_blindness;
extern sh_int gsn_knee;
extern sh_int gsn_sweep;
extern sh_int gsn_charm_person;
extern sh_int gsn_curse;
extern sh_int gsn_invis;
extern sh_int gsn_mass_invis;
extern sh_int gsn_poison;
extern sh_int gsn_backstab;
extern sh_int gsn_garotte;
extern sh_int gsn_disarm;
extern sh_int gsn_hide;
extern sh_int gsn_hurl;
extern sh_int gsn_kick;
extern sh_int gsn_circle;
extern sh_int gsn_peek;
extern sh_int gsn_pick_lock;
extern sh_int gsn_rescue;
extern sh_int gsn_sneak;
extern sh_int gsn_steal;
extern sh_int gsn_punch;
extern sh_int gsn_elbow;
extern sh_int gsn_headbutt;
extern sh_int gsn_berserk;
extern sh_int gsn_track;
extern sh_int gsn_fireball;
extern sh_int gsn_lightning;
extern sh_int gsn_nova;
extern sh_int gsn_paradox;


extern sh_int gsn_requiem;
extern sh_int gsn_darktendrils;
extern sh_int gsn_drowfire;
extern sh_int gsn_tentacle;
extern sh_int gsn_quills;
extern sh_int gsn_fangs;
extern sh_int gsn_claws;
extern sh_int gsn_holyaura;
extern sh_int gsn_bash;

/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))

/* Ensure coord is on the map */
#define BOUNDARY(x, y) (((x) < 0) || ((y) < 0) || ((x) > MAPX) || ((y) > MAPY))

#define COLOUR(ch,color,bit)    (ADD_COLOUR((ch),(bit),(color));)

/*
 * Memory allocation macros.
 */

#define CREATE(result, type, number)				\
do								\
{								\
   if (!((result) = (type *) calloc ((number), sizeof(type))))	\
	{ perror("malloc failure"); abort(); }			\
} while(0)

#define RECREATE(result,type,number)				\
do								\
{								\
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
	{ perror("realloc failure"); abort(); }			\
} while(0)

#define DISPOSE(point) 						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer",0 ); \
	fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else free(point);						\
  point = NULL;							\
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	quick_link((point))
#define QUICKMATCH(p1, p2)	(int) (p1) == (int) (p2)
#define STRFREE(point)						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer",0 );	 		\
  }								\
  else if (str_free((point))==-1) 				\
    fprintf( stderr, "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
} while(0)
#else
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	str_dup((point))
#define QUICKMATCH(p1, p2)	strcmp((p1), (p2)) == 0
#define STRFREE(point)						\
do								\
{								\
  if (point == NULL)						\
  {								\
	bug( "Freeing null pointer",0 );		        \
  }								\
  else free_string((point));					\
} while(0)
#endif

/* double-linked list handling macros -Thoric */

double fread_double( FILE *fp );

#define LINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(first) )						\
      (first)			= (link);			\
    else							\
      (last)->next		= (link);			\
    (link)->next		= NULL;				\
    (link)->prev		= (last);			\
    (last)			= (link);			\
} while(0)

#define INSERT(link, insert, first, next, prev)			\
do								\
{								\
    (link)->prev		= (insert)->prev;		\
    if ( !(insert)->prev )					\
      (first)			= (link);			\
    else							\
      (insert)->prev->next	= (link);			\
    (insert)->prev		= (link);			\
    (link)->next		= (insert);			\
} while(0)

#define UNLINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(link)->prev )					\
      (first)			= (link)->next;			\
    else							\
      (link)->prev->next	= (link)->next;			\
    if ( !(link)->next )					\
      (last)			= (link)->prev;			\
    else							\
      (link)->next->prev	= (link)->prev;			\
} while(0)

#define CHECK_LINKS(first, last, next, prev, type)		\
do {								\
  type *ptr, *pptr = NULL;					\
  if ( !(first) && !(last) )					\
    break;							\
  if ( !(first) )						\
  {								\
    bug( "CHECK_LINKS: last with NULL first!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (last); ptr->prev; ptr = ptr->prev );		\
    (first) = ptr;						\
  }								\
  else if ( !(last) )						\
  {								\
    bug( "CHECK_LINKS: first with NULL last!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (first); ptr->next; ptr = ptr->next );		\
    (last) = ptr;						\
  }								\
  if ( (first) )						\
  {								\
    for ( ptr = (first); ptr; ptr = ptr->next )			\
    {								\
      if ( ptr->prev != pptr )					\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->prev = pptr;					\
      }								\
      if ( ptr->prev && ptr->prev->next != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev->next != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->prev->next = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
    pptr = NULL;						\
  }								\
  if ( (last) )							\
  {								\
    for ( ptr = (last); ptr; ptr = ptr->prev )			\
    {								\
      if ( ptr->next != pptr )					\
      {								\
        bug( "CHECK_LINKS (%s): %p:->next != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->next = pptr;					\
      }								\
      if ( ptr->next && ptr->next->prev != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->next->prev != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->next->prev = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
  }								\
} while(0)

#define ASSIGN_GSN(gsn, skill)					\
do								\
{								\
    if ( ((gsn) = skill_lookup((skill))) == -1 )		\
	fprintf( stderr, "ASSIGN_GSN: Skill %s not found.\n",	\
		(skill) );					\
} while(0)

#define CHECK_SUBRESTRICTED(ch)					\
do								\
{								\
    if ( (ch)->substate == SUB_RESTRICTED )			\
    {								\
	send_to_char( "You cannot use this command from within another command.\n\r", ch );	\
	return;							\
    }								\
} while(0)

/*
 * Character macros.
 */

#define IS_CREATOR(ch)		(get_trust(ch) >= MAX_LEVEL)
#define GET_PROPER_NAME(ch)     (IS_NPC((ch)) ? (ch)->short_descr : (ch)->pcdata->switchname)
#define GET_PC_NAME(ch)     	(IS_NPC((ch)) ? "<npc>" : (ch)->pcdata->switchname)

#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_JUDGE(ch)		(get_trust(ch) >= LEVEL_JUDGE)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)

#define IS_DAY()	    	((weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT))
#define IS_NIGHT()	    	((!IS_DAY()))

#define CAN_PK(ch)		(get_trust(ch)>= 3 && get_trust(ch)<= 12)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected_by2, (sn)))
#define IS_AFF2(ch, sn)		(IS_SET((ch)->affected_by2, (sn)))
#define IS_SPEAKING(ch, sn)	(IS_SET((ch)->pcdata->language[0], (sn)))
#define CAN_SPEAK(ch, sn)	(IS_SET((ch)->pcdata->language[1], (sn)))
#define IS_ITEMAFF(ch, sn)	(IS_SET((ch)->itemaffect, (sn)))
#define IS_IMMUNE(ch, sn)	(IS_SET((ch)->immune, (sn)))
#define IS_FORM(ch, sn)		(IS_SET((ch)->form, (sn)))
#define IS_POLYAFF(ch, sn)	(IS_SET((ch)->polyaff, (sn)))
#define IS_EXTRA(ch, sn)	(IS_SET((ch)->extra, (sn)))
#define IS_STANCE(ch, sn)	(ch->stance[0] == sn)
#define IS_DEMPOWER(ch, sn)	(IS_SET((ch)->pcdata->powers[DPOWER_FLAGS], (sn)))
#define IS_DEMAFF(ch, sn)	(IS_SET((ch)->pcdata->powers[DPOWER_CURRENT], (sn)))
#define IS_CLASS(ch, CLASS)	(IS_SET((ch)->class, CLASS) && (ch->level >= LEVEL_AVATAR))
#define IS_HEAD(ch, sn)		(IS_SET((ch)->loc_hp[0], (sn)))
#define IS_BODY(ch, sn)		(IS_SET((ch)->loc_hp[1], (sn)))
#define IS_ARM_L(ch, sn)	(IS_SET((ch)->loc_hp[2], (sn)))
#define IS_ARM_R(ch, sn)	(IS_SET((ch)->loc_hp[3], (sn)))
//#define IS_ARM_T(ch, sn)  (IS_SET((ch)->loc_hp[7], (sn)))
//#define IS_ARM_F(ch, sn)  (IS_SET((ch)->loc_hp[8], (sn)))
#define IS_LEG_L(ch, sn)	(IS_SET((ch)->loc_hp[4], (sn)))
#define IS_LEG_R(ch, sn)	(IS_SET((ch)->loc_hp[5], (sn)))
#define IS_BLEEDING(ch, sn)	(IS_SET((ch)->loc_hp[6], (sn)))

#define IS_PLAYING( d )         (d->connected==CON_PLAYING)

#define IS_GOOD(ch)		(ch->alignment >= 1)
#define IS_EVIL(ch)		(ch->alignment <= -1)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armor				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	\
if (!IS_CREATOR((ch))) \
((ch)->wait = UMAX((ch)->wait, (npulse)))

// #define IS_SPHERE(ch, SPHERE)     ((ch)->pcsphere == SPHERE && (ch->level >= LEVEL_AVATAR))
#define IS_RACE(ch, RACE)       ((ch)->pcRace == RACE && (ch->level >= LEVEL_AVATAR))
#define IS_SPHERE(ch, SPHERE)	(IS_SET((ch)->pcsphere, SPHERE) && (ch->level >= LEVEL_AVATAR))
#define IS_PROF(ch, PROF)	((ch)->profession == PROF && (ch->level >= LEVEL_AVATAR))
#define IS_GUILD(ch, GUILD)     ((ch)->guild == GUILD && (ch->level >= LEVEL_AVATAR))
#define IS_VAMPAFF(ch, sn)	(IS_SET((ch)->pcdata->stats[UNI_AFF], (sn)))
#define IS_IMMUNE2(ch, sn)	(IS_SET((ch)->immune2, (sn)))
#define IS_RES(ch, sn)	        (IS_SET((ch)->res, (sn)))
#define IS_VULN(ch, sn)	        (IS_SET((ch)->vuln, (sn)))
/*
 * Object Macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_OBJ_STAT2(obj, stat) (IS_SET((obj)->extra_flags2,(stat)))
#define IS_WEAP(obj, stat)      (IS_SET((obj)->weapflags,  (stat)))

/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
                                : ( IS_AFFECTED( (ch), AFF_POLYMORPH) ? \
				(ch)->morph : (ch)->name ) )		\
				: "someone" )

/*
 * J.O.P.E.
 */
struct jope_type
{
  char *const name;
  DO_FUN *do_fun;
  sh_int level;
};



/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
  char   * const name;
  DO_FUN * do_fun;
  sh_int   position;
  sh_int   level;
  sh_int   log;
  int      race;
  sh_int   powertype;
  sh_int   powerlevel;
};

/*
 * Structure for a social in the socials table.
 */
struct social_type
{
  char * name;
  char * char_no_arg;
  char * others_no_arg;
  char * char_found;
  char * others_found;
  char * vict_found;
  char * char_auto;
  char * others_auto;
};

struct material
{
   char *name;
   sh_int number[MAX_MATERIAL];
   sh_int viable[MAX_MATERIAL];
};

struct pc_eyes
{
   char pc_eyes[40];
};

struct pc_looks
{
   char pc_looks[40];
};

struct pc_build
{
   char pc_build[40];
};

struct pc_hair
{ 
   char pc_hair[40];
};

struct pc_race_type  /* additional data for pc races */
{
    char *	name;			/* MUST be in race_type */
    char 	who_name[30];
    sh_int	points;			/* cost in points of the race */
    sh_int	class_mult[MAX_CLASS];	/* exp multiplier for class, * 100 */
    char *	skills[5];		/* bonus skills for the race */
    sh_int 	stats[MAX_STATS];	/* starting stats */
    sh_int	max_stats[MAX_STATS];	/* maximum stats */
    sh_int	size;			/* aff bits for the race */
};


struct race_type
{
    char *	name;			/* call name of the race */
    bool	pc_race;		/* can be chosen by pcs */
    long	act;			/* act bits for the race */
    long	aff;			/* aff bits for the race */
    long	off;			/* off bits for the race */
    long	imm;			/* imm bits for the race */
    long        res;			/* res bits for the race */
    long	vuln;			/* vuln bits for the race */
    long	form;			/* default form flag for the race */
    long	parts;			/* default parts for the race */
};


/* size */
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5



/*
 * Structure for an X-social in the socials table.
 */
struct xsocial_type
{
  char *const name;
  char *const char_no_arg;
  char *const others_no_arg;
  char *const char_found;
  char *const others_found;
  char *const vict_found;
  char *const char_auto;
  char *const others_auto;
  sh_int gender;
  sh_int stage;
  sh_int position;
  sh_int self;
  sh_int other;
  sh_int extra;
  bool chance;
};

/*
 * Global constants.
 */

extern const struct str_app_type str_app[36];
extern const struct int_app_type int_app[36];
extern const struct wis_app_type wis_app[36];
extern const struct dex_app_type dex_app[36];
extern const struct con_app_type con_app[36];

extern const struct jope_type jope_table[];
extern const struct cmd_type cmd_table[];
extern const struct liq_type liq_table[LIQ_MAX];
extern const struct skill_type skill_table[MAX_SKILL];
extern const struct xsocial_type xsocial_table[];


/* social table */
extern struct social_type *social_table;  


/*
 * Global variables.
 */
extern   HELP_DATA         *  first_help;
extern   HELP_DATA         *  last_help;
extern   BAN_DATA          *  ban_list;
extern   BAN_DATA          *  ban_free;
extern   QUEST_DATA        *  quest_free;
extern   HISTORY_DATA      *  history_free;
extern   BAN_DATA          *  newbieban_list;
extern   BAN_DATA          *  newbieban_free;
extern   CHAR_DATA         *  char_list;
extern   DESCRIPTOR_DATA   *  descriptor_list;
extern   NOTE_DATA         *  note_list;
extern   OBJ_DATA          *  object_list;
extern          ROOM_INDEX_DATA  *     update_room_list;
extern   ROOM_INDEX_DATA   *  room_list;
extern   AFFECT_DATA       *  affect_free;
extern   AFFECT2_DATA      *  affect2_free;
extern   DUMMY_ARG         *  dummy_free;
extern   DUMMY_ARG         *  dummy_list;
extern   ALIAS_DATA        *  alias_free;
extern   POLL_DATA         *  poll_list;
extern   CHAR_DATA         *  char_free;
extern   DESCRIPTOR_DATA   *  descriptor_free;
extern   EXTRA_DESCR_DATA  *  extra_descr_free;
extern   ROOMTEXT_DATA     *  roomtext_free;
extern   NOTE_DATA         *  note_free;
extern   OBJ_DATA          *  obj_free;
extern   PC_DATA           *  pcdata_free;
extern   CHANGE_DATA       *  change_list;
extern   CHANGE_DATA       *  change_last;
extern   CHANGE_DATA       *  change_free;
extern   AREA_AFFECT       *  area_affect_free;

extern   char bug_buf[];
extern   time_t current_time;
extern   bool fLogAll;
extern   FILE *fpReserve;
extern   KILL_DATA kill_table[];
extern   char log_buf[];
extern   TIME_INFO_DATA time_info;
extern   WEATHER_DATA weather_info;
extern   bool arena2;
extern   bool arena2death;
extern   bool arena_open;
extern   bool arena_base;
extern   bool extra_log;
extern   bool cmd_done;
extern   int players_logged;
extern   int players_decap;
extern   int players_gstolen;
extern   int thread_count;
extern   int iDelete;
extern   bool arenaopen;
extern   bool ragnarok;
extern   int ragnarok_timer;
extern   int ragnarok_cost;
extern   int pulse_arena;
extern   int total_output;
extern   int mudinfo[MUDINFO_MAX];
extern   int ccenter[CCENTER_MAX];
extern   int top_playerid;
extern   pthread_mutex_t memory_mutex;


extern bool                    double_exp;
extern bool                    double_fightstance;
extern bool                    double_coins;


void add_affect args ( ( OBJ_DATA * obj, int apply, long value ) );


// Xrakisis
DECLARE_DO_FUN( do_smith		);
DECLARE_DO_FUN( do_mix			);
DECLARE_DO_FUN( do_alloys		);
DECLARE_DO_FUN( do_forge		);
DECLARE_DO_FUN( do_materials		);
DECLARE_DO_FUN( do_destroy		);
DECLARE_DO_FUN( do_calc			);
DECLARE_DO_FUN( do_toggle		);
DECLARE_DO_FUN( do_advance		);
DECLARE_DO_FUN( do_gainlevel		);
DECLARE_DO_FUN( do_resetsphere		);
DECLARE_DO_FUN( do_resetspheres		);
DECLARE_DO_FUN( do_selfsphere		);
DECLARE_DO_FUN( do_newbiepack		);
DECLARE_DO_FUN( do_proficiencies	);
DECLARE_DO_FUN(do_ascension);
DECLARE_DO_FUN(do_raceself);
DECLARE_DO_FUN( do_convert		);
DECLARE_DO_FUN( do_fightstyle		);
DECLARE_DO_FUN( do_sell			);
DECLARE_DO_FUN( do_powers		);
DECLARE_DO_FUN( do_remort		);
DECLARE_DO_FUN( do_wizlist		);
DECLARE_DO_FUN(do_jutsus);


// Race Powers -xrak
DECLARE_DO_FUN(do_racepowers);
DECLARE_DO_FUN(do_thunderhammer);
DECLARE_DO_FUN(do_gore);
DECLARE_DO_FUN(do_jawlock);
DECLARE_DO_FUN(do_mistform);
DECLARE_DO_FUN(do_flameblade);
DECLARE_DO_FUN(do_goblingrenade);
DECLARE_DO_FUN(do_airshiptravel);
DECLARE_DO_FUN(do_elveninfra);
DECLARE_DO_FUN(do_songs);
DECLARE_DO_FUN(do_elfsing);
DECLARE_DO_FUN(do_lessons);
DECLARE_DO_FUN(do_tremble);
DECLARE_DO_FUN(do_pirate);
DECLARE_DO_FUN(do_toughskin);
DECLARE_DO_FUN(do_fly);
DECLARE_DO_FUN(do_darkness);
DECLARE_DO_FUN(do_drowfire);
DECLARE_DO_FUN(do_demonform);
DECLARE_DO_FUN(do_gfavor);
DECLARE_DO_FUN(do_earthpunch);
DECLARE_DO_FUN(do_tunnel);
DECLARE_DO_FUN(do_chant);
DECLARE_DO_FUN(do_stoneshape);

DECLARE_DO_FUN( do_elemshift		);
DECLARE_DO_FUN( do_superberserk		);
DECLARE_DO_FUN( do_leech		);
DECLARE_DO_FUN( do_graft		);
DECLARE_DO_FUN( do_bladejump		);
DECLARE_DO_FUN( do_entomb		);
DECLARE_DO_FUN( do_weaponenhance	);
DECLARE_DO_FUN( do_armorenhance		);
DECLARE_DO_FUN( do_caust		);
DECLARE_DO_FUN( do_immolate		);
DECLARE_DO_FUN( do_wfreeze		);
DECLARE_DO_FUN( do_poisonblade		);
DECLARE_DO_FUN( do_heal			);
DECLARE_DO_FUN( do_regenerate		);
DECLARE_DO_FUN( do_regrow		);
DECLARE_DO_FUN( do_hydro		);
DECLARE_DO_FUN( do_illusionaryclone	);
DECLARE_DO_FUN( do_creepingdoom		);
DECLARE_DO_FUN( do_powerword		);
DECLARE_DO_FUN( do_flameblade		);
DECLARE_DO_FUN( do_roar			);
DECLARE_DO_FUN( do_quills		);
DECLARE_DO_FUN( do_talons		);
DECLARE_DO_FUN( do_flameclaws		);
DECLARE_DO_FUN( do_burrow		);
DECLARE_DO_FUN( do_rend			);
DECLARE_DO_FUN( do_fangs		);
DECLARE_DO_FUN( do_claws		);
DECLARE_DO_FUN( do_sclaws		);
DECLARE_DO_FUN( do_ironclaw		);
DECLARE_DO_FUN( do_kakusu		);
DECLARE_DO_FUN( do_bomuzite		);
DECLARE_DO_FUN( do_darksummon		);
DECLARE_DO_FUN(	do_readaura		);
DECLARE_DO_FUN( do_zuloform		);
DECLARE_DO_FUN( do_formillusion		);
DECLARE_DO_FUN( do_change		);
DECLARE_DO_FUN( do_holyaura		);
DECLARE_DO_FUN( do_shatter		);
DECLARE_DO_FUN( do_enrage		);
DECLARE_DO_FUN( do_dimedodge		);
DECLARE_DO_FUN( do_cbreath		);
DECLARE_DO_FUN( do_fleshportal		);

// Fortress
DECLARE_DO_FUN( do_garrison		);
DECLARE_DO_FUN( do_hire			);

// Guild
DECLARE_DO_FUN( do_raise		); // Dark Hand
DECLARE_DO_FUN( do_perfectsight		); // Dark Hand
DECLARE_DO_FUN( do_knightshield		); // Crusaders
DECLARE_DO_FUN( do_faeriecurse		); // Damned
DECLARE_DO_FUN( do_guardianbless	); // Guardian
DECLARE_DO_FUN( do_beastform		); // Beast
DECLARE_DO_FUN( do_purification		); // Beast
DECLARE_DO_FUN( do_vineforge		); // Druid
DECLARE_DO_FUN( do_grecall		); // All

// Prof
DECLARE_DO_FUN( do_bash			); // Knight
DECLARE_DO_FUN( do_layonhands		); // Knight
DECLARE_DO_FUN( do_rage			); // Barbarian
DECLARE_DO_FUN( do_eyegouge		); // Barbarian
DECLARE_DO_FUN( do_dirtkick		); // Thief
DECLARE_DO_FUN( do_impale		); // Assassin
DECLARE_DO_FUN( do_sandman 		); // Assassin
DECLARE_DO_FUN( do_darkblaze		); // Monk
DECLARE_DO_FUN( do_blink		); // Soldier
DECLARE_DO_FUN( do_sharpen		); // Mercenary
DECLARE_DO_FUN( do_thorns		); // Cleric
DECLARE_DO_FUN( do_landscape		); // Cleric

DECLARE_DO_FUN( do_desanc		);
DECLARE_DO_FUN( do_solidify		);
DECLARE_DO_FUN( do_enflame		);
DECLARE_DO_FUN( do_magma		);
DECLARE_DO_FUN( do_naturesfury		);
DECLARE_DO_FUN( do_disperse		);
DECLARE_DO_FUN( do_enfold		);
DECLARE_DO_FUN( do_evaporate		);
DECLARE_DO_FUN( do_shouldercharge	);
DECLARE_DO_FUN( do_fbreath		);
DECLARE_DO_FUN( do_gbreath		);
DECLARE_DO_FUN( do_dragonform           );
DECLARE_DO_FUN( do_scry                 );
DECLARE_DO_FUN( do_chaossurge		);
DECLARE_DO_FUN( do_chaosgate		);
DECLARE_DO_FUN( do_chaosmagic		);
DECLARE_DO_FUN( do_chaosshield		);
DECLARE_DO_FUN( do_chaos		);
DECLARE_DO_FUN( do_spikes		);
DECLARE_DO_FUN( do_rockslide		);
DECLARE_DO_FUN( do_nightsight		);
DECLARE_DO_FUN( do_mask			);
DECLARE_DO_FUN( do_camouflage		);
DECLARE_DO_FUN( do_shield		);
DECLARE_DO_FUN( do_unnerve		);
DECLARE_DO_FUN( do_darktendrils		);
DECLARE_DO_FUN( do_fightdance		);
DECLARE_DO_FUN( do_earthshatter		);
DECLARE_DO_FUN( do_bonemod		);
DECLARE_DO_FUN( do_dtrick		);
DECLARE_DO_FUN( do_gleam		);
DECLARE_DO_FUN( do_earthswallow		);
DECLARE_DO_FUN( do_moonbeam		);
DECLARE_DO_FUN( do_grab			);
DECLARE_DO_FUN( do_moongate		);
DECLARE_DO_FUN( do_reveal		);
DECLARE_DO_FUN( do_shadowsight 		);
DECLARE_DO_FUN( do_shadowplane		);
DECLARE_DO_FUN( do_frost		);
DECLARE_DO_FUN( do_skin			);

// Religions, Zarius
DECLARE_DO_FUN( do_settie	);
DECLARE_DO_FUN( do_setlogout    );
DECLARE_DO_FUN( do_setlogin     );
DECLARE_DO_FUN( do_setdecap     );
DECLARE_DO_FUN( do_setavatar    );
DECLARE_DO_FUN( do_worship      );
DECLARE_DO_FUN( do_forsake	);
DECLARE_DO_FUN( do_setrank      );
DECLARE_DO_FUN( do_relbuy	);
DECLARE_DO_FUN( do_relset	);
DECLARE_DO_FUN( do_gzone	);
DECLARE_DO_FUN( do_darmor	);
DECLARE_DO_FUN( do_farmor	);
DECLARE_DO_FUN( do_holyword	);
DECLARE_DO_FUN( do_areaaffects	);
DECLARE_DO_FUN( do_rpurge	);
DECLARE_DO_FUN( do_deja		);
DECLARE_DO_FUN( do_holysymbol	);
DECLARE_DO_FUN( do_areaheal	);
DECLARE_DO_FUN( do_badmoon	);
DECLARE_DO_FUN( do_relpowers	);
DECLARE_DO_FUN( do_hdesire	);
DECLARE_DO_FUN( do_dscry	);
DECLARE_DO_FUN( do_pestilence	);
DECLARE_DO_FUN( do_banishment	);
DECLARE_DO_FUN( do_starsight	);
DECLARE_DO_FUN( do_gaia		);
DECLARE_DO_FUN( do_dbattle	);
DECLARE_DO_FUN( do_callwild	);
DECLARE_DO_FUN( do_madness	);
DECLARE_DO_FUN( do_gfury	);
DECLARE_DO_FUN( do_subvert	);
DECLARE_DO_FUN( do_rebirth      );
DECLARE_DO_FUN( do_supernova	);


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_affects		);
DECLARE_DO_FUN( do_resetpassword	);
DECLARE_DO_FUN( do_pstat		);
DECLARE_DO_FUN( do_sedit		);
DECLARE_DO_FUN( do_history		);
DECLARE_DO_FUN( do_showsilence		);
DECLARE_DO_FUN( do_pathfind		);
DECLARE_DO_FUN( do_showcompress		);
DECLARE_DO_FUN( do_ccenter		);
DECLARE_DO_FUN( do_openthearena		);
DECLARE_DO_FUN( do_ragnarok		);
DECLARE_DO_FUN( do_timer		);
DECLARE_DO_FUN( do_superlog		);
DECLARE_DO_FUN( do_selfdelete		);
DECLARE_DO_FUN( do_omni			);
DECLARE_DO_FUN( do_tick			);
DECLARE_DO_FUN( do_resetarea		);
DECLARE_DO_FUN( do_afk			);
DECLARE_DO_FUN( do_configure		);
DECLARE_DO_FUN( do_vt100		);
DECLARE_DO_FUN( do_prefix		);
DECLARE_DO_FUN( do_sound		);
DECLARE_DO_FUN( do_noset		);

/* OLC */
DECLARE_DO_FUN( do_hlist		);
DECLARE_DO_FUN( do_hedit		);
DECLARE_DO_FUN( do_hset			);
DECLARE_DO_FUN( do_aedit 	       );
DECLARE_DO_FUN( do_redit        	);
DECLARE_DO_FUN( do_oedit        	);
DECLARE_DO_FUN( do_medit        	);
DECLARE_DO_FUN( do_asave        	);
DECLARE_DO_FUN( do_alist        	);
DECLARE_DO_FUN( do_resets       	);

/* More commands */
DECLARE_DO_FUN( do_accept		);
DECLARE_DO_FUN( do_alignment		);
DECLARE_DO_FUN( do_alias		);
DECLARE_DO_FUN( do_showalias		);
DECLARE_DO_FUN( do_removealias		);
DECLARE_DO_FUN( do_createbase		);
DECLARE_DO_FUN( do_showquest		);
DECLARE_DO_FUN( do_qgain		);
DECLARE_DO_FUN( do_qcomplete		);
DECLARE_DO_FUN( do_trigger		);
DECLARE_DO_FUN( do_allow		);
DECLARE_DO_FUN( do_bounty		);
DECLARE_DO_FUN( do_bountylist		);
DECLARE_DO_FUN( do_ansi			);
DECLARE_DO_FUN( do_areas		);
DECLARE_DO_FUN( do_artifact		);
DECLARE_DO_FUN( do_at			);
DECLARE_DO_FUN( do_autoexit		);
DECLARE_DO_FUN( do_autoloot		);
DECLARE_DO_FUN( do_autosac		);
DECLARE_DO_FUN( do_autosave		);
DECLARE_DO_FUN( do_autostance		);
DECLARE_DO_FUN( do_backstab		);
DECLARE_DO_FUN( do_bamfin		);
DECLARE_DO_FUN( do_bamfout		);
DECLARE_DO_FUN( do_ban			);
DECLARE_DO_FUN( do_berserk		);
DECLARE_DO_FUN( do_berserk2		);
DECLARE_DO_FUN( do_bind			);
DECLARE_DO_FUN( do_birth		);
DECLARE_DO_FUN( do_blindfold		);
DECLARE_DO_FUN( do_brandish		);
DECLARE_DO_FUN( do_breakup		);
DECLARE_DO_FUN( do_map			);
DECLARE_DO_FUN( do_brief		);
DECLARE_DO_FUN( do_brief1		);
DECLARE_DO_FUN( do_brief2		);
DECLARE_DO_FUN( do_brief3		);
DECLARE_DO_FUN( do_brief4		);
DECLARE_DO_FUN( do_brief5		);
DECLARE_DO_FUN( do_brief6		);
DECLARE_DO_FUN( do_call			);
DECLARE_DO_FUN( do_cast			);
DECLARE_DO_FUN( do_changelight		);
DECLARE_DO_FUN( do_channels		);
DECLARE_DO_FUN( do_compress		);
DECLARE_DO_FUN( do_compres		);
DECLARE_DO_FUN( do_chat			);
DECLARE_DO_FUN( do_roleplay		);
DECLARE_DO_FUN( do_claim		);
DECLARE_DO_FUN( do_flame		);
DECLARE_DO_FUN( do_fortressstats	);
DECLARE_DO_FUN( do_arenastats		);
DECLARE_DO_FUN( do_arenajoin		);
DECLARE_DO_FUN( do_resign		);
DECLARE_DO_FUN( do_challenge		);
DECLARE_DO_FUN( do_bet			);
DECLARE_DO_FUN( do_decline		);
DECLARE_DO_FUN( do_accept2		);
DECLARE_DO_FUN( do_newban		);
DECLARE_DO_FUN( do_newallow		);
DECLARE_DO_FUN( do_addchange		);
DECLARE_DO_FUN( do_changes		);
DECLARE_DO_FUN( do_delchange		);
DECLARE_DO_FUN( do_displayvotes		);
DECLARE_DO_FUN( do_vote			);
DECLARE_DO_FUN( do_polledit		);
DECLARE_DO_FUN( do_addpoll		);
DECLARE_DO_FUN( do_stoppoll		);
DECLARE_DO_FUN( do_arenaset		);
DECLARE_DO_FUN( do_leader		);
DECLARE_DO_FUN( do_leaderclear		);
DECLARE_DO_FUN( do_clearstats		);
DECLARE_DO_FUN( do_clearstats2		);
DECLARE_DO_FUN( do_close		);
DECLARE_DO_FUN( do_commands		);
DECLARE_DO_FUN( do_config		);
DECLARE_DO_FUN( do_consent		);
DECLARE_DO_FUN( do_consider		);
DECLARE_DO_FUN( do_cprompt		);
DECLARE_DO_FUN( do_crack		);
DECLARE_DO_FUN( do_create		);
DECLARE_DO_FUN( do_credits		);
DECLARE_DO_FUN( do_dcredits		);
DECLARE_DO_FUN( do_decapitate		);
DECLARE_DO_FUN( do_deny			);
DECLARE_DO_FUN( do_description		);
DECLARE_DO_FUN( do_diagnose		);
DECLARE_DO_FUN( do_dismount		);
DECLARE_DO_FUN( do_disable		);
DECLARE_DO_FUN( do_disarm		);
DECLARE_DO_FUN( do_disconnect		);
DECLARE_DO_FUN( do_divorce		);
DECLARE_DO_FUN( do_down			);
DECLARE_DO_FUN( do_draw			);
DECLARE_DO_FUN( do_drink		);
DECLARE_DO_FUN( do_drop			);
DECLARE_DO_FUN( do_settie		);
DECLARE_DO_FUN( do_setlogout		);
DECLARE_DO_FUN( do_setlogin		);
DECLARE_DO_FUN( do_setdecap		);
DECLARE_DO_FUN( do_setavatar		);
DECLARE_DO_FUN( do_east			);
DECLARE_DO_FUN( do_eat			);
DECLARE_DO_FUN( do_echo			);
DECLARE_DO_FUN( do_empty		);
DECLARE_DO_FUN( do_escape		);
DECLARE_DO_FUN( do_emote		);
DECLARE_DO_FUN( do_enter		);
DECLARE_DO_FUN( do_equipment		);
DECLARE_DO_FUN( do_examine		);
DECLARE_DO_FUN( do_exits		);
DECLARE_DO_FUN( do_exlist		);
DECLARE_DO_FUN( do_favor		);
DECLARE_DO_FUN( do_fill			);
DECLARE_DO_FUN( do_finger		);
DECLARE_DO_FUN( do_flee			);
DECLARE_DO_FUN( do_flex			);
DECLARE_DO_FUN( do_follow		);
DECLARE_DO_FUN( do_force		);
DECLARE_DO_FUN( do_asperson		);
DECLARE_DO_FUN( do_offline		);
DECLARE_DO_FUN( do_forceauto		);
DECLARE_DO_FUN( do_freeze		);
DECLARE_DO_FUN( do_gag			);
DECLARE_DO_FUN( do_get			);
DECLARE_DO_FUN( do_generation		);
DECLARE_DO_FUN( do_gift			);
DECLARE_DO_FUN( do_give			);
DECLARE_DO_FUN( do_goto			);
DECLARE_DO_FUN( do_group		);
DECLARE_DO_FUN( do_gtell		);
DECLARE_DO_FUN( do_help			);
DECLARE_DO_FUN( do_hide			);
DECLARE_DO_FUN( do_holylight		);
DECLARE_DO_FUN( do_home			);
DECLARE_DO_FUN( do_huh			);
DECLARE_DO_FUN( do_hunt			);
DECLARE_DO_FUN( do_hurl			);
DECLARE_DO_FUN( do_idea			);
DECLARE_DO_FUN( do_immune		);
DECLARE_DO_FUN( do_immtalk		);
DECLARE_DO_FUN( do_info			);
DECLARE_DO_FUN( do_inventory		);
DECLARE_DO_FUN( do_invis		);
DECLARE_DO_FUN( do_kick			);
DECLARE_DO_FUN( do_kill			);
DECLARE_DO_FUN( do_combatswitch		);
DECLARE_DO_FUN( do_locate		);
DECLARE_DO_FUN( do_lock			);
DECLARE_DO_FUN( do_log			);
DECLARE_DO_FUN( do_logstat		);
DECLARE_DO_FUN( do_logstatclear		);
DECLARE_DO_FUN( do_look			);
DECLARE_DO_FUN( do_marry		);
DECLARE_DO_FUN( do_memory		);
DECLARE_DO_FUN( do_mfind		);
DECLARE_DO_FUN( do_mload		);
DECLARE_DO_FUN( do_mount		);
DECLARE_DO_FUN( do_mset			);
DECLARE_DO_FUN( do_undeny		);
DECLARE_DO_FUN( do_mstat		);
DECLARE_DO_FUN( do_mwhere		);
DECLARE_DO_FUN( do_music		);
DECLARE_DO_FUN( do_sing			);
DECLARE_DO_FUN( do_north		);
DECLARE_DO_FUN( do_note			);
DECLARE_DO_FUN( do_oclone		);
DECLARE_DO_FUN( do_ofind		);
DECLARE_DO_FUN( do_oload		);
DECLARE_DO_FUN( do_open			);
DECLARE_DO_FUN( do_order		);
DECLARE_DO_FUN( do_reimb		);
DECLARE_DO_FUN( do_oset			);
DECLARE_DO_FUN( do_ostat		);
DECLARE_DO_FUN( do_otransfer		);
DECLARE_DO_FUN( do_paradox		);
DECLARE_DO_FUN( do_bully		);
DECLARE_DO_FUN( do_password		);
DECLARE_DO_FUN( do_peace		);
DECLARE_DO_FUN( do_pick			);
DECLARE_DO_FUN( do_pload		);
DECLARE_DO_FUN( do_practice		);
DECLARE_DO_FUN( do_preturn		);
DECLARE_DO_FUN( do_prompt		);
DECLARE_DO_FUN( do_propose		);
DECLARE_DO_FUN( do_pset			);
DECLARE_DO_FUN( do_punch		);
DECLARE_DO_FUN( do_purge		);
DECLARE_DO_FUN( do_put			);
DECLARE_DO_FUN( do_quaff		);
DECLARE_DO_FUN( do_shop			);
DECLARE_DO_FUN( do_pull			);
DECLARE_DO_FUN( do_press		);
DECLARE_DO_FUN( do_touch		);
DECLARE_DO_FUN( do_push			);
DECLARE_DO_FUN( do_list			);
DECLARE_DO_FUN( do_buy			);
DECLARE_DO_FUN( do_qui			);
DECLARE_DO_FUN( do_quit			);
DECLARE_DO_FUN( do_policy		);
DECLARE_DO_FUN( do_ignore		);
DECLARE_DO_FUN( do_copyover		);
DECLARE_DO_FUN( do_cprime		); // By Hutoshi
DECLARE_DO_FUN( do_recall		);
DECLARE_DO_FUN( do_recho		);
DECLARE_DO_FUN( do_recite		);
DECLARE_DO_FUN( do_release		);
DECLARE_DO_FUN( do_relevel		);
DECLARE_DO_FUN( do_remove		);
DECLARE_DO_FUN( do_rent			);
DECLARE_DO_FUN( do_reply		);
DECLARE_DO_FUN( do_report		);
DECLARE_DO_FUN( do_rescue		);
DECLARE_DO_FUN( do_rest			);
DECLARE_DO_FUN( do_restore		);
DECLARE_DO_FUN( do_return		);
DECLARE_DO_FUN( do_rset			);
DECLARE_DO_FUN( do_rstat		);
DECLARE_DO_FUN( do_sacrifice		);
DECLARE_DO_FUN( do_safe			);
DECLARE_DO_FUN( do_save			);
DECLARE_DO_FUN( do_say			);
DECLARE_DO_FUN( do_scan			);
DECLARE_DO_FUN( do_stat			);
DECLARE_DO_FUN( do_score		);
DECLARE_DO_FUN( do_level		);
DECLARE_DO_FUN( do_mastery		);
DECLARE_DO_FUN( do_exp			);
DECLARE_DO_FUN( do_healme		);
DECLARE_DO_FUN( do_pkpowers		);
DECLARE_DO_FUN( do_gensteal		);
DECLARE_DO_FUN( do_setstance		);
DECLARE_DO_FUN( do_artisteal		);
DECLARE_DO_FUN( do_multicheck		);

/*
 * J.O.P.E. (these are not all in interp.c)
 */
DECLARE_DO_FUN( do_pfile	);
DECLARE_DO_FUN( jope_done	);
DECLARE_DO_FUN( jope_exp	);
DECLARE_DO_FUN( jope_show	);
DECLARE_DO_FUN( jope_list	);
DECLARE_DO_FUN( jope_spells	);
DECLARE_DO_FUN( jope_stances	);
DECLARE_DO_FUN( jope_weapons	);
DECLARE_DO_FUN( jope_action	);
DECLARE_DO_FUN( jope_inventory	);
DECLARE_DO_FUN( jope_drop	);
DECLARE_DO_FUN( jope_equipment	);
DECLARE_DO_FUN( jope_get	);
DECLARE_DO_FUN( jope_look	);
DECLARE_DO_FUN( jope_remove	);
DECLARE_DO_FUN( jope_wear	);
DECLARE_DO_FUN( jope_newbits	);
DECLARE_DO_FUN( jope_act	);
DECLARE_DO_FUN( jope_qps	);
DECLARE_DO_FUN( jope_primal	);
DECLARE_DO_FUN( jope_level	);
DECLARE_DO_FUN( jope_trust	);
DECLARE_DO_FUN( jope_hit	);
DECLARE_DO_FUN( jope_move	);
DECLARE_DO_FUN( jope_mana	);
DECLARE_DO_FUN( jope_pkill	);
DECLARE_DO_FUN( jope_pdeath	);
DECLARE_DO_FUN( jope_mkill	);
DECLARE_DO_FUN( jope_mdeath	);
DECLARE_DO_FUN( jope_fix	);
DECLARE_DO_FUN( jope_jflags	);
DECLARE_DO_FUN( jope_extra	);

/* More commands */
DECLARE_DO_FUN( do_pkscry		);
DECLARE_DO_FUN( do_pkmind		);
DECLARE_DO_FUN( do_pkcall		);
DECLARE_DO_FUN( do_pkheal		);
DECLARE_DO_FUN( do_pkaura		);
DECLARE_DO_FUN( do_pkobjscry		);
DECLARE_DO_FUN( do_sheath		);
DECLARE_DO_FUN( do_shutdow		);
DECLARE_DO_FUN( do_shutdown		);
DECLARE_DO_FUN( do_newbie		);
DECLARE_DO_FUN( do_silence		);
DECLARE_DO_FUN( do_sit			);
DECLARE_DO_FUN( do_skill		);
DECLARE_DO_FUN( do_sla			);
DECLARE_DO_FUN( do_slay			);
DECLARE_DO_FUN( do_sleep		);
DECLARE_DO_FUN( do_slookup		);
DECLARE_DO_FUN( do_spell		);
DECLARE_DO_FUN( do_stance		);
DECLARE_DO_FUN( do_smother		);
DECLARE_DO_FUN( do_sneak		);
DECLARE_DO_FUN( do_snoop		);
DECLARE_DO_FUN( do_socials		);
DECLARE_DO_FUN( do_south		);
DECLARE_DO_FUN( do_spy			);
DECLARE_DO_FUN( do_spydirection		);
DECLARE_DO_FUN( do_sset			);
DECLARE_DO_FUN( do_stand		);
DECLARE_DO_FUN( do_steal		);
DECLARE_DO_FUN( do_summon		);
DECLARE_DO_FUN( do_nosummon		);
DECLARE_DO_FUN( do_switch		);
DECLARE_DO_FUN( do_tell			);
DECLARE_DO_FUN( do_throw		);
DECLARE_DO_FUN( do_tie			);
DECLARE_DO_FUN( do_time			);
DECLARE_DO_FUN( do_title		);
DECLARE_DO_FUN( do_token		);
DECLARE_DO_FUN( do_track		);
DECLARE_DO_FUN( do_train		);
DECLARE_DO_FUN( do_transfer		);
DECLARE_DO_FUN( do_transport		);
DECLARE_DO_FUN( do_truesight		);
DECLARE_DO_FUN( do_trust		);
DECLARE_DO_FUN( do_typo			);
DECLARE_DO_FUN( do_unlock		);
DECLARE_DO_FUN( do_untie		);
DECLARE_DO_FUN( do_up			);
DECLARE_DO_FUN( do_upkeep		);
DECLARE_DO_FUN( do_users		);
DECLARE_DO_FUN( do_version		);
DECLARE_DO_FUN( do_visible		);
DECLARE_DO_FUN( do_wake			);
DECLARE_DO_FUN( do_wear			);
DECLARE_DO_FUN( do_west			);
DECLARE_DO_FUN( do_where		);
DECLARE_DO_FUN( do_whisper		);
DECLARE_DO_FUN( do_who			);
DECLARE_DO_FUN( do_wizhelp		);
DECLARE_DO_FUN( do_linkdead		);
DECLARE_DO_FUN( do_wizlist		);
DECLARE_DO_FUN( do_closemud		);
DECLARE_DO_FUN( do_wizlock		);
DECLARE_DO_FUN( do_xemote		);
DECLARE_DO_FUN( do_xsocials		);
DECLARE_DO_FUN( do_yell			);
DECLARE_DO_FUN( do_zap			);
DECLARE_DO_FUN( do_relearn		);

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(spell_drowfire);
DECLARE_SPELL_FUN(spell_holyaura);
DECLARE_SPELL_FUN( spell_null			);
DECLARE_SPELL_FUN( spell_acid_blast		);
DECLARE_SPELL_FUN( spell_armor			);
DECLARE_SPELL_FUN( spell_godbless		);
DECLARE_SPELL_FUN( spell_bless			);
DECLARE_SPELL_FUN( spell_blindness		);
DECLARE_SPELL_FUN( spell_charm_person		);
DECLARE_SPELL_FUN( spell_chill_touch		);
DECLARE_SPELL_FUN( spell_cure_blindness		);
DECLARE_SPELL_FUN( spell_cure_poison		);
DECLARE_SPELL_FUN( spell_curse			);
DECLARE_SPELL_FUN( spell_detect_evil		);
DECLARE_SPELL_FUN( spell_detect_hidden		);
DECLARE_SPELL_FUN( spell_detect_invis		);
DECLARE_SPELL_FUN( spell_detect_magic		);
DECLARE_SPELL_FUN( spell_detect_poison		);
DECLARE_SPELL_FUN( spell_dispel_evil		);
DECLARE_SPELL_FUN( spell_dispel_magic		);
DECLARE_SPELL_FUN( spell_earthquake		);
DECLARE_SPELL_FUN( spell_enchant_weapon		);
DECLARE_SPELL_FUN( spell_energy_drain		);
DECLARE_SPELL_FUN( spell_faerie_fire		);
DECLARE_SPELL_FUN( spell_faerie_fog		);
DECLARE_SPELL_FUN( spell_fireball		);
DECLARE_SPELL_FUN( spell_desanct		);
DECLARE_SPELL_FUN( spell_imp_heal		);
DECLARE_SPELL_FUN( spell_imp_fireball		);
DECLARE_SPELL_FUN( spell_imp_faerie_fire	);
DECLARE_SPELL_FUN( spell_imp_teleport		);
DECLARE_SPELL_FUN( spell_fly			);
DECLARE_SPELL_FUN( spell_giant_strength		);
DECLARE_SPELL_FUN( spell_harm			);
DECLARE_SPELL_FUN( spell_heal			);
DECLARE_SPELL_FUN( spell_heal2			);
DECLARE_SPELL_FUN( spell_group_heal		);
DECLARE_SPELL_FUN( spell_identify		);
DECLARE_SPELL_FUN( spell_readaura		);
DECLARE_SPELL_FUN( spell_infravision		);
DECLARE_SPELL_FUN( spell_invis			);
DECLARE_SPELL_FUN( spell_know_alignment		);
DECLARE_SPELL_FUN( spell_locate_object		);
DECLARE_SPELL_FUN( spell_magic_missile		);
DECLARE_SPELL_FUN( spell_mass_invis		);
DECLARE_SPELL_FUN( spell_pass_door		);
DECLARE_SPELL_FUN( spell_poison			);
DECLARE_SPELL_FUN( spell_protection		);
DECLARE_SPELL_FUN( spell_protection_vs_good	);
DECLARE_SPELL_FUN( spell_refresh		);
DECLARE_SPELL_FUN( spell_remove_curse		);
DECLARE_SPELL_FUN( spell_sanctuary		);
DECLARE_SPELL_FUN( spell_shield			);
DECLARE_SPELL_FUN( spell_stone_skin		);
DECLARE_SPELL_FUN( spell_summon			);
DECLARE_SPELL_FUN( spell_teleport		);
DECLARE_SPELL_FUN( spell_weaken			);
DECLARE_SPELL_FUN( spell_word_of_recall		);
DECLARE_SPELL_FUN( spell_fire_breath		);
DECLARE_SPELL_FUN( spell_godbless		);
DECLARE_SPELL_FUN( spell_soulblade		);
DECLARE_SPELL_FUN( spell_mana			);
DECLARE_SPELL_FUN( spell_frenzy			);
DECLARE_SPELL_FUN( spell_darkblessing		);
DECLARE_SPELL_FUN( spell_portal			);
DECLARE_SPELL_FUN( spell_energyflux		);
DECLARE_SPELL_FUN( spell_transport		);
DECLARE_SPELL_FUN( spell_regenerate		);
DECLARE_SPELL_FUN( spell_clot			);
DECLARE_SPELL_FUN( spell_mend			);
DECLARE_SPELL_FUN( spell_minor_creation		);
DECLARE_SPELL_FUN( spell_gate			);
DECLARE_SPELL_FUN( spell_spiritkiss		);
DECLARE_SPELL_FUN( spell_brew			);
DECLARE_SPELL_FUN( spell_scribe			);
DECLARE_SPELL_FUN( spell_carve			);
DECLARE_SPELL_FUN( spell_engrave		);
DECLARE_SPELL_FUN( spell_bake			);
DECLARE_SPELL_FUN( spell_mount			);
DECLARE_SPELL_FUN( spell_scan			);
DECLARE_SPELL_FUN( spell_repair			);
DECLARE_SPELL_FUN( spell_spellproof		);
DECLARE_SPELL_FUN( spell_preserve		);
DECLARE_SPELL_FUN( spell_major_creation		);
DECLARE_SPELL_FUN( spell_chaos_blast		);
DECLARE_SPELL_FUN( spell_resistance		);

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(apollo)
int atoi args((const char *string));
void *calloc args((unsigned nelem, size_t size));
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(hpux)
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(macintosh) || defined(WIN32)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char *crypt args((const char *key, const char *salt));
#endif

#if	defined(sequent)
char *crypt args((const char *key, const char *salt));
int fclose args((FILE * stream));
int fprintf args((FILE * stream, const char *format, ...));
int fread args((void *ptr, int size, int n, FILE * stream));
int fseek args((FILE * stream, long offset, int ptrname));
void perror args((const char *s));
int ungetc args((int c, FILE * stream));
#endif

#if	defined(sun)
char *crypt args((const char *key, const char *salt));
int fclose args((FILE * stream));
int fprintf args((FILE * stream, const char *format, ...));

#if 	defined(SYSV)
size_t fread args((void *ptr, size_t size, size_t n, FILE * stream));
#else
int fread args((void *ptr, int size, int n, FILE * stream));
#endif
int fseek args((FILE * stream, long offset, int ptrname));
void perror args((const char *s));
int ungetc args((int c, FILE * stream));
#endif

#if	defined(ultrix)
char *crypt args((const char *key, const char *salt));
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)

#define PLAYER_DIR	""          /* Player files     */
#define NULL_FILE	"proto.are"   /* To reserve one stream  */
#define BACKUP_DIR      ""      /*reimburse shit -Infidel */
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""          /* Player files                 */
#define NULL_FILE	"nul"         /* To reserve one stream  */
#endif

#if defined( WIN32 )
#define PLAYER_DIR       "..\\player\\" /* Player files                 */
#define NULL_FILE        "nul"  /* To reserve one stream        */
#define BACKUP_DIR	"..\\backup\\"
#endif

#if defined(unix)
#define PLAYER_DIR	"../player/"  /* Player files     */
#define BACKUP_DIR      "../player/backup/" /*reimb shit */

#define NULL_FILE	"/dev/null"   /* To reserve one stream  */
#endif

#define END_MARKER      "END"                      /* All sorts of load functions */
#define DISABLED_FILE   "../txt/disabled.txt"      /* disabled commands           */
#define AREA_LIST	"../txt/area.lst"          /* List of areas               */
#define BAN_LIST	"../txt/ban.txt"           /* ban                         */
#define NEWBIEBAN_LIST  "../txt/newban.txt"        /* Newbie ban                  */
#define BUG_FILE	"../txt/bugs.txt"          /* For 'bug' and bug( )        */
#define SHUTDOWN_FILE	"../txt/shutdown.txt"      /* For 'shutdown'              */
#define CRASH_TEMP_FILE "../txt/crashtmp.txt"      /* Need it for crash-recover   */





/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define QF      QUEST_FUN
#define ED	EXIT_DATA

int   collen           ( const char *str );
int cprintf   ( char *buf, char *ptr, ... ) __attribute__ ((format(printf, 2, 3)));
int cnprintf  ( char *buf, int maxlen, char *ptr, ... ) __attribute__ ((format(printf, 3, 4)));
char *generate_banner( char *title, int size, char *bCol );
char *middle_text( char *text, int size );
/* act_comm.c */
void  add_follower      args (( CHAR_DATA * ch, CHAR_DATA * master ));
void  stop_follower     args (( CHAR_DATA * ch, bool isDead ));
void  die_follower      args (( CHAR_DATA * ch ));
bool  is_same_group     args (( CHAR_DATA * ach, CHAR_DATA * bch ));
void  room_text         args (( CHAR_DATA * ch, char *argument ));
char *strlower          args (( char *ip ));
void  excessive_cpu     args (( int blx ));
bool  check_parse_name  args (( char *name ));
void  room_message      args (( ROOM_INDEX_DATA * room, char *message ));

/* act_info.c */
void  set_title           args (( CHAR_DATA * ch, char *title ));
void  show_list_to_char   args (( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing ));
int   char_hitroll        args (( CHAR_DATA * ch ));
int   char_damroll        args (( CHAR_DATA * ch ));
int   char_ac             args (( CHAR_DATA * ch ));
bool  check_blind         args (( CHAR_DATA * ch ));

/* act_move.c */
void  move_char       args (( CHAR_DATA * ch, int door ));
void  open_lift       args (( CHAR_DATA * ch ));
void  close_lift      args (( CHAR_DATA * ch ));
void  move_lift       args (( CHAR_DATA * ch, int to_room ));
void  move_door       args (( CHAR_DATA * ch ));
void  thru_door       args (( CHAR_DATA * ch, int doorexit ));
void  open_door       args (( CHAR_DATA * ch, bool be_open ));
bool  is_open         args (( CHAR_DATA * ch ));
bool  same_floor      args (( CHAR_DATA * ch, int cmp_room ));
void  check_hunt      args (( CHAR_DATA * ch ));
RID  *get_rand_room   args (( void ));

/* act_obj.c */
bool  is_ok_to_wear    args (( CHAR_DATA * ch, bool wolf_ok, char *argument ));
bool  remove_obj       args (( CHAR_DATA * ch, int iWear, bool fReplace ));
void  wear_obj         args (( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace ));
void  call_all         args (( CHAR_DATA * ch ));

/* act_wiz.c */
void  bind_char            args (( CHAR_DATA * ch ));
void  logchan              args (( char *argument ));
void  copyover_recover     args (( void ));


/* build.c */
ED   *get_exit        args (( ROOM_INDEX_DATA * room, sh_int dir ));
ED   *get_exit_to     args (( ROOM_INDEX_DATA * room, sh_int dir, int vnum ));
ED   *get_exit_num    args (( ROOM_INDEX_DATA * room, sh_int count ));
char *copy_buffer     args (( CHAR_DATA * ch ));
void  edit_buffer     args (( CHAR_DATA * ch, char *argument ));
char *strip_cr        args (( char *str ));
void  start_editing   args (( CHAR_DATA * ch, char *data ));
void  stop_editing    args (( CHAR_DATA * ch ));

/* comm.c */
void  logt                   args (( char *fmt, ... ));
void  close_socket           args (( DESCRIPTOR_DATA * dclose ));
void  close_socket2          args (( DESCRIPTOR_DATA * dclose, bool kickoff ));
void  write_to_buffer        args (( DESCRIPTOR_DATA * d, const char *txt, int length ));
void  send_to_char           args (( const char *txt, CHAR_DATA * ch ));
void  act                    args (( const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type ));
void  act_brief              args (( const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type, int iBrief ));
void  stc                    args (( const char *txt, CHAR_DATA * ch ));
void  printf_to_char         args (( CHAR_DATA *ch, char *fmt, ... ));
bool  write_to_descriptor    args (( DESCRIPTOR_DATA *d, char *txt, int length ));
bool  write_to_descriptor_2  args (( int desc, char *txt, int length ));


/* prototypes from db.c */
void load_disabled args((void));
void save_disabled args((void));

/* prototypes from social-edit.c */
void  load_social_table   args (( void ));
void  save_social_table   args (( void ));

/* db.c */
void  boot_db            args (( bool fCopyOver ));
void  area_update        args (( void ));
CD   *create_mobile      args (( MOB_INDEX_DATA * pMobIndex ));
OD   *create_object      args (( OBJ_INDEX_DATA * pObjIndex, int level ));
void  clear_char         args (( CHAR_DATA * ch ));
void  free_char          args (( CHAR_DATA * ch ));
char *get_extra_descr    args (( char *name, EXTRA_DESCR_DATA * ed ));
char *get_roomtext       args (( const char *name, ROOMTEXT_DATA * rt ));
MID  *get_mob_index      args (( int vnum ));
OID  *get_obj_index      args (( int vnum ));
RID  *get_room_index     args (( int vnum ));
char  fread_letter       args (( FILE * fp ));
int   fread_number       args (( FILE * fp ));
char *fread_string       args (( FILE * fp ));
void  fread_to_eol       args (( FILE * fp ));
char *fread_word         args (( FILE * fp ));
void *alloc_mem          args (( int sMem ));
void *alloc_perm         args (( int sMem ));
void  free_mem           args (( void *pMem, int sMem ));
char *str_dup            args (( const char *str ));
void  free_string        args (( char *pstr ));
int   number_fuzzy       args (( int number ));
int   number_range       args (( int from, int to ));
int   number_percent     args (( void ));
int   number_door        args (( void ));
int   number_bits        args (( int width ));
int   number_mm          args (( void ));
int   dice               args (( int number, int size ));
int   interpolate        args (( int level, int value_00, int value_32 ));
void  smash_tilde        args (( char *str ));
bool  str_cmp            args (( const char *astr, const char *bstr ));
bool  str_prefix         args (( const char *astr, const char *bstr ));
bool  str_infix          args (( const char *astr, const char *bstr ));
bool  str_suffix         args (( const char *astr, const char *bstr ));
char *capitalize         args (( const char *str ));
void  append_file        args (( CHAR_DATA * ch, char *file, char *str ));
void  bug                args (( const char *str, int param ));
void  log_string         args (( const char *str ));
void  tail_chain         args (( void ));
void  add_help           args (( HELP_DATA * pHelp ));
char *strupper           args (( const char *str ));

/* arena.c */
void  close_arena        args (( void ));
void  open_arena         args (( void ));
void  update_bets        args (( CHAR_DATA * ch, CHAR_DATA * victim, bool payoff ));

/* wizutils.c */
void  aggr_test          args (( CHAR_DATA * ch ));
void  set_switchname     args (( CHAR_DATA * ch, char *title ));

/* religion.c */
void  check_faith               args (( CHAR_DATA *ch ));
bool  rel_oppose                args (( CHAR_DATA *ch, CHAR_DATA *victim ));

/* jobo_fight.c */
void  fortresskill       args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  ragnarokdecap      args (( CHAR_DATA * ch, CHAR_DATA * victim ));

/* desc_map.c */
void  draw_map           args (( CHAR_DATA * ch, char *desc ));
int   get_line           args (( char *desc, int max_len ));

/* jobo_comm.c */
void  delete_last_line_in_note     args (( CHAR_DATA *ch ));
void  init_tell_storage            args (( void ));
void  showchannel                  args (( CHAR_DATA * ch, int chan ));
void  talk_channel                 args (( CHAR_DATA * ch, char *argument, int channel, int sub_channel, const char *verb ));
void  strip_history                args (( CHAR_DATA * ch ));
HISTORY_DATA *generate_history     args (( void ));
void  update_history               args (( CHAR_DATA * ch, CHAR_DATA * talker, char *buf, char *argument, bool tell ));
void  attach_history               args (( CHAR_DATA * ch, char *message ));

/* Guild */
void save_fortress      args( (void) );
void load_fortress      args( (void) );

typedef struct fortress
{
int guard;
int soldier;
} FORTRESS;

FORTRESS fortress[12];

/* jobo_save.c */
void  load_newbiebans           args (( void ));
void  save_newbiebans           args (( void ));
void  load_leaderboard          args (( void ));
void  save_leaderboard          args (( void ));
void  load_mudinfo              args (( void ));
void  save_mudinfo              args (( void ));
void  load_ccenter              args (( void ));
void  save_ccenter              args (( void ));
void  write_mudinfo_database    args (( void ));
void  save_coreinfo             args (( void ));
void  load_coreinfo             args (( void ));
void  load_bans                 args (( void ));
void  save_bans                 args (( void ));
void  load_changes              args (( void ));
void  save_changes              args (( void ));
void  load_polls                args (( void ));
void  save_polls                args (( void ));
void  load_subvotes             args (( POLL_DATA * poll ));
void  save_subvotes             args (( POLL_DATA * poll ));
void  load_religions            args (( void ));
void  save_religions            args (( void ));

/* jobo_update.c */
void  update_trackers           args (( void ));
void  update_midi               args (( void ));
void  update_edge               args (( CHAR_DATA *ch ));

/* jobo_util.c */
bool  has_area_affect           args (( AREA_DATA *pArea, int affect, int religion ));
void  affect_to_area            args (( AREA_DATA *pArea, AREA_AFFECT *paf ));
void  area_affect_remove        args (( AREA_DATA *pArea, AREA_AFFECT *paf ));

char *pathfind                  args (( CHAR_DATA *ch, CHAR_DATA *victim ));
void  init_vt100                args (( DESCRIPTOR_DATA *d, char *xbuf ));
char *get_exits                 args (( CHAR_DATA *ch ));
char *strip_ansi                args (( char *str ));
bool  in_arena                  args (( CHAR_DATA *ch ));
char *line_indent               args (( char *text, int wBegin, int wMax ));
void  ragnarok_stop             args (( void ));
void  dump_last_command         args (( void ));
int   get_ratio                 args (( CHAR_DATA * ch ));
void  win_prize                 args (( CHAR_DATA * ch ));
bool  is_contained              args (( const char *astr, const char *bstr ));
bool  is_contained2             args (( const char *astr, const char *bstr ));
void  increase_total_output     args (( int clenght ));
void  update_mudinfo            args (( void ));
void  recycle_descriptors       args (( void ));
void  recycle_dummys            args (( void ));
int   get_next_playerid         args (( void ));
void  log_string2               args (( const char *str ));
void  panic_shutdown            args (( int iSignal ));
void  check_help_soundex        args (( char *argument, CHAR_DATA * ch ));
void  special_decap_message     args (( CHAR_DATA * ch, CHAR_DATA * victim ));
int   strlen2                   args (( const char *b ));
int   calc_ratio                args (( int a, int b ));
void  logout_message            args (( CHAR_DATA * ch ));
void  login_message             args (( CHAR_DATA * ch ));
void  avatar_message            args (( CHAR_DATA * ch ));
void  tie_message               args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  powerdown                 args (( CHAR_DATA * ch ));
bool  multicheck                args (( CHAR_DATA * ch ));
bool  reachedDecapLimit         args (( CHAR_DATA * ch ));
void  enter_info                args (( char *str ));
void  leave_info                args (( char *str ));
void  death_info                args (( char *str ));
void  avatar_info               args (( char *str ));
int   getMight                  args (( CHAR_DATA * ch ));
int   getMightMod               args (( CHAR_DATA * ch ));
void  forge_affect              args (( OBJ_DATA * obj, int value ));
void  update_revision           args (( CHAR_DATA * ch ));
bool  in_fortress               args (( CHAR_DATA * ch ));
bool  fair_fight                args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  update_polls              args (( void ));
void  complete_poll             args (( POLL_DATA * poll ));
bool  remove_change             args (( int i ));
char *get_dystopia_banner       args (( char *title, int size ));

/* jobo_quest.c */
void  quest_to_char             args (( CHAR_DATA * ch, QUEST_DATA * quest ));
void  quest_from_char           args (( CHAR_DATA * ch, QUEST_DATA * quest ));
bool  is_quest_target           args (( CHAR_DATA * ch, CHAR_DATA * victim ));
QF   *quest_lookup              args (( const char *name ));
char *quest_string              args (( QUEST_FUN * fun ));

/* msp.c */
void  send_bluespell      args (( CHAR_DATA * ch ));
void  send_yellowspell    args (( CHAR_DATA * ch ));
void  send_redspell       args (( CHAR_DATA * ch ));
void  send_greenspell     args (( CHAR_DATA * ch ));
void  send_purplespell    args (( CHAR_DATA * ch ));
void  send_howl           args (( CHAR_DATA * ch ));
void  send_portal         args (( CHAR_DATA * ch ));
void  send_detonate       args (( CHAR_DATA * ch ));
void  send_losthead       args (( CHAR_DATA * ch ));
void  send_teleport       args (( CHAR_DATA * ch ));
void  send_decap          args (( CHAR_DATA * ch ));
void  send_thunder        args (( CHAR_DATA * ch ));
void  send_eagle          args (( CHAR_DATA * ch ));
void  update_music        args (( CHAR_DATA * ch ));

/* jope.c */
bool  jope_load_char     args (( CHAR_DATA * ch, char *arg ));
void  jope_free_char     args (( CHAR_DATA * ch ));
void  jope_interp        args (( CHAR_DATA * ch, char *argument ));

/* jobo_shop.c */
SHOP_FUN *shop_lookup   args (( const char *name ));
char     *shop_string   args (( SHOP_FUN * fun ));

/* fight.c */
void  dropinvis            args (( CHAR_DATA * ch ));
int   cap_dam              args (( CD * ch, CD * victim, int dam ));
int   up_dam               args (( CHAR_DATA * ch, CHAR_DATA * victim, int dam ));
int   randomize_damage     args (( CHAR_DATA * ch, int dam, int am ));
void  update_damcap        args (( CHAR_DATA * ch, CHAR_DATA * victim ));
bool  killperson           args (( CHAR_DATA * victim ));

void  violence_update      args (( void ));
void  multi_hit            args (( CHAR_DATA * ch, CHAR_DATA * victim, int dt ));
void  damage               args (( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt ));
void  update_pos           args (( CHAR_DATA * victim ));
void  stop_fighting        args (( CHAR_DATA * ch, bool fBoth ));
void  stop_embrace         args (( CHAR_DATA * ch, CHAR_DATA * victim ));
bool  is_safe              args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  hurt_person          args (( CHAR_DATA * ch, CHAR_DATA * victim, int dam ));
void  set_fighting         args (( CHAR_DATA * ch, CHAR_DATA * victim ));
bool  has_timer            args (( CHAR_DATA * ch ));

void  check_leaderboard    args (( CHAR_DATA * ch ));
void  crashrecov           args (( int iSignal ));
void  iBacktrace           args (( int iSignal ));
void  retell_mccp          args (( DESCRIPTOR_DATA * d ));

/* handler.c */
void  extract_exit            args (( ROOM_INDEX_DATA * room, EXIT_DATA * pexit ));
bool  char_exists             args (( bool backup, char *argument ));
int   get_trust               args (( CHAR_DATA * ch ));
int   get_age                 args (( CHAR_DATA * ch ));
int   get_curr_str            args (( CHAR_DATA * ch ));
int   get_curr_int            args (( CHAR_DATA * ch ));
int   get_curr_wis            args (( CHAR_DATA * ch ));
int   get_curr_dex            args (( CHAR_DATA * ch ));
int   get_curr_con            args (( CHAR_DATA * ch ));
int   can_carry_n             args (( CHAR_DATA * ch ));
int   can_carry_w             args (( CHAR_DATA * ch ));
bool  is_name                 args (( char *str, char *namelist ));
bool  is_full_name            args (( const char *str, char *namelist ));
void  affect_to_char          args (( CHAR_DATA * ch, AFFECT_DATA * paf ));
void  affect2_to_char         args (( CHAR_DATA *ch, AFFECT2_DATA *paf ) );
void  affect_to_obj           args (( OBJ_DATA * obj, AFFECT_DATA * paf ));
void  affect_remove           args (( CHAR_DATA * ch, AFFECT_DATA * paf ));
void  affect2_remove          args (( CHAR_DATA *ch, AFFECT2_DATA *paf ) );
void  alias_remove            args (( CHAR_DATA * ch, ALIAS_DATA * ali ));
void  affect_strip            args (( CHAR_DATA * ch, int sn ));
bool  is_affected             args (( CHAR_DATA * ch, int sn ));
void  affect_join             args (( CHAR_DATA * ch, AFFECT_DATA * paf ));
void    affect2_join    args( ( CHAR_DATA *ch, AFFECT2_DATA *paf ) );
void  char_from_room          args (( CHAR_DATA * ch ));
void  char_to_room            args (( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ));
void  obj_to_char             args (( OBJ_DATA * obj, CHAR_DATA * ch ));
void  obj_from_char           args (( OBJ_DATA * obj ));
int   apply_ac                args (( OBJ_DATA * obj, int iWear ));
OD   *get_eq_char             args (( CHAR_DATA * ch, int iWear ));
void  equip_char              args (( CHAR_DATA * ch, OBJ_DATA * obj, int iWear ));
void  unequip_char            args (( CHAR_DATA * ch, OBJ_DATA * obj ));
int   count_obj_list          args (( OBJ_INDEX_DATA * obj, OBJ_DATA * list ));
void  obj_from_room           args (( OBJ_DATA * obj ));
void  obj_to_room             args (( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex ));
void  obj_to_obj              args (( OBJ_DATA * obj, OBJ_DATA * obj_to ));
void  obj_from_obj            args (( OBJ_DATA * obj ));
void  extract_obj             args (( OBJ_DATA * obj ));
void  extract_char            args (( CHAR_DATA * ch, bool fPull ));
CD   *get_char_room           args (( CHAR_DATA * ch, char *argument ));
CD   *get_char_world          args (( CHAR_DATA * ch, char *argument ));
CD   *get_char_area           args (( CHAR_DATA *ch, char *argument ));
OD   *get_obj_type            args (( OBJ_INDEX_DATA * pObjIndexData ));
OD   *get_obj_list            args (( CHAR_DATA * ch, char *argument, OBJ_DATA * list ));
OD   *get_obj_in_obj          args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_carry           args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_wear            args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_here            args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_room            args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_world           args (( CHAR_DATA * ch, char *argument ));
OD   *get_obj_world2          args (( CHAR_DATA * ch, char *argument ));
int   get_obj_number          args (( OBJ_DATA * obj ));
int   get_obj_weight          args (( OBJ_DATA * obj ));
bool  room_is_dark            args (( ROOM_INDEX_DATA * pRoomIndex ));
bool  room_is_private         args (( ROOM_INDEX_DATA * pRoomIndex ));
bool  can_see                 args (( CHAR_DATA * ch, CHAR_DATA * victim ));
bool  can_see_obj             args (( CHAR_DATA * ch, OBJ_DATA * obj ));
bool  can_drop_obj            args (( CHAR_DATA * ch, OBJ_DATA * obj ));
char *item_type_name          args (( OBJ_DATA * obj ));
char *affect_loc_name         args (( int location ));
char *affect_bit_name         args (( int vector ));
char *affect2_bit_name        args (( int vector ) );
char *extra_bit_name          args (( int extra_flags ));
void  affect_modify           args (( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd, OBJ_DATA * obj ));


/* interp.c */
void  interpret               args (( CHAR_DATA * ch, char *argument ));
bool  is_number               args (( char *arg ));
int   number_argument         args (( char *argument, char *arg ));
char *one_argument            args (( char *argument, char *arg_first ));
void  stage_update            args (( CHAR_DATA * ch, CHAR_DATA * victim, int stage, char *argument ));
void  make_preg               args (( CHAR_DATA * mother, CHAR_DATA * father ));

/* magic.c */
int   skill_lookup       args (( const char *name ));
int   slot_lookup        args (( int slot ));
bool  saves_spell        args (( int level, CHAR_DATA * victim ));
void  obj_cast_spell     args (( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ));
void  enhance_stat       args (( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, int apply_bit, int bonuses, int affect_bit ));

/* save.c */
void  save_char_obj      args (( CHAR_DATA * ch ));
bool  load_char_obj      args (( DESCRIPTOR_DATA * d, char *name ));
bool  load_char_short    args (( DESCRIPTOR_DATA * d, char *name ));

/* special.c */
SF   *spec_lookup        args (( const char *name ));

/* mccp.c */
bool  compressStart      args (( DESCRIPTOR_DATA * desc ));
bool  compressEnd        args (( DESCRIPTOR_DATA * desc ));
bool  compressEnd2       args (( DESCRIPTOR_DATA * desc ));  // threadsafe version.
bool  processCompressed  args (( DESCRIPTOR_DATA * desc ));
bool  writeCompressed    args (( DESCRIPTOR_DATA * desc, char *txt, int length ));

/* update.c */
void  gain_exp           args (( CHAR_DATA * ch, int gain ));
void  gain_condition     args (( CHAR_DATA * ch, int iCond, int value ));
void  update_handler     args (( void ));
void  mobile_update      args (( void ));
void  weather_update     args (( void ));
void  char_update        args (( void ));
void  obj_update         args (( void ));
void  room_update        args (( void ));
void  reg_mend           args (( CHAR_DATA * ch ));

/* kav_fight.c */
void  special_move       args (( CHAR_DATA * ch, CHAR_DATA * victim ));

/* kav_info.c */
void  birth_date    args (( CHAR_DATA * ch, bool is_self ));
void  other_age     args (( CHAR_DATA * ch, int extra, bool is_preg, char *argument ));
int   years_old     args (( CHAR_DATA * ch ));

/* kav_wiz.c */
void  oset_affect   args (( CHAR_DATA * ch, OBJ_DATA * obj, int value, int affect, bool is_quest ));

/* xrakisis.c */
void improve_str args((CHAR_DATA * ch));
void improve_dex args((CHAR_DATA * ch));
void improve_con args((CHAR_DATA * ch));
void improve_int args((CHAR_DATA * ch));
void improve_wis args((CHAR_DATA * ch));
void make_wall     args( ( ROOM_INDEX_DATA *room, int dir, int wall ) );




#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef  ED

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * This structure is used in special.c to lookup spec funcs and
 * also in olc_act.c to display listings of spec funcs.
 */
struct spec_type
{
  char *spec_name;
  SPEC_FUN *spec_fun;
};

/*
 * Used in jobo_quest.c
 */
struct quest_type
{
  char *quest_name;
  QUEST_FUN *quest_fun;
};

/*
 * used in jobo_shop.c
 */
struct shop_type
{
  char *shop_name;
  SHOP_FUN *shop_fun;
};

/*
 * This structure is used in bit.c to lookup flags and stats.
 */
struct flag_type
{
  char *name;
  int bit;
  bool settable;
};

/*
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1 /* Area has been modified. */
#define         AREA_ADDED      2 /* Area has been added to. */
#define         AREA_LOADING    4 /* Used for counting in db.c */
#define 	AREA_VERBOSE	8

/*
 * Area bits
 */
#define AREA_BIT_NONE         0
#define AREA_BIT_NO_TRAVEL    1 /* it is not possibly to use class travels to mobs in this area */
#define AREA_BIT_IREGEN       2 /* increases regen rate for the area           */
#define AREA_BIT_DREGEN       4 /* decreates regen rate for the area           */
#define AREA_BIT_NOMAGIC      8 /* no magic spells (player/object) can be cast */
#define AREA_BIT_NOHOME      16 /* it is not possible to home in this area     */
#define AREA_BIT_NOEDIT      32 /* This room cannot be edited by anyone        */
#define AREA_BIT_NOEDIT2     64 /* need it to actually work with noedit        */

#define MAX_DIR	6
#define NO_FLAG -99             /* Must not be used in flags or stats. */

/*
 * Global Constants
 */
extern char *const dir_name[];
extern const sh_int rev_dir[];
extern const struct spec_type spec_table[];
extern const struct quest_type quest_table[];
extern const struct shop_type shop_table[];

/*
 * Global variables
 */

extern char *last_command;
extern int mccp_mem_usage;
extern int mccp_mem_freed;
extern int nAllocPerm;
extern int nAllocString;
extern int sAllocPerm;
extern AREA_DATA *area_first;
extern AREA_DATA *area_last;
extern int top_affect;
extern int top_area;
extern int top_ed;
extern int top_exit;
extern int top_help;
extern int top_mob_index;
extern int top_obj_index;
extern int top_reset;
extern int top_room;
extern int top_vnum_mob;
extern int top_vnum_obj;
extern int top_vnum_room;
extern char str_empty[1];

extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];

LEADER_BOARD leader_board;
MAP_TYPE map[MAPX + 1][MAPY + 1];
RELIGION_TABLE religion_table[MAX_RELIGIONS+1];


/* db.c */
void  reset_area     args (( AREA_DATA * pArea ));
void  reset_room     args (( ROOM_INDEX_DATA * pRoom ));

/* string.c */
void  string_edit           args (( CHAR_DATA * ch, char **pString ));
void  string_append         args (( CHAR_DATA * ch, char **pString ));
char *string_replace        args (( char *orig, char *old, char *new ));
void  string_add            args (( CHAR_DATA * ch, char *argument ));
char *format_string         args (( char *oldstring ));
char *first_arg             args (( char *argument, char *arg_first, bool fCase ));
char *string_unpad          args (( char *argument ));
char *string_proper         args (( char *argument ));
char *all_capitalize        args (( const char *str ));

/* olc.c */
bool  run_olc_editor        args (( DESCRIPTOR_DATA * d ));
char *olc_ed_name           args (( CHAR_DATA * ch ));
char *olc_ed_vnum           args (( CHAR_DATA * ch ));

/* special.c */
char *spec_string           args (( SPEC_FUN * fun )); /* OLC */

/* bit.c */
extern const struct flag_type area_flags[];
extern const struct flag_type area_bits[];
extern const struct flag_type sex_flags[];
extern const struct flag_type exit_flags[];
extern const struct flag_type door_resets[];
extern const struct flag_type room_flags[];
extern const struct flag_type sector_flags[];
extern const struct flag_type type_flags[];
extern const struct flag_type extra_flags[];
extern const struct flag_type wear_flags[];
extern const struct flag_type act_flags[];
extern const struct flag_type affect_flags[];
extern const struct flag_type affect2_flags[];
extern const struct flag_type apply_flags[];
extern const struct flag_type wear_loc_strings[];
extern const struct flag_type wear_loc_flags[];
extern const struct flag_type weapon_flags[];
extern const struct flag_type weapon_class_flags[];
extern const struct flag_type container_flags[];
extern const struct flag_type liquid_flags[];
extern const struct flag_type material_flags[];


struct slay_type
{
  char *owner;                  /* only this player can use this option */
  char *title;                  /* each one must have a unique title!   */
  char *char_msg;               /* is sent to the "slayer"    */
  char *vict_msg;               /* is sent to the "slayee" (poor sucker) */
  char *room_msg;               /* is sent to everyone else in the room */
};


extern const struct pc_eyes pc_eye_table [];
extern const struct pc_looks pc_looks_table [];
extern const struct pc_build pc_build_table [];
extern const struct pc_hair pc_hair_table [];

extern const struct material material_table[]; // By Xrakisis, custom


/* tables for J.O.P.E. */
extern const struct flag_type extra_table[];
extern const struct flag_type act_table[];
extern const struct flag_type jflags_table[];
extern const struct flag_type newbits_table[];
extern const struct pc_race_type pc_race_table[];
extern  const	struct  race_type	race_table	[];


#endif

