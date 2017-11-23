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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

/*
 * Local functions.
 */
void  mobile_update               args (( void ));
void  weather_update              args (( void ));
void  room_update                 args (( void ));
void  char_update                 args (( void ));
void  obj_update                  args (( void ));
void  ww_update                   args (( void ));
void  update_regen                args (( CHAR_DATA * ch, int multiplier ));
void  update_active_counters      args (( CHAR_DATA * ch ));
void  update_morted_timer         args (( CHAR_DATA * ch ));
void  update_sit_safe_counter     args (( CHAR_DATA * ch ));
void  update_drunks               args (( CHAR_DATA * ch ));
void  sex_update                  args (( CHAR_DATA * ch ));
void  update_arti_regen           args (( CHAR_DATA * ch ));
void  regen_limb                  args (( CHAR_DATA * ch ));


void gain_level (CHAR_DATA * ch, int gain)
{
    char buf[MAX_STRING_LENGTH];
    int lgainexp = 1550000 * ch->pcRaceLevel;
    int hp = 0;
    int mana = 0;
    int move = 0;

    hp = number_range(650,800);
    mana = number_range(650,800);
    move = number_range(650,800);

   ch->exp += gain;

    if (IS_NPC(ch))
        return;

    if (lgainexp == 0) lgainexp = 1550000;
    if (ch->exp < lgainexp)
    return;

    while (ch->pcRaceLevel < 200 && ch->exp >= lgainexp)
    {
        ch->pcRaceLevel++;
	ch->exp -= lgainexp;
        sprintf( buf, "#0%s is now #RLevel #R[#0%d#R]#n", ch->pcdata->switchname, ch->pcRaceLevel );
        do_info( ch, buf );
        sprintf( buf, "You gain %d HP, %d Move and %d Mana.\n\r", hp, move, mana );
        send_to_char( buf, ch );
        ch->max_hit += hp;
        ch->max_move += move;
        ch->max_mana += mana;
        update_pos ( ch );
    }
    return;
}


void gain_exp(CHAR_DATA * ch, int gain)
{
  CHAR_DATA *mount = NULL;
  CHAR_DATA *master = NULL;

  if (IS_NPC(ch) && (mount = ch->mount) != NULL && !IS_NPC(mount))
  {
    if ((master = ch->master) == NULL || master != mount)
      mount->exp += gain;
  }
  if (!IS_NPC(ch))
    ch->exp += gain;
  return;
}

