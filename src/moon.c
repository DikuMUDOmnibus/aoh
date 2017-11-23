

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"




void do_moonbeam( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
 
    argument = one_argument( argument, arg1 );
  
    if ( IS_NPC(ch) )
    	return;
 
        if (!IS_SPHERE(ch, SPHERE_MOON))
        {
                send_to_char("#0This is a moon Sphere Ability#n\n\r", ch);
                return;
        }

    if (ch->mana < 500)
    {
	send_to_char("You do not have enough energy to summon a Moonbeam.\n\r",ch);
	return;
    }
 
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Who do you wish to strike?\n\r", ch );
      return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
   	 send_to_char( "They aren't here.\n\r", ch );
	 return;
    }
    if (ch == victim) return;
    if (is_safe(ch,victim) == TRUE) return;
    if (ch->fighting == NULL)
        set_fighting(ch,victim);
        update_pos(victim);
  
    	act("$N is struck by a huge moonbeam.", ch, NULL, victim,TO_CHAR);
	act("$N is struck by a deadly beam of moonlight from $n.", ch, NULL, victim,TO_NOTVICT);
	act("You are struck by a deadly beam of moonlight!", ch, NULL, victim,TO_VICT);
    if (IS_GOOD(victim))
	hurt_person(ch,victim, 500);
    if (IS_EVIL(victim))
        hurt_person(ch,victim, 1000);  
    if (IS_NEUTRAL(victim))
        hurt_person(ch,victim, 750);  
	ch->mana -= 500;
    WAIT_STATE(ch,12);
	return;
    
}
void do_moongate(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];
 
    one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;



        if (!IS_SPHERE(ch, SPHERE_MOON))
        {
                send_to_char("#0This is a moon Sphere Ability#n\n\r", ch);
                return;
        }
    if ( arg[0] == '\0')
    {
        send_to_char( "Who do you wish to gate to?\n\r", ch );
	return;
    }
 
    if ( (victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char("They arent here.\n\r",ch);
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
 
    if ( (victim == ch) 
    ||   victim->in_room == NULL
    ||   (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) )
    ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
//    ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
//    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->in_room->vnum == ch->in_room->vnum)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
 
 
//    obj = create_object( get_obj_index( OBJ_VNUM_GATE2 ), 0 );
    obj->value[0] = victim->in_room->vnum;
    obj->value[3] = ch->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, ch->in_room );
 
//    obj = create_object( get_obj_index( OBJ_VNUM_GATE2 ), 0 );
    obj->value[0] = ch->in_room->vnum;
    obj->value[3] = victim->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, victim->in_room );
    act( "A look of concentration passes over $n's face.", ch, NULL,NULL, TO_ROOM );
    send_to_char("A look of concentration passes over your face.\n\r",ch);
    act( "$p appears in front of $n in a beam of moonlight.", ch, obj, NULL, TO_ROOM );
    act( "$p appears in front of you in a beam of moonlight.", ch, obj, NULL, TO_CHAR );
    act( "$p appears in front of $n in a beam of moonlight.", victim, obj, NULL, TO_ROOM );
    act( "$p appears in front of you in a beam of moonlight.", ch, obj, victim, TO_VICT );
    return;
}

