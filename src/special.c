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

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN( spec_priest		);
DECLARE_SPEC_FUN( spec_ninja		);

// Guilds
DECLARE_SPEC_FUN( spec_guard_guardian );
DECLARE_SPEC_FUN( spec_guard_dark_hand );
DECLARE_SPEC_FUN( spec_guard_beast );
DECLARE_SPEC_FUN( spec_guard_damned );
DECLARE_SPEC_FUN( spec_guard_crusaders );
DECLARE_SPEC_FUN( spec_guard_druid );

DECLARE_SPEC_FUN( spec_soldier_guardian );
DECLARE_SPEC_FUN( spec_soldier_dark_hand );
DECLARE_SPEC_FUN( spec_soldier_beast );
DECLARE_SPEC_FUN( spec_soldier_damned );
DECLARE_SPEC_FUN( spec_soldier_crusaders );
DECLARE_SPEC_FUN( spec_soldier_druid );

bool spec_soldier_dark_hand(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 1
     || victim->spec_fun == spec_guard_dark_hand
     || victim->spec_fun == spec_soldier_dark_hand)


	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}
bool spec_soldier_guardian(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 2
     || victim->spec_fun == spec_guard_guardian
     || victim->spec_fun == spec_soldier_guardian)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}
bool spec_soldier_beast(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 3
     || victim->spec_fun == spec_guard_beast
     || victim->spec_fun == spec_soldier_beast)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}
bool spec_soldier_damned(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 4
     || victim->spec_fun == spec_guard_damned
     || victim->spec_fun == spec_soldier_damned)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}
bool spec_soldier_crusaders(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 5
     || victim->spec_fun == spec_guard_crusaders
     || victim->spec_fun == spec_soldier_crusaders)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}
bool spec_soldier_druid(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim) commented out this so they can attack mobs
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 6
     || victim->spec_fun == spec_guard_druid
     || victim->spec_fun == spec_soldier_druid)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}

bool spec_guard_beast(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 3
     || victim->spec_fun == spec_guard_beast
     || victim->spec_fun == spec_soldier_beast)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}

bool spec_guard_damned(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 4
     || victim->spec_fun == spec_guard_damned
     || victim->spec_fun == spec_soldier_damned)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}


bool spec_guard_crusaders(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 5
     || victim->spec_fun == spec_guard_crusaders
     || victim->spec_fun == spec_soldier_crusaders)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}


bool spec_guard_druid(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 6
     || victim->spec_fun == spec_guard_druid
     || victim->spec_fun == spec_soldier_druid)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}





bool spec_guard_dark_hand(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 1
     || victim->spec_fun == spec_guard_dark_hand
     || victim->spec_fun == spec_soldier_dark_hand)

	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}



bool spec_guard_guardian( CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA       *d;
   CHAR_DATA             *victim;
   char                  buf[MAX_STRING_LENGTH];


   if (ch->fighting) return FALSE;

   if (ch->in_room == NULL) return FALSE;
   if (ch->position != POS_STANDING) return FALSE;


   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (!IS_PLAYING(d)
	   || (victim = d->character) == NULL
//           || IS_NPC(victim)
           || IS_IMMORTAL(victim)
	   || victim->in_room == NULL
           || victim->pcdata->chobj != NULL
           || victim->in_room->area != ch->in_room->area
	   || victim->guild == 2
     || victim->spec_fun == spec_guard_guardian
     || victim->spec_fun == spec_soldier_guardian)
	{
            continue;
        }

        if (ch->in_room!=victim->in_room)
        {
           act("$n disappears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You disappear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);

           char_from_room( ch );
           char_to_room(ch, victim->in_room);

           act("$n appears in a swirl of smoke!",ch,NULL,NULL,TO_ROOM);
           act("You appear in a swirl of smoke!",ch,NULL,NULL,TO_CHAR);
        }

        spell_curse( 24 , 100 , ch , victim ); /* No Recall */

        if (victim->position <= POS_MORTAL && IS_HERO(victim) &&
	  ch->position == POS_STANDING)
        {
           char_from_room( victim );
           char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
           continue;
        }
        if (is_safe(ch,victim)) return FALSE;

/*       sprintf( buf,"%s Is Invading The RULER CLAN HALL!\n\r",victim->name); 
        do_info( ch , buf);
        sprintf( buf,"%s RULERS! CLAN RECALL AND GET EM!\n\r",victim->name); 
        do_info( ch , buf);
*/      
      
        multi_hit( ch, victim, gsn_punch);

        return TRUE;

   }
   return FALSE;
}



