
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"




void do_chaossurge(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      buf[MAX_STRING_LENGTH];
        char      arg[MAX_INPUT_LENGTH];
        int       dam;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_SPHERE(ch, SPHERE_CHAOS))
        {
                send_to_char("#0This is a Chaos Sphere Ability#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }
        if (IS_NPC(victim))
        {
                send_to_char("Not on mobiles.\n\r", ch);
                return;
        }
        if (is_safe(ch, victim))
                return;
        if (victim == ch)
        {
                send_to_char("That doesn't seem like a good idea.\n\r", ch);
                return;
        }
        if (victim->alignment < -500)
        {
                send_to_char("They are too evil, you cannot affect them.\n\r",
                             ch);
                return;
        }
        if (victim->alignment > 500)
                dam = 5000;
        else if (victim->alignment > 0)
                dam = 2500;
        else
                dam = 1000;
        printf(buf, "You let chaos envelope $N and $E is hurt [%d]", dam);
        act(buf, ch, NULL, victim, TO_CHAR);
        printf(buf,
                "$n points a wicked finger at you and chaos strikes your mind! [%d]",
                dam);
        act(buf, ch, NULL, victim, TO_VICT);
        printf(buf, "$n strikes $N down with a burst of pure #RCHAOS!#n.");
        act(buf, ch, NULL, victim, TO_NOTVICT);
        hurt_person(ch, victim, dam);
        WAIT_STATE(ch, 12);
        return;
}




void do_chaosgate(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        ROOM_INDEX_DATA *location;
        CHAR_DATA *victim;
        int       random_room;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("Chaosgate to whom?\n\r", ch);
                return;
        }
        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("You can't find it's room.\n\r", ch);
                return;
        }
        if (IS_IMMUNE(victim, IMM_TRAVEL) && !IS_NPC(victim))
        {
                send_to_char("I don't think they want you to do that.\n\r",
                             ch);
                return;
        }
        if (IS_SET(victim->act, ACT_NOTRAVEL))
        {
                send_to_char("No Can Do.\n\r", ch);
                return;
        }
        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Your room is not connected to the astral plane.\n\r",
                    ch);
                return;
        }
        if (ch->move < 1000)
        {
                send_to_char
                        ("You don't have the move to open a chaosgate.\n\r",
                         ch);
                return;
        }
        if (number_range(1, 15) != 1)
                location = victim->in_room;
        else
        {
                send_to_char("Something went wrong.\n\r", ch);
                for (;;)
                {
                        random_room = number_range(1000, 65535);
                        location = get_room_index(random_room);
                        if (location != NULL)
                                if ( !IS_SET
                                    (location->room_flags, ROOM_PRIVATE)
//                                    !IS_SET(location->room_flags,
  //                                             ROOM_SOLITARY)
                                    && !IS_SET(location->room_flags,
                                               ROOM_NO_TELEPORT))
                                        break;
                }
        }
        act("You open a gate made from pure chaos and steps into it.", ch,
            NULL, NULL, TO_CHAR);
        act("$n opens a gate of pure chaos and steps into it.", ch, NULL,
            NULL, TO_ROOM);
        ch->move -= 1000;
        char_from_room(ch);
        char_to_room(ch, location);
        do_look(ch, "auto");
        act("You step out of the gate.", ch, NULL, NULL, TO_CHAR);
        act("A gate appears from out of nowhere and $n steps out of it.", ch,
            NULL, NULL, TO_ROOM);
        return;
}



void do_chaosmagic(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        int       sn, randnum, level;

        argument = one_argument(argument, arg);
        level = ch->spl[RED_MAGIC] / 4;

        if (IS_NPC(ch))
                return;
        if (!IS_SPHERE(ch, SPHERE_CHAOS))
        {
                send_to_char("#0You need the chaos sphere for this.#n\n\r", ch);
                return;
        }
        if (ch->mana < 1500)
        {
                send_to_char("You need more mana.\n\r", ch);
                return;
        }
        if (ch->in_room != NULL)
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
                {
                        send_to_char
                                ("You cannot invoke the forces of chaos in a safe room.\n\r",
                                 ch);
                        return;
                }
        }
        if (arg[0] == '\0' && ch->fighting != NULL)
                victim = ch->fighting;
        else if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }
        randnum = number_range(1, 12);
        if (randnum == 1)
                sn = skill_lookup("spirit kiss");
        else if (randnum == 2)
                sn = skill_lookup("desanct");
        else if (randnum == 3)
                sn = skill_lookup("imp heal");
        else if (randnum == 4)
                sn = skill_lookup("imp fireball");
        else if (randnum == 5)
                sn = skill_lookup("imp faerie fire");
        else if (randnum == 6)
                sn = skill_lookup("imp teleport");
        else if (randnum == 7)
                sn = skill_lookup("change sex");
        else if (randnum == 8)
                sn = skill_lookup("shield");
        else if (randnum == 9)
                sn = skill_lookup("readaura");
        else if (randnum == 10)
                sn = skill_lookup("earthquake");
        else if (randnum == 11)
                sn = skill_lookup("gate");
        else if (randnum == 12)
                sn = skill_lookup("dispel magic");
        else
                sn = 0;
        if (sn > 0)
                (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        ch->mana -= 1500;
        WAIT_STATE(ch, 6);
        return;
}


