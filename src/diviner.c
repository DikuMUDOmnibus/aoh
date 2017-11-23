
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_gleam(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_DIVINER))
        {
            stc("#0Gleam is in the Divination Sphere.#n\n\r",ch);
            return;
        }

        if (IS_SET(ch->act, PLR_HOLYLIGHT))
        {
                REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
                send_to_char("#0Your eyes stop gleaming.#n\n\r", ch);
        }
        else
        {
                SET_BIT(ch->act, PLR_HOLYLIGHT);
                send_to_char("#0Your eyes begin to gleam in the night.#n\n\r",
                             ch);
        }
        return;
}




void do_reveal(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *ich;

  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_DIVINER))
        {
            stc("#0Reveal is in the Divination Sphere.#n\n\r",ch);
            return;
        }

  if (ch->mana < 5000)
  {
    send_to_char("You don't have the mystical energies to do this.\n\r",ch);
    return;
  } 
  act( "$n mumles a few words, and you are suddenly blinded by a flash.", ch, NULL, NULL, TO_ROOM );   
  send_to_char( "You reveal everything hidden in the room.\n\r", ch );
  for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
  {
    if (ich==ch || ich->trust > 6) continue;
    affect_strip ( ich, gsn_invis );
    affect_strip ( ich, gsn_mass_invis );
    affect_strip ( ich, gsn_sneak );
    if (IS_SET(ich->affected_by, AFF_HIDE))      REMOVE_BIT(ich->affected_by, AFF_HIDE);
    if (IS_SET(ich->affected_by, AFF_INVISIBLE)) REMOVE_BIT(ich->affected_by, AFF_INVISIBLE);
    if (IS_SET(ich->affected_by, AFF_SNEAK))     REMOVE_BIT(ich->affected_by, AFF_SNEAK);
    if (IS_SET(ich->act, PLR_WIZINVIS))          REMOVE_BIT(ich->act, PLR_WIZINVIS);
    if (IS_SET(ich->act, AFF_HIDE))              REMOVE_BIT(ich->act, AFF_HIDE);
//    if (IS_SET(ich->affected_by, AFF_SHIFT))     REMOVE_BIT(ich->affected_by, AFF_SHIFT);
//    if (IS_SET(ich->extra, EXTRA_EARTHMELD))     REMOVE_BIT(ich->extra, EXTRA_EARTHMELD);
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE)) REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    if (IS_AFFECTED(ch, AFF_PEACE)) REMOVE_BIT(ch->affected_by, AFF_PEACE);
    if (IS_AFFECTED(ch,AFF_ETHEREAL)) REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);

/*    if (IS_CLASS(ich, CLASS_DROW) && IS_SET(ich->newbits, NEW_DARKNESS))
    {
      REMOVE_BIT(ich->newbits, NEW_DARKNESS);
      REMOVE_BIT(ich->in_room->room_flags, ROOM_TOTAL_DARKNESS);
    }
*/
    send_to_char("You are suddenly very visible.\n\r",ich);
  }
  ch->mana -= 5000;
  return;
}




void do_scry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *victimroom;
    char      arg [MAX_INPUT_LENGTH];
    int rand_room;
    bool chaosvalue = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_ITEMAFF(ch, ITEMA_VISION) && !ragnarok)
    {
	if (!IS_SPHERE(ch, SPHERE_DIVINER))
	{
	    stc("#0Scry is in the Divination Sphere.#n\n\r",ch);
	    return;
	}
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

    if (victim->trust > 6)
    {
      send_to_char("SPYING ON THE GODS!?!?!?!\n\r",ch);
      send_to_char("Someone is scrying you.\n\r",victim);
      return;
    }

    if (!IS_NPC(victim) && IS_IMMUNE(victim,IMM_SHIELDED) && !ragnarok)
    {
       send_to_char( "You cannot find them.\n\r",ch);
       return;
    }


    chroom = ch->in_room;
    victimroom = victim->in_room;
    if (chaosvalue && !ragnarok)
      for ( ; ; )
      {
        rand_room = number_range( 0, 65535 );
        victimroom = get_room_index( rand_room );
        if (victimroom != NULL) break;   
      }

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
    if (!IS_NPC(victim)) ch->fight_timer += 5;
    
    return;
}

void do_readaura( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    bool chaosvalue = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_DIVINER))
        {
            stc("#0ReadAura is in the Divination Sphere.#n\n\r",ch);
            return;
        }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Read the aura on what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "Read the aura on what?\n\r", ch );
	    return;
	}

	act("$n examines $p intently.",ch,obj,NULL,TO_ROOM);
	spell_identify( skill_lookup( "identify" ), ch->level, ch, obj );
	return;
    }


    act("$n examines $N intently.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you intently.",ch,NULL,victim,TO_VICT);
    if (IS_NPC(victim)) sprintf(buf, "%s is an NPC.\n\r",victim->short_descr);
    else 
    {
	if      (victim->level == 12) sprintf(buf, "%s is an Implementor.\n\r", victim->name);
	else if (victim->level == 11) sprintf(buf, "%s is a High Judge.\n\r", victim->name);
	else if (victim->level == 10) sprintf(buf, "%s is a Judge.\n\r", victim->name);
	else if (victim->level == 9 ) sprintf(buf, "%s is an Enforcer.\n\r", victim->name);
	else if (victim->level == 8 ) sprintf(buf, "%s is a Quest Maker.\n\r", victim->name);
	else if (victim->level == 7 ) sprintf(buf, "%s is a Builder.\n\r", victim->name);
	else if (victim->level >= 3 ) sprintf(buf, "%s is an Avatar.\n\r", victim->name);
	else sprintf(buf, "%s is a Mortal.\n\r", victim->name);
    }
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Str:%d, Int:%d, Wis:%d, Dex:%d, Con:%d.\n\r",get_curr_str(victim),get_curr_int(victim),get_curr_wis(victim),get_curr_dex(victim),get_curr_con(victim));
	send_to_char(buf,ch);
    }
    if (chaosvalue) sprintf(buf,"Hp:%d/%d, Mana:%d/%d, Move:%d/%d.\n\r", number_range(1000,5000), 
number_range(10000,20000) , number_range(1000,5000),
number_range(10000,20000), number_range(1000,5000), number_range(10000,20000));
    else
      sprintf(buf,"Hp:%d/%d, Mana:%d/%d, Move:%d/%d.\n\r",victim->hit,victim->max_hit,victim->mana,victim->max_mana,victim->move,victim->max_move);
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
      if (chaosvalue) sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d.\n\r", number_range(500,2500), number_range(500,2500), number_range(-2000,0));
      else
        sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d.\n\r",char_hitroll(victim),char_damroll(victim),char_ac(victim));
    }
    else sprintf(buf,"AC:%d.\n\r",char_ac(victim));
    send_to_char(buf,ch);
    sprintf(buf,"Alignment:%d.\n\r",victim->alignment);
    send_to_char(buf,ch);
    if (!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_PREGNANT))
	act("$N is pregnant.",ch,NULL,victim,TO_CHAR);

    return;



}


