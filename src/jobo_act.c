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
#include <unistd.h>
#include <time.h>
#include "merc.h"

ALIAS_DATA * alias_free;

void add_vote                     args (( POLL_DATA *poll, CHAR_DATA *ch, int choice ));

void do_bountylist(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  sprintf(buf, " %s\n\r", get_dystopia_banner("Bountylist", 64));
  send_to_char(buf, ch);
  stc("  #CName               Bounty     Pkscore      Generation\n\r#n",ch);
  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if ( d->character != NULL )
    {
       if (!d->connected == CON_PLAYING) continue;
       if (d->character->level > 6) continue;
       if (!can_see(ch, d->character)) continue;
       sprintf(buf, "  #G%-15s#n    %5d       %5d           %d\n\r",
         d->character->name, d->character->pcdata->bounty,
         get_ratio(d->character), d->character->generation);
       send_to_char(buf, ch);
    }
  }
  sprintf(buf, " %s\n\r", get_dystopia_banner("", 64));
  send_to_char(buf, ch);
  return;
}

void do_dcredits(CHAR_DATA *ch, char *argument)
{
  do_help(ch, "dcredits");
  return;
}

void do_areaaffects(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT *paf;
  bool found = FALSE;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  send_to_char("This area has been enchanted with :\n\r\n\r", ch);
  for (paf = ch->in_room->area->affects; paf; paf = paf->next)
  {
    found = TRUE;
    if (paf->type == AREA_AFF_GZONE)            sprintf(buf, "  God's Zone");
    else if (paf->type == AREA_AFF_THORNS)      sprintf(buf, "  Thorns and Brambles");
    else if (paf->type == AREA_AFF_PESTILENCE)  sprintf(buf, "  Pestilence");
    else if (paf->type == AREA_AFF_BADMOON)     sprintf(buf, "  Bad Moon");
    else if (paf->type == AREA_AFF_GAIA)        sprintf(buf, "  Gaia's Blessing");
    else if (paf->type == AREA_AFF_CHAOS)       sprintf(buf, "  Chaotic Landscape");
    else sprintf(buf, "  Unknown");

    sprintf(buf2, "%s by %s for a duration of %d hours\n\r",
      buf, religion_table[paf->religion].truename, paf->duration);
    send_to_char(buf2, ch);
  }
  if (!found) send_to_char("  nothing.\n\r", ch);
  return;
}


void do_trigger(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char time[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int tick;

  argument = one_argument(argument, arg);
  one_argument(argument, time);

  if (arg[0] == '\0')
  {
    send_to_char("Trigger what ?\n\r", ch);
    return;
  }
  if (!is_number(time))
  {
    send_to_char("What should the timer be set at (a number please) ?\n\r", ch);
    return;
  }
  tick = atoi(time);
  if (tick < 1 || tick > 3)
  {
    send_to_char("Between 1 and 3 ticks please.\n\r", ch);
    return;
  }
  if ((obj = get_obj_carry(ch, arg)) == NULL)
  {
    send_to_char("You don't have that item.\n\r", ch);
    return;
  }
  if (obj->item_type != ITEM_GRENADE)
  {
    send_to_char("That is not a grenade or a bomb.\n\r", ch);
    return;
  }
  if (obj->timer > 0)
  {
    send_to_char("It's already ticking, better run.\n\r", ch);
    return;
  }
  obj->timer = tick;
  send_to_char("You set the timer.\n\r", ch);
  act("$n fiddles with $p.", ch, obj, NULL, TO_ROOM);
  WAIT_STATE(ch, 6);
  return;
}

void do_version(CHAR_DATA *ch, char *argument) 
{
  send_to_char("#0/======================================\\#n\n\r", ch);
  send_to_char("#0|#W Age of Heroes 1.3.1 #f 2009 Xrakisis, #0|#n\n\r", ch);
  send_to_char("#0|#W     with code changes by Tijer.      #0|#n\n\r", ch);
  send_to_char("#0\\=====================================/#n\n\r", ch);
  return;
}

void do_upkeep(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  send_to_char("#RUpkeep list#n\n\r\n\r",ch);
  if (IS_SET(ch->act, AFF_HIDE))          send_to_char("* You cannot be seen by other players.\n\r",ch);
  if (IS_SET(ch->act, PLR_WIZINVIS))      send_to_char("* You cannot be seen by other players.\n\r",ch);
  if (IS_SET(ch->newbits, NEW_DARKNESS))  send_to_char("* You are shrouded in a globe of darkness.\n\r",ch);
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE))   send_to_char("* You are standing in the shadowplane.\n\r",ch);
  if (IS_AFFECTED(ch, AFF_ETHEREAL))      send_to_char("* You are in ethereal form.\n\r",ch);
  if (IS_SET(ch->act,PLR_HOLYLIGHT))      send_to_char("* You have superior vision.\n\r",ch);
  if (IS_IMMUNE(ch,IMM_SHIELDED))         send_to_char("* You are shielded from scrying eyes.\n\r",ch);

  return;
}

/* The costy healer */
void do_healme(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (ch->in_room != NULL && ch->in_room->vnum != ROOM_VNUM_ALTAR)
  {
    send_to_char("The healer is located at the altar in midgaard.\n\r",ch);
    return;
  }
  if (ch->fight_timer > 0)
  {
    send_to_char("Not with a fighttimer.\n\r",ch);
    return;
  }
  if ( ch->pcgold < 300 )
  {
    send_to_char("The healer demands 300 gold as payment for his service.\n\r",ch);
    return;
  }
  ch->hit  = ch->max_hit;
  ch->mana = ch->max_mana;
  ch->move = ch->max_move;
  ch->loc_hp[0] = 0;
  ch->loc_hp[1] = 0;
  ch->loc_hp[2] = 0;
  ch->loc_hp[3] = 0;
  ch->loc_hp[4] = 0;
  ch->loc_hp[5] = 0;
  ch->loc_hp[6] = 0;
  send_to_char("The healer heals your wounds and takes 300 gold as payment.\n\r",ch);
  ch->pcgold -= 300;
  WAIT_STATE(ch, 36);
  return;
}

