
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_darksummon( CHAR_DATA *ch, char *argument )
   {
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
 
   if (!IS_SPHERE(ch, SPHERE_NECROMANCER))
  {
    send_to_char("#0Darksummon is a Necromancer power.#n\n\r",ch);
    return;
  }
        if ( arg1[0] == '\0' )
    {
        send_to_char( "Darksummon who?\n\r", ch );
        return;
    }
    location = ch->in_room;
 if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( victim->fighting != NULL )
    {
        send_to_char( "They are fighting right now.\n\r", ch );
        return;
    }

    if (IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_ASTRAL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE))
        {
        send_to_char("You cant seem to focus on your victim.\n\r",ch);
        return;
        }
if (IS_SET (victim->in_room->room_flags, ROOM_ARENA))
        { 
        send_to_char("You crazy bitch, you cant do this in the arena.\n\r",ch);
        return;
        }
 if (ch->practice < 1000)
{
send_to_char("You need 1000 primal to use this\n\r",ch);
return;
}

if (ch->in_room->vnum >= 79000 && ch->in_room->vnum <= 80000)
{
send_to_char("You cannot do this from inside your clan recall\n\r",ch);
return;
}

  if (victim->fight_timer == 0)
  {
    send_to_char("You can only summon someone who is freshly done a battle.\n\r",ch);
    return;
  }  
    ch->practice -= 1000;  
    act( "$n slips away into a dimensional rift.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n is sucked through the dimensional rift you created.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
        act( "$n has sucked you into his dimensional rift.", ch, NULL, victim, TO_VICT );

}



