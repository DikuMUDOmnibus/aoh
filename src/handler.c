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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

AFFECT_DATA  *affect_free;
AFFECT2_DATA *affect2_free;

/*
 * Local functions.
 */

/* returns race number */
int race_lookup (const char *name)
{
   int race;

   for ( race = 0; race_table[race].name != NULL; race++)
   {
	if (LOWER(name[0]) == LOWER(race_table[race].name[0])
	&&  !str_prefix( name,race_table[race].name))
	    return race;
   }

   return 0;
} 


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust(CHAR_DATA * ch)
{
  if (ch->trust != 0)
    return ch->trust;

  if (IS_NPC(ch) && ch->level >= LEVEL_HERO)
    return LEVEL_HERO - 1;
  else
    return ch->level;
}

/*
 * Retrieve a character's age.
 */
int get_age(CHAR_DATA * ch)
{
  return 17 + (ch->played + (int) (current_time - ch->logon)) / 7200;
}

/*
 * Retrieve character's current strength.
 */
int get_curr_str(CHAR_DATA * ch)
{
  int max;

  if (IS_NPC(ch))
    return 13;

  max = 25;

  return URANGE(3, ch->pcdata->perm_str + ch->pcdata->mod_str, max);
}

/*
 * Retrieve character's current intelligence.
 */
int get_curr_int(CHAR_DATA * ch)
{
  int max;

  if (IS_NPC(ch))
    return 13;

  max = 25;

  return URANGE(3, ch->pcdata->perm_int + ch->pcdata->mod_int, max);
}

/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis(CHAR_DATA * ch)
{
  int max = 25;

  if (IS_NPC(ch))
    return 13;

  max = 25;

  return URANGE(3, ch->pcdata->perm_wis + ch->pcdata->mod_wis, max);
}

/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex(CHAR_DATA * ch)
{
  int max;

  if (IS_NPC(ch))
    return 13;

  max = 25;

  return URANGE(3, ch->pcdata->perm_dex + ch->pcdata->mod_dex, max);
}

/*
 * Retrieve character's current constitution.
 */
int get_curr_con(CHAR_DATA * ch)
{
  int max;

  if (IS_NPC(ch))
    return 13;

  max = 25;

  return URANGE(3, ch->pcdata->perm_con + ch->pcdata->mod_con, max);
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n(CHAR_DATA * ch)
{
  if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL)
    return 100000;

  return MAX_WEAR + 2 * get_curr_dex(ch) + 2000;
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w(CHAR_DATA * ch)
{
  if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL)
    return 1000000;

//  return str_app[get_curr_str(ch)].carry;
	return 100000;
}

/*
 * See if a string is one of the names of an object.
 */

bool is_name(char *str, char *namelist)
{
  char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
  char *list, *string;

  /* fix crash on NULL namelist */
  if (namelist == NULL || namelist[0] == '\0')
    return FALSE;

  /* fixed to prevent is_name on "" returning TRUE */
  if (str[0] == '\0')
    return FALSE;

  string = str;
  /* we need ALL parts of string to match part of namelist */
  for (;;)                      /* start parsing string */
  {
    str = one_argument(str, part);

    if (part[0] == '\0')
      return TRUE;

    /* check to see if this is part of namelist */
    list = namelist;
    for (;;)                    /* start parsing namelist */
    {
      list = one_argument(list, name);
      if (name[0] == '\0')      /* this name was not found */
        return FALSE;

      if (!str_prefix(string, name))
        return TRUE;            /* full pattern match */

      if (!str_prefix(part, name))
        break;
    }
  }
}

bool is_full_name(const char *str, char *namelist)
{
  char name[MAX_INPUT_LENGTH];

  for (;;)
  {
    namelist = one_argument(namelist, name);
    if (name[0] == '\0')
      return FALSE;
    if (!str_cmp(str, name))
      return TRUE;
  }
}

bool char_exists(bool backup, char *argument)
{   
  FILE *fp;
  char buf[MAX_STRING_LENGTH];
  bool found = FALSE;
   
  fclose(fpReserve);
  if (backup)
    sprintf(buf, "%sbackup/%s", PLAYER_DIR, capitalize(argument));
  else
    sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
  if ((fp = fopen(buf, "r")) != NULL)
  { 
    found = TRUE;
    fclose(fp);
  }
  fpReserve = fopen(NULL_FILE, "r");
  return found;
}   


/*
 * Apply or remove an affect to a character.
 */
void affect_modify(CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd, OBJ_DATA * obj)
{
  OBJ_DATA *wield;
  int mod;

  mod = paf->modifier;

  if (fAdd)
  {
    SET_BIT(ch->affected_by, paf->bitvector);
  }
  else
  {
    REMOVE_BIT(ch->affected_by, paf->bitvector);
    mod = 0 - mod;
  }

  if (IS_NPC(ch))
  {
    switch (paf->location)
    {
      default:
        break;
      case APPLY_NONE:
        break;
      case APPLY_MANA:
        ch->max_mana += mod;
        break;
      case APPLY_HIT:
        ch->max_hit += mod;
        break;
      case APPLY_MOVE:
        ch->max_move += mod;
        break;
      case APPLY_AC:
        ch->armor += mod;
        break;
      case APPLY_HITROLL:
        ch->hitroll += mod;
        break;
      case APPLY_DAMROLL:
        ch->damroll += mod;
        break;
      case APPLY_SAVING_PARA:
        ch->saving_throw += mod;
        break;
      case APPLY_SAVING_ROD:
        ch->saving_throw += mod;
        break;
      case APPLY_SAVING_PETRI:
        ch->saving_throw += mod;
        break;
      case APPLY_SAVING_BREATH:
        ch->saving_throw += mod;
        break;
      case APPLY_SAVING_SPELL:
        ch->saving_throw += mod;
        break;
      case APPLY_POLY:
        ch->polyaff += mod;
        break;
    }
    return;
  }

  switch (paf->location)
  {
    default:
      bug("Affect_modify: unknown location %d.", paf->location);
      return;

    case APPLY_NONE:
      break;
    case APPLY_STR:
      ch->pcdata->mod_str += mod;
      break;
    case APPLY_DEX:
      ch->pcdata->mod_dex += mod;
      break;
    case APPLY_INT:
      ch->pcdata->mod_int += mod;
      break;
    case APPLY_WIS:
      ch->pcdata->mod_wis += mod;
      break;
    case APPLY_CON:
      ch->pcdata->mod_con += mod;
      break;
    case APPLY_SEX:
    case APPLY_CLASS:
    case APPLY_LEVEL:
    case APPLY_AGE:
    case APPLY_HEIGHT:
    case APPLY_WEIGHT:
      break;
    case APPLY_MANA:
      ch->max_mana += mod;
      break;
    case APPLY_HIT:
      ch->max_hit += mod;
      break;
    case APPLY_MOVE:
      ch->max_move += mod;
      break;
    case APPLY_GOLD:
      break;
    case APPLY_EXP:
      break;
    case APPLY_AC:
      ch->armor += mod;
      break;
    case APPLY_HITROLL:
      ch->hitroll += mod;
      break;
    case APPLY_DAMROLL:
      ch->damroll += mod;
      break;
    case APPLY_SAVING_PARA:
      ch->saving_throw += mod;
      break;
    case APPLY_SAVING_ROD:
      ch->saving_throw += mod;
      break;
    case APPLY_SAVING_PETRI:
      ch->saving_throw += mod;
      break;
    case APPLY_SAVING_BREATH:
      ch->saving_throw += mod;
      break;
    case APPLY_SAVING_SPELL:
      ch->saving_throw += mod;
      break;
    case APPLY_POLY:
      ch->polyaff += mod;
      break;
  }

  /*
   * Check for weapon wielding.
   * Guard against recursion (for weapons with affects).
   */
  if ((wield = get_eq_char(ch, WEAR_WIELD)) != NULL && wield->item_type == ITEM_WEAPON && get_obj_weight(wield) > str_app[get_curr_str(ch)].wield)
  {
    static int depth;

    if (depth == 0)
    {
      depth++;
      act("You drop $p.", ch, wield, NULL, TO_CHAR);
      act("$n drops $p.", ch, wield, NULL, TO_ROOM);
      obj_from_char(wield);
      obj_to_room(wield, ch->in_room);
      depth--;
    }
  }

  return;
}