void gain_condition(CHAR_DATA * ch, int iCond, int value)
{
  int condition;

  if (value == 0 || IS_NPC(ch))
    return;

  if (!IS_NPC(ch) && IS_HERO(ch) && iCond != COND_DRUNK)
    return;

  condition = ch->pcdata->condition[iCond];
  if (!IS_NPC(ch))
  {
    ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);
  }
  else
    ch->pcdata->condition[iCond] = URANGE(0, condition + value, 20000 / ch->generation);

  if (ch->pcdata->condition[iCond] == 0)
  {
    switch (iCond)
    {
      case COND_FULL:
        send_to_char("You are REALLY hungry.\n\r", ch);
        act("You hear $n's stomach rumbling.", ch, NULL, NULL, TO_ROOM);
        break;

      case COND_THIRST:
        send_to_char("You are REALLY thirsty.\n\r", ch);
        break;

      case COND_DRUNK:
        if (condition != 0)
          send_to_char("You are sober.\n\r", ch);
        break;
    }
  }
  else if (ch->pcdata->condition[iCond] < 10)
  {
    switch (iCond)
    {
      case COND_FULL:
        send_to_char("You feel hungry.\n\r", ch);
        break;

      case COND_THIRST:
        send_to_char("You feel thirsty.\n\r", ch);
        break;
    }
  }

  return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to % of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update(void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  EXIT_DATA *pexit;
  int door;

  /* Examine all mobs. */
  for (ch = char_list; ch != NULL; ch = ch_next)
  {
    ch_next = ch->next;

    if (ch->in_room == NULL)
      continue;

    if (!IS_NPC(ch) && ch->hunting != NULL && ch->hunting != '\0' && strlen(ch->hunting) > 1)
    {
      check_hunt(ch);
      continue;
    }

   
    if (!IS_NPC(ch))
    {
      update_morted_timer(ch);
      update_sit_safe_counter(ch);
      update_drunks(ch);
      sex_update(ch);


	 if (IS_SPHERE(ch, SPHERE_NECROMANCER))
	{
	  if (ch->pcdata->powers[POWER_TICK] > 0) ch->pcdata->powers[POWER_TICK]--;
	}
	if (IS_PROF(ch, PROF_BARBARIAN))
	{
        if (ch->pcdata->powers[RAGE_TIMER] == 1)
	stc("#0You may now Rage again#n.\n\r", ch);
	if (ch->pcdata->powers[RAGE_TIMER] > 0) ch->pcdata->powers[RAGE_TIMER]--;
	}



      if (IS_HERO(ch) && ch->hit > 0 && !IS_SET(ch->extra, EXTRA_AFK))
      {
        if (ch->class == 0 && IS_ITEMAFF(ch, ITEMA_REGENERATE))
          update_arti_regen(ch);
      }
      else
      {
        ch->hit += number_range(1, 5);
        if (ch->hit > ch->max_hit)
          ch->hit = ch->max_hit;
        update_pos(ch);
        if (IS_ITEMAFF(ch, ITEMA_REGENERATE) && ch->hit > 0)
          update_arti_regen(ch);
      }
    }
    else                        // This is for the mobs
    {
      if (IS_AFFECTED(ch, AFF_CHARM))
        continue;
      if (ch->spec_fun != 0)
      {
        if ((*ch->spec_fun) (ch, "update"))
          continue;
        if (ch == NULL)
          continue;
      }
      if (ch->position != POS_STANDING)
      {
        do_stand(ch, "");
        continue;
      }
      if (IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->contents != NULL && number_bits(2) == 0)
      {
        OBJ_DATA *obj;
        OBJ_DATA *obj_best = 0;
        int max = 1;

        for (obj = ch->in_room->contents; obj; obj = obj->next_content)
        {
          if (!can_see_obj(ch, obj))
            continue;
          if (IS_SET(obj->quest, QUEST_ARTIFACT))
            continue;
          if (CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max)
          {
            obj_best = obj;
            max = obj->cost;
          }
        }
        if (obj_best)
        {
          obj_from_room(obj_best);
          obj_to_char(obj_best, ch);
          act("$n picks $p up.", ch, obj_best, NULL, TO_ROOM);
          act("You pick $p up.", ch, obj_best, NULL, TO_CHAR);
        }
      }
      if (!IS_SET(ch->act, ACT_SENTINEL) && (door = number_bits(5)) <= 5 && (pexit = ch->in_room->exit[door]) != NULL
        && pexit->to_room != NULL && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
        && (ch->hunting == NULL || strlen(ch->hunting) < 1)
        && !(IS_SET(ch->act, ACT_FISH) && pexit->to_room->sector_type != SECT_WATER_SWIM && pexit->to_room->sector_type != SECT_WATER_NOSWIM)
        && !(IS_SET(ch->act, ACT_NOSWIM) && (pexit->to_room->sector_type == SECT_WATER_SWIM || pexit->to_room->sector_type == SECT_WATER_NOSWIM))
        && ((!IS_SET(ch->act, ACT_STAY_AREA) && ch->level < 900) || pexit->to_room->area == ch->in_room->area))
      {
        move_char(ch, door);
      }
      if (ch->hit < ch->max_hit / 2 && (door = number_bits(3)) <= 5 && (pexit = ch->in_room->exit[door]) != NULL
          && pexit->to_room != NULL && ch->level < 900 && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
      {
        CHAR_DATA *rch;
        bool found;

        found = FALSE;
        for (rch = pexit->to_room->people; rch != NULL; rch = rch->next_in_room)
        {
          if (!IS_NPC(rch))
          {
            found = TRUE;
            break;
          }
        }
        if (!found)
          move_char(ch, door);
      }
    }
  }
  return;
}

/*
 * Update the weather.
 */
void weather_update(void)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch = NULL;
  int diff;
  bool char_up;
  bool thunder = FALSE;

  buf[0] = '\0';

  switch (++time_info.hour)
  {
    case 1:
      strcat(buf, "#LThe #7White Moon#L, #7Solinari#L Rises.#n\n\r");
      break;

    case 3:
      strcat(buf, "#LThe #RRed Moon#L, #RLunitari#L Rises.#n\n\r");
      break;

    case 5:
      weather_info.sunlight = SUN_LIGHT;
      strcat(buf, "#LAnother day has #0begun.#n\n\r");
      break;

    case 6:
      weather_info.sunlight = SUN_RISE;
      strcat(buf, "#LThe #YSun #Lrises in the sky.#n\n\r");
      break;

    case 19:
      weather_info.sunlight = SUN_SET;
      strcat(buf, "#LThe #YSun #Lslowly disappears in the west.#n\n\r");
      break;

    case 20:
      weather_info.sunlight = SUN_DARK;
      strcat(buf, "#LThe #0night #Lhas #0begun.#n\n\r");
      break;

    case 22:
      strcat(buf, "#LThe #0Black Moon#L, #0Nuitari#L Rises.#n\n\r");
      break;

    case 24:
      time_info.hour = 0;
      time_info.day++;
      for (d = descriptor_list; d != NULL; d = d->next)
      {
        char_up = FALSE;
        if ((d->connected == CON_PLAYING || d->connected == CON_EDITING) && (ch = d->character) != NULL && !IS_NPC(ch))
        {
          send_to_char("#LYou hear a #0clock #Lin the #0distance #7strike #0midnight.#n\n\r", ch);
          if (ch->pcdata->followers > 0)
            ch->pcdata->followers = 0;
          if (IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
          {
            send_to_char("The silence leaves the room.\n\r", ch);
            act("The silence leaves the room.", ch, NULL, NULL, TO_ROOM);
            REMOVE_BIT(ch->in_room->room_flags, ROOM_SILENCE);

          }

          if (IS_SET(ch->in_room->room_flags, ROOM_FLAMING))
          {
            send_to_char("The flames in the room die down.\n\r", ch);
            act("The flames in the room die down.", ch, NULL, NULL, TO_ROOM);
            REMOVE_BIT(ch->in_room->room_flags, ROOM_FLAMING);
          }
        }

      }
      break;
  }

  if (time_info.day >= 35)
  {
    time_info.day = 0;
    time_info.month++;
  }

  if (time_info.month >= 17)
  {
    time_info.month = 0;
    time_info.year++;
  }

  /*
   * Weather change.
   */
  if (time_info.month >= 9 && time_info.month <= 16)
    diff = weather_info.mmhg > 985 ? -2 : 2;
  else
    diff = weather_info.mmhg > 1015 ? -2 : 2;

  weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
  weather_info.change = UMAX(weather_info.change, -12);
  weather_info.change = UMIN(weather_info.change, 12);

  weather_info.mmhg += weather_info.change;
  weather_info.mmhg = UMAX(weather_info.mmhg, 960);
  weather_info.mmhg = UMIN(weather_info.mmhg, 1040);

  switch (weather_info.sky)
  {
    default:
      bug("Weather_update: bad sky %d.", weather_info.sky);
      weather_info.sky = SKY_CLOUDLESS;
      break;

    case SKY_CLOUDLESS:
      if (weather_info.mmhg < 990 || (weather_info.mmhg < 1010 && number_bits(2) == 0))
      {
        strcat(buf, "The sky is getting cloudy.\n\r");
        weather_info.sky = SKY_CLOUDY;
      }
      break;

    case SKY_CLOUDY:
      if (weather_info.mmhg < 970 || (weather_info.mmhg < 990 && number_bits(2) == 0))
      {
        strcat(buf, "It starts to rain.\n\r");
        weather_info.sky = SKY_RAINING;
      }

      if (weather_info.mmhg > 1030 && number_bits(2) == 0)
      {
        strcat(buf, "The clouds disappear.\n\r");
        weather_info.sky = SKY_CLOUDLESS;
      }
      break;

    case SKY_RAINING:
      if (weather_info.mmhg < 970 && number_bits(2) == 0)
      {
        strcat(buf, "Lightning flashes in the sky.\n\r");
        thunder = TRUE;
        weather_info.sky = SKY_LIGHTNING;
      }

      if (weather_info.mmhg > 1030 || (weather_info.mmhg > 1010 && number_bits(2) == 0))
      {
        strcat(buf, "The rain stopped.\n\r");
        weather_info.sky = SKY_CLOUDY;
      }
      break;

    case SKY_LIGHTNING:
      if (weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits(2) == 0))
      {
        strcat(buf, "The lightning has stopped.\n\r");
        weather_info.sky = SKY_RAINING;
        break;
      }
      break;
  }

  if (buf[0] != '\0')
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if ((d->connected == CON_PLAYING || d->connected == CON_EDITING) && IS_OUTSIDE(d->character) && IS_AWAKE(d->character))
      {
        if (thunder)
          send_thunder(d->character);
        send_to_char(buf, d->character);
      }
    }
  }

  return;
}

