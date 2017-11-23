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

void horn args((CHAR_DATA * ch));

char *const dir_name[] = {
  "north", "east", "south", "west", "up", "down"
};

const sh_int rev_dir[] = {
  2, 3, 0, 1, 5, 4
};

const sh_int movement_loss[SECT_MAX] = {
  1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};

/*
 * Local functions.
 */
int    find_door     args (( CHAR_DATA * ch, char *arg ));
bool   has_key       args (( CHAR_DATA * ch, int key ));
int    count_imms    args (( CHAR_DATA * ch ));
bool   check_track   args (( CHAR_DATA * ch, int direction ));
void   add_tracks    args (( CHAR_DATA * ch, int direction ));

void move_char(CHAR_DATA * ch, int door)
{
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  CHAR_DATA *mount;
  ROOM_INDEX_DATA *in_room;
  ROOM_INDEX_DATA *to_room;
  OBJ_DATA *obj;
  EXIT_DATA *pexit;
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
  char poly[MAX_STRING_LENGTH];
  char mount2[MAX_INPUT_LENGTH];
  char leave[20];
  int revdoor;
  bool bad_wall = FALSE;

  if (door < 0 || door > 5)

  {
    bug("Do_move: bad door %d.", door);
    return;
  }

  in_room = ch->in_room;

  if (!in_room)
  {
    bug("Move_char : ch not in any room.", 0);
    return;
  }

  if ((pexit = in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
  {
    send_to_char("Alas, you cannot go that way.\n\r", ch);
    return;
  }

  if (door == DIR_NORTH && ((obj = get_obj_list(ch, "walln", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "walls", to_room->contents)) != NULL))
    bad_wall = TRUE;

  if (door == DIR_SOUTH && ((obj = get_obj_list(ch, "walls", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "walln", to_room->contents)) != NULL))
    bad_wall = TRUE;

  if (door == DIR_EAST && ((obj = get_obj_list(ch, "walle", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "wallw", to_room->contents)) != NULL))
    bad_wall = TRUE;

  if (door == DIR_WEST && ((obj = get_obj_list(ch, "wallw", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "walle", to_room->contents)) != NULL))
    bad_wall = TRUE;

  if (door == DIR_UP && ((obj = get_obj_list(ch, "wallu", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "walld", to_room->contents)) != NULL))
    bad_wall = TRUE;

  if (door == DIR_DOWN && ((obj = get_obj_list(ch, "walld", ch->in_room->contents)) != NULL || (obj = get_obj_list(ch, "wallu", to_room->contents)) != NULL))
    bad_wall = TRUE;
  if (bad_wall)
  {
    send_to_char("You are unable to pass the wall.\n\r", ch);
    return;
  }

  if (IS_SET(pexit->exit_info, EX_CLOSED) && (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS)) && !IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(ch, AFF_SHADOWPLANE))
  {
      act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
      return;
  }

  if (IS_SET(pexit->exit_info, EX_PRISMATIC_WALL) && IS_SET(pexit->exit_info, EX_CLOSED))
  {
    stc("The prismatic wall prevents movement through this closed exit.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL && in_room == ch->master->in_room)
  {
    send_to_char("What?  And leave your beloved master?\n\r", ch);
    return;
  }

  if (IS_NPC(ch) && (mount = ch->mount) != NULL && IS_SET(ch->mounted, IS_MOUNT))
  {
    send_to_char("You better wait for instructions from your rider.\n\r", ch);
    return;
  }

  if (room_is_private(to_room))
  {
    if (IS_NPC(ch) || ch->trust < MAX_LEVEL)
    {
      send_to_char("That room is private right now.\n\r", ch);
      return;
    }
    else
      send_to_char("That room is private (Access granted).\n\r", ch);
  }

  if ((IS_LEG_L(ch, BROKEN_LEG) || IS_LEG_L(ch, LOST_LEG)) &&
      (IS_LEG_R(ch, BROKEN_LEG) || IS_LEG_R(ch, LOST_LEG)) &&
      (IS_ARM_L(ch, BROKEN_ARM) || IS_ARM_L(ch, LOST_ARM) || get_eq_char(ch, WEAR_HOLD) != NULL) && (IS_ARM_R(ch, BROKEN_ARM) || IS_ARM_R(ch, LOST_ARM) || get_eq_char(ch, WEAR_WIELD) != NULL))
  {
    send_to_char("You need at least one free arm to drag yourself with.\n\r", ch);
    return;
  }
  else if (IS_BODY(ch, BROKEN_SPINE) &&
           (IS_ARM_L(ch, BROKEN_ARM) || IS_ARM_L(ch, LOST_ARM) || get_eq_char(ch, WEAR_HOLD) != NULL) && (IS_ARM_R(ch, BROKEN_ARM) || IS_ARM_R(ch, LOST_ARM) || get_eq_char(ch, WEAR_WIELD) != NULL))
  {
    send_to_char("You cannot move with a broken spine.\n\r", ch);
    return;
  }

  if (!IS_NPC(ch))
  {
    int move;

    if (in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR)
    {
      if (!IS_AFFECTED(ch, AFF_FLYING))
      {
        send_to_char("You can't fly.\n\r", ch);
        return;
      }
    }

    if (in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
    {
      OBJ_DATA *obj;
      bool found;

      /*
       * Look for a boat.
       */
      found = FALSE;

      if (IS_AFFECTED(ch, AFF_FLYING))
        found = TRUE;
      if (!found)
      {
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
        {
          if (obj->item_type == ITEM_BOAT)
          {
            found = TRUE;
            break;
          }
        }
        if (!found)
        {
          send_to_char("You need a boat to go there.\n\r", ch);
          return;
        }
      }
    }
    else if (!IS_AFFECTED(ch, AFF_FLYING) && IS_POLYAFF(ch, POLY_FISH))
    {
      bool from_ok = FALSE;
      bool to_ok = FALSE;

      if (in_room->sector_type == SECT_WATER_NOSWIM)
        from_ok = TRUE;
      if (in_room->sector_type == SECT_WATER_SWIM)
        from_ok = TRUE;
      if (to_room->sector_type == SECT_WATER_NOSWIM)
        to_ok = TRUE;
      if (to_room->sector_type == SECT_WATER_SWIM)
        to_ok = TRUE;
      if (!from_ok || !to_ok)
      {
        send_to_char("You cannot cross land.\n\r", ch);
        return;
      }
    }

    move = movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)] + movement_loss[UMIN(SECT_MAX - 1, to_room->sector_type)];

    if (IS_HERO(ch))
      move = 0;

    if (ch->move <= 0)
    {
      send_to_char("You are too Exhausted.\n\r", ch);
      return;
    }

    if (IS_SET(ch->mounted, IS_RIDING) && (ch->move < move || ch->move < 1))
    {
      send_to_char("You are too exhausted.\n\r", ch);
      return;
    }

    if (IS_SET(pexit->exit_info, EX_IRON_WALL) && !IS_AFFECTED(ch, AFF_PASS_DOOR))
    {
      send_to_char("A towering wall of iron blocks your path.\n\r", ch);
      act("$n's path is blocked by the wall of iron.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    if (IS_SET(pexit->exit_info, EX_MUSHROOM_WALL))
    {
      send_to_char("The mushrooms block your path.\n\r", ch);
      return;
    }

    if (IS_SET(pexit->exit_info, EX_FIRE_WALL) && ch->class == 0)
    {
      act("$n bursts through the wall of fire.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You jump through the flames and are unaffected.\n\r", ch);
    }

    else if (IS_SET(pexit->exit_info, EX_FIRE_WALL) && ch->class != 0 && !IS_NPC(ch) && ch->level > 2)
    {
      act("$n bursts through the wall of fire.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You jump through the flames.\n\r", ch);
      ch->hit -= dice(6, 50);
      stc("The flames sear your flesh.\n\r", ch);
    }

    if (IS_SET(pexit->exit_info, EX_SWORD_WALL) && ch->class == 0)
    {
      act("$n bursts through the wall of swords.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You jump through the swords and are unaffected.\n\r", ch);
    }

    else if (IS_SET(pexit->exit_info, EX_SWORD_WALL) && ch->class != 0 && !IS_NPC(ch) && ch->level > 2)
    {
      act("$n jumps through the wall of swords.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You jump through the swords.\n\r", ch);
      send_to_char("Aaaaaaaaarghhhhhhh! That hurt!\n\r", ch);
      ch->hit -= dice(6, 70);
    }

    if (IS_SET(pexit->exit_info, EX_ASH_WALL))
    {
      send_to_char("You scream in agony as the wall of ash rips apart your life force.\n\r", ch);
      act("$n screams in agony as the wall of ash rips $s life force apart.", ch, NULL, NULL, TO_ROOM);
      ch->hit /= 2;
      ch->move /= 2;
    }

    WAIT_STATE(ch, 1);
    if (!IS_SET(ch->mounted, IS_RIDING))
      ch->move -= move;
  }

  /* Check for mount message - KaVir */
  if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
  {
    if (IS_NPC(mount))
      sprintf(mount2, " on %s.", mount->short_descr);
    else
      sprintf(mount2, " on %s.", mount->name);
  }
  else
    sprintf(mount2, ".");

  if (IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH))
    sprintf(leave, "rolls");
  else if (IS_AFFECTED(ch, AFF_ETHEREAL))
    sprintf(leave, "floats");
  else if (ch->in_room->sector_type == SECT_WATER_SWIM)
    sprintf(leave, "swims");
  else if (IS_SET(ch->polyaff, POLY_SERPENT))
    sprintf(leave, "slithers");
  else if (IS_SET(ch->polyaff, POLY_WOLF))
    sprintf(leave, "stalks");
  else if (IS_SET(ch->polyaff, POLY_FROG))
    sprintf(leave, "hops");
  else if (IS_SET(ch->polyaff, POLY_FISH))
    sprintf(leave, "swims");
  else if (IS_BODY(ch, BROKEN_SPINE))
    sprintf(leave, "drags $mself");
  else if (IS_LEG_L(ch, LOST_LEG) && IS_LEG_R(ch, LOST_LEG))
    sprintf(leave, "drags $mself");
  else if ((IS_LEG_L(ch, BROKEN_LEG) || IS_LEG_L(ch, LOST_LEG) || IS_LEG_L(ch, LOST_FOOT)) && (IS_LEG_R(ch, BROKEN_LEG) || IS_LEG_R(ch, LOST_LEG) || IS_LEG_R(ch, LOST_FOOT)))
    sprintf(leave, "crawls");
  else if (ch->hit < (ch->max_hit / 4))
    sprintf(leave, "crawls");
  else if ((IS_LEG_R(ch, LOST_LEG) || IS_LEG_R(ch, LOST_FOOT)) && (!IS_LEG_L(ch, BROKEN_LEG) && !IS_LEG_L(ch, LOST_LEG) && !IS_LEG_L(ch, LOST_FOOT)))
    sprintf(leave, "hops");
  else if ((IS_LEG_L(ch, LOST_LEG) || IS_LEG_L(ch, LOST_FOOT)) && (!IS_LEG_R(ch, BROKEN_LEG) && !IS_LEG_R(ch, LOST_LEG) && !IS_LEG_R(ch, LOST_FOOT)))
    sprintf(leave, "hops");
  else if ((IS_LEG_L(ch, BROKEN_LEG) || IS_LEG_L(ch, LOST_FOOT)) && (!IS_LEG_R(ch, BROKEN_LEG) && !IS_LEG_R(ch, LOST_LEG) && !IS_LEG_R(ch, LOST_FOOT)))
    sprintf(leave, "limps");
  else if ((IS_LEG_R(ch, BROKEN_LEG) || IS_LEG_R(ch, LOST_FOOT)) && (!IS_LEG_L(ch, BROKEN_LEG) && !IS_LEG_L(ch, LOST_LEG) && !IS_LEG_L(ch, LOST_FOOT)))
    sprintf(leave, "limps");
  else if (ch->hit < (ch->max_hit / 3))
    sprintf(leave, "limps");
  else if (ch->hit < (ch->max_hit / 2))
    sprintf(leave, "staggers");
  else if (!IS_NPC(ch))
  {
    if (ch->pcdata->condition[COND_DRUNK] > 10)
      sprintf(leave, "staggers");
    else
      sprintf(leave, "walks");
  }
  else if (IS_SET(ch->act, ACT_FISH))
    sprintf(leave, "swims");
  else
    sprintf(leave, "walks");

  if (!IS_NPC(ch) && ch->stance[0] != -1)
    do_stance(ch, "");
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *victim;

    if ((victim = d->character) == NULL)
      continue;

    if (ch->in_room == NULL || victim->in_room == NULL)
      continue;

    if (ch == victim || ch->in_room != victim->in_room)
      continue;

    if (d->connected != CON_PLAYING || !can_see(ch, victim))
      continue;

    if (!IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch, AFF_POLYMORPH) && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)) && can_see(victim, ch))
    {
      if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED(mount, AFF_FLYING)) || IS_AFFECTED(ch, AFF_FLYING))
        sprintf(poly, "%s flies $T%s", ch->morph, mount2);
      else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
        sprintf(poly, "%s rides $T%s", ch->morph, mount2);
      else
        sprintf(poly, "%s %s $T%s", ch->morph, leave, mount2);
      act(poly, victim, NULL, dir_name[door], TO_CHAR);
    }
    else if (!IS_AFFECTED(ch, AFF_SNEAK) && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)) && can_see(victim, ch))
    {
      if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED(mount, AFF_FLYING)) || IS_AFFECTED(ch, AFF_FLYING))
        sprintf(poly, "$n flies %s%s", dir_name[door], mount2);
      else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
        sprintf(poly, "$n rides %s%s", dir_name[door], mount2);
      else
        sprintf(poly, "$n %s %s%s", leave, dir_name[door], mount2);
      act(poly, ch, NULL, victim, TO_VICT);
    }
  }
  char_from_room(ch);
  char_to_room(ch, to_room);
  if (door == 0)
  {
    revdoor = 2;
    sprintf(buf, "the south");
  }
  else if (door == 1)
  {
    revdoor = 3;
    sprintf(buf, "the west");
  }
  else if (door == 2)
  {
    revdoor = 0;
    sprintf(buf, "the north");
  }
  else if (door == 3)
  {
    revdoor = 1;
    sprintf(buf, "the east");
  }
  else if (door == 4)
  {
    revdoor = 5;
    sprintf(buf, "below");
  }
  else
  {
    revdoor = 4;
    sprintf(buf, "above");
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *victim;

    if ((victim = d->character) == NULL)
      continue;

    if (ch->in_room == NULL || victim->in_room == NULL)
      continue;

    if (ch == victim || ch->in_room != victim->in_room)
      continue;

    if (d->connected != CON_PLAYING || !can_see(ch, victim))
      continue;

    if (!IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch, AFF_POLYMORPH) && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)) && can_see(victim, ch))
    {
      if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED(mount, AFF_FLYING)) || IS_AFFECTED(ch, AFF_FLYING))
        sprintf(poly, "%s flies in from %s%s", ch->morph, buf, mount2);
      else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
        sprintf(poly, "%s rides in from %s%s", ch->morph, buf, mount2);
      else
        sprintf(poly, "%s %s in from %s%s", ch->morph, leave, buf, mount2);
      act(poly, ch, NULL, victim, TO_VICT);
    }
    else if (!IS_AFFECTED(ch, AFF_SNEAK) && can_see(victim, ch) && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
    {
      if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED(mount, AFF_FLYING)) || IS_AFFECTED(ch, AFF_FLYING))
        sprintf(poly, "$n flies in from %s%s", buf, mount2);
      else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
        sprintf(poly, "$n rides in from %s%s", buf, mount2);
      else
        sprintf(poly, "$n %s in from %s%s", leave, buf, mount2);
      act(poly, ch, NULL, victim, TO_VICT);
    }
  }

  do_look(ch, "auto");

  for (fch = in_room->people; fch != NULL; fch = fch_next)
  {
    fch_next = fch->next_in_room;
    if ((mount = fch->mount) != NULL && mount == ch && IS_SET(fch->mounted, IS_MOUNT))
    {
      act("$N digs $S heels into you.", fch, NULL, ch, TO_CHAR);
      char_from_room(fch);
      char_to_room(fch, ch->in_room);
    }

    if (fch->master == ch && fch->position == POS_STANDING && fch->in_room != ch->in_room)
    {
      act("You follow $N.", fch, NULL, ch, TO_CHAR);
      move_char(fch, door);
    }
  }
  room_text(ch, ">enter<");
  return;
}