void do_chaosshield(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;
        if (!IS_SPHERE(ch, SPHERE_CHAOS))
        {
                send_to_char("#0This is a Chaos Sphere Ability#n\n\r", ch);
                return;
        }
        if (!IS_IMMUNE(ch, IMM_SHIELD2))
        {
                send_to_char
                        ("You shield your aura from those around you.\n\r",
                         ch);
                SET_BIT(ch->immune, IMM_SHIELD2);
                return;
        }
        send_to_char("You stop shielding your aura.\n\r", ch);
        REMOVE_BIT(ch->immune, IMM_SHIELD2);
        return;
}

void do_chaos(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        AFFECT_DATA af;
        int       chaos, dam;
        char      arg[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];

        argument = one_argument(argument, arg);


        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_CHAOS))
        {
                send_to_char("#0This is a Chaos Sphere Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#RCast chaos orb on whom?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }
        if (is_safe(ch, victim))
        {
                stc("You cannot attack them.\n\r", ch);
                return;
        }

        if (ch->mana < 5000)
        {
                stc("You do not ave the required 5000 mana.\n\r", ch);
                return;
        }


        chaos = number_range(1, 9);


        act("#r$n#0 begins chanting dark words, summoning forth a sphere of #rc#lh#pa#go#cs#0 to hurl at #r$N!#n", ch, NULL, victim, TO_NOTVICT);
        printf(buf,
                "#0You summon forth a perfect sphere of #rc#lh#pa#go#cs#0 and hurl it at #r$N!#n\n\r");
        act(buf, ch, NULL, victim, TO_CHAR);
        printf(buf,
                "#r$n#0 summons forth a perfect sphere of #rc#lh#pa#go#cs#0 and hurls it at you!#n\n\r");
        act(buf, ch, NULL, victim, TO_VICT);


        if (chaos == 1 || chaos == 6)
        {
                stc("The sphere explodes in a shower of #Ggreen bubbles#n.\n\r", victim);
                stc("Your sphere explodes in a shower of #Ggreen bubbles#n.\n\r", ch);
                if (IS_AFFECTED(victim, AFF_POISON))
                        return;
                af.type = gsn_poison;
                af.duration = chaos * 10;
                af.location = APPLY_STR;
                af.modifier = 0 - number_range(3, 8);
                af.bitvector = AFF_POISON;
                affect_join(victim, &af);
                WAIT_STATE(victim, number_range(5, 20));
                send_to_char("#GYou are poisoned!#n\n\r", victim);
        }
        else if (chaos == 2 || chaos == 7)
        {
                stc("The sphere explodes, forming a #yyellow haze#n.\n\r",
                    victim);
                stc("Your sphere explodes, forming a #yyellow haze#n.\n\r",
                    ch);
                if (IS_AFFECTED(victim, AFF_BLIND))
                {
                        af.type = gsn_blindness;
                        af.location = APPLY_HITROLL;
                        af.modifier = -750;
                        af.duration = chaos * 10;
                        af.bitvector = AFF_BLIND;
                        affect_to_char(victim, &af);
                        WAIT_STATE(victim, number_range(5, 15));
                        send_to_char("#yYou are blinded!#n\n\r", victim);
                }
                else
                {
                        stc("They are already blinded.\n\r", ch);
                }
        }
        else if (chaos == 3 || chaos == 8)
        {
                stc("The sphere explodes in a shower of #Rred flames#n.\n\r",
                    victim);
                stc("Your sphere explodes in a shower of #Rred flames#n.\n\r",
                    ch);
                WAIT_STATE(victim, number_range(5, 20));
                SET_BIT(victim->affected_by, AFF_FLAMING);
        }
        else if (chaos == 4 || chaos == 9)
        {
                dam = number_range(7500, 15000);
                printf(buf,
                        "The sphere explodes in a scattering of #Lblue sparks#n.  #R[#y%d#R]#n\n\r",
                        dam);
                stc(buf, victim);
                printf(buf,
                        "Your sphere explodes in a scattering of #Lblue sparks#n.  #R[#y%d#R]#n\n\r",
                        dam);
                stc(buf, ch);
                WAIT_STATE(victim, number_range(5, 8));
                hurt_person(ch, victim, dam);
        }
        else if (chaos == 5)
        {
                stc("The sphere dissipates before it hits you#n.\n\r",
                    victim);
                stc("Your dissipates shortly after casting#n.\n\r", ch);
        }

        WAIT_STATE(ch, 4);
        ch->mana -= 5000;
        if (victim->hit < -10)
                victim->hit = -10;
        return;
}

