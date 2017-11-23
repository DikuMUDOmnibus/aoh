
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"




void do_earthswallow(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *mount;
        char      arg[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
    if (!IS_SPHERE(ch, SPHERE_GEOMANCER))
  {
  send_to_char("#0Earthswallow is a Geomancer power.#n\n\r",ch);
  return;
  }
        if (ch->mana < 5000)
        {
                send_to_char("You don't have enough mana.\n\r", ch);
                return;
        }
        if (has_timer(ch))
                return;
        if (ch->in_room != NULL)
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
                {
                        send_to_char("Your in the arena.\n\r", ch);
                        return;
                }
        }
        if (in_fortress(ch))
        {
                send_to_char("Your in The Forbidden Fortress.\n\r", ch);
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
        ch->mana -= 5000;
        act("$n is swallowed by the earth and disappears.", victim, NULL,
            NULL, TO_ROOM);
        char_from_room(victim);
        char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
        act("$n is flung out of a vulcano.", victim, NULL, NULL, TO_ROOM);
        do_look(victim, "auto");
        if ((mount = victim->mount) == NULL)
                return;
        char_from_room(mount);
        char_to_room(mount, victim->in_room);
        do_look(mount, "auto");
        WAIT_STATE(ch, 8);
        return;
}



void do_earthshatter( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int level;
    

	if (IS_NPC(ch)) return;

    if (!IS_SPHERE(ch, SPHERE_GEOMANCER))
  {
  send_to_char("#0EarthShatter is a Geomancer power.#n\n\r",ch);
  return;
  }
	if (ch->mana < 1150) {
	send_to_char("You need more mana.\n\r", ch );
	return;}

	level = ch->spl[PURPLE_MAGIC];
	ch->mana -= 1150;

	send_to_char("You summon the power of the underworld, shattering the earth.\n\r", ch );
	act("$n causes the earth to shatter",ch,NULL,NULL,TO_ROOM);

    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;
	if (vch == ch) continue;
        if (vch->trust>6) continue;
            dam  = dice(level, 80 );
            if ( saves_spell( level, vch ) )
                dam /= 2;
            damage( ch, vch, dam, skill_lookup("earthquake"));
            damage( ch, vch, dam, skill_lookup("earthquake"));
            damage( ch, vch, dam, skill_lookup("earthquake"));
  
  }
	WAIT_STATE(ch, 6);
    return;
}



void do_solidify(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
    if (!IS_SPHERE(ch, SPHERE_GEOMANCER))
  {
  send_to_char("#0Solidify is a Geomancer power.#n\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_SKIN))
  {
  send_to_char("Your Earthen Skin Hardens.\n\r",ch);
  ch->armor -= 100 ;
  SET_BIT(ch->newbits, NEW_SKIN);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_SKIN))
  { 
  send_to_char("Your Earthen Skin Softens.\n\r",ch);
  ch->armor += 100;
  REMOVE_BIT(ch->newbits,NEW_SKIN);
  return;
  }
return;
}

	void do_rockslide( CHAR_DATA *ch, char *argument )
	{
  	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
  	int dam;

	if (IS_NPC(ch)) return;
    	if(!IS_SPHERE(ch,SPHERE_GEOMANCER))
  	{
    	send_to_char("#0This is a Geomancer Ability.#n\n\r",ch);
    	return;
  	}
  	if ( ( victim = ch->fighting ) == NULL )
  	{
    	send_to_char( "You aren't fighting anyone.\n\r", ch );
    	return;
  	}
  	WAIT_STATE( ch, 12 );
  	if (!IS_NPC(victim)) dam = number_range(7000,12000);
  	else dam = number_range(10000,14000);
  	sprintf(buf,"#RYour #0Rockslide#n #Rstrike #Y$N #0incredibly hard!#n [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_CHAR);
  	sprintf(buf,"$n's #0Rockslide strikes you incredibly hard!#n [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_VICT);
  	sprintf(buf,"$n's rockslide strike $N incredibly hard! [%d]\n\r",dam);
  	act(buf,ch,NULL,victim,TO_NOTVICT); 
  	hurt_person(ch,victim, dam);
 	return;
	}