void do_pkscry(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *chroom;
  ROOM_INDEX_DATA *victimroom;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;

  if (get_ratio(ch) < 1000)
  {
    send_to_char("huh?\n\r",ch);
    return;
  }
  if ( arg[0] == '\0' )
  {
    send_to_char( "Scry on whom?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobs.\n\r",ch);
    return;
  }
  if (victim->trust > 6)
  {
    send_to_char("SPYING ON THE GODS!?!?!?!\n\r",ch);
    send_to_char("Someone is scrying you.\n\r",victim);
    return;
  }
  chroom = ch->in_room;
  victimroom = victim->in_room;
  char_from_room(ch);
  char_to_room(ch,victimroom);
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  { 
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch,"scry");
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
  }
  else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  {
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch,"scry");
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
  } 
  else
    do_look(ch,"scry");
  char_from_room(ch);
  char_to_room(ch,chroom);
  if (ch->level < 7) ch->fight_timer += 3;
  if (get_ratio(victim) > 1499)
  {
    send_eagle(victim);
    send_to_char("You feel a slight tingle.\n\r",victim);
  }
  return;
}

void do_pkobjscry(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  CHAR_DATA *victim; 
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *location;
  ROOM_INDEX_DATA *from;
  DESCRIPTOR_DATA *tmp;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  bool afk = FALSE;

  one_argument(argument, arg);
   
  if (IS_NPC(ch)) return;  
  if (get_ratio(ch) < 3500)
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ((obj = get_obj_world(ch, arg)) == NULL)
  {
    send_to_char("Nothing like that in hell, earth, or heaven.\n\r", ch);
    return;
  }
  if (IS_OBJ_STAT(obj, ITEM_NOLOCATE))
  {
    send_to_char("A powerful force prevents the scrying of this object.\n\r", ch);
    return;
  }
  if ((gch = obj->carried_by) != NULL)
  {
    if (gch->in_room == NULL)
    {
      send_to_char("All you see is a chaotic mass of colors, nothing seems right.\n\r",ch);
      return;
    }
    tmp = gch->desc;
    gch->desc = ch->desc;
    sprintf(buf,"A pair of eyes grows on %s's %s.\n\rThe eyes blink once, then disappear.\n\r", gch->name, obj->short_descr);
    for (victim = gch->in_room->people; victim != NULL; victim = victim->next_in_room)
    {
      if (victim == gch) continue;  // the victim cannot see this.
      send_to_char(buf,victim);
    }
    if (IS_SET(gch->extra, EXTRA_AFK))
    {
      afk = TRUE;
      REMOVE_BIT(gch->extra, EXTRA_AFK);
    }
    do_look(gch,"");
    gch->desc = tmp;
    if (afk) SET_BIT(gch->extra, EXTRA_AFK);
    return;
  }
  else if ((location = obj->in_room) != NULL)
  {
    sprintf(buf,"A pair of eyes grows on %s.\n\rThe eyes blink once, then disappear.\n\r", obj->short_descr);
    for (victim = location->people; victim != NULL; victim = victim->next_in_room)
    {
      if (victim == ch) continue; // the player is seeing through the item, and will not see the eyes even if he is in the room.
      send_to_char(buf,victim);
    }
    obj_from_room(obj);
    from = ch->in_room;  
    char_from_room(ch);
    char_to_room(ch, location);
    do_look(ch, "");
    char_from_room(ch);
    char_to_room(ch, from);
    obj_to_room(obj, location);
    return;
  }
  else  // inside something.
  {
    send_to_char("It's pitch black, wonder where this item is.\n\r",ch);
    return;
  }
}

void do_pkaura (CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char      arg [MAX_INPUT_LENGTH];
  char      buf [MAX_INPUT_LENGTH];
   
  argument = one_argument( argument, arg );
   
  if (IS_NPC(ch)) return;
  if (get_ratio(ch) < 2000)
  {
    send_to_char("huh?\n\r",ch);
    return;
  }
  if ( arg[0] == '\0' )
  {
    send_to_char("Read the aura of who?\n\r",ch);
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobs.\n\r",ch);
    return;
  }
  if (victim->trust > 6)
  {
    send_to_char("SPYING ON THE GODS!?!?!?!\n\r",ch);
    send_to_char("Someone is trying to read your aura.\n\r",victim);
    return;
  }
  sprintf(buf,"Hp:%d/%d, Mana:%d/%d,Move:%d/%d.\n\r",
    victim->hit,victim->max_hit,victim->mana,victim->max_mana,victim->move,victim->max_move);
  send_to_char(buf,ch);
  if (ch->trust < 7) send_to_char("You are being examined.\n\r",victim); 
  sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d.\n\r",char_hitroll(victim),char_damroll(victim),char_ac(victim));
  send_to_char(buf,ch);
  sprintf(buf,"Generation:%d\n\r",victim->generation);
  send_to_char(buf,ch);
  if (ch->level < 7) ch->fight_timer += 3;
  return;
}

void do_pkheal(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (get_ratio(ch) < 500)
  {
    send_to_char("huh?\n\r",ch);
    return;
  }
  if (ch->fight_timer > 0)
  {
    send_to_char("You cannot focus your mind.\n\r",ch);
    return;
  }
  ch->hit += UMIN(get_ratio(ch), 2500);
  if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
  send_to_char("You release your control and let harmony and peace flow over you.\n\r",ch);
  WAIT_STATE(ch,12);
  return;
}