void do_creepingdoom(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *ich;
        CHAR_DATA *dummychar = ch;
        int       totaldam = ch->hit;
        int       dam;
        char      buf1[MAX_STRING_LENGTH];
        char      buf2[MAX_STRING_LENGTH];
        char      buf3[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;
        if (!IS_SPHERE(ch, SPHERE_NECROMANCER))
        {
                send_to_char("This requires Necromancer Sphere.\n\r", ch);
                return;
        }
        if (ch->mana < 50)
        {
                send_to_char
                        ("You don't have the power to summon the insects.\n\r",
                         ch);
                return;
        }
        send_to_char("You send out a call for the insects in the area.\n\r",
                     ch);
        for (ich = ch->in_room->people; dummychar != NULL; ich = dummychar)
        {
                if (totaldam <= 0)
                        break;
                dummychar = ich->next_in_room;
                if (!IS_NPC(ich))
                        continue;
                if (is_safe(ch, ich))
                        continue;
                dam = UMIN(totaldam, ich->hit + 10);
                sprintf(buf1,
                        "$N screams in horror as insects from all over swarm over $M and starts eating [#C%d#n]",
                        dam);
                sprintf(buf2, "Your insects bite $N [#C%d#n]", dam);
                sprintf(buf3,
                        "$n laughs as insects swarm from nowhere and attack you [#C%d#n]",
                        dam);
                act(buf1, ch, NULL, ich, TO_NOTVICT);
                act(buf2, ch, NULL, ich, TO_CHAR);
                act(buf3, ch, NULL, ich, TO_VICT);
                hurt_person(ch, ich, dam);
                totaldam -= dam;
                if (ich->position > POS_STUNNED && ich != ch)
                {
                        if (ch->fighting == NULL)
                                set_fighting(ch, ich);
                        if (ich->fighting == NULL)
                                set_fighting(ich, ch);
                }
        }
        ch->mana -= 50; // ch->mana / 2;
        WAIT_STATE(ch, 5);
        return;
}


void do_powerword(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        char      arg2[MAX_INPUT_LENGTH];
        char      buf1[MAX_STRING_LENGTH];
        char      buf2[MAX_STRING_LENGTH];
        char      buf3[MAX_STRING_LENGTH];
        AFFECT_DATA af;
        CHAR_DATA *ich;
        CHAR_DATA *ich_next;
        CHAR_DATA *victim;

        argument = one_argument(argument, arg);
        argument = one_argument(argument, arg2);
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_NECROMANCER))
        {
                send_to_char("#0This requires the Necromancer sphere.#n\n\r", ch);
                return;
        }
        if (arg[0] == '\0')
        {
                send_to_char
                        ("Valid powerwords are kill,stun,blind and flames.\n\r",
                         ch);
                return;
        }
        if (!str_cmp(arg, "stun"))
        {
                if (!(arg2[0] == '\0') || ch->fighting != NULL)
                {
                        if (arg2[0] == '\0')
                                victim = ch->fighting;
                        else if ((victim =
                                  get_char_room(ch, arg2)) == NULL)
                        {
                                send_to_char("They are not here.\n\r", ch);
                                return;
                        }
                        if (ch->pcdata->powers[POWER_TICK] > 0)
                        {
                                send_to_char
                                        ("You cannot cast another powerword yet.\n\r",
                                         ch);
                                return;
                        }
                        if (is_safe(ch, victim))
                                return;
                        act("$n points his finger at $N and says '#rFREEZE!!!#n'.", ch, NULL, victim, TO_NOTVICT);
                        act("You point your finger at $N and say '#rFREEZE!!!#n'.", ch, NULL, victim, TO_CHAR);
                        act("$n points his finger at $N and says '#rFREEZE!!!#n'.", ch, NULL, victim, TO_VICT);
                        WAIT_STATE(victim, 24);
                        WAIT_STATE(ch, 8);
                        ch->pcdata->powers[POWER_TICK] = 4;
                        return;
                }
                else
                {
                        send_to_char("Stun whom?\n\r", ch);
                        return;
                }
        }
        else if (!str_cmp(arg, "blind"))
        {
                if (!(arg2[0] == '\0') || ch->fighting != NULL)
                {
                        if (arg2[0] == '\0')
                                victim = ch->fighting;
                        else if ((victim =
                                  get_char_room(ch, arg2)) == NULL)
                        {
                                send_to_char("They are not here.\n\r", ch);
                                return;
                        }
                        if (ch->pcdata->powers[POWER_TICK] > 0)
                        {
                                send_to_char
                                        ("You cannot cast another powerword yet.\n\r",
                                         ch);
                                return;
                        }
                        if (is_safe(ch, victim))
                                return;
                        act("$n touches $N's forehead and screams '#rMIDNIGHT!!!#n'.", ch, NULL, victim, TO_NOTVICT);
                        act("You touch $N's forehead and screams '#rMIDNIGHT!!!#n'.", ch, NULL, victim, TO_CHAR);
                        act("$n touches your forehead and screams '#rMIDNIGHT!!!#n'.", ch, NULL, victim, TO_VICT);
                        if (IS_SET(victim->act, PLR_HOLYLIGHT))
                                REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
                        if (IS_SET(victim->affected_by, AFF_DETECT_HIDDEN))
                                REMOVE_BIT(victim->affected_by,
                                           AFF_DETECT_HIDDEN);
                        if (IS_SET(victim->affected_by, AFF_DETECT_INVIS))
                                REMOVE_BIT(victim->affected_by,
                                           AFF_DETECT_INVIS);
                        af.type = skill_lookup("blindness");
                        af.location = APPLY_HITROLL;
                        af.modifier = -4;
                        af.duration = 60;
                        af.bitvector = AFF_BLIND;
                        affect_to_char(victim, &af);
                        WAIT_STATE(ch, 12);
                        ch->pcdata->powers[POWER_TICK] = 3;
                        return;
                }
                else
                {
                        send_to_char("Blind whom?\n\r", ch);
                        return;
                }
        }
        else if (!str_cmp(arg, "kill"))
        {
                if (!(arg2[0] == '\0'))
                {
                        if ((victim = get_char_room(ch, arg2)) == NULL)
                        {
                                send_to_char("They are not here.\n\r", ch);
                                return;
                        }
                        if (ch->pcdata->powers[POWER_TICK] > 0)
                        {
                                send_to_char
                                        ("You cannot cast another powerword yet.\n\r",
                                         ch);
                                return;
                        }
                        if (ch == victim)
                        {
                                send_to_char("Not yourself, silly!\n\r", ch);
                                return;
                        }
                        if (is_safe(ch, victim))
                                return;
                        act("$n looks at $N and utters '#rDIE!!!#n'.", ch,
                            NULL, victim, TO_NOTVICT);
                        act("You look at $N and say '#rDIE!!!#n'.", ch, NULL,
                            victim, TO_CHAR);
                        act("$n looks at you and utters '#rDIE!!!#n'.", ch,
                            NULL, victim, TO_VICT);
                        if (victim->position > POS_STUNNED)
                        {
                                if (ch->fighting == NULL)
                                        set_fighting(ch, victim);
                                if (victim->fighting == NULL)
                                        set_fighting(victim, ch);
                        }
                        if (!IS_NPC(victim) || victim->level > 100)
                        {
                                int       dam = victim->hit * .4;

                                if (IS_NPC(victim) && dam > 20000)
                                        dam = 50000;
                                if (!IS_NPC(victim) && dam > 8000)
                                        dam = 8000;
                                hurt_person(ch, victim, dam);
                                sprintf(buf1,
                                        "$n's powerword strikes $N [#C%d#n]",
                                        dam);
                                sprintf(buf2,
                                        "Your powerword strikes $N [#C%d#n]",
                                        dam);
                                sprintf(buf3,
                                        "$n's powerword strikes you [#C%d#n]",
                                        dam);
                                act(buf1, ch, NULL, victim, TO_NOTVICT);
                                act(buf2, ch, NULL, victim, TO_CHAR);
                                act(buf3, ch, NULL, victim, TO_VICT);
                                ch->pcdata->powers[POWER_TICK] = 5;
                                WAIT_STATE(ch, 8);
                                return;
                        }
                        ch->level = 12;
                        do_slay(ch, arg2);
                        ch->level = 3;
                        ch->pcdata->powers[POWER_TICK] = 2;
                        return;
                }
                else
                {
                        send_to_char("Kill whom?\n\r", ch);
                        return;
                }
        }
        else if (!str_cmp(arg, "flames"))
        {
                if (ch->pcdata->powers[POWER_TICK] > 2)
                {
                        send_to_char
                                ("You cannot cast another powerword yet.\n\r",
                                 ch);
                        return;
                }
                stc("#gYou raise your hands to encompass the room and scream, #n'#rBURN!!!#n'\n\r", ch);
                ich_next = ch->in_room->people;
                ich = ich_next;
                while (ich_next != NULL)
                {
                        ich_next = ich->next_in_room;
                        if (ich != ch)
                        {
                                if (is_safe(ch, ich))
                                        break;
                                one_hit(ch, ich, gsn_fireball, 1);
                                one_hit(ch, ich, gsn_fireball, 1);
                                one_hit(ch, ich, gsn_fireball, 1);
				one_hit(ch, ich, gsn_fireball, 1);
                        }
                        ich = ich_next;
                }
                WAIT_STATE(ch, 12);
                ch->pcdata->powers[POWER_TICK] += 1;
                return;
        }
        else
        {
                send_to_char("You have not learned that powerword yet.\n\r",
                             ch);
                return;
        }
}