void reg_mend(CHAR_DATA * ch)
{
  int ribs = 0;
  int teeth = 0;
 
  if (IS_BODY(ch, BROKEN_RIBS_1))
    ribs += 1;
  if (IS_BODY(ch, BROKEN_RIBS_2))
    ribs += 2;
  if (IS_BODY(ch, BROKEN_RIBS_4))
    ribs += 4;
  if (IS_BODY(ch, BROKEN_RIBS_8))
    ribs += 8;
  if (IS_BODY(ch, BROKEN_RIBS_16))
    ribs += 16;
  if (IS_HEAD(ch, LOST_TOOTH_1))
    teeth += 1;
  if (IS_HEAD(ch, LOST_TOOTH_2))
    teeth += 2;
  if (IS_HEAD(ch, LOST_TOOTH_4))
    teeth += 4;
  if (IS_HEAD(ch, LOST_TOOTH_8))
    teeth += 8;
  if (IS_HEAD(ch, LOST_TOOTH_16))
    teeth += 16;

  if (ribs > 0)
  {
    if (IS_BODY(ch, BROKEN_RIBS_1))
      REMOVE_BIT(ch->loc_hp[1], BROKEN_RIBS_1);
    if (IS_BODY(ch, BROKEN_RIBS_2))
      REMOVE_BIT(ch->loc_hp[1], BROKEN_RIBS_2);
    if (IS_BODY(ch, BROKEN_RIBS_4))
      REMOVE_BIT(ch->loc_hp[1], BROKEN_RIBS_4);
    if (IS_BODY(ch, BROKEN_RIBS_8))
      REMOVE_BIT(ch->loc_hp[1], BROKEN_RIBS_8);
    if (IS_BODY(ch, BROKEN_RIBS_16))
      REMOVE_BIT(ch->loc_hp[1], BROKEN_RIBS_16);
    ribs -= 1;
    if (ribs >= 16)
    {
      ribs -= 16;
      SET_BIT(ch->loc_hp[1], BROKEN_RIBS_16);
    }
    if (ribs >= 8)
    {
      ribs -= 8;
      SET_BIT(ch->loc_hp[1], BROKEN_RIBS_8);
    }
    if (ribs >= 4)
    {
      ribs -= 4;
      SET_BIT(ch->loc_hp[1], BROKEN_RIBS_4);
    }
    if (ribs >= 2)
    {
      ribs -= 2;
      SET_BIT(ch->loc_hp[1], BROKEN_RIBS_2);
    }
    if (ribs >= 1)
    {
      ribs -= 1;
      SET_BIT(ch->loc_hp[1], BROKEN_RIBS_1);
    }
    act("One of $n's ribs snap back into place.", ch, NULL, NULL, TO_ROOM);
    act("One of your ribs snap back into place.", ch, NULL, NULL, TO_CHAR);
  }
  else if (IS_HEAD(ch, LOST_EYE_L))
  {
    act("An eyeball appears in $n's left eye socket.", ch, NULL, NULL, TO_ROOM);
    act("An eyeball appears in your left eye socket.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_L);
  }
  else if (IS_HEAD(ch, LOST_EYE_R))
  {
    act("An eyeball appears in $n's right eye socket.", ch, NULL, NULL, TO_ROOM);
    act("An eyeball appears in your right eye socket.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_R);
  }
  else if (IS_HEAD(ch, LOST_EAR_L))
  {
    act("An ear grows on the left side of $n's head.", ch, NULL, NULL, TO_ROOM);
    act("An ear grows on the left side of your head.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EAR_L);
  }
  else if (IS_HEAD(ch, LOST_EAR_R))
  {
    act("An ear grows on the right side of $n's head.", ch, NULL, NULL, TO_ROOM);
    act("An ear grows on the right side of your head.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EAR_R);
  }
  else if (IS_HEAD(ch, LOST_NOSE))
  {
    act("A nose grows on the front of $n's face.", ch, NULL, NULL, TO_ROOM);
    act("A nose grows on the front of your face.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_NOSE);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], BROKEN_NOSE);
  }  
  else if (teeth > 0)
  {  
    if (IS_HEAD(ch, LOST_TOOTH_1))
      REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_1);
    if (IS_HEAD(ch, LOST_TOOTH_2))
      REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_2);
    if (IS_HEAD(ch, LOST_TOOTH_4))
      REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_4);
    if (IS_HEAD(ch, LOST_TOOTH_8))
      REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_8);
    if (IS_HEAD(ch, LOST_TOOTH_16))
      REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_16);
    teeth -= 1; 
    if (teeth >= 16)
    {
      teeth -= 16;
      SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_16);
    }
    if (teeth >= 8)
    {
      teeth -= 8;
      SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_8);
    }
    if (teeth >= 4)
    {
      teeth -= 4;
      SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_4);
    }
    if (teeth >= 2)
    {
      teeth -= 2;
      SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_2);
    }
    if (teeth >= 1)
    {
      teeth -= 1;
      SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_1);
    }
    act("A missing tooth grows in your mouth.", ch, NULL, NULL, TO_CHAR);
    act("A missing tooth grows in $n's mouth.", ch, NULL, NULL, TO_ROOM);
  }
  else if (IS_HEAD(ch, BROKEN_NOSE))
  {
    act("$n's nose snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your nose snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], BROKEN_NOSE);
  }
  else if (IS_HEAD(ch, BROKEN_JAW))
  {
    act("$n's jaw snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your jaw snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], BROKEN_JAW);
  }
  else if (IS_HEAD(ch, BROKEN_SKULL))
  {
    act("$n's skull knits itself back together.", ch, NULL, NULL, TO_ROOM);
    act("Your skull knits itself back together.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_HEAD], BROKEN_SKULL);  
  }
  else if (IS_BODY(ch, BROKEN_SPINE))
  {
    act("$n's spine knits itself back together.", ch, NULL, NULL, TO_ROOM);
    act("Your spine knits itself back together.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_BODY], BROKEN_SPINE);  
  }
  else if (IS_BODY(ch, BROKEN_NECK))
  {
    act("$n's neck snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your neck snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_BODY], BROKEN_NECK);
  }
  else if (IS_ARM_L(ch, LOST_ARM))
  {
    act("An arm grows from the stump of $n's left shoulder.", ch, NULL, NULL, TO_ROOM);
    act("An arm grows from the stump of your left shoulder.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_ARM);  
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_ARM);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_HAND);
  }  
  else if (IS_ARM_R(ch, LOST_ARM))
  {
    act("An arm grows from the stump of $n's right shoulder.", ch, NULL, NULL, TO_ROOM);
    act("An arm grows from the stump of your right shoulder.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_ARM);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_ARM);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_HAND);
  }  
  else if (IS_LEG_L(ch, LOST_LEG))
  {  
    act("A leg grows from the stump of $n's left hip.", ch, NULL, NULL, TO_ROOM);
    act("A leg grows from the stump of your left hip.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_L], LOST_LEG);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_L], BROKEN_LEG);
    SET_BIT(ch->loc_hp[LOC_LEG_L], LOST_FOOT);
  }
  else if (IS_LEG_R(ch, LOST_LEG))  
  {
    act("A leg grows from the stump of $n's right hip.", ch, NULL, NULL, TO_ROOM);
    act("A leg grows from the stump of your right hip.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_R], LOST_LEG);  
    REMOVE_BIT(ch->loc_hp[LOC_LEG_R], BROKEN_LEG);
    SET_BIT(ch->loc_hp[LOC_LEG_R], LOST_FOOT);
  }
  else if (IS_ARM_L(ch, BROKEN_ARM))
  {
    act("$n's left arm snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left arm snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_ARM);
  }
  else if (IS_ARM_R(ch, BROKEN_ARM))
  {
    act("$n's right arm snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right arm snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_ARM);
  }
  else if (IS_LEG_L(ch, BROKEN_LEG))
  {
    act("$n's left leg snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left leg snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_L], BROKEN_LEG);
  }
  else if (IS_LEG_R(ch, BROKEN_LEG))
  {
    act("$n's right leg snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right leg snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_R], BROKEN_LEG);
  }
  else if (IS_ARM_L(ch, LOST_HAND))
  {
    act("A hand grows from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM);
    act("A hand grows from the stump of your left wrist.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_HAND);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_THUMB);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_I);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_M);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_R);
    SET_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_L);
  }
  else if (IS_ARM_R(ch, LOST_HAND))
  {
    act("A hand grows from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM);
    act("A hand grows from the stump of your right wrist.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_HAND);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_THUMB);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_I);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_M);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_R);
    SET_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_L);
  }
  else if (IS_LEG_L(ch, LOST_FOOT)) 
  {
    act("A foot grows from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM);
    act("A foot grows from the stump of your left ankle.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_L], LOST_FOOT); 
  }
  else if (IS_LEG_R(ch, LOST_FOOT))
  {
    act("A foot grows from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM);
    act("A foot grows from the stump of your right ankle.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_LEG_R], LOST_FOOT);
  }
  else if (IS_ARM_L(ch, LOST_THUMB))
  {
    act("A thumb slides out of $n's left hand.", ch, NULL, NULL, TO_ROOM);
    act("A thumb slides out of your left hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_THUMB);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_THUMB);
  }
  else if (IS_ARM_L(ch, BROKEN_THUMB))
  {
    act("$n's left thumb snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left thumb snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_THUMB);
  }
  else if (IS_ARM_L(ch, LOST_FINGER_I))
  {
    act("An index finger slides out of $n's left hand.", ch, NULL, NULL, TO_ROOM);
    act("An index finger slides out of your left hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_I);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_I);
  }
  else if (IS_ARM_L(ch, BROKEN_FINGER_I))
  {
    act("$n's left index finger snaps back into place.", ch, NULL, NULL, TO_ROOM);  
    act("Your left index finger snaps back into place.", ch, NULL, NULL, TO_CHAR);  
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_I);
  }
  else if (IS_ARM_L(ch, LOST_FINGER_M))
  {
    act("A middle finger slides out of $n's left hand.", ch, NULL, NULL, TO_ROOM);
    act("A middle finger slides out of your left hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_M);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_M);
  }
  else if (IS_ARM_L(ch, BROKEN_FINGER_M))
  {
    act("$n's left middle finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left middle finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_M);
  }
  else if (IS_ARM_L(ch, LOST_FINGER_R))
  {
    act("A ring finger slides out of $n's left hand.", ch, NULL, NULL, TO_ROOM);
    act("A ring finger slides out of your left hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_R);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_R);
  }
  else if (IS_ARM_L(ch, BROKEN_FINGER_R))
  {
    act("$n's left ring finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left ring finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_R);
  }
  else if (IS_ARM_L(ch, LOST_FINGER_L))
  {
    act("A little finger slides out of $n's left hand.", ch, NULL, NULL, TO_ROOM);
    act("A little finger slides out of your left hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_L);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_L);
  }
  else if (IS_ARM_L(ch, BROKEN_FINGER_L))
  {
    act("$n's left little finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your left little finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_L);
  }
  else if (IS_ARM_R(ch, LOST_THUMB))
  {
    act("A thumb slides out of $n's right hand.", ch, NULL, NULL, TO_ROOM);
    act("A thumb slides out of your right hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_THUMB);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_THUMB);
  }
  else if (IS_ARM_R(ch, BROKEN_THUMB))
  {
    act("$n's right thumb snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right thumb snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_THUMB);
  }
  else if (IS_ARM_R(ch, LOST_FINGER_I))
  {
    act("An index finger slides out of $n's right hand.", ch, NULL, NULL, TO_ROOM);
    act("An index finger slides out of your right hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_I);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_I);
  }
  else if (IS_ARM_R(ch, BROKEN_FINGER_I))
  {
    act("$n's right index finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right index finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_I);
  }
  else if (IS_ARM_R(ch, LOST_FINGER_M))
  {
    act("A middle finger slides out of $n's right hand.", ch, NULL, NULL, TO_ROOM);
    act("A middle finger slides out of your right hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_M);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_M);
  }
  else if (IS_ARM_R(ch, BROKEN_FINGER_M))
  {
    act("$n's right middle finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right middle finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_M);
  }
  else if (IS_ARM_R(ch, LOST_FINGER_R))
  {
    act("A ring finger slides out of $n's right hand.", ch, NULL, NULL, TO_ROOM);
    act("A ring finger slides out of your right hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_R);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_R);
  }
  else if (IS_ARM_R(ch, BROKEN_FINGER_R))
  {
    act("$n's right ring finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right ring finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_R);
  }
  else if (IS_ARM_R(ch, LOST_FINGER_L))
  {
    act("A little finger slides out of $n's right hand.", ch, NULL, NULL, TO_ROOM);
    act("A little finger slides out of your right hand.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_L);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_L);
  }
  else if (IS_ARM_R(ch, BROKEN_FINGER_L))
  {
    act("$n's right little finger snaps back into place.", ch, NULL, NULL, TO_ROOM);
    act("Your right little finger snaps back into place.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_L);
  }
  else if (IS_BODY(ch, CUT_THROAT))
  {
    if (IS_SET(ch->loc_hp[6], BLEEDING_THROAT))
      return;
    act("The wound in $n's throat closes up.", ch, NULL, NULL, TO_ROOM);
    act("The wound in your throat closes up.", ch, NULL, NULL, TO_CHAR);
    REMOVE_BIT(ch->loc_hp[LOC_BODY], CUT_THROAT);
  }
  return;
}


int is_wall(EXIT_DATA * ex)
{

  if (IS_SET(ex->exit_info, EX_ICE_WALL))
    return 1;
  if (IS_SET(ex->exit_info, EX_FIRE_WALL))
    return 2;
  if (IS_SET(ex->exit_info, EX_SWORD_WALL))
    return 3;
  if (IS_SET(ex->exit_info, EX_PRISMATIC_WALL))
    return 4;
  if (IS_SET(ex->exit_info, EX_IRON_WALL))
    return 5;
  if (IS_SET(ex->exit_info, EX_MUSHROOM_WALL))
    return 6;
  if (IS_SET(ex->exit_info, EX_CALTROP_WALL))
    return 7;
  if (IS_SET(ex->exit_info, EX_ASH_WALL))
    return 8;
  return 0;
}

/*
 * Update all rooooooms, like gore, poison clouds etc....
 */
void room_update(void)
{
  int i;
  ROOM_INDEX_DATA *room;

  for (room = room_list; room != NULL; room = room->next_room)
  {

    if (RTIMER(room, RTIMER_SILENCE) < 0)
      room->tick_timer[i] = UMAX(room->tick_timer[i], 0);
    if (RTIMER(room, RTIMER_SILENCE) == 1)
      room_message(room, "The silence disappates.");

    for (i = 0; i < MAX_RTIMER; i++)
      room->tick_timer[i] = UMAX(room->tick_timer[i] - 1, 0);

  }

  return;

}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update(void)
{
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  QUEST_DATA *quest;
  QUEST_DATA *quest_next;
  CHAR_DATA *ch_quit = NULL;
  CHAR_DATA *ch_save = NULL;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  char buf[MAX_STRING_LENGTH];
  bool is_obj;
  time_t save_time;
  int count = 0, i;

  save_time = current_time;

  for (ch = char_list; ch != NULL; ch = ch_next)
  {
    ch_next = ch->next;
    count++;

    /*
     * Is the player an object ?
     */

        if (!IS_NPC(ch) && ch->desc) write_to_descriptor(ch->desc, "", 1);

    if (!IS_NPC(ch) && (IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)))
      is_obj = TRUE;
    else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
    {
      is_obj = TRUE;
      SET_BIT(ch->extra, EXTRA_OSWITCH);
    }
    else
      is_obj = FALSE;

    /* fae djinns */
    if (IS_NPC(ch))
    {
      if (ch->decay_pulse > 0)
      {
        if (--ch->decay_pulse <= 0)
        {
          act("$n vanishes in a puff of smoke.", ch, NULL, NULL, TO_ROOM);
          extract_char(ch, TRUE);
          continue;
        }
      }
    }

    /*
     * Tick Timers and other PC only stuff
     */
    if (!IS_NPC(ch))
    {
      if (ch->tick_timer[TIMER_CHALLENGE] == 1)
        send_to_char("You can use challenge again.\n\r", ch);

      for (i = 0; i < MAX_TIMER; i++)
        if (ch->tick_timer[i] > 0)
          ch->tick_timer[i]--;

      /*
       * update active quests
       */
      for (quest = ch->pcdata->quests; quest; quest = quest_next)
      {
        quest_next = quest->next;
        if (--quest->time > 0)
          continue;
        quest_from_char(ch, quest);
        send_to_char("You have failed to complete a quest.\n\r", ch);
      }

      /*
       * void, autosave, time bonus, etc
       */
      if ((ch->level < LEVEL_IMMORTAL || !ch->desc) && !is_obj && !IS_SET(ch->extra, EXTRA_AFK))
      {
        if ((ch->desc == NULL || ch->desc->connected == CON_PLAYING) && ch->level >= 2 && ch->save_time < save_time)
        {
          ch_save = ch;
          save_time = ch->save_time;
        }
        if (++ch->timer >= 12 && !in_fortress(ch))
        {
          if (ch->was_in_room == NULL && ch->in_room != NULL)
          {
            ch->was_in_room = ch->in_room;
            if (ch->fighting != NULL)
              stop_fighting(ch, TRUE);
            act("$n disappears into the void.", ch, NULL, NULL, TO_ROOM);
            send_to_char("You disappear into the void.\n\r", ch);
            save_char_obj(ch);
            char_from_room(ch);
            char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
          }
        }
        update_active_counters(ch);
        update_regen(ch, 2);

        if (ch->timer > 20)
          ch_quit = ch;
      }
    }

    /*
     * updating spells on all mobs and players
     */
    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      if (paf->duration > 0)
        paf->duration--;
      else
      {
        if (paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0)
        {
          if (paf->type > 0 && paf->type < MAX_SKILL && skill_table[paf->type].msg_off && !is_obj)
          {
            send_to_char(skill_table[paf->type].msg_off, ch);
            send_to_char("\n\r", ch);
          }
          else
          {
            sprintf(buf, "Update Bug : Bad affect on %s.", ch->name);
            bug(buf, 0);
          }
        }
        affect_remove(ch, paf);
      }
    }

    /*
     * Updating current position
     */
    if (ch->position == POS_MORTAL || ch->position == POS_STUNNED || ch->position == POS_INCAP)
    {
      update_pos(ch);
    }
    if (ch->position > POS_STUNNED && !is_obj)
    {
      if (ch->hit < ch->max_hit)
        ch->hit = UMIN(ch->hit + number_range(5, 10), ch->max_hit);
      if (ch->mana < ch->max_mana)
        ch->mana = UMIN(ch->mana + number_range(5, 10), ch->max_mana);
      if (ch->move < ch->max_move)
        ch->move = UMIN(ch->move + number_range(5, 10), ch->max_move);
    }
    else if (ch->position <= POS_STUNNED && !is_obj)
    {
      ch->hit = ch->hit + number_range(2, 4);
      update_pos(ch);
      if (ch->position > POS_STUNNED)
      {
        act("$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM);
        act("You clamber back to your feet.", ch, NULL, NULL, TO_CHAR);
      }
    }

    /*
     * Dealing damage due to missling limbs, etc.
     */
    if (ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL)
    {
      int dam = 0;

      if (IS_BLEEDING(ch, BLEEDING_HEAD))
      {
        act("A spray of blood shoots from the stump of $n's neck.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your neck.\n\r", ch);
        dam += number_range(20, 50);
      }
      if (IS_BLEEDING(ch, BLEEDING_THROAT))
      {
        act("Blood pours from the slash in $n's throat.", ch, NULL, NULL, TO_ROOM);
        send_to_char("Blood pours from the slash in your throat.\n\r", ch);
        dam += number_range(10, 20);
      }
      if (IS_BLEEDING(ch, BLEEDING_ARM_L))
      {
        act("A spray of blood shoots from the stump of $n's left arm.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your left arm.\n\r", ch);
        dam += number_range(10, 20);
      }
      else if (IS_BLEEDING(ch, BLEEDING_HAND_L))
      {
        act("A spray of blood shoots from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your left wrist.\n\r", ch);
        dam += number_range(5, 10);
      }
      if (IS_BLEEDING(ch, BLEEDING_ARM_R))
      {
        act("A spray of blood shoots from the stump of $n's right arm.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your right arm.\n\r", ch);
        dam += number_range(10, 20);
      }
      else if (IS_BLEEDING(ch, BLEEDING_HAND_R))
      {
        act("A spray of blood shoots from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your right wrist.\n\r", ch);
        dam += number_range(5, 10);
      }
      if (IS_BLEEDING(ch, BLEEDING_LEG_L))
      {
        act("A spray of blood shoots from the stump of $n's left leg.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your left leg.\n\r", ch);
        dam += number_range(10, 20);
      }
      else if (IS_BLEEDING(ch, BLEEDING_FOOT_L))
      {
        act("A spray of blood shoots from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your left ankle.\n\r", ch);
        dam += number_range(5, 10);
      }
      if (IS_BLEEDING(ch, BLEEDING_LEG_R))
      {
        act("A spray of blood shoots from the stump of $n's right leg.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your right leg.\n\r", ch);
        dam += number_range(10, 20);
      }
      else if (IS_BLEEDING(ch, BLEEDING_FOOT_R))
      {
        act("A spray of blood shoots from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM);
        send_to_char("A spray of blood shoots from the stump of your right ankle.\n\r", ch);
        dam += number_range(5, 10);
      }
      ch->hit = ch->hit - dam;
      if (ch->hit < 1)
        ch->hit = 1;
      update_pos(ch);
      ch->in_room->blood += dam;
      if (ch->in_room->blood > 1000)
        ch->in_room->blood = 1000;
    }
    if (IS_AFFECTED(ch, AFF_FLAMING) && !is_obj)
    {
      int dam;

      act("$n's flesh burns and crisps.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your flesh burns and crisps.\n\r", ch);
      dam = number_range(250, 300);
      ch->hit = ch->hit - dam;
      if (ch->hit < 1)
        ch->hit = 1;
      update_pos(ch);
    }

    /*
     * More damage stuff
     */
    if (IS_AFFECTED(ch, AFF_POISON) && !is_obj)
    {
      act("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You shiver and suffer.\n\r", ch);
      ch->hit = ch->hit - number_range(100, 200);
      if (ch->hit < 1)
        ch->hit = 1;
      if (number_range(1, 4) == 1)
      {
        REMOVE_BIT(ch->affected_by, AFF_POISON);
        send_to_char("You feel the poison leave your system.\n\r", ch);
      }
    }

    /*
     * and then we do some healing - messy ?
     */
    if (ch->position == POS_INCAP && !is_obj)
    {
      ch->hit = ch->hit + number_range(2, 4);
      update_pos(ch);
      if (ch->position > POS_INCAP)
      {
        act("$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM);
        send_to_char("Your wounds stop bleeding and seal up.\n\r", ch);
      }
      if (ch->position > POS_STUNNED)
      {
        act("$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You clamber back to your feet.\n\r", ch);
      }
    }
    else if (ch->position == POS_MORTAL && !is_obj)
    {
      ch->hit = ch->hit + number_range(2, 4);
      update_pos(ch);
      if (ch->position == POS_INCAP)
      {
        act("$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM);
        send_to_char("Your wounds begin to close, and your bones pop back into place.\n\r", ch);
      }
    }
  }


//   if (ch->pcdata->powers[POWER_TICK] > 0) ch->pcdata->powers[POWER_TICK]--;

  /*
   * Autosave, Autoquit checks
   */
  if (ch_save != NULL || ch_quit != NULL)
  {
    for (ch = char_list; ch != NULL; ch = ch_next)
    {
      ch_next = ch->next;
      if (ch == ch_save)
        save_char_obj(ch);
      if (ch == ch_quit)
        do_quit(ch, "");
    }
  }
  return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  for (obj = object_list; obj != NULL; obj = obj_next)
  {
    CHAR_DATA *rch;
    char *message;

    obj_next = obj->next;
    if (obj->timer <= 0 || --obj->timer > 0)
      continue;

    switch (obj->item_type)
    {
      default:
        message = "$p vanishes.";
        break;
      case ITEM_FOUNTAIN:
        message = "$p dries up.";
        break;
      case ITEM_CORPSE_NPC:
        message = "$p decays into dust.";
        break;
      case ITEM_CORPSE_PC:
        message = "$p decays into dust.";
        break;
      case ITEM_FOOD:
        message = "$p decomposes.";
        break;
      case ITEM_TRASH:
        message = "$p crumbles into dust.";
        break;
//      case ITEM_WEAPON:
//        message = "$p turns to fine dust and blows away.";
//        break;
      case ITEM_WALL:
        message = "$p flows back into the ground.";
        break;
      case ITEM_GRENADE:
        message = "$p explodes in a shower of flames.";
        break;
    }

    if (obj->carried_by != NULL)
    {
      act(message, obj->carried_by, obj, NULL, TO_CHAR);
    }
    else if (obj->in_room != NULL && (rch = obj->in_room->people) != NULL)
    {
      act(message, rch, obj, NULL, TO_ROOM);
      act(message, rch, obj, NULL, TO_CHAR);
    }


    /*
     * BUGFIX : The famous extract_obj bug....
     *          basicly we just make sure that we don't skip into the obj_free list
     *          by accident. (That's what caused it).
     */

    if (obj == object_list)
    {
      extract_obj(obj);
      obj_next = object_list;
    }
    else
    {
      OBJ_DATA *bugObj;

      for (bugObj = object_list; bugObj; bugObj = bugObj->next)
      {
        if (bugObj->next == obj)
          break;
      }
      /*
       * This shouldn't happen, but if it does, I want to make sure we never see that object again,
       * so we let extract_obj() try and handle it, terminating to avoid further corruption.
       */
      if (!bugObj)
      {
        bug("obj_update: obj %d not in object_list. Terminating obj_update.", obj->pIndexData->vnum);
        return;
      }
      extract_obj(obj);
      obj_next = bugObj->next;
    }
  }

  return;
}


void ww_update(void)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  float dam = 0;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (!IS_PLAYING(d) || (victim = d->character) == NULL || IS_NPC(victim) || IS_IMMORTAL(victim) || victim->in_room == NULL || victim->pcdata->chobj != NULL)
    {
      continue;
    }
    if (!IS_SET(d->character->in_room->room_flags, ROOM_BLADE_BARRIER))
      continue;

    act("The scattered blades on the ground fly up into the air ripping into you.", d->character, NULL, NULL, TO_CHAR);
    act("The scattered blades on the ground fly up into the air ripping into $n.", d->character, NULL, NULL, TO_ROOM);

    act("The blades drop to the ground inert.", d->character, NULL, NULL, TO_CHAR);
    act("The blades drop to the ground inert.", d->character, NULL, NULL, TO_ROOM);

    dam = number_range(7, 14);
    dam = dam / 100;
    dam = d->character->hit * dam;
    if (dam < 100)
      dam = 100;
    d->character->hit = d->character->hit - dam;
    if (d->character->hit < -10)
      d->character->hit = -10;
    update_pos(victim);
  }

  return;
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler(void)
{
  static int pulse_area;
  static int pulse_mobile;
  static int pulse_violence;
  static int pulse_point;
  static int pulse_ww;
  static int pulse_mudinfo;
  static int pulse_track;

  recycle_descriptors();

  if (--pulse_track <= 0)
  {
    pulse_track = 2;
    update_trackers();
  }
  if (++ragnarok_timer > PULSE_RAGNAROK && ragnarok)
  {
    ragnarok_stop();
    ragnarok_timer = 0;
  }
  if (--pulse_ww <= 0)
  {
    pulse_ww = PULSE_WW;
    ww_update();
  }
  if (arena_open)
  {
    pulse_arena = PULSE_ARENA - (45 * PULSE_PER_SECOND + 1);
    arena_open = FALSE;
  }
  if (++pulse_arena >= PULSE_ARENA - 45 * PULSE_PER_SECOND)
  {
    if (pulse_arena < PULSE_ARENA && !arenaopen)
      open_arena();
    else if (pulse_arena >= PULSE_ARENA)
    {
      pulse_arena = 0;
      close_arena();
    }
  }
  if (--pulse_area <= 0)
  {
    pulse_area = number_range(PULSE_AREA / 2, 3 * PULSE_AREA / 2);
    area_update();
  }

  if (--pulse_mudinfo <= 0)
  {
    pulse_mudinfo = PULSE_THIRTY;
    update_mudinfo();
    update_polls();
    recycle_dummys();           // every 30 secs is enough
  }

  if (--pulse_mobile <= 0)
  {
    pulse_mobile = PULSE_MOBILE;
    mobile_update();
  }

  if (--pulse_violence <= 0)
  {
    pulse_violence = PULSE_VIOLENCE;
    violence_update ( );
    update_midi     ( );
  }
  if (--pulse_point <= 0)
  {
    pulse_point = number_range(PULSE_TICK / 2, 3 * PULSE_TICK / 2);
    weather_update();
    char_update();
    obj_update();
    room_update();

    /*
     * The following is some excessive force.
     * If the mud crashes again within a pulse from last crash,
     * let it go down to avoid crashloops.
     */
    if (iDelete == 0)
      iDelete++;
    else if (iDelete == 1)
    {
      unlink(CRASH_TEMP_FILE);
      iDelete++;
    }

  }

  tail_chain();
  return;
}

void update_morted_timer(CHAR_DATA * ch)
{
  if (ch->hit > 0 && ch->pcdata->mortal > 0)
    ch->pcdata->mortal = 0;
  else
  {
    ch->pcdata->mortal += 1;
    if (ch->pcdata->mortal > 10 && !in_fortress(ch))
    {
      ch->hit = 100;
      update_pos(ch);
      char_from_room(ch);
      char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
      stc("You recieve a heal from the gods.\n\r", ch);
      ch->pcdata->mortal = 0;
      ch->fight_timer = 0;
    }
  }
}

void update_sit_safe_counter(CHAR_DATA * ch)
{
  if (IS_NPC(ch))
    return;

  if (ch->pcdata->safe_counter > 0)
  {
    ch->pcdata->safe_counter--;
    if (ch->pcdata->safe_counter == 0)
      send_to_char("You can now pkill or be pkilled again.\n\r", ch);
  }
  if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && ch->level > 2 && ch->level < 7 && getMight(ch) >= 150)
  {
    if (!IS_SET(ch->extra, EXTRA_AFK))
      ch->pcdata->sit_safe += number_range(5, 10);

    if ((ch->pcdata->sit_safe > 200 || ch->fight_timer != 0) && !ragnarok && !ch->fighting)
    {
      stc("\nThe gods are tired of granting you refuge.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
    }
  }
  else
  {
    if (ch->pcdata->sit_safe > 0)
      ch->pcdata->sit_safe -= 10;
    else
      ch->pcdata->sit_safe = 0;
  }
}

void update_drunks(CHAR_DATA * ch)
{
  if (ch->pcdata->condition[COND_DRUNK] > 10 && number_range(1, 10) == 1)
  {
    send_to_char("You hiccup loudly.\n\r", ch);
    act("$n hiccups.", ch, NULL, NULL, TO_ROOM);
  }
  if (ch->pcdata->condition[COND_DRUNK] > 0 && number_range(1, 10) == 5)
  {
    ch->pcdata->condition[COND_DRUNK]--;
    if (ch->pcdata->condition[COND_DRUNK] == 0)
      send_to_char("You feel sober again.\n\r", ch);
  }
}

void sex_update(CHAR_DATA * ch)
{
  if (ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
  {
    CHAR_DATA *vch;

    if (ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
    {
      ch->pcdata->stage[2] += 1;
      if ((vch = ch->pcdata->partner) != NULL &&
          !IS_NPC(vch) && vch->pcdata->partner == ch && ((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) || (ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
      {
        if (ch->in_room != vch->in_room)
          return;
        if (vch->pcdata->stage[2] >= 225 && ch->pcdata->stage[2] >= 225 && vch->pcdata->stage[2] < 240 && ch->pcdata->stage[2] < 240)
        {
          ch->pcdata->stage[2] = 240;
          vch->pcdata->stage[2] = 240;
        }
        if (ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
        {
          act("You thrust deeply between $N's warm, damp thighs.", ch, NULL, vch, TO_CHAR);
          act("$n thrusts deeply between your warm, damp thighs.", ch, NULL, vch, TO_VICT);
          act("$n thrusts deeply between $N's warm, damp thighs.", ch, NULL, vch, TO_NOTVICT);
          if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
            ch->pcdata->stage[2] = vch->pcdata->stage[2];
        }
        else if (ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
        {
          act("You squeeze your legs tightly around $N, moaning loudly.", ch, NULL, vch, TO_CHAR);
          act("$n squeezes $s legs tightly around you, moaning loudly.", ch, NULL, vch, TO_VICT);
          act("$n squeezes $s legs tightly around $N, moaning loudly.", ch, NULL, vch, TO_NOTVICT);
          if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
            ch->pcdata->stage[2] = vch->pcdata->stage[2];
        }
      }
      if (ch->pcdata->stage[2] >= 250)
      {
        if ((vch = ch->pcdata->partner) != NULL && !IS_NPC(vch) && vch->pcdata->partner == ch && ch->in_room == vch->in_room)
        {
          vch->pcdata->stage[2] = 250;
          if (ch->sex == SEX_MALE)
          {
            stage_update(ch, vch, 2, "xm-thrust");
            stage_update(vch, ch, 2, "xf-squeeze");
          }
          else
          {
            stage_update(vch, ch, 2, "xm-thrust");
            stage_update(ch, vch, 2, "xf-squeeze");
          }

          ch->pcdata->stage[0] = 0;
          vch->pcdata->stage[0] = 0;

          if (!IS_EXTRA(ch, EXTRA_EXP))
          {
            send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", ch);
            SET_BIT(ch->extra, EXTRA_EXP);
            ch->exp += 100000;
          }
          if (!IS_EXTRA(vch, EXTRA_EXP))
          {
            send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", vch);
            SET_BIT(vch->extra, EXTRA_EXP);
            vch->exp += 100000;
          }
        }
      }
    }
    else
    {
      if (ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 && ch->position != POS_RESTING)
      {
        if (ch->pcdata->stage[0] > 1)
          ch->pcdata->stage[0] -= 1;
        else
          ch->pcdata->stage[0] = 0;
      }
      else if (ch->pcdata->stage[2] > 0 && ch->pcdata->stage[0] < 1)
      {
        if (ch->pcdata->stage[2] > 10)
          ch->pcdata->stage[2] -= 10;
        else
          ch->pcdata->stage[2] = 0;
        if (ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
          send_to_char("You feel fully recovered.\n\r", ch);
      }
    }
  }
}

void regen_limb(CHAR_DATA * ch)
{
  if (ch->loc_hp[6] > 0)
  {
    int sn = skill_lookup("clot");

    (*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
  }
  else if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] + ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
    reg_mend(ch);
  return;
}

void update_arti_regen(CHAR_DATA * ch)
{
  if (ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    update_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_regen(CHAR_DATA * ch, int multiplier)
{
  int min = 5; // 5
  int max = 10; // 10
  int hit_gain = 0;
  int mana_gain = 0;
  int move_gain = 0;

  if (IS_NPC(ch))
    return;

  if (ch->position == POS_SLEEPING)
  {
    if (!IS_RACE(ch, RACE_TROLL))
    {
    min += 800;
    max += 1000;
    }
    else
    {
    min += 1400;
    max += 2000;
    }
    hit_gain += number_range(min, max);
    mana_gain += number_range(min, max);
    move_gain += number_range(min, max);
  }
  else
  {
    if (!IS_RACE(ch, RACE_TROLL))
    {
    min += 600;
    max += 700;
    }
    else
    {
    min += 1200;
    max += 1400;
    }
    hit_gain += number_range(min, max);
    mana_gain += number_range(min, max);
    move_gain += number_range(min, max);
  }
  ch->hit = UMIN(ch->hit + (hit_gain * multiplier), ch->max_hit);
  ch->mana = UMIN(ch->mana + (mana_gain * multiplier), ch->max_mana);
  ch->move = UMIN(ch->move + (move_gain * multiplier), ch->max_move);
  update_pos(ch);
  if (ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move)
    send_to_char("Your body has completely regenerated itself.\n\r", ch);
  return;
}

void update_active_counters(CHAR_DATA *ch)
{
  /* we do not update idle players */
  if (ch->timer >= 6) return;

  if (ch->pcdata->time_tick < 250) ch->pcdata->time_tick++;
}