void do_flex(CHAR_DATA * ch, char *argument)
{
  act("You flex your bulging muscles.", ch, NULL, NULL, TO_CHAR);
  act("$n flexes $s bulging muscles.", ch, NULL, NULL, TO_ROOM);
  
  if (IS_NPC(ch))
    return;
    
  if (IS_EXTRA(ch, TIED_UP))
  {
    act("The ropes restraining you snap.", ch, NULL, NULL, TO_CHAR);
    act("The ropes restraining $n snap.", ch, NULL, NULL, TO_ROOM);
    REMOVE_BIT(ch->extra, TIED_UP);
  }
    
  WAIT_STATE(ch, 12);
  return;
}

void do_north(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *in_room;


  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_NORTH);


  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_NORTH);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

void do_east(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *in_room;

  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_EAST);
  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_EAST);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

void do_south(CHAR_DATA * ch, char *argument)
{

  ROOM_INDEX_DATA *in_room;

  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_SOUTH);
  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_SOUTH);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

void do_west(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *in_room;

  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_WEST);
  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_WEST);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

void do_up(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *in_room;

  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_UP);
  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_UP);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

void do_down(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *in_room;

  if (IS_EXTRA(ch, PINNEDWALL))
  {
    send_to_char("Not while pinned to the wall.\n\r",ch);
    return;
  }

  in_room = ch->in_room;
  move_char(ch, DIR_DOWN);
  if (!IS_NPC(ch) && ch->in_room != in_room)
  {
    ROOM_INDEX_DATA *old_room;

    old_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, in_room);
    add_tracks(ch, DIR_DOWN);
    char_from_room(ch);
    char_to_room(ch, old_room);
  }
  return;
}

int find_door(CHAR_DATA * ch, char *arg)
{
  EXIT_DATA *pexit;
  int door;

  if (!str_cmp(arg, "n") || !str_cmp(arg, "north"))
    door = 0;
  else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))
    door = 1;
  else if (!str_cmp(arg, "s") || !str_cmp(arg, "south"))
    door = 2;
  else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))
    door = 3;
  else if (!str_cmp(arg, "u") || !str_cmp(arg, "up"))
    door = 4;
  else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))
    door = 5;
  else
  {
    for (door = 0; door <= 5; door++)
    {
      if ((pexit = ch->in_room->exit[door]) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR) && pexit->keyword != NULL && is_name(arg, pexit->keyword))
        return door;
    }
    act("I see no $T here.", ch, NULL, arg, TO_CHAR);
    return -1;
  }

  if ((pexit = ch->in_room->exit[door]) == NULL)
  {
    act("I see no door $T here.", ch, NULL, arg, TO_CHAR);
    return -1;
  }

  if (!IS_SET(pexit->exit_info, EX_ISDOOR))
  {
    send_to_char("You can't do that.\n\r", ch);
    return -1;
  }

  return door;
}