void do_pkpowers(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (get_ratio(ch) < 500)
    send_to_char("You suck, get lost.\n\r",ch);
  if (get_ratio(ch) > 499)
    send_to_char("* Sanctum      - Concentrate on your inner peace.\n\r",ch);
  if (get_ratio(ch) > 999)
    send_to_char("* Eaglesight   - Scry on players even through shield.\n\r",ch);
  if (get_ratio(ch) > 1999)
    send_to_char("* Lifesense    - Global readaura.\n\r",ch);
  if (get_ratio(ch) > 2999)
    send_to_char("* Calltoarms   - Call for help during battle.\n\r",ch);
  if (get_ratio(ch) > 3499)
    send_to_char("* Objectscry   - See through the 'eyes' of an item.\n\r",ch);
  if (get_ratio(ch) > 3999)
    send_to_char("* Ironmind     - Prepare yourself mentally for battle.\n\r",ch);
  if (get_ratio(ch) > 4499)
    send_to_char("* Crystalsight - Gain superior sight for a short while.\n\r",ch);
  if (get_ratio(ch) > 4999)
    send_to_char("* Darkhunter   - The ability to summon The Dark Hunter.\n\r",ch);
  if (get_ratio(ch) > 2499)
    send_to_char("* You have enhanced fighting abilities.\n\r",ch);
  if (get_ratio(ch) > 1499)
    send_to_char("* You have enhanced awareness.\n\r",ch);
  return;
}

/* Mastery command to gain mastery items */

void do_mastery(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  int vnum;
  char buf[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch)) return;
  
  if (IS_SET(ch->newbits, NEW_MASTERY) && ch->level < 6)
  {
    send_to_char("You've already gotten your mastery. If you lost it, tough luck!\n\r",ch);
    return;
  }
  if (ch->wpn[0] < 200 || ch->wpn[1] < 200 || ch->wpn[2] < 200 || ch->wpn[3] < 200 || ch->wpn[4] < 200
   || ch->wpn[5] < 200 || ch->wpn[6] < 200 || ch->wpn[7] < 200 || ch->wpn[8] < 200
   || ch->wpn[9] < 200 || ch->wpn[10] < 200 || ch->wpn[11] < 200 || ch->wpn[12] < 200 )
  {
    send_to_char("Maybe you should grandmaster your weapons first.\n\r",ch);
    return;
  }
  if (ch->spl[0] < 200 || ch->spl[1] < 200 || ch->spl[2] < 200 || ch->spl[3] < 200 || ch->spl[4] < 200 )
  {
    send_to_char("Maybe you should be grand sorcerer in all spell colors first.\n\r",ch);
    return;
  }
/*  if (ch->stance[1] < 200 || ch->stance[2] < 200 || ch->stance[3] < 200 || ch->stance[4] < 200
   || ch->stance[5] < 200 || ch->stance[6] < 200 || ch->stance[7] < 200 || ch->stance[8] < 200
   || ch->stance[9] < 200 || ch->stance[10] < 200 )
  {
    send_to_char("Maybe you should grandmaster your stances first.\n\r",ch);
    return;
  }
*/
  /* no mastery items, sorry */
  {
    send_to_char("Your class mastery is not done yet, please notify Jobo that you want him to make it.\n\r",ch);
    return;
  }

  if ((pObjIndex = get_obj_index(vnum)) == NULL)
  {
    send_to_char("Missing object, inform Xrakisis.\n\r",ch);
    return;
  }
  obj = create_object(pObjIndex, 50);
  obj_to_char(obj,ch);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  sprintf(buf,"%s has achieved mastery.",ch->name);
  do_info(ch,buf);
  SET_BIT(ch->newbits, NEW_MASTERY);
  return;
}


void do_ragnarok( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  int amount;

  argument = one_argument(argument, arg);
  if (IS_NPC(ch)) return;
  if (ragnarok_timer < 120 * PULSE_AREA)
  {
    send_to_char("You may not call for ragnarok yet.\n\r",ch);
    return;
  }
  if (has_timer(ch)) return;
  if (arg[0] == '\0')
  {
    send_to_char("How many qps do you want to bid ?\n\r",ch);
    return;
  }
  if (!is_number(arg))
  {
    send_to_char("A number please, how many qps do you want to bid towards ragnarok.\n\r",ch);
    return;
  }
  amount = atoi(arg);
  if (amount < 100)
  {
    send_to_char("You cannot tempt the gods with this puny amount.\n\r",ch);
    return;
  }
  if (amount > 1000)
  {
    send_to_char("You don't want to spend this many qps to end the world.\n\r",ch);
    return;
  }
  if (ch->pcdata->quest < amount)
  {
    send_to_char("Hah!\n\r",ch);
    return;
  }
  ch->pcdata->quest -= amount;
  ragnarok_cost -= amount;
  if (ragnarok_cost <= 0)
  {
    ragnarok_cost = 3000;
    ragnarok = TRUE;
    ragnarok_timer = 0;
    do_info(ch,"#0The world comes to an end, #yRAGNAROK#0 is over us all!!!!#n");
  }
  else do_info(ch,"The ragnarok moves closer, the gods shiver with fear");
  return;
}

void do_timer( CHAR_DATA *ch, char *argument)
{
  int arenatime, ragnaroktime;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (arenaopen) arenatime = 0;
  else arenatime = (PULSE_ARENA - 45 * PULSE_PER_SECOND - pulse_arena);
  if (ragnarok) ragnaroktime = 0;
  else ragnaroktime = (120 * PULSE_AREA - ragnarok_timer);
  if (ragnaroktime < 0 ) ragnaroktime = 0;

  sprintf(buf, "Next arena will happen in     :  %3d minutes.\n\r", arenatime/PULSE_AREA);
  send_to_char(buf,ch);

  sprintf(buf, "Bidding on ragnarok starts in :  %3d minutes.\n\r", ragnaroktime/PULSE_AREA);
  send_to_char(buf,ch);

  return;
}