/***************************************************
 * This is a prototype of a special program, feel  *
 * free to copy and edit this to suit your needs   *
 ***************************************************

bool spec_prototype(CHAR_DATA *ch, char *argument)
{
  if (!str_cmp(argument, "update"))
  {
    if (ch->fighting) return FALSE;

    // This part should contain movement, spell-casting, etc,
    // this part is called once every 4 seconds, and is not
    // related to combat...
  }
  else if (!str_cmp(argument, "midround"))
  {
    if (!ch->fighting) return FALSE;

    // This part is the combat oriented part of the code,
    // and is called once every combat round. It should
    // contain special moves/powers the mob uses in combat.
  }

  return FALSE;
}

****************************************************/

bool spec_ninja(CHAR_DATA *ch, char *argument)
{
  if (!str_cmp(argument, "update"))
  {
    if (ch->fighting) return FALSE;

    /* if not hidden, hide */
    if (ch->position == POS_STANDING && !IS_AFFECTED(ch, AFF_HIDE))
      do_hide(ch, "");
    else return FALSE;
  }
  else if (!str_cmp(argument, "midround"))
  {
    CHAR_DATA *victim;

    if ((victim = ch->fighting) == NULL)
      return FALSE;

    if (number_range(1,3) == 2)
      one_hit(ch, victim, gsn_backstab, 0);
  }

  return FALSE;
}

bool spec_priest(CHAR_DATA *ch, char *argument)
{
  if (!str_cmp(argument, "update"))
  {
    CHAR_DATA *vch;
    CHAR_DATA *gch;
    bool found = FALSE;
    int sn;

    for (vch = ch->in_room->people; vch && !found; vch = vch->next_in_room)
    {
      if (vch == ch) continue;
      if (number_range(1,2) == 1) continue;
      found = TRUE;
      gch = vch;
    }

    if (!found) return FALSE;

    switch(number_range(1,3))
    {
      case 1:
        sn = skill_lookup("heal");
        break;
      case 2:
        sn = skill_lookup("bless");
        break;
      case 3:
        sn = skill_lookup("darkblessing");
        break;
    }
    if (sn < 1) return FALSE;

    act("$n mumbles a few arcane words.", ch, NULL, NULL, TO_ROOM);
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, gch);

    return TRUE;
  }
  else if (!str_cmp(argument, "midround"))
  {
    /* the priest does not fight */
    return FALSE;
  }

  return FALSE;
}



/*
 * Special Functions Table.     OLC
 */
const   struct  spec_type       spec_table      [ ] =
{
    /*
     * Special function commands.
     */
    { "spec_priest",                spec_priest         },
    { "spec_ninja",                 spec_ninja          },
  { "spec_guard_guardian", spec_guard_guardian },
  { "spec_guard_dark_hand", spec_guard_dark_hand },
  { "spec_guard_beast", spec_guard_beast },
  { "spec_guard_damned", spec_guard_damned },
  { "spec_guard_crusaders", spec_guard_crusaders },
  { "spec_guard_druid", spec_guard_druid },

  { "spec_soldier_guardian", spec_soldier_guardian },
  { "spec_soldier_dark_hand", spec_soldier_dark_hand },
  { "spec_soldier_beast", spec_soldier_beast },
  { "spec_soldier_damned", spec_soldier_damned },
  { "spec_soldier_crusaders", spec_soldier_crusaders },
  { "spec_soldier_druid", spec_soldier_druid },
    /*
     * End of list.
     */
    { "",                       0       }
};


/*****************************************************************************
 Name:          spec_lookup
 Purpose:       Given a name, return the appropriate spec fun.
 Called by:     do_mset(act_wiz.c) load_specials,reset_area(db.c)
 ****************************************************************************/
SPEC_FUN *spec_lookup( const char *name )       /* OLC */
{   
    int cmd;

    for ( cmd = 0; *spec_table[cmd].spec_name; cmd++ )  /* OLC 1.1b */
        if ( !str_cmp( name, spec_table[cmd].spec_name ) )
            return spec_table[cmd].spec_fun;
 
    return 0;
}

/*****************************************************************************
 Name:          spec_string
 Purpose:       Given a function, return the appropriate name.
 Called by:     <???>
 ****************************************************************************/
char *spec_string( SPEC_FUN *fun )      /* OLC */
{
    int cmd;

    for ( cmd = 0; *spec_table[cmd].spec_fun; cmd++ )   /* OLC 1.1b */
        if ( fun == spec_table[cmd].spec_fun )
            return spec_table[cmd].spec_name;
    
    return 0;
}
 