ROOM_INDEX_DATA *get_rand_room()
{
  ROOM_INDEX_DATA *room;

  for (;;)
  {
    room = get_room_index(number_range(1000, 32000));
    if (room != NULL)
      if (!IS_SET(room->room_flags, ROOM_PRIVATE) && !IS_SET(room->room_flags, ROOM_ASTRAL) &&
          !IS_SET(room->room_flags, ROOM_SAFE))
        break;
  }

  return room;
}

/* Designed for the portal spell, but can also have other uses...KaVir
 * V0 = Where the portal will take you.
 * V1 = The access code.
 * V2 = if =! 0, cannot be entered.
 * V3 = The room the portal is currently in.
 */
void do_enter(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *pRoomIndex;
  ROOM_INDEX_DATA *location;
  char arg[MAX_INPUT_LENGTH];
  char poly[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *portal;
  OBJ_DATA *portal_next;
  CHAR_DATA *mount;
  CHAR_DATA *gch;
  CHAR_DATA *gch_next;
  bool found;

  argument = one_argument(argument, arg);

  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
    {
      send_to_char("The portal cannot function in an astral room.\n\r", ch);
      return;
    }
  }

  if (arg[0] == '\0')
  {
    send_to_char("Enter what?\n\r", ch);
    return;
  }
  obj = get_obj_list(ch, arg, ch->in_room->contents);
  if (obj == NULL)
  {
    act("I see no $T here.", ch, NULL, arg, TO_CHAR);
    return;
  }
  if (obj->item_type != ITEM_PORTAL)
  {
    act("You cannot enter that.", ch, NULL, arg, TO_CHAR);
    return;
  }
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))
  {
    send_to_char("You are too insubstantual.\n\r", ch);
    return;
  }
  else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))
  {
    send_to_char("It is too insubstantual.\n\r", ch);
    return;
  }
  if (obj->item_type == ITEM_PORTAL)
  {
    if (obj->value[2] != 0)
    {
      act("It seems to be closed.", ch, NULL, arg, TO_CHAR);
      return;
    }
    pRoomIndex = get_room_index(obj->value[0]);
    location = ch->in_room;
    if (pRoomIndex == NULL)
    {
      act("You are unable to enter.", ch, NULL, arg, TO_CHAR);
      return;
    }
    act("You step into $p.", ch, obj, NULL, TO_CHAR);
    if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
      sprintf(poly, "%s steps into $p.", ch->morph);
    else
      sprintf(poly, "$n steps into $p.");
    act(poly, ch, obj, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, pRoomIndex);
    if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
      sprintf(poly, "%s steps out of $p.", ch->morph);
    else
      sprintf(poly, "$n steps out of $p.");
    act(poly, ch, obj, NULL, TO_ROOM);

    found = FALSE;
    for (portal = ch->in_room->contents; portal != NULL; portal = portal_next)
    {
      portal_next = portal->next_content;
      if ((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
      {
        found = TRUE;

        if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
        {
          REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
          break;
        }
        else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
        {
          SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
          break;
        }
      }
    }

    do_look(ch, "auto");
    if ((mount = ch->mount) != NULL)
    {
      char_from_room(mount);
      char_to_room(mount, ch->in_room);
    }

    for (gch = location->people; gch; gch = gch_next)
    {
      gch_next = gch->next_in_room;

      if (gch->master == ch && gch->position == POS_STANDING && gch->in_room != ch->in_room)
      {
        act("You follow $N.", gch, NULL, ch, TO_CHAR);
        do_enter(gch, arg);
      }
    }
    return;
  }
  return;
}

void do_smother(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *inroom;

  one_argument(argument, arg);
  if (IS_NPC(ch))
    return;
  inroom = ch->in_room;
  if (arg[0] == '\0' && !IS_SET(inroom->room_flags, ROOM_FLAMING))
  {
    send_to_char("Smother whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (!IS_AFFECTED(victim, AFF_FLAMING))
  {
    send_to_char("But they are not on fire!\n\r", ch);
    return;
  }

  if (number_percent() > (ch->level * 10))
  {
    act("You try to smother the flames around $N but fail!", ch, NULL, victim, TO_CHAR);
    act("$n tries to smother the flames around you but fails!", ch, NULL, victim, TO_VICT);
    act("$n tries to smother the flames around $N but fails!", ch, NULL, victim, TO_NOTVICT);
    if (number_percent() > 98 && !IS_AFFECTED(ch, AFF_FLAMING))
    {
      act("A spark of flame from $N's body sets you on fire!", ch, NULL, victim, TO_CHAR);
      act("A spark of flame from your body sets $n on fire!", ch, NULL, victim, TO_VICT);
      act("A spark of flame from $N's body sets $n on fire!", ch, NULL, victim, TO_NOTVICT);
      SET_BIT(ch->affected_by, AFF_FLAMING);
    }
    return;
  }

  act("You manage to smother the flames around $M!", ch, NULL, victim, TO_CHAR);
  act("$n manages to smother the flames around you!", ch, NULL, victim, TO_VICT);
  act("$n manages to smother the flames around $N!", ch, NULL, victim, TO_NOTVICT);
  REMOVE_BIT(victim->affected_by, AFF_FLAMING);
  return;
}


void do_open(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Open what?\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    /* 'open object' */
    if (obj->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSED))
    {
      send_to_char("It's already open.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
    {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_SET(obj->value[1], CONT_LOCKED))
    {
      send_to_char("It's locked.\n\r", ch);
      return;
    }

    REMOVE_BIT(obj->value[1], CONT_CLOSED);
    send_to_char("Ok.\n\r", ch);
    act("$n opens $p.", ch, obj, NULL, TO_ROOM);
    return;
  }

  if ((door = find_door(ch, arg)) >= 0)
  {
    /* 'open door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if (!IS_SET(pexit->exit_info, EX_CLOSED))
    {
      send_to_char("It's already open.\n\r", ch);
      return;
    }
    if (IS_SET(pexit->exit_info, EX_LOCKED))
    {
      send_to_char("It's locked.\n\r", ch);
      return;
    }

    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
    act("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);
    send_to_char("Ok.\n\r", ch);

    /* open the other side */
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room == ch->in_room)
    {
      CHAR_DATA *rch;

      REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
      for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
        act("The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR);
    }
  }

  return;
}

void do_close(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Close what?\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    /* 'close object' */
    if (obj->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (IS_SET(obj->value[1], CONT_CLOSED))
    {
      send_to_char("It's already closed.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
    {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    SET_BIT(obj->value[1], CONT_CLOSED);
    send_to_char("Ok.\n\r", ch);
    act("$n closes $p.", ch, obj, NULL, TO_ROOM);
    return;
  }

  if ((door = find_door(ch, arg)) >= 0)
  {
    /* 'close door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED))
    {
      send_to_char("It's already closed.\n\r", ch);
      return;
    }

    SET_BIT(pexit->exit_info, EX_CLOSED);
    act("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM);
    send_to_char("Ok.\n\r", ch);

    /* close the other side */
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
    {
      CHAR_DATA *rch;

      SET_BIT(pexit_rev->exit_info, EX_CLOSED);
      for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
        act("The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR);
    }
  }

  return;
}

bool has_key(CHAR_DATA * ch, int key)
{
  OBJ_DATA *obj;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->pIndexData->vnum == key)
      return TRUE;
  }

  return FALSE;
}

void do_lock(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Lock what?\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    /* 'lock object' */
    if (obj->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (obj->value[2] < 0)
    {
      send_to_char("It can't be locked.\n\r", ch);
      return;
    }
    if (!has_key(ch, obj->value[2]))
    {
      send_to_char("You lack the key.\n\r", ch);
      return;
    }
    if (IS_SET(obj->value[1], CONT_LOCKED))
    {
      send_to_char("It's already locked.\n\r", ch);
      return;
    }

    SET_BIT(obj->value[1], CONT_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n locks $p.", ch, obj, NULL, TO_ROOM);
    return;
  }

  if ((door = find_door(ch, arg)) >= 0)
  {
    /* 'lock door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if (!IS_SET(pexit->exit_info, EX_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (pexit->key < 0)
    {
      send_to_char("It can't be locked.\n\r", ch);
      return;
    }
    if (!has_key(ch, pexit->key))
    {
      send_to_char("You lack the key.\n\r", ch);
      return;
    }
    if (IS_SET(pexit->exit_info, EX_LOCKED))
    {
      send_to_char("It's already locked.\n\r", ch);
      return;
    }

    SET_BIT(pexit->exit_info, EX_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

    /* lock the other side */
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
    {
      SET_BIT(pexit_rev->exit_info, EX_LOCKED);
    }
  }

  return;
}

void do_unlock(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Unlock what?\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    /* 'unlock object' */
    if (obj->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (obj->value[2] < 0)
    {
      send_to_char("It can't be unlocked.\n\r", ch);
      return;
    }
    if (!has_key(ch, obj->value[2]))
    {
      send_to_char("You lack the key.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_LOCKED))
    {
      send_to_char("It's already unlocked.\n\r", ch);
      return;
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
    return;
  }

  if ((door = find_door(ch, arg)) >= 0)
  {
    /* 'unlock door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if (!IS_SET(pexit->exit_info, EX_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (pexit->key < 0)
    {
      send_to_char("It can't be unlocked.\n\r", ch);
      return;
    }
    if (!has_key(ch, pexit->key))
    {
      send_to_char("You lack the key.\n\r", ch);
      return;
    }
    if (!IS_SET(pexit->exit_info, EX_LOCKED))
    {
      send_to_char("It's already unlocked.\n\r", ch);
      return;
    }

    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

    /* unlock the other side */
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room == ch->in_room)
    {
      REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
    }
  }

  return;
}

void do_pick(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  OBJ_DATA *obj;
  int door;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Pick what?\n\r", ch);
    return;
  }

  WAIT_STATE(ch, skill_table[gsn_pick_lock].beats);

  /* look for guards */
  for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
  {
    if (IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level)
    {
      act("$N is standing too close to the lock.", ch, NULL, gch, TO_CHAR);
      return;
    }
  }

  if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_pick_lock])
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    /* 'pick object' */
    if (obj->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (obj->value[2] < 0)
    {
      send_to_char("It can't be unlocked.\n\r", ch);
      return;
    }
    if (!IS_SET(obj->value[1], CONT_LOCKED))
    {
      send_to_char("It's already unlocked.\n\r", ch);
      return;
    }
    if (IS_SET(obj->value[1], CONT_PICKPROOF))
    {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n picks $p.", ch, obj, NULL, TO_ROOM);
    return;
  }

  if ((door = find_door(ch, arg)) >= 0)
  {
    /* 'pick door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if (!IS_SET(pexit->exit_info, EX_CLOSED))
    {
      send_to_char("It's not closed.\n\r", ch);
      return;
    }
    if (pexit->key < 0)
    {
      send_to_char("It can't be picked.\n\r", ch);
      return;
    }
    if (!IS_SET(pexit->exit_info, EX_LOCKED))
    {
      send_to_char("It's already unlocked.\n\r", ch);
      return;
    }
    if (IS_SET(pexit->exit_info, EX_PICKPROOF))
    {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    send_to_char("*Click*\n\r", ch);
    act("$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

    /* pick the other side */
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room == ch->in_room)
    {
      REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
    }
  }

  return;
}

void do_stand(CHAR_DATA * ch, char *argument)
{
  switch (ch->position)
  {
    case POS_SLEEPING:
      send_to_char("You wake and stand up.\n\r", ch);
      act("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_STANDING;
      break;

    case POS_RESTING:
    case POS_SITTING:
      send_to_char("You stand up.\n\r", ch);
      act("$n stands up.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_STANDING;
      break;

    case POS_MEDITATING:
      send_to_char("You uncross your legs and stand up.\n\r", ch);
      act("$n uncrosses $s legs and stands up.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_STANDING;
      break;

    case POS_STANDING:
      send_to_char("You are already standing.\n\r", ch);
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
  }

  return;
}

void do_rest(CHAR_DATA * ch, char *argument)
{
  switch (ch->position)
  {
    case POS_SLEEPING:
      send_to_char("You are already sleeping.\n\r", ch);
      break;

    case POS_RESTING:
      send_to_char("You are already resting.\n\r", ch);
      break;

    case POS_MEDITATING:
    case POS_SITTING:
    case POS_STANDING:
      send_to_char("You rest.\n\r", ch);
      act("$n rests.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_RESTING;
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
  }

  return;
}

void do_sit(CHAR_DATA * ch, char *argument)
{
  switch (ch->position)
  {
    case POS_SLEEPING:
      send_to_char("You are already sleeping.\n\r", ch);
      break;

    case POS_RESTING:
      send_to_char("You are already resting.\n\r", ch);
      break;

    case POS_MEDITATING:
      send_to_char("You are already meditating.\n\r", ch);
      break;

    case POS_SITTING:
      send_to_char("You are already sitting.\n\r", ch);
      break;

    case POS_STANDING:
      send_to_char("You sit down.\n\r", ch);
      act("$n sits down.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_SITTING;
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
  }

  return;
}

void do_sleep(CHAR_DATA * ch, char *argument)
{
  switch (ch->position)
  {
    case POS_SLEEPING:
      send_to_char("You are already sleeping.\n\r", ch);
      break;

    case POS_SITTING:
    case POS_MEDITATING:
    case POS_RESTING:
    case POS_STANDING:
      send_to_char("You sleep.\n\r", ch);
      act("$n sleeps.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_SLEEPING;
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
  }

  return;
}

void do_wake(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    do_stand(ch, argument);
    return;
  }

  if (!IS_AWAKE(ch))
  {
    send_to_char("You are asleep yourself!\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_AWAKE(victim))
  {
    act("$N is already awake.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim->position < POS_SLEEPING)
  {
    act("$E doesn't respond!", ch, NULL, victim, TO_CHAR);
    return;
  }

  act("You wake $M.", ch, NULL, victim, TO_CHAR);
  act("$n wakes you.", ch, NULL, victim, TO_VICT);
  victim->position = POS_STANDING;
  return;
}

void do_sneak(CHAR_DATA * ch, char *argument)
{
  AFFECT_DATA af;

  send_to_char("You attempt to move silently.\n\r", ch);
  affect_strip(ch, gsn_sneak);

  if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_sneak])
  {
    af.type = gsn_sneak;
    af.duration = ch->level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char(ch, &af);
  }

  return;
}

void do_hide(CHAR_DATA * ch, char *argument)
{

  send_to_char("You attempt to hide.\n\r", ch);

  if (IS_AFFECTED(ch, AFF_HIDE))
    REMOVE_BIT(ch->affected_by, AFF_HIDE);

  if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_hide])
    SET_BIT(ch->affected_by, AFF_HIDE);

  return;
}

/*
 * Contributed by Alander.
 */
void do_visible(CHAR_DATA * ch, char *argument)
{
  affect_strip(ch, gsn_invis);
  affect_strip(ch, gsn_mass_invis);
  affect_strip(ch, gsn_sneak);
  REMOVE_BIT(ch->affected_by, AFF_HIDE);
  REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
  REMOVE_BIT(ch->affected_by, AFF_SNEAK);
  send_to_char("Ok.\n\r", ch);
  return;
}

void do_recall(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *mount;
  ROOM_INDEX_DATA *location;

  if (IS_NPC(ch))
    return;

  act("$n's body flickers with green energy.", ch, NULL, NULL, TO_ROOM);
  act("Your body flickers with green energy.", ch, NULL, NULL, TO_CHAR);

  if ((location = get_room_index(ch->home)) == NULL)
  {
    send_to_char("You are completely lost.\n\r", ch);
    return;
  }

 

  if (ch->in_room == location)
    return;
/*
  if (IS_SET(location->room_flags, ROOM_ASTRAL))
  {
    send_to_char("You cannot recall back into astral rooms.\n\r", ch);
    return;
  }
*/
  if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE))
  {
    send_to_char("You are unable to recall.\n\r", ch);
    return;
  }

  if (IS_SET(ch->flag2, AFF_TOTALBLIND))
  {
    stc("You are unable to recall.\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) != NULL)
  {
    if (number_bits(1) == 0)
    {
      WAIT_STATE(ch, 4);
      sprintf(buf, "You failed!\n\r");
      send_to_char(buf, ch);
      return;
    }
    sprintf(buf, "You recall from combat!\n\r");
    send_to_char(buf, ch);
    stop_fighting(ch, TRUE);
  }

  act("$n disappears.", ch, NULL, NULL, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, location);
  act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  if ((mount = ch->mount) == NULL)
    return;
  char_from_room(mount);
  char_to_room(mount, ch->in_room);
  return;
}

void do_home(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (arg[0] == '\0' || str_cmp(arg, "here"))
  {
    send_to_char("If you wish this to be your room, you must type 'home here'.\n\r", ch);
    return;
  }

  if (ch->in_room->vnum == ch->home)
  {
    send_to_char("But this is already your home!\n\r", ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
      IS_SET(ch->in_room->room_flags, ROOM_ASTRAL) || IS_SET(ch->in_room->area->areabits, AREA_BIT_NOHOME) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
  {
    send_to_char("You are unable to make this room your home.\n\r", ch);
    return;
  }
  if (ch->in_room->vnum == 3001)
  {
    send_to_char("You are unable to make this room your home.\n\r", ch);
    return;
  }
  ch->home = ch->in_room->vnum;
  send_to_char("This room is now your home.\n\r", ch);

  return;
}

void do_escape(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *location;

  if (IS_NPC(ch) || !IS_HERO(ch))
    return;

  if (ch->position >= POS_SLEEPING)
  {
    send_to_char("You can only do this if you are dying.\n\r", ch);
    return;
  }
  if ((location = get_room_index(ch->home)) == NULL)
  {
    send_to_char("You are completely lost.\n\r", ch);
    return;
  }
  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("No silly.\n\r", ch);
      return;
    }
  }
  if (in_fortress(ch))
  {
    send_to_char("Theres nowhere to run.\n\r", ch);
    return;
  }
  if (ch->in_room == location)
    return;
  ch->move = 0;
  ch->mana = 0;
  act("$n fades out of existance.", ch, NULL, NULL, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, location);
  act("$n fades into existance.", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  sprintf(buf, "%s has escaped #Gdefenceless#n, easy kill for the quick.", ch->pcdata->switchname);
  do_info(ch, buf);
  if (IS_SET(ch->newbits, NEW_DARKNESS))
  {
    REMOVE_BIT(ch->newbits, NEW_DARKNESS);
    if (ch->in_room != NULL)
      if (IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
        REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
    return;
  }
  return;
}

void do_relearn(CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];

  argument = one_argument(argument, arg1);

  if (IS_NPC(ch))
    return;

  if (!str_cmp(arg1, "slash") || !str_cmp(arg1, "slice"))
  {
    if (IS_IMMUNE(ch, IMM_SLASH))
    {
      REMOVE_BIT(ch->immune, IMM_SLASH);
      send_to_char("You forget Slash & Slice resistances.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "stab") || !str_cmp(arg1, "pierce"))
  {
    if (IS_IMMUNE(ch, IMM_STAB))
    {
      REMOVE_BIT(ch->immune, IMM_STAB);
      send_to_char("You forget Stab & Pierce resistances.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "smash") || !str_cmp(arg1, "pound") || !str_cmp(arg1, "blast") || !str_cmp(arg1, "crush"))
  {
    if (IS_IMMUNE(ch, IMM_SMASH))
    {
      REMOVE_BIT(ch->immune, IMM_SMASH);
      send_to_char("You forget Pound, Blast & Crush resistances.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "beast") || !str_cmp(arg1, "claw") || !str_cmp(arg1, "bite"))
  {
    if (IS_IMMUNE(ch, IMM_ANIMAL))
    {
      REMOVE_BIT(ch->immune, IMM_ANIMAL);
      send_to_char("You forget Claw & Bite resistances.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "grab") || !str_cmp(arg1, "grep") || !str_cmp(arg1, "whip") || !str_cmp(arg1, "suck"))
  {
    if (IS_IMMUNE(ch, IMM_MISC))
    {
      REMOVE_BIT(ch->immune, IMM_MISC);
      send_to_char("You forget Grep, Whip & Suck resistances.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "charm"))
  {
    if (IS_IMMUNE(ch, IMM_CHARM))
    {
      REMOVE_BIT(ch->immune, IMM_CHARM);
      send_to_char("You forget Charm immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "heat"))
  {
    if (IS_IMMUNE(ch, IMM_HEAT))
    {
      REMOVE_BIT(ch->immune, IMM_HEAT);
      send_to_char("You forget Heat immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "cold"))
  {
    if (IS_IMMUNE(ch, IMM_COLD))
    {
      REMOVE_BIT(ch->immune, IMM_COLD);
      send_to_char("You forget Cold immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "lightning"))
  {
    if (IS_IMMUNE(ch, IMM_LIGHTNING))
    {
      REMOVE_BIT(ch->immune, IMM_LIGHTNING);
      send_to_char("You forget Lightning immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "acid"))
  {
    if (IS_IMMUNE(ch, IMM_ACID))
    {
      REMOVE_BIT(ch->immune, IMM_ACID);
      send_to_char("You forget Acid immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "cold"))
  {
    if (IS_IMMUNE(ch, IMM_COLD))
    {
      REMOVE_BIT(ch->immune, IMM_COLD);
      send_to_char("You forget Cold immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "drain"))
  {
    if (IS_IMMUNE(ch, IMM_DRAIN))
    {
      REMOVE_BIT(ch->immune, IMM_DRAIN);
      send_to_char("You forget Drain immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "hurl"))
  {
    if (IS_IMMUNE(ch, IMM_HURL))
    {
      REMOVE_BIT(ch->immune, IMM_HURL);
      send_to_char("You forget Hurl immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "backstab"))
  {
    if (IS_IMMUNE(ch, IMM_BACKSTAB))
    {
      REMOVE_BIT(ch->immune, IMM_BACKSTAB);
      send_to_char("You forget Backstab immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "kick"))
  {
    if (IS_IMMUNE(ch, IMM_KICK))
    {
      REMOVE_BIT(ch->immune, IMM_KICK);
      send_to_char("You forget Kick immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "disarm"))
  {
    if (IS_IMMUNE(ch, IMM_DISARM))
    {
      REMOVE_BIT(ch->immune, IMM_DISARM);
      send_to_char("You forget Disarm immunity.\n\r", ch);
      return;
    }
  }
  if (!strcmp(arg1, "steal"))
  {
    if (IS_IMMUNE(ch, IMM_STEAL))
    {
      REMOVE_BIT(ch->immune, IMM_STEAL);
      send_to_char("You forget Steal immunity.\n\r", ch);
      return;
    }
  }
  else
  {
    send_to_char("Command: relearn an already learned immunity/resistance.\n\r", ch);
    return;
  }
}

void do_train(CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  sh_int *pAbility;
  char *pOutput;
  int cost;
  int immcost;
  int primal;
  bool last = TRUE;
  bool is_ok = FALSE;
  int legacy_xp = 50000000 * ch->pcLegacy;
  int legacy_gold = 15000 * ch->pcLegacy;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (IS_NPC(ch))
    return;

  if( !str_cmp( arg1, "legacy" ) ) {
      if( ch->pcLegacy >= 20 ) {
      send_to_char( "You have maximum Legacy.\n\r", ch );
      return;
      }
      if( ch->exp < legacy_xp ) {
         sprintf( buf, "Not enough Experience points, you need %2.0f more.\n\r", ( legacy_xp - ch->exp ) );
         send_to_char( buf, ch );
         return;
      }
      if( ch->pcgold < legacy_gold ) {
         sprintf( buf, "Not enough gold, you need %d more.\n\r", (legacy_gold - ch->pcgold ) );
         send_to_char( buf, ch );
         return;
      }
      send_to_char("#0Legacy Increased.#n\n\r",ch);
      ch->pcgold -= legacy_gold;
      ch->exp -= legacy_xp;
      ch->pcLegacy++;
      return;
   }


  if (!str_cmp(arg1, "generation"))
  {
    int gencost = 0;

    if (ch->generation < 3)
    {
      do_train(ch, "");
      return;
    }
    if (ch->generation == 3)
      gencost = 400000000;
    else if (ch->generation == 4)
      gencost = 200000000;
    else if (ch->generation == 5)
      gencost = 50000000;
    else
      gencost = 10000000;
    if (ch->exp < gencost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->generation--;
    ch->exp -= gencost;
    return;
  }
  if (!str_cmp(arg1, "primal"))
  {
    int amount = 0;
    int urin = 0;
    int urin_counter = 0;

    if (!is_number(arg2) && strcmp(arg2, "all"))
    {
      send_to_char("Please enter a numeric value.\n\r", ch);
      return;
    }
    if (is_number(arg2))
    {
      amount = atoi(arg2);
      if (amount < 1 || amount > 500)
      {
        send_to_char("Please enter a value between 1 and 500.\n\r", ch);
        return;
      }
    }
    else
    {
      amount = 100000;
    }

    for (urin = 0; urin < amount; urin++)
    {
      if (ch->exp >= (ch->practice + 1) * 500)
      {
        ch->practice++;
        ch->exp -= ch->practice * 500;
        urin_counter++;
      }
    }
    if (urin_counter == 0)
    {
      send_to_char("You need more exp to gain any primal.\n\r", ch);
    }
    if (urin_counter == 1)
    {
      send_to_char("You gain 1 primal.\n\r", ch);
    }
    if (urin_counter > 1)
    {
      sprintf(buf, "You gain %d primal.\n\r", urin_counter);
      send_to_char(buf, ch);
    }
    return;
  }



  if (!str_cmp(arg1, "slash") || !str_cmp(arg1, "stab") || !str_cmp(arg1, "smash") || !str_cmp(arg1, "beast") || !str_cmp(arg1, "grab") || !str_cmp(arg1, "charm") || !str_cmp(arg1, "heat")
      || !str_cmp(arg1, "cold") || !str_cmp(arg1, "lightning") || !str_cmp(arg1, "acid") || !str_cmp(arg1, "drain") || !str_cmp(arg1, "hurl") || !str_cmp(arg1, "backstab") || !str_cmp(arg1, "kick")
      || !str_cmp(arg1, "disarm") || !str_cmp(arg1, "steal"))
  {
    int immune_counter = 0;

    if (IS_IMMUNE(ch, IMM_SLASH))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_STAB))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_SMASH))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_ANIMAL))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_MISC))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_CHARM))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_HEAT))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_COLD))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_LIGHTNING))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_ACID))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_DRAIN))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_HURL))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_BACKSTAB))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_DISARM))
      immune_counter += 1;
    if (IS_IMMUNE(ch, IMM_STEAL))
      immune_counter += 1;

    if (immune_counter > 30)
    {
      send_to_char("You already have 10 immunities/resistances.\n\r", ch);
      send_to_char("Use relearn to remove an immunity/resistance.\n\r", ch);
      return;
    }
  }

  if (arg1[0] == '\0')
  {
    sprintf(buf, "You have %2.0f experience points.\n\r", ch->exp);
    send_to_char(buf, ch);
    sprintf(arg1, "foo");
  }
  if (!str_cmp(arg1,"toughness"))  is_ok = TRUE;

  else if (!str_cmp(arg1, "slash") || !str_cmp(arg1, "smash") ||
           !str_cmp(arg1, "beast") || !str_cmp(arg1, "grab") ||
           !str_cmp(arg1, "heat") || !str_cmp(arg1, "cold") || !str_cmp(arg1, "lightning") ||
           !str_cmp(arg1, "acid") || !str_cmp(arg1, "drain") || !str_cmp(arg1, "hurl") ||
           !str_cmp(arg1, "backstab") || !str_cmp(arg1, "kick") || !str_cmp(arg1, "disarm") || !str_cmp(arg1, "steal") || !str_cmp(arg1, "stab"))
    is_ok = TRUE;

  cost = 200;
  immcost = count_imms(ch);

  primal = (1 + ch->practice) * 500;

  if (!str_cmp(arg1, "avatar") && (ch->level == 2))
  {
    if (ch->max_hit < 2000)
    {
      stc("You need 2000 hp to train avatar.\n\r", ch);
      return;
    }
    do_clearstats2(ch, "");
    if (ch->max_hit < 2000)
    {
      stc("You need 2000 hp to train avatar.\n\r", ch);
      return;
    }
    ch->level = 3;
    if (!ragnarok)
      ch->pcdata->safe_counter = 10;
    else
      ch->pcdata->safe_counter = 3;
    act("You become an avatar!", ch, NULL, NULL, TO_CHAR);
    if (IS_SET(ch->pcdata->jflags, JFLAG_SETAVATAR))
      avatar_message(ch);
    else
    {
      sprintf(buf, "%s has become an avatar!", ch->pcdata->switchname);
      avatar_info(buf);
    }
    return;
  }

else if (!str_cmp(arg1, "toughness"))
{
  if (IS_NPC(ch))
    return;
  if (ch->pcdata->damreduct < 50 || (ch->pcdata->damreduct == 50 && ch->pcdata->damreductdec <= 0))
  {
    send_to_char("You may not become any tougher.\n\r",ch);
    return;
  }
  if (ch->exp < 2000000000)
  {
    send_to_char("It cost 2,000,000,000 exp per .1% of toughness.\n\r",ch);
    return;
  }
  ch->exp -= 2000000000;
  ch->pcdata->damreductdec -= 1;
  if (ch->pcdata->damreductdec < 0)
  {
    ch->pcdata->damreductdec = 9;
    ch->pcdata->damreduct -= 1;
  }
  send_to_char("You have gained a bit of stamina.\n\r",ch);
  return;
}


  else if (!str_cmp(arg1, "slash") && !IS_IMMUNE(ch, IMM_SLASH))

  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_SLASH);
    send_to_char("You are now more resistant to slashing and slicing weapons.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "stab") && !IS_IMMUNE(ch, IMM_STAB))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_STAB);
    send_to_char("You are now more resistant to stabbing and piercing weapons.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "smash") && !IS_IMMUNE(ch, IMM_SMASH))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_SMASH);
    send_to_char("You are now more resistant to blasting, pounding and crushing weapons.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "beast") && !IS_IMMUNE(ch, IMM_ANIMAL))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_ANIMAL);
    send_to_char("You are now more resistant to claw and bite attacks.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "grab") && !IS_IMMUNE(ch, IMM_MISC))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_MISC);
    send_to_char("You are now more resistant to grepping, sucking and whipping weapons.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "charm") && !IS_IMMUNE(ch, IMM_CHARM))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_CHARM);
    send_to_char("You are now immune to charm spells.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "heat") && !IS_IMMUNE(ch, IMM_HEAT))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_HEAT);
    send_to_char("You are now immune to heat and fire spells.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "cold") && !IS_IMMUNE(ch, IMM_COLD))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_COLD);
    send_to_char("You are now immune to cold spells.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "lightning") && !IS_IMMUNE(ch, IMM_LIGHTNING))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_LIGHTNING);
    send_to_char("You are now immune to lightning and electrical spells.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "acid") && !IS_IMMUNE(ch, IMM_ACID))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_ACID);
    send_to_char("You are now immune to acid spells.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "drain") && !IS_IMMUNE(ch, IMM_DRAIN))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_DRAIN);
    send_to_char("You are now immune to the energy drain spell.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "hurl") && !IS_IMMUNE(ch, IMM_HURL))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_HURL);
    send_to_char("You are now immune to being hurled.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "backstab") && !IS_IMMUNE(ch, IMM_BACKSTAB))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_BACKSTAB);
    send_to_char("You are now immune to being backstabbed.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "kick") && !IS_IMMUNE(ch, IMM_KICK))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_KICK);
    send_to_char("You are now immune to being kicked.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "disarm") && !IS_IMMUNE(ch, IMM_DISARM))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_DISARM);
    send_to_char("You are now immune to being disarmed.\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "steal") && !IS_IMMUNE(ch, IMM_STEAL))
  {
    if (ch->exp < immcost)
    {
      send_to_char("You don't have enough exp.\n\r", ch);
      return;
    }
    ch->exp = ch->exp - immcost;
    SET_BIT(ch->immune, IMM_STEAL);
    send_to_char("You are now immune to being stolen from.\n\r", ch);
    return;
  }

  else
  {
      send_to_char("#r+--------------------[#0AOH Helper#r]--------------------+#n\n\r", ch);
      send_to_char("#r|#0Proficiencies: Show Fightstyles, and Weapon types   #r|\n\r", ch);
      send_to_char("#r|#0Fightstyle: Select Jiujitsu, Aikido, or Kendo       #r|\n\r", ch);
      send_to_char("#r|#0Selfrace: Choose a race. Syntax selfrace highelf ex.#r|\n\r", ch);
      send_to_char("#r|#0Selfsphere:You get 5 sphers plus one per remort     #r|\n\r", ch);
      send_to_char("#r|#0Gainlevel: up to 200, for (hp, mana, move, damcap)  #r|\n\r", ch);
      send_to_char("#r+--------------------[#0AOH Helper#r]--------------------+#n\n\r", ch);



    sprintf(buf, "You can train the following:\n\r");
    send_to_char(buf, ch);

    if (ch->level == 2 && ch->max_hit > 1000)
    {
      sprintf(buf, "Become an avatar -  free.\n\r");
      send_to_char(buf, ch);
    }

   if( ch->pcLegacy < 20 ) {
      sprintf( buf, "#7L#0egacy#n           - %d exp and %d gold.\n\r", legacy_xp, legacy_gold );
      send_to_char( buf, ch );
   }

    if (ch->practice < 999)
    {
      sprintf(buf, "#7P#0rimal#n           - %d exp per point of primal energy.\n\r", (1 + ch->practice) * 500);
      send_to_char(buf, ch);
    }
    if (ch->generation == 3)
      send_to_char("#7G#0eneration#n       - 400 mill exp\n\r", ch);
    else if (ch->generation == 4)
      send_to_char("#7G#0eneration#n       - 200 mill exp\n\r", ch);
    else if (ch->generation == 5)
      send_to_char("#7G#0eneration#n       - 50 mill exp\n\r", ch);
    else if (ch->generation > 5)
      send_to_char("#7G#0eneration#n       - 10 mill exp\n\r", ch);
      sprintf(buf, "#7T#0oughness#n        - 2 billion exp to lower your damage taken to %d.%d%%.#n\n\r", ch->pcdata->damreductdec == 0 ? ch->pcdata->damreduct - 1 : ch->pcdata->damreduct, ch->pcdata->damreductdec == 0 ? 9 : ch->pcdata->damreductdec - 1);

        if (!IS_NPC(ch))
        {
          if (ch->pcdata->damreduct > 50 || (ch->pcdata->damreduct == 50 && ch->pcdata->damreductdec > 0))
          {
	    sprintf(buf, "#7T#0oughness#n        - 2 billion exp to lower your damage taken to %d.%d%%.#n\n\r", ch->pcdata->damreductdec == 0 ? ch->pcdata->damreduct - 1 : ch->pcdata->damreduct, ch->pcdata->damreductdec == 0 ? 9 : ch->pcdata->damreductdec - 1);
            send_to_char(buf,ch);
          }
        }


    sprintf(buf, "#7N#0atural #7r#0esistances #7a#0nd #7i#0mmunities#n - %d exp each.\n\r", immcost);
    send_to_char(buf, ch);

    /* Weapon resistance affects */
    send_to_char("#7W#0eapon #7r#0esistances:#n", ch);
    if (!IS_IMMUNE(ch, IMM_SLASH))
      send_to_char(" Slash", ch);
    if (!IS_IMMUNE(ch, IMM_STAB))
      send_to_char(" Stab", ch);
    if (!IS_IMMUNE(ch, IMM_SMASH))
      send_to_char(" Smash", ch);
    if (!IS_IMMUNE(ch, IMM_ANIMAL))
      send_to_char(" Beast", ch);
    if (!IS_IMMUNE(ch, IMM_MISC))
      send_to_char(" Grab", ch);
    if (IS_IMMUNE(ch, IMM_SLASH) && IS_IMMUNE(ch, IMM_STAB) && IS_IMMUNE(ch, IMM_SMASH) && IS_IMMUNE(ch, IMM_ANIMAL) && IS_IMMUNE(ch, IMM_MISC))
      send_to_char(" None left to learn.\n\r", ch);
    else
      send_to_char(".\n\r", ch);

    /* Spell immunity affects */
    send_to_char("#7M#0agical #7i#0mmunities:#n", ch);
    if (!IS_IMMUNE(ch, IMM_CHARM))
      send_to_char(" Charm", ch);
    if (!IS_IMMUNE(ch, IMM_HEAT))
      send_to_char(" Heat", ch);
    if (!IS_IMMUNE(ch, IMM_COLD))
      send_to_char(" Cold", ch);
    if (!IS_IMMUNE(ch, IMM_LIGHTNING))
      send_to_char(" Lightning", ch);
    if (!IS_IMMUNE(ch, IMM_ACID))
      send_to_char(" Acid", ch);
    if (!IS_IMMUNE(ch, IMM_DRAIN))
      send_to_char(" Drain", ch);
    if (IS_IMMUNE(ch, IMM_HEAT) && IS_IMMUNE(ch, IMM_COLD) && IS_IMMUNE(ch, IMM_LIGHTNING) && IS_IMMUNE(ch, IMM_ACID) && IS_IMMUNE(ch, IMM_DRAIN))
      send_to_char(" None left to learn.\n\r", ch);
    else
      send_to_char(".\n\r", ch);

    /* Skill immunity affects */
    send_to_char("#7S#0kill #7i#0mmunities:#n", ch);
    if (!IS_IMMUNE(ch, IMM_HURL))
      send_to_char(" Hurl", ch);
    if (!IS_IMMUNE(ch, IMM_BACKSTAB))
      send_to_char(" Backstab", ch);
    if (!IS_IMMUNE(ch, IMM_KICK))
      send_to_char(" Kick", ch);
    if (!IS_IMMUNE(ch, IMM_DISARM))
      send_to_char(" Disarm", ch);
    if (!IS_IMMUNE(ch, IMM_STEAL))
      send_to_char(" Steal", ch);
    if (IS_IMMUNE(ch, IMM_HURL) && IS_IMMUNE(ch, IMM_BACKSTAB) && IS_IMMUNE(ch, IMM_KICK) && IS_IMMUNE(ch, IMM_DISARM) && IS_IMMUNE(ch, IMM_STEAL))
      send_to_char(" None left to learn.\n\r", ch);
    else
      send_to_char(".\n\r", ch);

    return;
  }

  if ((*pAbility >= 999) && (!str_cmp(arg1, "primal")))
  {
    if (last)
      act("Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR);
    return;
  }

  if (cost < 1)    cost = 1;

  if (cost > ch->exp)
  {
    if (last)
      send_to_char("You don't have enough exp.\n\r", ch);
    return;
  }

  ch->exp -= cost;
  *pAbility += 1;

  if (last)
    act("Your $T increases!", ch, NULL, pOutput, TO_CHAR);
  return;
}

void do_mount(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Mount what?\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_POLYMORPH))
  {
    send_to_char("You cannot ride in this form.\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (ch == victim)
  {
    send_to_char("You cannot ride on your own back!\n\r", ch);
    return;
  }

  if (ch->mounted > 0)
  {
    send_to_char("You are already riding.\n\r", ch);
    return;
  }

  if (!IS_NPC(victim) && !IS_IMMORTAL(ch))
  {
    send_to_char("You cannot mount them.\n\r", ch);
    return;
  }
  if (victim->mounted > 0)
  {
    send_to_char("You cannot mount them.\n\r", ch);
    return;
  }
  if (IS_NPC(victim) && !IS_SET(victim->act, ACT_MOUNT) && !IS_IMMORTAL(ch))
  {
    send_to_char("You cannot mount them.\n\r", ch);
    return;
  }

  if (victim->position < POS_STANDING)
  {
    if (victim->position < POS_SLEEPING)
      act("$N is too badly hurt for that.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_SLEEPING)
      act("First you better wake $m up.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_RESTING)
      act("First $e better stand up.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_MEDITATING)
      act("First $e better stand up.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_SITTING)
      act("First $e better stand up.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_SLEEPING)
      act("First you better wake $m up.", ch, NULL, victim, TO_CHAR);
    else if (victim->position == POS_FIGHTING)
      act("Not while $e's fighting.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (!IS_NPC(ch) && ch->stance[0] != -1)
    do_stance(ch, "");

  ch->mounted = IS_RIDING;
  victim->mounted = IS_MOUNT;
  ch->mount = victim;
  victim->mount = ch;

  act("You clamber onto $N's back.", ch, NULL, victim, TO_CHAR);
  act("$n clambers onto $N's back.", ch, NULL, victim, TO_ROOM);
  return;
}

void do_dismount(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;

  if (IS_SET(ch->mounted, IS_RIDING))
  {
    if ((victim = ch->mount) == NULL)
    {
      ch->mounted = 0;
      send_to_char("You stop riding the air.\n\r", ch);
      return;
    }
  }
  if (ch->mounted == 0)
  {
    send_to_char("But you are not riding!\n\r", ch);
    return;
  }

  if ((victim = ch->mount) == NULL)
  {
    send_to_char("But you are not riding!\n\r", ch);
    return;
  }

  act("You clamber off $N's back.", ch, NULL, victim, TO_CHAR);
  act("$n clambers off $N's back.", ch, NULL, victim, TO_ROOM);

  ch->mounted = IS_ON_FOOT;
  victim->mounted = IS_ON_FOOT;

  ch->mount = NULL;
  victim->mount = NULL;

  return;
}

void do_tie(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  bool found = FALSE;

  if (IS_NPC(ch))
    return;

  argument = one_argument(argument, arg);
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You cannot tie a mob up!\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("You cannot tie yourself up!\n\r", ch);
    return;
  }
  if (IS_EXTRA(victim, TIED_UP))
  {
    send_to_char("But they are already tied up!\n\r", ch);
    return;
  }
  if (victim->position > POS_STUNNED || victim->hit > 0)
  {
    send_to_char("You can only tie up a defenceless person.\n\r", ch);
    return;
  }
  if (victim->in_room != NULL)
  {
    if (IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    {
      sprintf(buf, "#C%s #ohas been vanquished from the #Rarena#o by #C%s#n", victim->name, ch->name);
      do_info(ch, buf);
      victim->pcdata->alosses++;
      if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL)
        return;
      char_from_room(victim);
      char_to_room(victim, location);
      victim->fight_timer = 0;
      do_restore(victim, "self");
      do_call(victim, "all");
      /*  Check for winner  */
      found = FALSE;
      for (victim = char_list; victim != NULL; victim = victim->next)
      {
        if (IS_NPC(victim))
          continue;
        if (victim->in_room != NULL && victim->in_room->area == ch->in_room->area && victim->pcdata->chobj == NULL && victim != ch)
        {
          found = TRUE;
        }
      }
      if (!found)
      {
        sprintf(buf, "#C%s #oemerges victorious from the #Rarena#n", ch->name);
        ch->pcdata->awins++;
        do_info(ch, buf);
        if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL)
          return;
        char_from_room(ch);
        char_to_room(ch, location);
        ch->fight_timer = 0;
        do_restore(ch, "self");
        win_prize(ch);
      }
      return;
    }
  }
  act("You quickly tie up $N.", ch, NULL, victim, TO_CHAR);
  act("$n quickly ties up $N.", ch, NULL, victim, TO_ROOM);
  send_to_char("You have been tied up!\n\r", victim);
  SET_BIT(victim->extra, TIED_UP);
  if (IS_SET(ch->pcdata->jflags, JFLAG_SETTIE))
    tie_message(ch, victim);
  else
  {
    sprintf(buf, "#P%s #yhas been tied up by #R%s#n", victim->name, ch->name);
    do_info(ch, buf);
  }
  return;
}

void do_untie(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (!IS_EXTRA(victim, TIED_UP))
  {
    send_to_char("But they are not tied up!\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("You cannot untie yourself!\n\r", ch);
    return;
  }
  act("You quickly untie $N.", ch, NULL, victim, TO_CHAR);
  act("$n quickly unties $N.", ch, NULL, victim, TO_NOTVICT);
  act("$n quickly unties you.", ch, NULL, victim, TO_VICT);
  REMOVE_BIT(victim->extra, TIED_UP);
  return;
}

void do_gag(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch && !IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, TIED_UP))
  {
    send_to_char("You cannot gag yourself!\n\r", ch);
    return;
  }
  if (!IS_EXTRA(victim, TIED_UP) && !IS_EXTRA(victim, GAGGED))
  {
    send_to_char("You can only gag someone who is tied up!\n\r", ch);
    return;
  }
  if (!IS_EXTRA(victim, GAGGED))
  {
    act("You place a gag over $N's mouth.", ch, NULL, victim, TO_CHAR);
    act("$n places a gag over $N's mouth.", ch, NULL, victim, TO_NOTVICT);
    act("$n places a gag over your mouth.", ch, NULL, victim, TO_VICT);
    SET_BIT(victim->extra, GAGGED);
    return;
  }
  if (ch == victim)
  {
    act("You remove the gag from your mouth.", ch, NULL, victim, TO_CHAR);
    act("$n removes the gag from $s mouth.", ch, NULL, victim, TO_ROOM);
    REMOVE_BIT(victim->extra, GAGGED);
    return;
  }
  act("You remove the gag from $N's mouth.", ch, NULL, victim, TO_CHAR);
  act("$n removes the gag from $N's mouth.", ch, NULL, victim, TO_NOTVICT);
  act("$n removes the gag from your mouth.", ch, NULL, victim, TO_VICT);
  REMOVE_BIT(victim->extra, GAGGED);
  return;
}

void do_blindfold(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch && !IS_EXTRA(victim, BLINDFOLDED) && IS_EXTRA(victim, TIED_UP))
  {
    send_to_char("You cannot blindfold yourself!\n\r", ch);
    return;
  }
  if (!IS_EXTRA(victim, TIED_UP) && !IS_EXTRA(victim, BLINDFOLDED))
  {
    send_to_char("You can only blindfold someone who is tied up!\n\r", ch);
    return;
  }
  if (!IS_EXTRA(victim, BLINDFOLDED))
  {
    act("You place a blindfold over $N's eyes.", ch, NULL, victim, TO_CHAR);
    act("$n places a blindfold over $N's eyes.", ch, NULL, victim, TO_NOTVICT);
    act("$n places a blindfold over your eyes.", ch, NULL, victim, TO_VICT);
    SET_BIT(victim->extra, BLINDFOLDED);
    return;
  }
  if (ch == victim)
  {
    act("You remove the blindfold from your eyes.", ch, NULL, victim, TO_CHAR);
    act("$n removes the blindfold from $s eyes.", ch, NULL, victim, TO_ROOM);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    return;
  }
  act("You remove the blindfold from $N's eyes.", ch, NULL, victim, TO_CHAR);
  act("$n removes the blindfold from $N's eyes.", ch, NULL, victim, TO_NOTVICT);
  act("$n removes the blindfold from your eyes.", ch, NULL, victim, TO_VICT);
  REMOVE_BIT(victim->extra, BLINDFOLDED);
  return;
}

int count_imms(CHAR_DATA * ch)
{
  int count = 0;

  if (IS_IMMUNE(ch, IMM_SLASH))
    count += 1;
  if (IS_IMMUNE(ch, IMM_STAB))
    count += 1;
  if (IS_IMMUNE(ch, IMM_SMASH))
    count += 1;
  if (IS_IMMUNE(ch, IMM_ANIMAL))
    count += 1;
  if (IS_IMMUNE(ch, IMM_MISC))
    count += 1;
  if (IS_IMMUNE(ch, IMM_CHARM))
    count += 1;
  if (IS_IMMUNE(ch, IMM_HEAT))
    count += 1;
  if (IS_IMMUNE(ch, IMM_COLD))
    count += 1;
  if (IS_IMMUNE(ch, IMM_LIGHTNING))
    count += 1;
  if (IS_IMMUNE(ch, IMM_ACID))
    count += 1;
  if (IS_IMMUNE(ch, IMM_SLEEP))
    count += 1;
  if (IS_IMMUNE(ch, IMM_DRAIN))
    count += 1;
  if (IS_IMMUNE(ch, IMM_HURL))
    count += 1;
  if (IS_IMMUNE(ch, IMM_BACKSTAB))
    count += 1;
  if (IS_IMMUNE(ch, IMM_KICK))
    count += 1;
  if (IS_IMMUNE(ch, IMM_DISARM))
    count += 1;
  if (IS_IMMUNE(ch, IMM_STEAL))
    count += 1;
  return ((count * 10000) + 10000);
}
void do_track(CHAR_DATA * ch, char *argument)
{
  bool found = FALSE;

  if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
  {
    send_to_char("You cannot sense any trails from this room.\n\r", ch);
    return;
  }
  if (check_track(ch, 0))
    found = TRUE;
  if (check_track(ch, 1))
    found = TRUE;
  if (check_track(ch, 2))
    found = TRUE;
  if (check_track(ch, 3))
    found = TRUE;
  if (check_track(ch, 4))
    found = TRUE;
  if (found == FALSE)
  {
    send_to_char("You cannot sense any trails from this room.\n\r", ch);
    return;
  }
  act("$n carefully examines the ground for tracks.", ch, NULL, NULL, TO_ROOM);
  return;
}

void do_hunt(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (IS_NPC(ch)) return;

  if (arg[0] == '\0')
  {
    if (strlen(ch->hunting) > 1)
    {
      free_string(ch->hunting);
      ch->hunting = str_dup("");
      send_to_char("You stop hunting your prey.\n\r", ch);
    }
    else
      send_to_char("Who do you wish to hunt?\n\r", ch);
    return;
  }
  if (!str_cmp(arg, ch->name))
  {
    send_to_char("How can you hunt yourself?\n\r", ch);
    return;
  }
  free_string(ch->hunting);
  ch->hunting = str_dup(arg);
  send_to_char("Ok.\n\r", ch);
  return;
}

void check_hunt(CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  bool found = FALSE;
  int direction = 0;
  ROOM_INDEX_DATA *in_room;

  in_room = ch->in_room;
  if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
  {
    send_to_char("You cannot sense any trails from this room.\n\r", ch);
    free_string(ch->hunting);
    ch->hunting = str_dup("");
    return;
  }
  if (check_track(ch, 0))
  {
    found = TRUE;
    direction = ch->in_room->track_dir[0];
  }
  else if (check_track(ch, 1))
  {
    found = TRUE;
    direction = ch->in_room->track_dir[1];
  }
  else if (check_track(ch, 2))
  {
    found = TRUE;
    direction = ch->in_room->track_dir[2];
  }
  else if (check_track(ch, 3))
  {
    found = TRUE;
    direction = ch->in_room->track_dir[3];
  }
  else if (check_track(ch, 4))
  {
    found = TRUE;
    direction = ch->in_room->track_dir[4];
  }
  else if ((victim = get_char_room(ch, ch->hunting)) == NULL)
  {
    send_to_char("You cannot sense any trails from this room.\n\r", ch);
    free_string(ch->hunting);
    ch->hunting = str_dup("");
    return;
  }
  if (strlen(ch->hunting) < 2)
    return;
  if ((victim = get_char_room(ch, ch->hunting)) != NULL)
    return;
  act("$n carefully examines the ground for tracks.", ch, NULL, NULL, TO_ROOM);
  move_char(ch, direction);
  if (in_room == ch->in_room || victim != NULL)
  {
    free_string(ch->hunting);
    ch->hunting = str_dup("");
  }
  return;
}

void add_tracks(CHAR_DATA * ch, int direction)
{
  int loop;

  if (IS_NPC(ch))
    return;
  if (IS_ITEMAFF(ch, ITEMA_STALKER))
    return;
  for (loop = 0; loop <= 4; loop++)
  {
    if (ch->in_room->track[loop] != NULL && !str_cmp(ch->in_room->track[loop], ch->name))
    {
      free_string(ch->in_room->track[loop]);
      ch->in_room->track[loop] = str_dup("");
    }
  }
  if (ch->in_room->track[0] != NULL && strlen(ch->in_room->track[0]) < 2)
  {
    free_string(ch->in_room->track[0]);
    ch->in_room->track[0] = str_dup(ch->pcdata->switchname);
    ch->in_room->track_dir[0] = direction;
  }
  else if (ch->in_room->track[1] != NULL && strlen(ch->in_room->track[1]) < 2)
  {
    free_string(ch->in_room->track[1]);
    ch->in_room->track[1] = str_dup(ch->pcdata->switchname);
    ch->in_room->track_dir[1] = direction;
  }
  else if (ch->in_room->track[2] != NULL && strlen(ch->in_room->track[2]) < 2)
  {
    free_string(ch->in_room->track[2]);
    ch->in_room->track[2] = str_dup(ch->pcdata->switchname);
    ch->in_room->track_dir[2] = direction;
  }
  else if (ch->in_room->track[3] != NULL && strlen(ch->in_room->track[3]) < 2)
  {
    free_string(ch->in_room->track[3]);
    ch->in_room->track[3] = str_dup(ch->pcdata->switchname);
    ch->in_room->track_dir[3] = direction;
  }
  else if (ch->in_room->track[4] != NULL && strlen(ch->in_room->track[4]) < 2)
  {
    free_string(ch->in_room->track[4]);
    ch->in_room->track[4] = str_dup(ch->pcdata->switchname);
    ch->in_room->track_dir[4] = direction;
  }
  else
  {
    free_string(ch->in_room->track[0]);
    ch->in_room->track[0] = str_dup(ch->in_room->track[1]);
    ch->in_room->track_dir[0] = ch->in_room->track_dir[1];
    free_string(ch->in_room->track[1]);
    ch->in_room->track[1] = str_dup(ch->in_room->track[2]);
    ch->in_room->track_dir[1] = ch->in_room->track_dir[2];
    free_string(ch->in_room->track[2]);
    ch->in_room->track[2] = str_dup(ch->in_room->track[3]);
    ch->in_room->track_dir[2] = ch->in_room->track_dir[3];
    free_string(ch->in_room->track[3]);
    ch->in_room->track[3] = str_dup(ch->in_room->track[4]);
    ch->in_room->track_dir[3] = ch->in_room->track_dir[4];
    free_string(ch->in_room->track[4]);
    ch->in_room->track[4] = str_dup(ch->name);
    ch->in_room->track_dir[4] = direction;
  }
  return;
}

bool check_track(CHAR_DATA * ch, int direction)
{
  CHAR_DATA *victim;
  char buf[MAX_INPUT_LENGTH];
  char vict[MAX_INPUT_LENGTH];
  int door;

  if (ch->hunting != NULL)
    strcpy(vict, ch->hunting);
  else
    return FALSE;
  if (!str_cmp(ch->hunting, vict))
  {
    if ((victim = get_char_room(ch, vict)) != NULL)
    {
      act("You have found $N!", ch, NULL, victim, TO_CHAR);
      free_string(ch->hunting);
      ch->hunting = str_dup("");
      return TRUE;
    }
  }
  if (strlen(ch->in_room->track[direction]) < 2)
    return FALSE;
  if (!str_cmp(ch->in_room->track[direction], ch->name))
    return FALSE;
  if (strlen(ch->hunting) > 1 && str_cmp(ch->in_room->track[direction], ch->hunting))
    return FALSE;
  door = ch->in_room->track_dir[direction];
  sprintf(buf, "You sense the trail of %s leading $T from here.", ch->in_room->track[direction]);
  act(buf, ch, NULL, dir_name[door], TO_CHAR);
  return TRUE;
}