void do_exp( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  unsigned int cost, to, from;

  argument = one_argument(argument,arg1);
  argument = one_argument(argument,arg2);

  if (IS_NPC(ch)) return;
  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Usage is :\n\r",ch);
    send_to_char("exp <from> <to>\n\r",ch);
    send_to_char("ie. exp 4000 6000 will tell you how many exp it will cost to train from 4K to 6K\n\r",ch);
    send_to_char("can be used for primal as wel, just time the result by 500.\n\r", ch);
    return;
  }
  if (!is_number(arg1) || !is_number(arg2))
  {
    send_to_char("Please use numbers.\n\r",ch);
    return;
  }
  from = atoi(arg1);
  to = atoi(arg2);
  if (from >= to)
  {
    send_to_char("Begin with a smaller number.\n\r",ch);
    return;
  }
  if (to > 120000)
  {
    send_to_char("Thats beyond the statcap.\n\r",ch);
    return;
  }
  if ((to > 50000 && to - from > 20000) || (to > 100000 && to - from > 10000) || (to - from > 50000))
  {
    send_to_char("Please use a smaller interval to avoid integer overflows.\n\r",ch);
    return;
  }
  cost = (from + to) * (to - from + 1) / 2 - from;
  if (cost > 500000000)
    sprintf(buf,"The total cost in exp will be %d million\n\r", cost/1000000);
  else sprintf(buf,"The total cost in exp will be %d\n\r", cost);
  send_to_char(buf,ch);
  return;
}

void do_favor( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument,arg);

  if (IS_NPC(ch)) return;
  if (IS_SET(ch->act, PLR_RIGHTHAND) || IS_SET(ch->act, PLR_LEFTHAND) || IS_SET(ch->act, PLR_AMBI))
  {
    send_to_char("Already set.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Favor left, right or both hands ?\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"left"))
  {
    SET_BIT(ch->act, PLR_LEFTHAND);
    send_to_char("You favor your left arm in combat.\n\r",ch);
  }
  else if (!str_cmp(arg,"right"))
  {
    SET_BIT(ch->act, PLR_RIGHTHAND);
    send_to_char("You favor your right arm in combat.\n\r",ch);
  }
  else if (!str_cmp(arg,"both"))
  {
    SET_BIT(ch->act, PLR_AMBI);
    send_to_char("You fight well with both arms.\n\r",ch);
  }
  else do_favor(ch,"");
  return;
}

void do_alias(CHAR_DATA *ch, char *argument)
{
  ALIAS_DATA *ali;
  char arg1[MAX_INPUT_LENGTH];
  char testarg[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;

  argument = one_argument(argument, arg1);
  one_argument(argument, testarg);

  if ((ch->in_room == NULL || ch->in_room->vnum != 3054))
  {
    send_to_char("You must be at the Temple Altar of Midgaard to create a new alias.\n\r",ch);
    return;
  }
  if (ch->pcdata->alias_count >= MAX_ALIAS)
  {
    send_to_char("Sorry, you have reached the limit for aliases, you need to remove some before you can make more.\n\r",ch);
    return;
  }
  if (arg1[0] == '\0' || argument[0] == '\0')
  {
    send_to_char("Syntax : alias 'short' 'long'\n\r",ch);
    send_to_char("\n\rThe 'short' is the alias, and the 'long' is the string that the alias replaces.\n\r",ch);
    send_to_char("ie. alias tt teleport orc.\n\r",ch);
    return;
  }
  if (strlen2(argument) > 400)
  {
    send_to_char("Behave... that's way to big.\n\r",ch);
    return;
  }
  smash_tilde(arg1);
  smash_tilde(testarg);
  smash_tilde(argument);
  if (!str_cmp(arg1, testarg))
  {
    send_to_char("You cannot alias the alias.\n\r",ch);
    return;
  }
  if (!str_cmp(arg1, "showalias") || !str_cmp(arg1, "alias") || !str_cmp(arg1, "removealias"))
  {
    send_to_char("You really shouldn't alias the alias commands.\n\r", ch);
    return;
  }
  for (ali = ch->pcdata->alias; ali; ali = ali->next)
  {
    if (!str_cmp(arg1, ali->short_n))
    {
      send_to_char("You already have such an alias.\n\r",ch);
      return;
    }
    if (!str_cmp(arg1, ali->long_n) || !str_cmp(argument, ali->short_n) || !str_cmp(argument, ali->long_n))
    {
      send_to_char("No looping aliases please.\n\r", ch);
      return;
    }
  }
  if (alias_free == NULL )
  {
    ali = alloc_perm(sizeof(*ali));
  }
  else
  {
    ali = alias_free;
    alias_free = alias_free->next;
  }
  ali->short_n = str_dup(arg1);
  ali->long_n = str_dup(argument);
  ali->next = ch->pcdata->alias;
  ch->pcdata->alias = ali;
  ch->pcdata->alias_count++;
  send_to_char("Ok.\n\r",ch);
  return;
}

void do_showalias(CHAR_DATA *ch, char *argument)
{
  ALIAS_DATA *ali;
  bool found = FALSE;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  for (ali = ch->pcdata->alias; ali; ali = ali->next)
  {
    found = TRUE;
    sprintf(buf, "%s '%s'\n\r", ali->short_n, ali->long_n);
    send_to_char(buf,ch);
  }
  if (!found) send_to_char("You have no aliases.\n\r",ch);
  return;
}

void do_removealias(CHAR_DATA *ch, char *argument)
{
  ALIAS_DATA *ali;
  char arg[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Remove which alias?\n\r",ch);
    return;
  }
  for (ali = ch->pcdata->alias; ali; ali = ali->next)
  {
    if (!str_cmp(arg, ali->short_n))
    {
      alias_remove(ch, ali);
      send_to_char("Alias removed.\n\r",ch);
      ch->pcdata->alias_count--;
      return;
    }
  }
  send_to_char("No such alias.\n\r",ch);
  return;
}

void do_birth(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *dummy;

  if (IS_NPC(ch)) return;

  argument = one_argument(argument, arg);

  if (ch->sex != SEX_FEMALE)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_EXTRA(ch, EXTRA_PREGNANT))
  {
    send_to_char("But you are not even pregnant!\n\r",ch);
    return;
  }
  if (!IS_EXTRA(ch, EXTRA_LABOUR))
  {
    send_to_char("You're not ready to give birth yet.\n\r",ch);
    return;
  }
  if ( arg[0] == '\0' )
  {
    if (ch->pcdata->genes[4] == SEX_MALE)
      send_to_char( "What do you wish to name your little boy?\n\r", ch );
    else if (ch->pcdata->genes[4] == SEX_FEMALE)
      send_to_char( "What do you wish to name your little girl?\n\r", ch );
    else
      send_to_char( "What do you wish to name your child?\n\r", ch );
    return;
  }
  if (!check_parse_name(arg))
  {
    send_to_char("Thats an illegal name.\n\r", ch);
    return;
  }
  if (char_exists(FALSE, arg))
  {
    send_to_char("That player already exists.\n\r", ch);
    return;
  }
  if (descriptor_free == NULL )
  {
    dummy = alloc_perm(sizeof(*dummy));
  }
  else
  {
    dummy = descriptor_free;
    descriptor_free = descriptor_free->next;
  }
  arg[0] = UPPER(arg[0]);
  load_char_short(dummy, arg);
  victim = dummy->character;

  victim->pcdata->perm_str = 18;
  victim->pcdata->perm_int = 18;
  victim->pcdata->perm_wis = 18;
  victim->pcdata->perm_dex = 18;
  victim->pcdata->perm_con = 18;
  victim->stance[19] = -1;
  victim->stance[20] = -1;  
  victim->stance[21] = -1;
  victim->stance[22] = -1;
  victim->stance[23] = -1;
  victim->class = 0;
  victim->max_hit = 5000;
  victim->hit = 5000;
  victim->max_mana = 5000;
  victim->mana = 5000;
  victim->max_move = 5000;
  victim->move = 5000;
  victim->sex = ch->pcdata->genes[4];
  victim->level = 2;
  victim->generation = 6;
  victim->in_room = ch->in_room;
  save_char_obj(victim);
  free_char(victim);

  /*
   * Being nice and putting the descriptor back into the free list.
   */
  dummy->next     = descriptor_free;
  descriptor_free = dummy;

  ch->pcdata->genes[9] += 1;
  REMOVE_BIT(ch->extra, EXTRA_PREGNANT);
  REMOVE_BIT(ch->extra, EXTRA_LABOUR);  
  save_char_obj(ch);
  if (ch->pcdata->genes[4] == SEX_MALE)
    sprintf(buf, "%s gives birth to %s's son, named %s!", ch->name, ch->pcdata->cparents, arg);
  else
    sprintf(buf, "%s gives birth to %s's daughter, named %s!", ch->name, ch->pcdata->cparents, arg);
  do_info(ch, buf);
  send_to_char("Pop!\n\r", ch);
  return;
}

void do_selfdelete(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (argument[0] == '\0')
  {
    send_to_char("Syntax : selfdelete <your password>\n\r", ch);
    send_to_char("WARNING : THIS WILL REMOVE YOU PERMANENTLY!!\n\r", ch);
    return;
  }

  if (has_timer(ch)) return;

  /*
   * check for password.
   */
  if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
  {
    send_to_char("Illegal password.\n\r", ch);
    WAIT_STATE(ch, 12);
    return;
  }

  /*
   * logged.
   */
  sprintf(buf, "%s has selfdeleted, what a fool.", ch->name);
  log_string(buf);

  /*
   * set up the deletion
   */
  sprintf(buf, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->switchname));

  /*
   * make sure the player is offline.
   */
  ch->fight_timer = 0;   
  char_from_room(ch);
  char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
  do_quit(ch, "");

  unlink(buf);

  return;
}

