
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_skin(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL2))
        {
                send_to_char("#0You need the Animal Sphere Two for this.#n\n\r", ch);
                return;
        }


  if (!IS_SET(ch->newbits, NEW_SKIN))
  {
  send_to_char("Your skin hardens.\n\r",ch);
  ch->armor -= 100 ;
  SET_BIT(ch->newbits, NEW_SKIN);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_SKIN))
  { 
  send_to_char("Your skin softens.\n\r",ch);
  ch->armor += 100;
  REMOVE_BIT(ch->newbits,NEW_SKIN);
  return;
  }
return;
}


void do_sclaws(CHAR_DATA * ch, char *argument)
{

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("#0You need the Animal Sphere for this.#n\n\r", ch);
                return;
        }

        if (!IS_SET(ch->newbits, NEW_SCLAWS))
        {
                send_to_char("Your claws turn into pure silver!\n\r", ch);
                act("$n's claws turn to pure silver!", ch, NULL, NULL,
                    TO_ROOM);
                SET_BIT(ch->newbits, NEW_SCLAWS);
                return;
        }

        if (IS_SET(ch->newbits, NEW_SCLAWS))
        {
                send_to_char("Your claws return to their normal state.\n\r",
                             ch);
                act("$n's claws are no longer composed of pure silver.\n\r",
                    ch, NULL, NULL, TO_ROOM);
                REMOVE_BIT(ch->newbits, NEW_SCLAWS);
                return;
        }

}

void do_claws(CHAR_DATA * ch, char *argument)
{

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("You need the Animal Sphere for this.\n\r", ch);
                return;
        }
	
	if (IS_SET(ch->pcdata->stats[UNI_AFF], VAM_CLAWS))
	{
                send_to_char("You retract your claws.\n\r", ch);
                act("$n retracts $s claws.", ch, NULL, NULL, TO_ROOM);
                REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
        }
        else
        {
                send_to_char("Wicked claws extend from your fingers.\n\r",
                             ch);
                act("Long sharp claws extend from $n's fingers.", ch, NULL,
                    NULL, TO_ROOM);
                remove_obj(ch, WEAR_WIELD, TRUE);
                remove_obj(ch, WEAR_HOLD, TRUE);
                remove_obj(ch, WEAR_THIRD, TRUE);
                remove_obj(ch, WEAR_FOURTH, TRUE);
                SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
        }
}

void do_fangs(CHAR_DATA * ch, char *argument)
{

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("You need the Animal Sphere for this.\n\r", ch);
                return;
        }
        if (IS_SET(ch->pcdata->stats[UNI_AFF], VAM_FANGS))
	{
                send_to_char("Your fangs slide back into your gums.\n\r", ch);
                act("$n retracts $s fangs back into $s gums.", ch, NULL, NULL,
                    TO_ROOM);
                REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
        }
        else
        {
                send_to_char("Sharp fangs slide out of your gums.\n\r", ch);
                act("Sharp fangs slide out of $n's gums.", ch, NULL, NULL,
                    TO_ROOM);
                SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
        }
}



void do_flameclaws(CHAR_DATA * ch, char *argument)
{

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL2))
        {
                send_to_char("You need the second Animal Sphere for this.\n\r", ch);
                return;
        }

        if (IS_SET(ch->newbits, NEW_MONKFLAME))
        {
                send_to_char("The flames surrounding your claws fade.\n\r",
                             ch);
                act("The flames surrounding $n's claws fade.\n\r", ch, NULL,
                    NULL, TO_ROOM);
                REMOVE_BIT(ch->newbits, NEW_MONKFLAME);
                return;
        }

        if (!IS_SET(ch->newbits, NEW_MONKFLAME))
        {
                send_to_char
                        ("Your claws are surrounded by blazing flames.\n\r",
                         ch);
                act("$n's claws are surrounded by blazing flames.\n\r", ch,
                    NULL, NULL, TO_ROOM);
                SET_BIT(ch->newbits, NEW_MONKFLAME);
                return;
        }

}