void do_fleshportal (CHAR_DATA * ch, char *argument)
{

  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  CHAR_DATA *victim;

  one_argument (argument, arg);
  if (IS_NPC (ch))
    return;

        if (!IS_SPHERE(ch, SPHERE_NECROMANCER))
        {
                send_to_char("#0This requires the Necromancer sphere.#n\n\r", ch);
                return;
        }

  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("Who do you wish to fleshportal to?\n\r", ch);
      return;
    }
  if (IS_SET (victim->in_room->room_flags, ROOM_ASTRAL))
    {
      stc ("You can't find it's room.\n\r", ch);
      return;
    }
  if( IS_IMMUNE(victim, IMM_TRAVEL ) && !ragnarok && victim->fight_timer == 0) {
     send_to_char("He doesnt want you near him!\n\r",ch);
     return;
  }
  if (IS_SET (ch->in_room->room_flags, ROOM_ASTRAL))
    {
      stc ("Your room is not connected to the astral plane.\n\r", ch);
      return;
    }
  location = victim->in_room;
  if (ch->move < 500)
    {
      send_to_char ("You don't have the strength.\n\r", ch);
      return;
    }


  act ("You enter a portal made from flesh.", ch, NULL, NULL, TO_CHAR);
  act ("$n grows a portal of flesh from their body and enters it.", ch, NULL,
       NULL, TO_ROOM);

  char_from_room (ch);
  char_to_room (ch, location);
  do_look (ch, "auto");
  if (victim->fight_timer > 0)
    WAIT_STATE (ch, 8);
  act ("You exit your fleshportal by $N.", ch, NULL, victim, TO_CHAR);
  act ("$n appears from a portal of flesh.", ch, NULL, NULL, TO_ROOM);
  return;

}