void do_leader( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  
  if (IS_NPC(ch)) return;
  
  sprintf(buf, " %s\n\r\n\r", get_dystopia_banner("Leader Board", 64));
  send_to_char(buf, ch);
  
  send_to_char("   #oMost Player Kills  #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.pk_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oKills#n\n\r", leader_board.pk_number);
  send_to_char(buf, ch);
  
  send_to_char("   #oMost Hours Played  #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.time_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oHours#n\n\r", leader_board.time_number);
  send_to_char(buf, ch);
 
  send_to_char("   #oMost Gold Earned   #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.quest_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oGold#n\n\r", leader_board.quest_number);
  send_to_char(buf, ch);
  
  send_to_char("   #oMost Mobs Killed   #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.mobkills_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oKills#n\n\r", leader_board.mobkills_number);
  send_to_char(buf, ch);
    
  send_to_char("   #oHighest PK score   #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.pkscore_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oPoints#n\n\r", leader_board.pkscore_number);
  send_to_char(buf, ch);
    
  send_to_char("   #oMost Arena Wins    #C--->   ", ch);
  sprintf(buf, "#G%-13s", leader_board.arena_name);
  send_to_char(buf, ch);
  sprintf(buf, " #owith #G%d #oWins#n\n\r", leader_board.arena_number);
  send_to_char(buf, ch);

  sprintf(buf, "\n\r %s\n\r", get_dystopia_banner("", 64));
  send_to_char(buf, ch);
    
  return;
}