void affect2_modify( CHAR_DATA *ch, AFFECT2_DATA *paf, bool fAdd, OBJ_DATA *obj )
{
    OBJ_DATA *wield;
    int mod;

    mod = paf->modifier;

    if ( fAdd )
    {
	SET_BIT( ch->affected_by2, paf->bitvector );
    }
    else
    {
	REMOVE_BIT( ch->affected_by2, paf->bitvector );
	mod = 0 - mod;
    }

    if ( IS_NPC(ch) )
    {
    	switch ( paf->location )
    	{
	    default: break;
	    case APPLY_NONE:						break;
	    case APPLY_MANA:          ch->max_mana		+= mod;	break;
	    case APPLY_HIT:           ch->max_hit		+= mod;	break;
	    case APPLY_MOVE:          ch->max_move		+= mod;	break;
	    case APPLY_AC:            ch->armor			+= mod;	break;
	    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
	    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
	    case APPLY_SAVING_PARA:   ch->saving_throw		+= mod;	break;
	    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
	    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
	    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
	    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
	    case APPLY_POLY:	      ch->polyaff		+= mod;	break;
	}
	return;
    }
/*    if( !IS_NPC(ch) && obj != NULL && IS_CLASS(ch, CLASS_SAMURAI) && obj->pIndexData->vnum == 33177 )
    {
    switch ( paf->location )
    {
    default:
	bug( "Affect_modify: unknown location %d.", paf->location );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:           ch->pcdata->mod_str	+= mod;	break;
    case APPLY_DEX:           ch->pcdata->mod_dex	+= mod;	break;
    case APPLY_INT:           ch->pcdata->mod_int	+= mod;	break;
    case APPLY_WIS:           ch->pcdata->mod_wis	+= mod;	break;
    case APPLY_CON:           ch->pcdata->mod_con	+= mod;	break;

    case APPLY_SEX:           ch->sex			+= mod;	break;

    case APPLY_SEX:						break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->max_mana		+= mod;	break;
    case APPLY_HIT:           ch->max_hit		+= mod;	break;
    case APPLY_MOVE:          ch->max_move		+= mod;	break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor			+= mod;	break;
    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
    case APPLY_SAVING_PARA:   ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
    case APPLY_POLY:	      ch->polyaff		+= mod;	break;
    }
	return;
    }
    if ( IS_CLASS(ch, CLASS_SAMURAI) )
    {
    	switch ( paf->location )
    	{
	    default: break;
	    case APPLY_NONE:						break;
	}
	return;
    }
*/

    switch ( paf->location )
    {
    default:
	bug( "Affect_modify: unknown location %d.", paf->location );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:           ch->pcdata->mod_str	+= mod;	break;
    case APPLY_DEX:           ch->pcdata->mod_dex	+= mod;	break;
    case APPLY_INT:           ch->pcdata->mod_int	+= mod;	break;
    case APPLY_WIS:           ch->pcdata->mod_wis	+= mod;	break;
    case APPLY_CON:           ch->pcdata->mod_con	+= mod;	break;
/*
    case APPLY_SEX:           ch->sex			+= mod;	break;
*/
    case APPLY_SEX:						break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->max_mana		+= mod;	break;
    case APPLY_HIT:           ch->max_hit		+= mod;	break;
    case APPLY_MOVE:          ch->max_move		+= mod;	break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor			+= mod;	break;
    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
    case APPLY_SAVING_PARA:   ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
    case APPLY_POLY:	      ch->polyaff		+= mod;	break;
    }


    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL
    &&   wield->item_type == ITEM_WEAPON
    &&   get_obj_weight(wield) > str_app[(get_curr_str(ch)>35)?35:get_curr_str(ch)].wield )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act( "You drop $p.", ch, wield, NULL, TO_CHAR );
	    act( "$n drops $p.", ch, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, ch->in_room );
	    depth--;
	}
    }

    return;
}


/*
 * Remove an exit from a room					-Thoric
 */
void extract_exit(ROOM_INDEX_DATA * room, EXIT_DATA * pexit)
{
  UNLINK(pexit, room->first_exit, room->last_exit, next, prev);
  if (pexit->rexit)
    pexit->rexit->rexit = NULL;
  STRFREE(pexit->keyword);
  STRFREE(pexit->description);
  DISPOSE(pexit);
}

void affect_to_obj(OBJ_DATA * obj, AFFECT_DATA * paf)
{
  AFFECT_DATA *paf_new;

  if (affect_free == NULL)
  {
    paf_new = alloc_perm(sizeof(*paf_new));
  }
  else
  {
    paf_new = affect_free;
    affect_free = affect_free->next;
  }

  *paf_new = *paf;
  paf_new->next = obj->affected;
  obj->affected = paf_new;

  return;
}

/*
 * Give an affect to a char.
 */
void affect_to_char(CHAR_DATA * ch, AFFECT_DATA * paf)
{
  AFFECT_DATA *paf_new;

  if (affect_free == NULL)
  {
    paf_new = alloc_perm(sizeof(*paf_new));
  }
  else
  {
    paf_new = affect_free;
    affect_free = affect_free->next;
  }

  *paf_new = *paf;
  paf_new->next = ch->affected;
  ch->affected = paf_new;

  affect_modify(ch, paf_new, TRUE, NULL);
  return;
}

void affect2_to_char( CHAR_DATA *ch, AFFECT2_DATA *paf )
{
    AFFECT2_DATA *paf_new;

    if ( affect2_free == NULL )
    {
	paf_new		= alloc_perm( sizeof(*paf_new) );
    }
    else
    {
	paf_new		= affect2_free;
	affect2_free	= affect2_free->next;
    }

    *paf_new		= *paf;
    paf_new->next	= ch->affected2;
    ch->affected2	= paf_new;

    affect2_modify( ch, paf_new, TRUE, NULL );
    return;
}


void alias_remove(CHAR_DATA * ch, ALIAS_DATA * ali)
{
  if (IS_NPC(ch))
    return;
  if (ch->pcdata->alias == NULL)
  {
    bug("Alias_remove: no alias.", 0);
    return;
  }
  if (ali == ch->pcdata->alias)
  {
    ch->pcdata->alias = ali->next;
  }
  else
  {
    ALIAS_DATA *prev;

    for (prev = ch->pcdata->alias; prev; prev = prev->next)
    {
      if (prev->next == ali)
      {
        prev->next = ali->next;
        break;
      }
    }
    if (prev == NULL)
    {
      bug("Alias_remove: cannot find ali.", 0);
      return;
    }
  }
  ali->next = alias_free;
  alias_free = ali;
  return;
}

/*
 * Remove an affect from a char.
 */
void affect_remove(CHAR_DATA * ch, AFFECT_DATA * paf)
{
  if (ch->affected == NULL)
  {
    bug("Affect_remove: no affect.", 0);
    return;
  }

  affect_modify(ch, paf, FALSE, NULL);

  if (paf == ch->affected)
  {
    ch->affected = paf->next;
  }
  else
  {
    AFFECT_DATA *prev;

    for (prev = ch->affected; prev != NULL; prev = prev->next)
    {
      if (prev->next == paf)
      {
        prev->next = paf->next;
        break;
      }
    }

    if (prev == NULL)
    {
      bug("Affect_remove: cannot find paf.", 0);
      return;
    }
  }

  paf->next = affect_free;
  affect_free = paf;
  return;
}

