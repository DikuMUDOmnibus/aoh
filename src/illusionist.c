#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"




void do_grab(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;   
    char buf[MAX_STRING_LENGTH];
   one_argument( argument,arg);
        
    if (IS_NPC(ch)) return;

     if (!IS_SPHERE(ch, SPHERE_ILLUSIONIST))
        {
            send_to_char("#0This is an Illusionist Sphere Power.#n\n\r",ch);
            return;
        }
    
   if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
   {
        send_to_char("You need to be in the Shadowplane, first!\n\r",ch);
        return;
   }
  if (ch->in_room != NULL)
  {
    if (IS_SET (ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your in the arena.\n\r",ch);
      return;
    }
  }  
    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to Grab?\n\r", ch );
        return;
    }
    if ((victim = get_char_room( ch, arg)) == NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
    
     if (IS_AFFECTED(victim, AFF_SHADOWPLANE))
   {
        send_to_char("They are already in the Shadowplane!\n\r",ch);
        return;
   } 
        
    if (IS_NPC(victim))
    {
    send_to_char("You cannot Grab a mobile!\n\r",ch);
    return;
    }
    sprintf(buf,
    "%s pulls %s into the Shadowplane.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "You pull %s into the Shadowplane!\n\r",victim->name);
    send_to_char(buf,ch);
    sprintf(buf,
    "%s pulls you into the Shadowplane!\n\r",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    WAIT_STATE(ch, 8);
    SET_BIT(victim->affected_by, AFF_SHADOWPLANE);
    return;
}




void do_shadowsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

     if (!IS_SPHERE(ch, SPHERE_ILLUSIONIST))
	{
	    send_to_char("#0This is an Illusionist Sphere Power.#n\n\r",ch);
	    return;
	}
    if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
    {
	send_to_char("You can no longer see between planes.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_SHADOWSIGHT);
	return;
    }
    send_to_char("You can now see between planes.\n\r",ch);
    SET_BIT(ch->affected_by, AFF_SHADOWSIGHT);
    return;
}

void do_shadowplane( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

     if (!IS_SPHERE(ch, SPHERE_ILLUSIONIST))
        {
            send_to_char("#0This is an Illusionist Sphere Power.#n\n\r",ch);
            return;
        }
    if ( arg[0] == '\0' )
    {
    	if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	{
            if (has_timer(ch)) return;
	    send_to_char("You fade into the plane of shadows.\n\r",ch);
	    act("The shadows flicker and swallow up $n.",ch,NULL,NULL,TO_ROOM);
	    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    do_look(ch,"auto");
	    return;
    	}
    	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    	send_to_char("You fade back into the real world.\n\r",ch);
	act("The shadows flicker and $n fades into existance.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "What do you wish to toss into the shadow plane?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	send_to_char( "You toss it to the ground and it vanishes.\n\r", ch );
    else
    	send_to_char( "You toss it into a shadow and it vanishes.\n\r", ch );
    return;
}




void do_formillusion(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      buf[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

                if (!IS_SPHERE(ch, SPHERE_ILLUSIONIST))
                {
                        send_to_char("#0You dont have the illusion sphere.#n\n\r", ch);
                        return;
                }

        if (ch->pcdata->followers > 4)
        {
                send_to_char("Nothing happens.\n\r", ch);
                return;
        }

        ch->pcdata->followers++;


        victim = create_mobile(get_mob_index(2031));
        victim->short_descr = str_dup(ch->name);
        printf(buf, "%s is hovering here.\n\r", ch->name);
        victim->long_descr = str_dup(buf);
        victim->name = str_dup(ch->name);
        victim->level = 200;
        victim->max_hit = ch->max_hit;
        victim->hit = victim->max_hit;
        victim->max_mana = ch->max_mana;
        victim->mana = victim->max_mana;
        victim->max_move = ch->max_move;
        victim->move = victim->max_move;
        victim->hitroll = ch->hitroll;
        victim->damroll = ch->damroll;
        SET_BIT(victim->flag2, VAMP_CLONE);
        SET_BIT(victim->act, ACT_NOEXP);
        char_to_room(victim, ch->in_room);
        ch->pcdata->condition[COND_THIRST] -= 30;
        act("You concentrate your powers and form an illusion of yourself.",
            ch, NULL, victim, TO_CHAR);
        act("$n waves $s hands and splits in two.", ch, NULL, victim,
            TO_ROOM);
        return;

}


void do_illusionaryclone(CHAR_DATA *ch, char *argument)
{
             int       illusioncost = 5;
                if (!IS_SPHERE(ch, SPHERE_ILLUSIONIST))
                {
                        send_to_char("#0You dont have the illusion sphere.#n\n\r", ch);
                        return;
                }
                if (!IS_ITEMAFF(ch, ITEMA_ILLUSIONS)
                    && ch->practice >= illusioncost)
                {
                        SET_BIT(ch->itemaffect, ITEMA_ILLUSIONS);
                        ch->practice -= illusioncost;
                        send_to_char
                                ("You conjure several images of yourself, making it impossibly to tell which is really you.\n\r",
                                 ch);
                        return;
                }
                else if (IS_ITEMAFF(ch, ITEMA_ILLUSIONS))
                {
                        send_to_char
                                ("You cannot summon any more illusions.\n\r",
                                 ch);
                        return;
                }
                else
                {
                        send_to_char
                                ("it costs 5 primal to invoke the illusions of yourself.\n\r",
                                 ch);
                        return;
                }
                return;
        }