void do_policy(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  argument = one_argument(argument, arg);
  if (IS_SET(ch->pcdata->jflags, JFLAG_POLICY))
  {
    send_to_char("You have already accepted the policy.\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "accept"))
  {
    if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
    {
      send_to_char("Illegal password.\n\r", ch);  
      WAIT_STATE(ch, 12);
      return;
    }
    send_to_char("You have accepted the player policy at Dystopia.\n\r", ch);
    SET_BIT(ch->pcdata->jflags, JFLAG_POLICY);
    do_autosave(ch, "");
    sprintf(buf, "%s has accepted the player policy.", ch->name);
    log_string(buf);
    return;
  }
  else if (!str_cmp(arg, "decline"))
  {
    if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
    {
      send_to_char("Illegal password.\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
    }
    sprintf(buf, "%s has declined the player policy.", ch->name);
    log_string(buf);
    sprintf(buf, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->switchname));
    ch->fight_timer = 0;
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
    do_quit(ch, "");
    unlink(buf);
  }
  else
  {
    send_to_char("Syntax : policy [accept|decline] [password]\n\r", ch);
    return;
  }
  return;
}

void do_vote(CHAR_DATA *ch, char *argument)
{
  POLL_DATA *poll;
  VOTE_DATA *vote;
  char *strtime;  
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char buf2[20];
  bool found = FALSE;
  int i, choice;
  
  if (IS_NPC(ch)) return;
  
  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);
  
  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("      #R[#G***#R] #CCurrent Polls at Dystopia #R[#G***#R]#n\n\r", ch);
    for (poll = poll_list; poll; poll = poll->next)
    {
      strtime = ctime(&poll->expire);
      for (i = 0; i < 6; i++)
      {
        buf2[i] = strtime[i + 4];
      }
      buf2[6] = '\0';
      sprintf(buf, "\n\r  A poll on #y%s#n with the following options. (expires %s)\n\r\n\r",
        poll->name, buf2);
      send_to_char(buf, ch);
      for (i = 0; i < MAX_VOTE_OPTIONS; i++)
      {
        if (!str_cmp("<null>", poll->options[i])) continue;
        sprintf(buf, " #0[#R%2d#0] #y%s\n\r", i+1, poll->options[i]);
        send_to_char(buf, ch);
      }
    }
    send_to_char("\n\r  #CPlease read HELP VOTE for details on voting.#n\n\r", ch);
    return;
  }
  if ((get_age(ch) - 17) < 5 || ch->mkill < 1000)
  {
    send_to_char("Please read HELP VOTE.\n\r", ch);
    return;
  }
  for (poll = poll_list; poll; poll = poll->next)
  {
    if (str_cmp(arg1, poll->name)) continue;
    found = TRUE;
    break;
  }
  if (!found)
  {
    send_to_char("Sorry, no such poll.\n\r", ch);
    return;
  }
  found = FALSE;
  for (vote = poll->votes; vote; vote = vote->next)
  {
    if (str_cmp(ch->name, vote->pname)) continue;
    found = TRUE;
    break;
  }
  if (found)
  {
    send_to_char("You have already voted on this poll.\n\r", ch);
    return;
  }
  for (i = 0; i < MAX_VOTE_OPTIONS; i++)
  {  
    if (str_cmp(poll->options[i], "<null>")) continue;
    break; 
  }
  if ((choice = atoi(arg2)) < 1 || choice > i)   
  {
    send_to_char("Please pick a valid choice.\n\r", ch);
    return;
  }
  add_vote(poll, ch, choice);
  send_to_char("You vote has been accepted.\n\r", ch);
  save_subvotes(poll);
  return;
}

void add_vote(POLL_DATA *poll, CHAR_DATA *ch, int choice)
{  
  VOTE_DATA *vote;
     
  if (!ch->desc) return;
  if (!ch->desc->host) return;
   
  poll->vcount[choice-1]++;
   
  vote = alloc_perm(sizeof(*vote));
  vote->pname = str_dup(ch->name);
  vote->phost = str_dup(ch->desc->host);
  vote->choice = choice;
  
  vote->next = poll->votes;
  poll->votes = vote;
}