/*
 * Strip all affects of a given sn.
 */
void affect_strip(CHAR_DATA * ch, int sn)
{
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    if (paf->type == sn)
      affect_remove(ch, paf);
  }

  return;
}

/*
 * Return true if a char is affected by a spell.
 */
bool is_affected(CHAR_DATA * ch, int sn)
{
  AFFECT_DATA *paf;

  for (paf = ch->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type == sn)
      return TRUE;
  }

  return FALSE;
}

/*
 * Add or enhance an affect.
 */
void affect_join(CHAR_DATA * ch, AFFECT_DATA * paf)
{
  AFFECT_DATA *paf_old;
  bool found;

  found = FALSE;
  for (paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next)
  {
    if (paf_old->type == paf->type)
    {
      paf->duration += paf_old->duration;
      paf->modifier += paf_old->modifier;
      affect_remove(ch, paf_old);
      break;
    }
  }

  affect_to_char(ch, paf);
  return;
}

/*
 * Move a char out of a room.
 */
void char_from_room(CHAR_DATA * ch)
{
  OBJ_DATA *obj;

  if (ch->in_room == NULL)
  {
    bug("Char_from_room: NULL.", 0);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room->light > 0)
    --ch->in_room->light;
  else if ((obj = get_eq_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room->light > 0)
    --ch->in_room->light;

  if (!IS_NPC(ch) && IS_SET(ch->newbits, NEW_DARKNESS))
  {
    if (ch->in_room != NULL)
      REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
  }

  if (ch == ch->in_room->people)
  {
    ch->in_room->people = ch->next_in_room;
  }
  else
  {
    CHAR_DATA *prev;

    for (prev = ch->in_room->people; prev; prev = prev->next_in_room)
    {
      if (prev->next_in_room == ch)
      {
        prev->next_in_room = ch->next_in_room;
        break;
      }
    }

    if (prev == NULL)
      bug("Char_from_room: ch not found.", 0);
  }

  ch->in_room = NULL;
  ch->next_in_room = NULL;
  return;
}

/*
 * Move a char into a room.
 */
void char_to_room(CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex)
{
  OBJ_DATA *obj;

  if (!pRoomIndex)
  {
    char buf[MAX_STRING_LENGTH];

    sprintf(buf, "Char_to_room: %s -> NULL room!  Putting char in limbo (%d)", ch->name, ROOM_VNUM_LIMBO);
    bug(buf, 0);
    /* This used to just return, but there was a problem with crashing
       and I saw no reason not to just put the char in limbo. -Narn */
    pRoomIndex = get_room_index(ROOM_VNUM_LIMBO);
  }

  ch->in_room = pRoomIndex;
  ch->next_in_room = pRoomIndex->people;
  pRoomIndex->people = ch;

  if ((obj = get_eq_char(ch, WEAR_WIELD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
    ++ch->in_room->light;
  else if ((obj = get_eq_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
    ++ch->in_room->light;

  if (ch->loc_hp[6] > 0 && ch->in_room->blood < 1000)
    ch->in_room->blood += 1;

  if (!IS_NPC(ch) && IS_SET(ch->newbits, NEW_DARKNESS))
    SET_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);

  return;
}

/*
 * Give an obj to a char.
 */
void obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch)
{
  if (obj == NULL)
    return;
  obj->next_content = ch->carrying;
  ch->carrying = obj;
  obj->carried_by = ch;
  obj->in_room = NULL;
  obj->in_obj = NULL;
  ch->carry_number += 1;
  ch->carry_weight += get_obj_weight(obj);

  /* Artifact stuff */

  if (!IS_NPC(ch) && IS_SET(obj->quest, QUEST_ARTIFACT))
  {
    if (IS_SET(ch->extra, EXTRA_AFK) || IS_SET(ch->pcdata->tempflag, TEMP_ARTIFACT) || ch->level < 3 || getMight(ch) < 150)
    {
      bool flag = IS_SET(ch->pcdata->tempflag, TEMP_ARTIFACT);

      act("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
      act("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
      if (flag)
        SET_BIT(ch->pcdata->tempflag, TEMP_ARTIFACT); // got removed under obj_from_char()
    }
    else
    {
      SET_BIT(ch->pcdata->tempflag, TEMP_ARTIFACT);
    }
  }
  return;
}

/*
 * Take an obj from its character.
 */
void obj_from_char(OBJ_DATA * obj)
{
  CHAR_DATA *ch;

  if (obj == NULL)
    return;
  if ((ch = obj->carried_by) == NULL)
  {
    log_string("Obj_from_char: null ch.");
    return;
  }

  if (obj->wear_loc != WEAR_NONE)
    unequip_char(ch, obj);

  if (ch->carrying == obj)
  {
    ch->carrying = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = ch->carrying; prev != NULL; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL)

      log_string("Obj_from_char: obj not in list.");
  }

  obj->carried_by = NULL;
  obj->next_content = NULL;
  ch->carry_number -= 1;
  ch->carry_weight -= get_obj_weight(obj);

  /* Artifact stuff */
  if (IS_SET(obj->quest, QUEST_ARTIFACT) && !IS_NPC(ch))
  {
    REMOVE_BIT(ch->pcdata->tempflag, TEMP_ARTIFACT);
  }

  return;
}

/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac(OBJ_DATA * obj, int iWear)
{
  if (obj->item_type != ITEM_ARMOR)
    return 0;

  switch (iWear)
  {
    case WEAR_BODY:
      return 3 * obj->value[0];
    case WEAR_HEAD:
      return 2 * obj->value[0];
    case WEAR_LEGS:
      return 2 * obj->value[0];
    case WEAR_FEET:
      return obj->value[0];
    case WEAR_HANDS:
      return obj->value[0];
    case WEAR_ARMS:
      return obj->value[0];
    case WEAR_SHIELD:
      return obj->value[0];
    case WEAR_FINGER_L:
      return obj->value[0];
    case WEAR_FINGER_R:
      return obj->value[0];
    case WEAR_NECK_1:
      return obj->value[0];
    case WEAR_NECK_2:
      return obj->value[0];
    case WEAR_ABOUT:
      return 2 * obj->value[0];
    case WEAR_WAIST:
      return obj->value[0];
    case WEAR_WRIST_L:
      return obj->value[0];
    case WEAR_WRIST_R:
      return obj->value[0];
    case WEAR_HOLD:
      return obj->value[0];
    case WEAR_FACE:
      return obj->value[0];
    case WEAR_SCABBARD_L:
      return 0;
    case WEAR_SCABBARD_R:
      return 0;
  }

  return 0;
}

/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char(CHAR_DATA * ch, int iWear)
{
  OBJ_DATA *obj;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == iWear)
      return obj;
  }

  return NULL;
}

/*
 * Equip a char with an obj.
 */
void equip_char(CHAR_DATA * ch, OBJ_DATA * obj, int iWear)
{
  AFFECT_DATA *paf;
  int sn;

  if (obj->item_type == ITEM_ARMOR)
    sn = obj->value[3];
  else
    sn = obj->value[0] / 1000;

  if ((sn == 7 || sn == 54) && !IS_GOOD(ch))
  {
    act("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
    act("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    do_autosave(ch, "");
    return;
  }
  if (sn == 139 && !IS_EVIL(ch))
  {
    act("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
    act("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    do_autosave(ch, "");
    return;
  }

  if (!IS_NPC(ch) && IS_OBJ_STAT(obj, ITEM_LOYAL))
  {
    if (obj->questowner != NULL && str_cmp(ch->pcdata->switchname, obj->questowner) && strlen(obj->questowner) > 1)
    {
      act("$p leaps from $n to the ground.", ch, obj, NULL, TO_ROOM);
      act("$p leaps from you, and onto the ground.", ch, obj, NULL, TO_CHAR);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
      return;
    }
  }

  if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch)) || (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch)) || (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)))
  {
    /*
     * Thanks to Morgenes for the bug fix here!
     */
    act("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
    act("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    do_autosave(ch, "");
    return;
  }

  /* zap mobiles if they try to wear relics */
  if (IS_NPC(ch) && IS_SET(obj->quest, QUEST_RELIC))
  {
    act("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
    act("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    do_autosave(ch, "");
    return;
  }

  if (iWear == WEAR_SCABBARD_L || iWear == WEAR_SCABBARD_R)
  {
    obj->wear_loc = iWear;
    return;
  }

  ch->armor -= apply_ac(obj, iWear);
  obj->wear_loc = iWear;

  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    affect_modify(ch, paf, TRUE, obj);
  for (paf = obj->affected; paf != NULL; paf = paf->next)
    affect_modify(ch, paf, TRUE, obj);

  if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room != NULL)
    ++ch->in_room->light;

  if (obj->wear_loc == WEAR_NONE)
  {
    return;
  }
  if (((obj->item_type == ITEM_ARMOR) && (obj->value[3] >= 1))
      || ((obj->item_type == ITEM_WEAPON) && (obj->value[0] >= 1000))
      || IS_SET(obj->quest, QUEST_ARTIFACT))
  {
    /* artifact bonus */
    if (IS_SET(obj->quest, QUEST_ARTIFACT))
      SET_BIT(ch->itemaffect, ITEMA_ARTIFACT);

    if ((sn == 1) && (IS_AFFECTED(ch, AFF_BLIND)))
      return;
    else if ((sn == 2) && (IS_AFFECTED(ch, AFF_DETECT_INVIS)))
      return;
    else if ((sn == 3) && (IS_AFFECTED(ch, AFF_FLYING)))
      return;
    else if ((sn == 4) && (IS_AFFECTED(ch, AFF_INFRARED)))
      return;
    else if ((sn == 5) && (IS_AFFECTED(ch, AFF_INVISIBLE)))
      return;
    else if ((sn == 6) && (IS_AFFECTED(ch, AFF_PASS_DOOR)))
      return;
    else if ((sn == 7) && (IS_AFFECTED(ch, AFF_PROTECT)))
      return;
    else if ((sn == 8) && (IS_AFFECTED(ch, AFF_PROTECT_GOOD)))
      return;
    else if ((sn == 9) && (IS_AFFECTED(ch, AFF_SANCTUARY)))
      return;
    else if ((sn == 10) && (IS_AFFECTED(ch, AFF_DETECT_HIDDEN)))
      return;
    else if ((sn == 11) && (IS_AFFECTED(ch, AFF_SNEAK)))
      return;
    else if ((sn == 12) && (IS_ITEMAFF(ch, ITEMA_CHAOSSHIELD)))
      return;
    else if ((sn == 13) && (IS_ITEMAFF(ch, ITEMA_REGENERATE)))
      return;
    else if ((sn == 14) && (IS_ITEMAFF(ch, ITEMA_SPEED)))
      return;
    else if ((sn == 15) && (IS_ITEMAFF(ch, ITEMA_VORPAL)))
      return;
    else if ((sn == 16) && (IS_ITEMAFF(ch, ITEMA_RESISTANCE)))
      return;
    else if ((sn == 17) && (IS_ITEMAFF(ch, ITEMA_VISION)))
      return;
    else if ((sn == 18) && (IS_ITEMAFF(ch, ITEMA_STALKER)))
      return;
    else if ((sn == 19) && (IS_ITEMAFF(ch, ITEMA_VANISH)))
      return;

    if (sn == 1)
    {
      SET_BIT(ch->affected_by, AFF_BLIND);
      send_to_char("You cannot see a thing!\n\r", ch);
      act("$n seems to be blinded!", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 2)
    {
      SET_BIT(ch->affected_by, AFF_DETECT_INVIS);
      send_to_char("Your eyes tingle.\n\r", ch);
      act("$n's eyes flicker with light.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 3)
    {
      SET_BIT(ch->affected_by, AFF_FLYING);
      send_to_char("Your feet rise off the ground.\n\r", ch);
      act("$n's feet rise off the ground.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 4)
    {
      SET_BIT(ch->affected_by, AFF_INFRARED);
      send_to_char("Your eyes glow red.\n\r", ch);
      act("$n's eyes glow red.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 5)
    {
      SET_BIT(ch->affected_by, AFF_INVISIBLE);
      send_to_char("You fade out of existance.\n\r", ch);
      act("$n fades out of existance.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 6)
    {
      SET_BIT(ch->affected_by, AFF_PASS_DOOR);
      send_to_char("You turn translucent.\n\r", ch);
      act("$n turns translucent.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 7)
    {
      SET_BIT(ch->affected_by, AFF_PROTECT);
      send_to_char("You are surrounded by a divine aura.\n\r", ch);
      act("$n is surrounded by a divine aura.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 8)
    {
      SET_BIT(ch->affected_by, AFF_PROTECT_GOOD);
      send_to_char("You are surrounded by an unholy aura.\n\r", ch);
      act("$n is surrounded by an unholy aura.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 9)
    {
      SET_BIT(ch->affected_by, AFF_SANCTUARY);
      send_to_char("You are surrounded by a white aura.\n\r", ch);
      act("$n is surrounded by a white aura.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 10)
    {
      SET_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
      send_to_char("You awarenes improves.\n\r", ch);
      act("$n eyes tingle.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 11)
    {
      SET_BIT(ch->affected_by, AFF_SNEAK);
      send_to_char("Your footsteps stop making any sound.\n\r", ch);
      act("$n's footsteps stop making any sound.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 12)
    {
      SET_BIT(ch->itemaffect, ITEMA_CHAOSSHIELD);
      send_to_char("You are surrounded by a swirling shield of chaos.\n\r", ch);
      act("$n is surrounded by a swirling shield of chaos.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 13)
    {
      send_to_char("You feel revived.\n\r", ch);
      SET_BIT(ch->itemaffect, ITEMA_REGENERATE);
    }
    else if (sn == 14)
    {
      SET_BIT(ch->itemaffect, ITEMA_SPEED);
      send_to_char("You start moving faster than the eye can follow.\n\r", ch);
      act("$n starts moving faster than the eye can follow.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 15)
    {
      SET_BIT(ch->itemaffect, ITEMA_VORPAL);
    }
    else if (sn == 16)
    {
      SET_BIT(ch->itemaffect, ITEMA_RESISTANCE);
    }
    else if (sn == 17)
    {
      SET_BIT(ch->itemaffect, ITEMA_VISION);
      send_to_char("Your eyes begin to glow bright white.\n\r", ch);
      act("$n's eyes begin to glow bright white.", ch, NULL, NULL, TO_ROOM);
    }
    else if (sn == 18)
    {
      SET_BIT(ch->itemaffect, ITEMA_STALKER);
    }
    else if (sn == 19)
    {
      SET_BIT(ch->itemaffect, ITEMA_VANISH);
      send_to_char("You blend into the shadows.\n\r", ch);
      act("$n gradually fades into the shadows.", ch, NULL, NULL, TO_ROOM);
    }
  }
  return;
}

/*
 * Unequip a char with an obj.
 */
void unequip_char(CHAR_DATA * ch, OBJ_DATA * obj)
{
  CHAR_DATA *chch;
  OBJ_DATA *cObj;
  AFFECT_DATA *paf;
  int sn, sn2, i;
  bool spell_remove = TRUE;

  if (obj->wear_loc == WEAR_NONE)
  {
    bug("Unequip_char: already unequipped.", 0);
    return;
  }

  if (obj->wear_loc == WEAR_SCABBARD_L || obj->wear_loc == WEAR_SCABBARD_R)
  {
    obj->wear_loc = -1;
    return;
  }

  ch->armor += apply_ac(obj, obj->wear_loc);
  obj->wear_loc = -1;

  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    affect_modify(ch, paf, FALSE, obj);
  for (paf = obj->affected; paf != NULL; paf = paf->next)
    affect_modify(ch, paf, FALSE, obj);

  if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room != NULL && ch->in_room->light > 0)
    --ch->in_room->light;

  if ((chch = get_char_world(ch, ch->name)) == NULL)
    return;
  if (chch->desc != ch->desc)
    return;

  if (!IS_NPC(ch) && (ch->desc != NULL && ch->desc->connected != CON_PLAYING))
    return;

  if (((obj->item_type == ITEM_ARMOR) && (obj->value[3] >= 1))
      || ((obj->item_type == ITEM_WEAPON) && (obj->value[0] >= 1000))
      || IS_SET(obj->quest, QUEST_ARTIFACT))
  {
    /* take the arti bonus */
    if (IS_SET(obj->quest, QUEST_ARTIFACT))
      REMOVE_BIT(ch->itemaffect, ITEMA_ARTIFACT);

    if (obj->item_type == ITEM_ARMOR)
      sn = obj->value[3];
    else
      sn = obj->value[0] / 1000;

    /* checking for affects already on other pieces */
    for (i = 0; i < MAX_WEAR; i++)
    {
      if ((cObj = get_eq_char(ch, i)) != NULL) 
      {
        if (cObj == obj) continue;
        
        if (cObj->item_type == ITEM_ARMOR)
          sn2 = cObj->value[3];
        else sn2 = cObj->value[0] / 1000;
         
        if (sn == sn2) spell_remove = FALSE;
      }
    }

    if (spell_remove)
    {
      if (IS_AFFECTED(ch, AFF_BLIND) && sn == 1)
      {
        REMOVE_BIT(ch->affected_by, AFF_BLIND);
        send_to_char("You can see again.\n\r", ch);
        act("$n seems to be able to see again.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_DETECT_INVIS) && sn == 2)
      {
        REMOVE_BIT(ch->affected_by, AFF_DETECT_INVIS);
        send_to_char("Your eyes stop tingling.\n\r", ch);
        act("$n's eyes stop flickering with light.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_FLYING) && sn == 3)
      {
        REMOVE_BIT(ch->affected_by, AFF_FLYING);
        send_to_char("You slowly float to the ground.\n\r", ch);
        act("$n slowly floats to the ground.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_INFRARED) && sn == 4)
      {
        REMOVE_BIT(ch->affected_by, AFF_INFRARED);
        send_to_char("Your eyes stop glowing red.\n\r", ch);
        act("$n's eyes stop glowing red.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_INVISIBLE) && sn == 5)
      {
        REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
        send_to_char("You fade into existance.\n\r", ch);
        act("$n fades into existance.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_PASS_DOOR) && sn == 6)
      {
        REMOVE_BIT(ch->affected_by, AFF_PASS_DOOR);
        send_to_char("You feel solid again.\n\r", ch);
        act("$n is no longer translucent.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_PROTECT) && sn == 7)
      {
        REMOVE_BIT(ch->affected_by, AFF_PROTECT);
        send_to_char("The divine aura around you fades.\n\r", ch);
        act("The divine aura around $n fades.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_PROTECT_GOOD) && sn == 8)
      {
        REMOVE_BIT(ch->affected_by, AFF_PROTECT_GOOD);
        send_to_char("The unholy aura around you fades.\n\r", ch);
        act("The unholy aura around $n fades.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_SANCTUARY) && sn == 9)
      {
        REMOVE_BIT(ch->affected_by, AFF_SANCTUARY);
        send_to_char("The white aura around your body fades.\n\r", ch);
        act("The white aura about $n's body fades.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_DETECT_HIDDEN) && sn == 10)
      {
        REMOVE_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
        send_to_char("You feel less aware of your surrondings.\n\r", ch);
        act("$n eyes tingle.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_AFFECTED(ch, AFF_SNEAK) && sn == 11)
      {
        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        send_to_char("You are no longer moving so quietly.\n\r", ch);
        act("$n is no longer moving so quietly.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_ITEMAFF(ch, ITEMA_CHAOSSHIELD) && sn == 12)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_CHAOSSHIELD);
        send_to_char("The swirling shield of chaos around you fades.\n\r", ch);
        act("The swirling shield of chaos around $n fades.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_ITEMAFF(ch, ITEMA_REGENERATE) && sn == 13)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_REGENERATE);
      }
      else if (IS_ITEMAFF(ch, ITEMA_SPEED) && sn == 14)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_SPEED);
        send_to_char("Your actions slow down to normal speed.\n\r", ch);
        act("$n stops moving at supernatural speed.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_ITEMAFF(ch, ITEMA_VORPAL) && sn == 15)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_VORPAL);
      }
      else if (IS_ITEMAFF(ch, ITEMA_RESISTANCE) && sn == 16)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_RESISTANCE);
      }
      else if (IS_ITEMAFF(ch, ITEMA_VISION) && sn == 17)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_VISION);
        send_to_char("Your eyes stop glowing bright white.\n\r", ch);
        act("$n's eyes stop glowing bright white.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_ITEMAFF(ch, ITEMA_STALKER) && sn == 18)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_STALKER);
      }
      else if (IS_ITEMAFF(ch, ITEMA_VANISH) && sn == 19)
      {
        REMOVE_BIT(ch->itemaffect, ITEMA_VANISH);
        send_to_char("You emerge from the shadows.\n\r", ch);
        act("$n gradually fades out of the shadows.", ch, NULL, NULL, TO_ROOM);
      }
    }
  }
  return;
}

/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list(OBJ_INDEX_DATA * pObjIndex, OBJ_DATA * list)
{
  OBJ_DATA *obj;
  int nMatch;

  nMatch = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (obj->pIndexData == pObjIndex)
      nMatch++;
  }

  return nMatch;
}

/*
 * Move an obj out of a room.
 */
void obj_from_room(OBJ_DATA * obj)
{
  ROOM_INDEX_DATA *in_room;

  if (obj == NULL)
    return;
  if ((in_room = obj->in_room) == NULL)
  {
    bug("obj_from_room: NULL.", 0);
    return;
  }

  if (obj == in_room->contents)
  {
    in_room->contents = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = in_room->contents; prev; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL)
    {
      bug("Obj_from_room: obj not found.", 0);
      return;
    }
  }

  obj->in_room = NULL;
  obj->next_content = NULL;
  return;
}

/*
 * Move an obj into a room.
 */
void obj_to_room(OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex)
{
  if (obj == NULL)
    return;
  if (pRoomIndex == NULL)
    return;
  obj->next_content = pRoomIndex->contents;
  pRoomIndex->contents = obj;
  obj->in_room = pRoomIndex;
  obj->carried_by = NULL;
  obj->in_obj = NULL;
  return;
}

/*
 * Move an object into an object.
 */
void obj_to_obj(OBJ_DATA * obj, OBJ_DATA * obj_to)
{
  if (obj == NULL)
    return;
  obj->next_content = obj_to->contains;
  obj_to->contains = obj;
  obj->in_obj = obj_to;
  obj->in_room = NULL;
  obj->carried_by = NULL;

  for (; obj_to != NULL; obj_to = obj_to->in_obj)
  {
    if (obj_to->carried_by != NULL)
      obj_to->carried_by->carry_weight += get_obj_weight(obj);
  }

  return;
}

/*
 * Move an object out of an object.
 */
void obj_from_obj(OBJ_DATA * obj)
{
  OBJ_DATA *obj_from;

  if (obj == NULL)
    return;
  if ((obj_from = obj->in_obj) == NULL)
  {
    bug("Obj_from_obj: null obj_from.", 0);
    return;
  }

  if (obj == obj_from->contains)
  {
    obj_from->contains = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = obj_from->contains; prev; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL)
    {
      bug("Obj_from_obj: obj not found.", 0);
      return;
    }
  }

  obj->next_content = NULL;
  obj->in_obj = NULL;

  for (; obj_from != NULL; obj_from = obj_from->in_obj)
  {
    if (obj_from->carried_by != NULL)
      obj_from->carried_by->carry_weight -= get_obj_weight(obj);
  }

  return;
}

/*
 * Extract an obj from the world.
 */
void extract_obj(OBJ_DATA * obj)
{
  CHAR_DATA *ch;
  OBJ_DATA *obj_content;
  OBJ_DATA *obj_next;

  if (obj == NULL)
    return;

  if (obj->item_type == ITEM_CORPSE_PC && obj->contains)
  {
    OBJ_DATA *t_obj, *next_obj;

    for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
    {
      next_obj = t_obj->next_content;
      obj_from_obj(t_obj);

      if (obj->in_obj)          /* in another object */
        obj_to_obj(t_obj, obj->in_obj);

      else if (obj->carried_by) /* carried */
        obj_to_char(t_obj, obj->carried_by);

      else if (obj->in_room == NULL)  /* destroy it */
        extract_obj(t_obj);

      else                      /* to a room */
        obj_to_room(t_obj, obj->in_room);
    }
  }

  if (obj->in_room != NULL)
    obj_from_room(obj);
  else if (obj->carried_by != NULL)
    obj_from_char(obj);
  else if (obj->in_obj != NULL)
    obj_from_obj(obj);

  if ((ch = obj->chobj) != NULL && !IS_NPC(ch) && ch->pcdata->chobj == obj && IS_HEAD(ch, LOST_HEAD))
  {
    REMOVE_BIT(ch->loc_hp[0], LOST_HEAD);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    free_string(ch->morph);
    ch->morph = str_dup("");
    ch->hit = 1;
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
    ch->pcdata->chobj = NULL;
    obj->chobj = NULL;
    send_to_char("You have been KILLED!!\n\r", ch);
    do_look(ch, "auto");
    ch->position = POS_RESTING;
  }
  else if ((ch = obj->chobj) != NULL && !IS_NPC(ch) && ch->pcdata->chobj == obj && (IS_EXTRA(ch, EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0))
  {
    if (ch->pcdata->obj_vnum != 0)
    {
      send_to_char("You have been destroyed!\n\r", ch);
      ch->pcdata->chobj = NULL;
      obj->chobj = NULL;
    }
    else
    {
      REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string(ch->morph);
      ch->morph = str_dup("");
      char_from_room(ch);
      char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
      ch->pcdata->chobj = NULL;
      obj->chobj = NULL;
      send_to_char("You return to your body.\n\r", ch);
    }
  }
  else if (obj->chobj != NULL)
  {
    if (!IS_NPC(obj->chobj))
      obj->chobj->pcdata->chobj = NULL;
    obj->chobj = NULL;
    bug("Extract_obj: obj %d chobj invalid.", obj->pIndexData->vnum);
  }

  for (obj_content = obj->contains; obj_content; obj_content = obj_next)
  {
    obj_next = obj_content->next_content;
    if (obj->contains != NULL)
      extract_obj(obj->contains);
  }

  if (object_list == obj)
  {
    object_list = obj->next;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = object_list; prev != NULL; prev = prev->next)
    {
      if (prev->next == obj)
      {
        prev->next = obj->next;
        break;
      }
    }

    if (prev == NULL)
    {
      bug("Extract_obj: obj %d not found.", obj->pIndexData->vnum);
      return;
    }
  }

  {
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      paf->next = affect_free;
      affect_free = paf;
    }
  }

  {
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ed_next;

    for (ed = obj->extra_descr; ed != NULL; ed = ed_next)
    {
      ed_next = ed->next;

      free_string(ed->description);
      free_string(ed->keyword);

      ed->next = extra_descr_free;
      extra_descr_free = ed;
    }
  }

  free_string(obj->name);
  free_string(obj->description);
  free_string(obj->short_descr);
  if (obj->questmaker != NULL)
    free_string(obj->questmaker);
  if (obj->questowner != NULL)
    free_string(obj->questowner);
  --obj->pIndexData->count;
  obj->next = obj_free;
  obj_free = obj;
  return;
}

/*
 * Extract a char from the world.
 */
void extract_char(CHAR_DATA * ch, bool fPull)
{
  CHAR_DATA *wch;
  CHAR_DATA *familiar;
//  CHAR_DATA *wizard;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  if (ch == NULL)
    return;

  if (ch->in_room == NULL)
  {
    bug("Extract_char: NULL.", 0);
    return;
  }

  if (fPull)
    die_follower(ch);

  stop_fighting(ch, TRUE);

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj(obj);
  }

  char_from_room(ch);

  if (IS_NPC(ch))
    --ch->pIndexData->count;
  else if (ch->pcdata->chobj != NULL)
  {
    ch->pcdata->chobj->chobj = NULL;
    ch->pcdata->chobj = NULL;
  }

  if (!fPull)
  {
    char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
    return;
  }

  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (wch->reply == ch)
      wch->reply = NULL;
  }

  if (ch == char_list)
  {
    char_list = ch->next;
  }
  else
  {
    CHAR_DATA *prev;

    for (prev = char_list; prev != NULL; prev = prev->next)
    {
      if (prev->next == ch)
      {
        prev->next = ch->next;
        break;
      }
    }

    if (prev == NULL)
    {
      bug("Extract_char: char not found.", 0);
      return;
    }
  }

  if (ch->desc)
    ch->desc->character = NULL;
/*
  if ((wizard = ch->wizard) != NULL)
  {
    if (!IS_NPC(wizard))
      wizard->pcdata->familiar = NULL;
    ch->wizard = NULL;
  }
*/
  if (!IS_NPC(ch))
  {
    if ((familiar = ch->pcdata->familiar) != NULL)
    {
//      familiar->wizard = NULL;
      ch->pcdata->familiar = NULL;
      if (IS_NPC(familiar))
      {
        act("$n slowly fades away to nothing.", familiar, NULL, NULL, TO_ROOM);
        extract_char(familiar, TRUE);
      }
    }
    if ((familiar = ch->pcdata->partner) != NULL)
      ch->pcdata->partner = NULL;
    if ((familiar = ch->pcdata->propose) != NULL)
      ch->pcdata->propose = NULL;
    for (familiar = char_list; familiar != NULL; familiar = familiar->next)
    {
      if (!IS_NPC(familiar) && familiar->pcdata->propose != NULL && familiar->pcdata->propose == ch)
        familiar->pcdata->propose = NULL;
      if (!IS_NPC(familiar) && familiar->pcdata->partner != NULL && familiar->pcdata->partner == ch)
        familiar->pcdata->partner = NULL;
    }
  }
  free_char(ch);
  return;
}

/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *rch;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  if (!str_cmp(arg, "self") && (IS_NPC(ch) || ch->pcdata->chobj == NULL))
    return ch;
  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
  {
    if (!IS_NPC(rch) && IS_HEAD(rch, LOST_HEAD))
      continue;
    else if (!IS_NPC(rch) && IS_EXTRA(rch, EXTRA_OSWITCH))
      continue;
    else if (!can_see(ch, rch) || (!is_name(arg, rch->name) && (IS_NPC(rch) || !is_name(arg, rch->pcdata->switchname)) && (IS_NPC(rch) || !is_name(arg, rch->morph))))
      continue;
    if (++count == number)
      return rch;
  }

  return NULL;
}

/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *wch;
  int number;
  int count;

  if ((wch = get_char_room(ch, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (!IS_NPC(wch) && IS_HEAD(wch, LOST_HEAD))
      continue;
    else if (!IS_NPC(wch) && IS_EXTRA(wch, EXTRA_OSWITCH))
      continue;
    if (wch->in_room == NULL)
      continue;                 // wonder if this ever happens.
    else if (!can_see(ch, wch) || (!is_name(arg, wch->name) && (IS_NPC(wch) || !is_name(arg, wch->pcdata->switchname)) && (IS_NPC(wch) || !is_name(arg, wch->morph))))
      continue;
    if (++count == number)
      return wch;
  }

  return NULL;
}

/*
 * Find a char in the same area
 */
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *wch;
  int number, count;

  if (!ch->in_room) return NULL;

  if ((wch = get_char_room(ch, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count  = 0;

  for (wch = char_list; wch; wch = wch->next)
  {
    if (!IS_NPC(wch) && IS_HEAD(wch, LOST_HEAD)) continue;
    if (!IS_NPC(wch) && IS_EXTRA(wch, EXTRA_OSWITCH)) continue;
    if (wch->in_room == NULL) continue; // wonder if this ever happens.
    if (wch->in_room->area != ch->in_room->area) continue;
    if (!can_see( ch, wch ) || (!is_name(arg, wch->name) &&
       (IS_NPC(wch) || !is_name(arg, wch->pcdata->switchname)) &&
       (IS_NPC(wch) || !is_name(arg, wch->morph)))) continue;

    if (++count == number) return wch;
  }

  return NULL;
}


/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type(OBJ_INDEX_DATA * pObjIndex)
{
  OBJ_DATA *obj;

  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (obj->pIndexData == pObjIndex)
      return obj;
  }

  return NULL;
}

/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list(CHAR_DATA * ch, char *argument, OBJ_DATA * list)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj(ch, obj) && is_name(arg, obj->name))
    {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an object within the object you are in.
 */
OBJ_DATA *get_obj_in_obj(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj2;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  if (IS_NPC(ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj->in_obj == NULL)
    return NULL;
  obj = ch->pcdata->chobj;
  if (obj->in_obj->item_type != ITEM_CONTAINER && obj->in_obj->item_type != ITEM_CORPSE_NPC && obj->in_obj->item_type != ITEM_CORPSE_PC)
    return NULL;

  for (obj2 = obj->in_obj->contains; obj2 != NULL; obj2 = obj2->next_content)
  {
    if (obj != obj2 && is_name(arg, obj2->name))
    {
      if (++count == number)
        return obj2;
    }
  }

  return NULL;
}

/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && is_name(arg, obj->name))
    {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj) && is_name(arg, obj->name))
    {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;

  obj = get_obj_list(ch, argument, ch->in_room->contents);
  if (obj != NULL)
    return obj;

  if ((obj = get_obj_carry(ch, argument)) != NULL)
    return obj;

  if ((obj = get_obj_wear(ch, argument)) != NULL)
    return obj;

  if ((obj = get_obj_in_obj(ch, argument)) != NULL)
    return obj;

  return NULL;
}

/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_room(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;

  obj = get_obj_list(ch, argument, ch->in_room->contents);
  if (obj != NULL)
    return obj;

  return NULL;
}

/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  if ((obj = get_obj_here(ch, argument)) != NULL)
    return obj;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (can_see_obj(ch, obj) && is_name(arg, obj->name))
    {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight(OBJ_DATA * obj)
{
  int weight;

  weight = obj->weight;
  for (obj = obj->contains; obj != NULL; obj = obj->next_content)
    weight += get_obj_weight(obj);

  return weight;
}

/*
 * True if room is dark.
 */
bool room_is_dark(ROOM_INDEX_DATA * pRoomIndex)
{
  if (!pRoomIndex)
  {
    bug("Room_is_dark: No room", 0);
    return TRUE;
  }

  if (pRoomIndex->light > 0)
    return FALSE;

  if (IS_SET(pRoomIndex->room_flags, ROOM_LIGHT))
    return FALSE;

  if (IS_SET(pRoomIndex->room_flags, ROOM_DARK))
    return TRUE;

  if (pRoomIndex->sector_type == SECT_INSIDE || pRoomIndex->sector_type == SECT_CITY)
    return FALSE;

  if (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)
    return TRUE;

  return FALSE;
}

/*
 * True if room is private.
 */
bool room_is_private(ROOM_INDEX_DATA * pRoomIndex)
{
  CHAR_DATA *rch;
  int count;

  count = 0;
  for (rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room)
    if (!IS_NPC(rch))
      count++;

  if (IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE) && count >= 2)
    return TRUE;

  return FALSE;
}

/*
 * True if char can see victim.
 */
bool can_see(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch == victim)
    return TRUE;

  if (get_trust(ch) > 6)
    return TRUE;

  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) && victim->level > 6)
    return FALSE;
  if (IS_ITEMAFF(ch, ITEMA_VISION))
    return TRUE;
  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS))
    return FALSE;
  if (!IS_NPC(victim) && IS_SET(victim->act, AFF_HIDE))
    return FALSE;
  if (!IS_NPC(victim) && IS_SET(victim->newbits, NEW_DARKNESS))
    return FALSE;

  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
    {
      if (!IS_IMMORTAL(ch))
        return FALSE;
      else
        return TRUE;
    }
  }

  if (IS_EXTRA(ch, BLINDFOLDED))
    return FALSE;

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    return TRUE;

  if (IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R))
    return FALSE;

  if (IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT))
    return FALSE;

  if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
    return FALSE;

  if (IS_AFFECTED(victim, AFF_INVISIBLE) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
    return FALSE;

  if (IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
    return FALSE;

  if (!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD))
    return TRUE;

  if (!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH))
    return TRUE;

  if (!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    return TRUE;

  if (!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    return TRUE;

  return TRUE;
}

/*
 * True if char can see obj.
 */
bool can_see_obj(CHAR_DATA * ch, OBJ_DATA * obj)
{
  CHAR_DATA *gch;

  if ((gch = obj->carried_by) != NULL)
  {
    if (gch->desc != NULL)
    {
      if (gch->desc->connected != CON_PLAYING)
        return FALSE;
    }
  }

  /* for quests */
  if (IS_OBJ_STAT(obj, ITEM_NOLOCATE))
  {
    if (IS_NPC(ch))
      return FALSE;
    if (ch->pcdata->playerid != obj->ownerid)
      return FALSE;
  }

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    return TRUE;

  if (IS_ITEMAFF(ch, ITEMA_VISION))
    return TRUE;

  if (IS_OBJ_STAT(obj, ITEM_GLOW))
    return TRUE;

  if ((IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) && obj->carried_by == NULL) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT) && !IS_AFFECTED(ch, AFF_SHADOWPLANE))
    return FALSE;

  if ((!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) && obj->carried_by == NULL) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT) && IS_AFFECTED(ch, AFF_SHADOWPLANE))
    return FALSE;

  if (obj->item_type == ITEM_POTION)
    return TRUE;

  if (IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R))
    return FALSE;

  if (IS_EXTRA(ch, BLINDFOLDED))
    return FALSE;

  if (IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT))
    return FALSE;

  if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
    return TRUE;

  if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
    return FALSE;

  if (IS_SET(obj->extra_flags, ITEM_INVIS) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
    return FALSE;

  if (!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    return TRUE;

  if (!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    return TRUE;

  return TRUE;
}

/*
 * True if char can drop obj.
 */
bool can_drop_obj(CHAR_DATA * ch, OBJ_DATA * obj)
{
  if (!IS_SET(obj->extra_flags, ITEM_NODROP))
    return TRUE;

  if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL)
    return TRUE;

  return FALSE;
}

/*
 * Return ascii name of an item type.
 */
char *item_type_name(OBJ_DATA * obj)
{
  switch (obj->item_type)
  {
    case ITEM_LIGHT:
      return "light";
    case ITEM_SCROLL:
      return "scroll";
    case ITEM_WAND:
      return "wand";
    case ITEM_STAFF:
      return "staff";
    case ITEM_WEAPON:
      return "weapon";
    case ITEM_TREASURE:
      return "treasure";
    case ITEM_GRENADE:
      return "grenade";
    case ITEM_ARMOR:
      return "armor";
    case ITEM_POTION:
      return "potion";
    case ITEM_FURNITURE:
      return "furniture";
    case ITEM_TRASH:
      return "trash";
    case ITEM_CONTAINER:
      return "container";
    case ITEM_DRINK_CON:
      return "drink container";
    case ITEM_KEY:
      return "key";
    case ITEM_FOOD:
      return "food";
    case ITEM_MONEY:
      return "money";
    case ITEM_BOAT:
      return "boat";
    case ITEM_CORPSE_NPC:
      return "npc corpse";
    case ITEM_CORPSE_PC:
      return "pc corpse";
    case ITEM_FOUNTAIN:
      return "fountain";
    case ITEM_PILL:
      return "pill";
    case ITEM_PORTAL:
      return "portal";
    case ITEM_HOLYSYMBOL:
      return "holy symbol";
    case ITEM_STAKE:
      return "stake";
    case ITEM_QUEST:
      return "quest token";
    case ITEM_HEAD:
      return "head";
    case ITEM_COOKINGPOT:
      return "cooking pot";
    case ITEM_FAITHTOKEN:
      return "faith token";
  }

  bug("Item_type_name: unknown type %d.", obj->item_type);
  return "(unknown)";
}

/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name(int location)
{
  switch (location)
  {
    case APPLY_NONE:
      return "none";
    case APPLY_STR:
      return "strength";
    case APPLY_DEX:
      return "dexterity";
    case APPLY_INT:
      return "intelligence";
    case APPLY_WIS:
      return "wisdom";
    case APPLY_CON:
      return "constitution";
    case APPLY_SEX:
      return "sex";
    case APPLY_CLASS:
      return "class";
    case APPLY_LEVEL:
      return "level";
    case APPLY_AGE:
      return "age";
    case APPLY_MANA:
      return "mana";
    case APPLY_HIT:
      return "hp";
    case APPLY_MOVE:
      return "moves";
    case APPLY_GOLD:
      return "gold";
    case APPLY_EXP:
      return "experience";
    case APPLY_AC:
      return "armor class";
    case APPLY_HITROLL:
      return "hit roll";
    case APPLY_DAMROLL:
      return "damage roll";
    case APPLY_SAVING_PARA:
      return "save vs paralysis";
    case APPLY_SAVING_ROD:
      return "save vs rod";
    case APPLY_SAVING_PETRI:
      return "save vs petrification";
    case APPLY_SAVING_BREATH:
      return "save vs breath";
    case APPLY_SAVING_SPELL:
      return "save vs spell";
    case APPLY_POLY:
      return "polymorph form";
  }

  bug("Affect_location_name: unknown location %d.", location);
  return "(unknown)";
}

/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name(int vector)
{
  static char buf[512];

  buf[0] = '\0';
  if (vector & AFF_BLIND)
    strcat(buf, " blind");
  if (vector & AFF_INVISIBLE)
    strcat(buf, " invisible");
  if (vector & AFF_DETECT_EVIL)
    strcat(buf, " detect_evil");
  if (vector & AFF_DETECT_INVIS)
    strcat(buf, " detect_invis");
  if (vector & AFF_DETECT_MAGIC)
    strcat(buf, " detect_magic");
  if (vector & AFF_DETECT_HIDDEN)
    strcat(buf, " detect_hidden");
  if (vector & AFF_SHADOWPLANE)
    strcat(buf, " shadowplane");
  if (vector & AFF_SANCTUARY)
    strcat(buf, " sanctuary");
  if (vector & AFF_FAERIE_FIRE)
    strcat(buf, " faerie_fire");
  if (vector & AFF_INFRARED)
    strcat(buf, " infrared");
  if (vector & AFF_CURSE)
    strcat(buf, " curse");
  if (vector & AFF_FLAMING)
    strcat(buf, " flaming");
  if (vector & AFF_POISON)
    strcat(buf, " poison");
  if (vector & AFF_PROTECT)
    strcat(buf, " protect");
  if (vector & AFF_ETHEREAL)
    strcat(buf, " ethereal");
  if (vector & AFF_SNEAK)
    strcat(buf, " sneak");
  if (vector & AFF_HIDE)
    strcat(buf, " hide");
  if (vector & AFF_CHARM)
    strcat(buf, " charm");
  if (vector & AFF_FLYING)
    strcat(buf, " flying");
  if (vector & AFF_PASS_DOOR)
    strcat(buf, " pass_door");
  if (vector & AFF_POLYMORPH)
    strcat(buf, " polymorph");
  if (vector & AFF_SHADOWSIGHT)
    strcat(buf, " shadowsight");
  return (buf[0] != '\0') ? buf + 1 : "none";
}

/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name(int extra_flags)
{
  static char buf[512];

  buf[0] = '\0';
  if (extra_flags & ITEM_GLOW)
    strcat(buf, " glow");
  if (extra_flags & ITEM_HUM)
    strcat(buf, " hum");
  if (extra_flags & ITEM_INVIS)
    strcat(buf, " invis");
  if (extra_flags & ITEM_MAGIC)
    strcat(buf, " magic");
  if (extra_flags & ITEM_NODROP)
    strcat(buf, " nodrop");
  if (extra_flags & ITEM_ANTI_GOOD)
    strcat(buf, " anti-good");
  if (extra_flags & ITEM_ANTI_EVIL)
    strcat(buf, " anti-evil");
  if (extra_flags & ITEM_ANTI_NEUTRAL)
    strcat(buf, " anti-neutral");
  if (extra_flags & ITEM_BLESS)
    strcat(buf, " bless");
  if (extra_flags & ITEM_NOREMOVE)
    strcat(buf, " noremove");
  if (extra_flags & ITEM_INVENTORY)
    strcat(buf, " inventory");
  if (extra_flags & ITEM_LOYAL)
    strcat(buf, " loyal");
  if (extra_flags & ITEM_SHADOWPLANE)
    strcat(buf, " shadowplane");
  if (extra_flags & ITEM_THROWN)
    strcat(buf, " thrown");
  if (extra_flags & ITEM_KEEP)
    strcat(buf, " keep");
  if (extra_flags & ITEM_VANISH)
    strcat(buf, " vanish");
  if (extra_flags & ITEM_NOCLAIM)
    strcat(buf, " noclaim");
  if (extra_flags & ITEM_NOLOCATE)
    strcat(buf, " nolocate");
  return (buf[0] != '\0') ? buf + 1 : "none";
}


OBJ_DATA *get_obj_world2(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  int vnum = 0;

  if (argument[0] == '\0')
    return NULL;

  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (!str_cmp(argument, obj->short_descr))
    {
      if ((vnum = obj->pIndexData->vnum) == 30037 || vnum == 30041)
        continue;
      return obj;
    }
  }

  return NULL;
}