void do_rend(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("You need the Animal Sphere for this.\n\r", ch);
                return;
        }

        if (!IS_SET(ch->newbits, NEW_REND))
        {
                send_to_char
                        ("You will now ruthlessly rend your opponents eq.\n\r",
                         ch);
                SET_BIT(ch->newbits, NEW_REND);
                return;
        }
        else if (IS_SET(ch->newbits, NEW_REND))
        {
                send_to_char("Your no longer mean to your enemies eq.\n\r",
                             ch);
                REMOVE_BIT(ch->newbits, NEW_REND);
                return;
        }
        return;
}
void do_roar(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      buf[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL2))
        {
                send_to_char("You need the second Animal Sphere for this.\n\r", ch);
                return;
        }

        if ((victim = ch->fighting) == NULL)
        {
                send_to_char("You arent fighting anyone.\n\r", ch);
                return;
        }

        if (number_range(1, 6) == 2)
        {
                printf(buf,
                        "%s roars and screams, intimidating the hell out of %s.",
                        ch->name, victim->name);
                act(buf, ch, NULL, NULL, TO_ROOM);
                printf(buf,
                        "You roar and scream, intimidating the hell out of %s.\n\r",
                        victim->name);
                send_to_char(buf, ch);
                do_flee(victim, "");
                WAIT_STATE(ch, 18);
                return;
        }
        else
        {
                stc("You roar loudly.\n\r", ch);
                stc("Nothing happens.\n\r", ch);
                act("$n roars loudly.", ch, NULL, NULL, TO_ROOM);
                WAIT_STATE(ch, 12);
                return;
        }
}

void do_talons(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      buf[MAX_STRING_LENGTH];
        int       dam;

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("You need the Animal Sphere for this.\n\r", ch);
                return;
        }

        if ((victim = ch->fighting) == NULL)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }
        WAIT_STATE(ch, 12);

        if (IS_NPC(victim))
                dam = number_range(400, 600);
        else
                dam = number_range(3000, 5000);

  	sprintf(buf,"Your talons strike $N incredibly hard! [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_CHAR);
  	sprintf(buf,"$n's talons strike you incredibly hard! [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_VICT);
  	sprintf(buf,"$n's talons strike $N incredibly hard! [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_NOTVICT);

        hurt_person(ch, victim, dam);
        return;
}

void do_quills(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL2))
        {
                send_to_char("You need the second Animal Sphere for this.\n\r", ch);
                return;
        }

        if (IS_SET(ch->newbits, NEW_QUILLS))
        {
                stc("Your quills soften and twist into fur.\n\r", ch);
                act("$n's quills soften and revert back to fur.", ch, NULL,
                    NULL, TO_ROOM);
                REMOVE_BIT(ch->newbits, NEW_QUILLS);
                return;
        }

        if (!IS_SET(ch->newbits, NEW_QUILLS))
        {
                stc("Your fur becomes bristly and sharp.\n\r", ch);
                act("$n's fur becomes bristly and sharp.", ch, NULL, NULL,
                    TO_ROOM);
                SET_BIT(ch->newbits, NEW_QUILLS);
                return;
        }
        return;
}

void do_burrow(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ANIMAL))
        {
                send_to_char("You need the Animal Sphere for this.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Burrow to who?\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Your room is not connected to the astral plane.\n\r",
                    ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }

        if (IS_IMMUNE(victim, IMM_TRAVEL))
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

        if (victim == ch)
        {
                send_to_char("You cannot burrow to yourself.\n\r", ch);
                return;
        }

        if (victim->in_room == NULL)
        {
                send_to_char("Nothing happens.\n\r", ch);
                return;
        }
        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Their room is not connected to the astral plane.\n\r",
                    ch);
                return;
        }

        send_to_char("You burrow into the ground.\n\r", ch);
        act("$n burrows into the ground.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        do_look(ch, "");
        send_to_char("You burrow out of the ground.\n\r", ch);
        act("$n burrows out of the ground.", ch, NULL, NULL, TO_ROOM);
        return;
}