void do_score(CHAR_DATA *ch, char *argument)
{
  double lgainexp  = 1550000 * ch->pcRaceLevel;
  char buf[MAX_STRING_LENGTH];
  char tempbuf[MAX_STRING_LENGTH];
  char linebuf1[MAX_STRING_LENGTH];
  char linebuf2[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  sprintf(linebuf1, " %s\n\r", generate_banner("#yAGE OF HEROES", 66, "#0"));
  sprintf(linebuf2, "\n\r %s\n\r", generate_banner("", 66, "#0"));

  send_to_char(linebuf1, ch);
  send_to_char("\n\r", ch);

  sprintf(buf, "  Your name is #C%s #nand you have been playing for #C%d #nhours\n\r",
    ch->name, (ch->played + (int) (current_time - ch->logon))/3600);
  send_to_char(buf, ch);
  birth_date(ch, FALSE);
  send_to_char("\n\r", ch);

  sprintf(tempbuf, "  STR: #G%2d#n    Hitpoints : #C%d#n/#C%d#n",
    get_curr_str(ch), ch->hit, ch->max_hit);
  sprintf(buf, "%-55s #uLast two kills#n\n\r", tempbuf);
  send_to_char(buf, ch);

  sprintf(tempbuf, "  DEX: #G%2d#n    Movement  : #C%d#n/#C%d#n",
    get_curr_dex(ch), ch->move, ch->max_move);
  sprintf(buf, "%-55s #C%s#n\n\r", tempbuf, ch->pcdata->last_decap[0]);
  send_to_char(buf, ch);

  sprintf(tempbuf, "  CON: #G%2d#n    Magics    : #C%d#n/#C%d#n",
    get_curr_con(ch), ch->mana, ch->max_mana);
  sprintf(buf, "%-55s #C%s#n\n\r", tempbuf, ch->pcdata->last_decap[1]);
  send_to_char(buf, ch);

  sprintf(tempbuf, "  WIS: #G%2d#n    PKscore   : #y%d#n",
    get_curr_wis(ch), get_ratio(ch));
  sprintf(buf, "%-51s #uRetaliation#n\n\r", tempbuf);
  send_to_char(buf, ch);

  sprintf(tempbuf, "  INT: #G%2d#n    Quest pts : #y%d#n/#C%d#n",
    get_curr_int(ch), ch->pcdata->quest, ch->pcdata->questtotal);
  sprintf(buf, "%-55s #C%s#n\n\r", tempbuf, ch->pcdata->retaliation);
  send_to_char(buf, ch);

  send_to_char(linebuf2, ch);
  send_to_char("\n\r", ch);

  sprintf(buf, "  Damroll   : #C%6d#n      Damcap : #C%6d#n    Saving Throw : #C%d#n (%s%d)\n\r",
    char_damroll(ch), ch->damcap[0],
    (ch->spl[0] + ch->spl[1] + ch->spl[2] + ch->spl[3] + ch->spl[4]) / 20,
    (ch->saving_throw >= 0) ? "+" : "", ch->saving_throw);
  stc(buf, ch);  
  sprintf(buf, "  Hitroll   : #C%6d#n      Armor  : #C%6d#n    Paradox Tick : #C%d#n\n\r",
    char_hitroll(ch), char_ac(ch), ch->pcdata->mean_paradox_counter);
  stc(buf, ch);  
  
  sprintf(buf, "  Primal    : #C%6d#n      Level  : #C%6d#n    Remort Level : #C%d#n\n\r",
    ch->practice, ch->pcRaceLevel, ch->pcremort);
  stc(buf, ch);  
  sprintf(buf, "  Gold      : #C%6d#n      Silver : #C%6d#n    Copper       : #C%d#n\n\r", ch->pcgold, ch->pcsilver, ch->pccopper);
  stc(buf, ch);    
  
  sprintf(buf, "  Alignment : #C%6d#n      Legacy : #C%3d#n/#C20#n\n\r", ch->alignment, ch->pcLegacy);
  stc(buf, ch);    

  if (ch->pcRaceLevel < 200)
     sprintf(buf, "\n\r  You have #C%2.0f#n experience, and need #C%2.0f#n experience to gain a level\n\r", ch->exp, (lgainexp-ch->exp));
  else
     sprintf(buf, "\n\r  You have #C%2.0f#n experience\n\r", ch->exp);
  stc(buf, ch);
  sprintf(buf, "\n\r  You are carrying #C%d#n/#C%d#n items with weight #C%d#n/#C%d#n lbs\n\r",
    ch->carry_number, can_carry_n(ch), ch->carry_weight, can_carry_w(ch));
  stc(buf, ch);  

  send_to_char(linebuf2, ch);
  send_to_char("\n\r", ch);

  sprintf(tempbuf, "  Players  : #C%6d#n %s and #C%3d#n %s",
    ch->pkill, (ch->pkill == 1) ? "kill" : "kills",
    ch->pdeath, (ch->pdeath == 1) ? "death" : "deaths");
  sprintf(buf, "%-50s Ratio #C%3d#n%s\n\r",
    tempbuf, calc_ratio(ch->pkill, ch->pdeath), "%");
  send_to_char(buf, ch);

  sprintf(tempbuf, "  Arena    : #C%6d#n %s and #C%3d#n %s",
    ch->pcdata->awins, (ch->pcdata->awins == 1) ? "kill" : "kills",
    ch->pcdata->alosses, (ch->pcdata->alosses == 1) ? "death" : "deaths");
  sprintf(buf, "%-50s Ratio #C%3d#n%s\n\r",
    tempbuf, calc_ratio(ch->pcdata->awins, ch->pcdata->alosses), "%");
  send_to_char(buf, ch);

  sprintf(buf, "  Monsters : #C%6d#n %s and #C%3d#n %s\n\r",
    ch->mkill, (ch->mkill == 1) ? "kill" : "kills",
    ch->mdeath, (ch->mdeath == 1) ? "death" : "deaths");
  send_to_char(buf, ch);

  send_to_char(linebuf2, ch);
  return;
}

void extra_action(CHAR_DATA *ch, char *argument, int type)
{
  EXTRA_DESCR_DATA *ed;
  ROOM_INDEX_DATA *location;
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  CHAR_DATA *gch;
  CHAR_DATA *gch_next;
  char arg[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  int sn;

  one_argument(argument, arg);
 
  for (ed = ch->in_room->extra_descr; ed; ed = ed->next)
  {
    if (is_full_name(arg, ed->keyword))
    {
      if (ed->type == type)
      {
        switch(type)
        {
          case ED_TYPE_PULL:
            sprintf(buf1, "You pull %s.\n\r", ed->keyword);
            sprintf(buf2, "$n pulls %s.", ed->keyword);
            break;
          case ED_TYPE_PRESS:
            sprintf(buf1, "You press %s.\n\r", ed->keyword);
            sprintf(buf2, "$n presses %s.", ed->keyword);
            break;
          case ED_TYPE_PUSH:
            sprintf(buf1, "You push %s.\n\r", ed->keyword);
            sprintf(buf2, "$n pushes %s.", ed->keyword);
            break;
          case ED_TYPE_TOUCH:
            sprintf(buf1, "You touch %s.\n\r", ed->keyword);
            sprintf(buf2, "$n touches %s.", ed->keyword);
            break;
        }
        send_to_char(buf1, ch);
        act(buf2, ch, NULL, NULL, TO_ROOM);
      }
      else
      {
        switch(type)
        {
          case ED_TYPE_PULL:
            send_to_char("That cannot be pulled.\n\r", ch);
            break;
          case ED_TYPE_PUSH:
            send_to_char("That cannot be pushed.\n\r", ch);
            break;
          case ED_TYPE_PRESS:
            send_to_char("That cannot be pressed.\n\r", ch);
            break;
          case ED_TYPE_TOUCH:
            send_to_char("You seem unable to touch it.\n\r", ch);
            break;
        }
        return;
      }
      switch(ed->action)  
      {
        default:
          send_to_char("Nothing happens.\n\r", ch);
          return;
        case ED_ACTION_TELEPORT:
          if ((location = get_room_index(ed->vnum)) == NULL)
          {
            send_to_char("Nothing happens.\n\r", ch);
            return;
          }
          act(ed->buffer1, ch, NULL, NULL, TO_CHAR);
          act(ed->buffer2, ch, NULL, NULL, TO_ROOM);
          char_from_room(ch);
          char_to_room(ch, location);
          do_look(ch, "auto");
          return;
        case ED_ACTION_OBJECT:
          if ((pObjIndex = get_obj_index(ed->vnum)) == NULL)
          {
            send_to_char("Bug : Please report this.\n\r", ch);
            return;
          }
          obj = create_object( pObjIndex, 50);
          if (CAN_WEAR(obj, ITEM_TAKE)) obj_to_char(obj, ch);
          else obj_to_room(obj, ch->in_room);
          act(ed->buffer1, ch, NULL, NULL, TO_CHAR);
          act(ed->buffer2, ch, NULL, NULL, TO_ROOM);
          return;  
        case ED_ACTION_SPELL:
          if ((sn = skill_lookup(ed->buffer1)) < 0)
          {
            send_to_char("Bug : Please report this.\n\r", ch);
            return;
          }
          (*skill_table[sn].spell_fun) (sn, number_range(40,50), ch, ch);
          return;
        case ED_ACTION_ELEVATOR:
          if ((location = get_room_index(ed->vnum)) == NULL)
          {
            send_to_char("Bug : Please report this.\n\r", ch);
            return;
          }
          if (location->people)
          {
            act(ed->buffer2, ch, NULL, NULL, TO_ALL);
            return;
          }
          for (gch = ch->in_room->people; gch; gch = gch_next)
          {
            gch_next = gch->next_in_room;
            char_from_room(gch);
            char_to_room(gch, location);
            act(ed->buffer1, gch, NULL, NULL, TO_CHAR);
          }
          return;
      }
    }
  }
  send_to_char("You are unable to find that.\n\r", ch);
}

void do_pull(CHAR_DATA *ch, char *argument)
{
  extra_action(ch, argument, ED_TYPE_PULL);
}

void do_press(CHAR_DATA *ch, char *argument)
{
  extra_action(ch, argument, ED_TYPE_PRESS);
}

void do_push(CHAR_DATA *ch, char *argument)
{
  extra_action(ch, argument, ED_TYPE_PUSH);
}

void do_touch(CHAR_DATA *ch, char *argument)
{
  extra_action(ch, argument, ED_TYPE_TOUCH);
}

void do_ignore(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int i;
  
  if (IS_NPC(ch)) return;
  one_argument(argument, arg);
  
  if (arg[0] == '\0')
  {
    sprintf(buf, "You are currently ignoring :\n\r");
    for (d = descriptor_list; d; d = d->next)
    {
      if (d->connected != CON_PLAYING) continue;
      if ((victim = d->character) == NULL) continue;
      for (i = 0; i < MAX_IGNORE; i++)
      {
        if (ch->pcdata->ignore[i] == victim->pcdata->playerid)
        {
          strcat(buf, "* ");  
          strcat(buf, victim->name);
          strcat(buf, "\n\r");
        }
      }
    }
    send_to_char(buf, ch);
    return;
  }
  if (!str_cmp(arg, "clear"))
  {
    for (i = 0; i < MAX_IGNORE; i++)
    {
      ch->pcdata->ignore[i] = -1;
    }  
    send_to_char("All ignores cleared.\n\r", ch);
    return;
  }
  for (victim = char_list; victim; victim = victim->next)
  {
    if (IS_NPC(victim)) continue;
    if (!str_prefix(victim->name, arg))
    {
      for (i = 0; i < MAX_IGNORE; i++)
      {
        if (ch->pcdata->ignore[i] == victim->pcdata->playerid)
        {
          ch->pcdata->ignore[i] = -1;
          sprintf(buf, "You stop ignoring %s.\n\r", victim->name);
          send_to_char(buf, ch);
          return;
        }
      }
      for (i = 0; i < MAX_IGNORE; i++)
      {
        if (ch->pcdata->ignore[i] == -1)
        {
          ch->pcdata->ignore[i] = victim->pcdata->playerid;
          sprintf(buf, "You now ignore %s.\n\r", victim->name);
          send_to_char(buf, ch);
          return;
        }
      }
      send_to_char("You cannot ignore any more people, try 'ignore clear'.\n\r", ch);
      return;
    }
  }
  send_to_char("Ignore whom?\n\r", ch);
  return;
}

void do_vt100(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->tempflag, TEMP_VT100))
  {
    REMOVE_BIT(ch->pcdata->tempflag, TEMP_VT100);
    send_to_char(VT_RESET_TERMINAL, ch);
    return;
  }
  if (!IS_SET(ch->in_room->room_flags, ROOM_SAFE))
  {
    send_to_char("You cannot enable vt100 outside safe rooms.\n\r", ch);
    return;
  }
  send_to_char("Requesting VT100 information from your client.\n\r", ch);
  send_to_char("\e[999;999H", ch);
  send_to_char("\e[6n", ch);
  return;
}

