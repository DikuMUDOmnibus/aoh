#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



void do_evaporate( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;
	
	if (!IS_SPHERE(ch, SPHERE_AEROMANCER) )
	{
		send_to_char("This is an Aeromancer ability.\n\r", ch);
		return;
	}
	
	
	if (ch->move < 1000)
	{
	  send_to_char("You don't have 1000 move to activate your power.\n\r", ch);
	  return;
	} 

        if (ch->fight_timer >0) 
        {
          send_to_char("Not until your fight timer expires.\n\r", ch );
          return;
        }

       if (!IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], WATER_FORM))
       {
          stc("You can only evaporate in Water Form!!\n\r", ch );
          return;
       }

       if ( IS_SET(ch->act, AFF_HIDE) )
       {
   	 REMOVE_BIT(ch->act, AFF_HIDE);
	 act( "$n reforms from a cloud of mist.", ch, NULL, NULL, TO_ROOM );
	 send_to_char( "You reform from a cloud of mist.\n\r", ch );
       }
       else
       {
	 act( "$n evaporates into mist.", ch, NULL, NULL, TO_ROOM );
	 send_to_char( "You evaporate into mist.\n\r", ch );
	 ch->move -= 1000;
         SET_BIT(ch->act, AFF_HIDE);
       }
       return;
}





void do_enfold( CHAR_DATA *ch, char *argument )
  {
  if (IS_NPC(ch)) return;
  if (!IS_SPHERE(ch, SPHERE_AEROMANCER))
  {
    send_to_char("Only Aeromancers have the enfold ability.\n\r",ch);
    return;
  }  
    if (IS_SET(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFOLD))
    {
      REMOVE_BIT(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFOLD);
      send_to_char("Your Air Particles return to your body allowing people to escape.\n\r",ch);
      return;
    }
    else
    {
      send_to_char("You release air particles to keep your enemys from fleeing.\n\r",ch);
      SET_BIT(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFOLD);
      return;
    }
  }


void do_disperse ( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  argument = one_argument(argument, arg);
 
  if (!IS_SPHERE(ch, SPHERE_AEROMANCER))
  {
    send_to_char("Only Aeromancers may disperce.\n\r",ch);
    return;
  }
  if (ch->move < 250)
  {
    stc("You do not have enough vitality to do that!\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char( "Travel to Who?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }   
  if( victim == ch )
  {
    stc ( "Not to yourself.\n\r",ch);
    return;
  }
  if( IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
  {
    stc( "You can't find it's room.\n\r",ch); 
    return;
  }
  if( IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
  {
    stc( "Your room is not connected to the astral plane.\n\r",ch);
    return;
  }
  if  (!IS_NPC(victim) && !IS_IMMUNE(victim,IMM_SUMMON) )
  {
    send_to_char("They don't want you near them!\n\r",ch);
    return;
  }
  if ( room_is_private(victim->in_room ) )
  {
    send_to_char( "That room is private right now.\n\r", ch );
    return;
  }
  if (victim->in_room->vnum == ch->in_room->vnum)
  {
    send_to_char("But you're already there!\n\r",ch);
    return;
  }
  char_from_room(ch);
  char_to_room(ch,victim->in_room);
  if (IS_NPC(victim))
  {
    sprintf(buf,"You Disperce into particles to travel to %s!\n\r",victim->short_descr);
    send_to_char(buf, ch);
  }
  if (!IS_NPC(victim))
  {
    sprintf(buf,"Disperces into particles to travel to %s!\n\r",victim->name);
    send_to_char(buf, ch);
  }
  act("You see $n slowly solidify infront of you.\n\r",ch,NULL,NULL,TO_ROOM);
  do_look(ch, "auto");
  ch->move -= 250;
  return;
}


