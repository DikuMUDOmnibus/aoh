
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



void do_frost( CHAR_DATA *ch, char *argument) // monk adam hands
{
  if (IS_NPC(ch)) return;
  if (!IS_SPHERE(ch, SPHERE_HYDROMANCER))
  {
    send_to_char("#0Frost Hands is a Hydromancer Abilitiy#n\n\r",ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_MONKADAM))
  {
    REMOVE_BIT(ch->newbits, NEW_MONKADAM);
    send_to_char("Your hands resume their normal toughness.\n\r",ch);
    act("$n's hands resume their normal toughness.\n\r",ch,NULL,NULL,TO_ROOM);
    return;
  }
  if (!IS_SET(ch->newbits, NEW_MONKADAM))
  {
    SET_BIT(ch->newbits, NEW_MONKADAM);
    send_to_char("Your hands turn to Magic Ice as hard as adamantium!\n\r",ch);
    act("$n's hands turn to Magic Ice as hard as adamantium!\n\r",ch,NULL,NULL,TO_ROOM);
    return;
  }
}



void do_cbreath( CHAR_DATA *ch, char *argument )
{
	 CHAR_DATA *victim;
	 int i=0;
	 int dam = 0;
	 int chance =0;
	 char buf[MAX_STRING_LENGTH];
	 char buf2[MAX_STRING_LENGTH];
	char arg [MAX_INPUT_LENGTH];

	 if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_HYDROMANCER))
        {
                send_to_char("#0This is a Hydromancer ability.#n\n\r", ch);
                return;
        }


  if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    if (ch->fighting == NULL) 
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
    else victim = ch->fighting;
  }

	 if ((victim = ch->fighting) == NULL)
	  {
		  send_to_char( "You aren't fighting anyone.\n\r", ch );
		  return;
	  }

	 if (ch->mana < 100)
	  {
		  send_to_char("You don't have enough mana.\n\r", ch);
		  return;
	  }
  chance = number_range (1, (120));

  if (chance <= 7) i = 1;
  else if (chance > 7 && chance <= 22)   i = 2;
  else if (chance > 22 && chance <= 32)   i = 3;
  else if (chance > 32 && chance <= 52)   i = 4;
  else if (chance > 52 && chance <= 90)   i = 5;
  else if (chance > 90 && chance <= 94)   i = 6;
  else if (chance > 94 && chance <= 110)  i = 7;
  else if (chance > 110 && chance <= 125) i = 8;
  else if (chance > 125 && chance <= 128) i = 9;
  else if (128 < chance ) i = 10;

  for(i=i;i>0;i--)
	  {
		dam = number_range(1000 ,2500);
		if (dam > 5000) dam = 5000;
		sprintf(buf, "%s's cone of #Cf#7r#Co#7s#Ct#n freezes you [#C%d#n]\n\r", ch->name, dam);
		send_to_char( buf, victim );
		sprintf(buf2, "Your cone of #Cf#7r#Co#7s#Ct#n freezes %s [#C%d#n]\n\r",victim->name, dam);
		send_to_char( buf2, ch );
		act( "$n's cone of #Cf#7r#Co#7s#Ct#n freezes $N.",ch,NULL,victim,TO_NOTVICT);
		hurt_person(ch,victim,dam);
		if (victim->hit < 1) break;
//		cold_effect(victim,ch->explevel,dam,TARGET_CHAR);
	  }

  ch->mana = ch->mana - 100;
  WAIT_STATE(ch,12);
  WAIT_STATE(victim, 6);
  return;
}



void do_hydro(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;

        if (!IS_SPHERE(ch, SPHERE_HYDROMANCER))
        {
                send_to_char("#0This is a Hydromancer ability.#n\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                stc("Who do you wish to unnerve?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, argument)) == NULL)
        {
                stc("They aren't here.\n\r", ch);
                return;
        }

        if (number_range(1, 3) == 2)
        {
   		do_say(ch, "#rEat Shit and Die!#n");
        	do_stance(victim, "");
		WAIT_STATE(victim, 8);
	}
	WAIT_STATE(ch, 8);
	return;
}