void do_setdecap(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
  
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument) || !is_contained2("$N", argument))
  {
    send_to_char("You really need to include both $n and $N in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETDECAP)) SET_BIT(ch->pcdata->jflags, JFLAG_SETDECAP);
  free_string(ch->pcdata->decapmessage);
  ch->pcdata->decapmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}
 
void do_settie(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
  
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument) || !is_contained2("$N", argument))
  {
    send_to_char("You really need to include both $n and $N in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETTIE)) SET_BIT(ch->pcdata->jflags, JFLAG_SETTIE);
  free_string(ch->pcdata->tiemessage);
  ch->pcdata->tiemessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}
 
void do_setlogout(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
  
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETLOGOUT)) SET_BIT(ch->pcdata->jflags, JFLAG_SETLOGOUT);
  free_string(ch->pcdata->logoutmessage);
  ch->pcdata->logoutmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}
 
void do_setlogin(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
  
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETLOGIN)) SET_BIT(ch->pcdata->jflags, JFLAG_SETLOGIN);
  free_string(ch->pcdata->loginmessage);
  ch->pcdata->loginmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}
 
void do_setavatar(CHAR_DATA *ch, char *argument)
{
  int cost = 1000;
  
  if (IS_NPC(ch)) return;
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETAVATAR)) SET_BIT(ch->pcdata->jflags, JFLAG_SETAVATAR);
  free_string(ch->pcdata->avatarmessage);
  ch->pcdata->avatarmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}

