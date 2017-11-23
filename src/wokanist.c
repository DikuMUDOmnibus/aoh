#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>   // we need these to call wait()

#include <execinfo.h>   // internal backtraces.

#include "merc.h"

void do_gbreath(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int hpch;
    int chhp;

        if (!IS_SPHERE(ch, SPHERE_WOKANIST))
        {
        send_to_char("Wokanists Only.\n\r",ch);
        return;
        }

	 if (ch->mana < 100)
	 {
		send_to_char ("You dont have enough mana.\n\r",ch);
		return;
	 }

	if( IS_SET(ch->in_room->room_flags, ROOM_SAFE)) return;

    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
	
	vch_next = vch->next_in_room;
	if (can_see(ch, vch)) 
	if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range(14000,28000);
	chhp = vch->hit;
	dam *= 2;
       if ( IS_AFFECTED(vch, AFF_SANCTUARY) ) dam /=2;
        hurt_person(ch, vch, dam);
	set_fighting(ch,vch);
	}
    }
	ch->mana -= 100;
	WAIT_STATE(ch,8);

    return;
}

void do_naturesfury (CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int dam;
  int red_magic = ch->spl[RED_MAGIC];
  int blue_magic = ch->spl[BLUE_MAGIC];
  int green_magic = ch->spl[GREEN_MAGIC];
  int purple_magic = ch->spl[PURPLE_MAGIC];
  int yellow_magic = ch->spl[YELLOW_MAGIC];
  one_argument( argument, arg );
  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_WOKANIST))
        {
        send_to_char("Wokanists Only?\n\r",ch);
        return;
        }



  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
    if (victim == ch)
    {
      send_to_char("You really don't want to hurt yourself.\n\r", ch );
      return;
    }
    if (is_safe(ch, victim)) return;
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
      return;
    if (IS_AFFECTED(ch, AFF_PEACE)) REMOVE_BIT(ch->affected_by, AFF_PEACE);
    if (victim->position > POS_STUNNED)
    {
      if (victim->fighting == NULL) set_fighting(victim, ch);
      if (ch->fighting == NULL) set_fighting(ch, victim);
    }
    WAIT_STATE(ch,8);
    dam = number_range(red_magic*10, red_magic*15); // was 10 and 15
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    dam = dam * 8;

    if (!IS_NPC(victim))
    {
      if (IS_SPHERE(ch, SPHERE_WOKANIST) && dam > 10) dam = number_range(2000,4500);
      if (dam > 1000) dam = number_range(4500,6500); // 45 and 65
    }
    sprintf(buf1,"$n calls upon the forces of Nature to use #rFIRE#n to destroy $N [#C%d#n]" , dam);
    sprintf(buf2,"You call for the forces of Nature to use #rFIRE#n to destroy $N [#C%d#n]" , dam);
    sprintf(buf3,"$n calls for the forces of Nature to use#rFIRE#n to destroy you [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(blue_magic*4, blue_magic*5); // 4 and 5
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
      if (IS_SPHERE(ch, SPHERE_WOKANIST) && dam > 10) dam = number_range(2000,4500);
      if (dam > 1000) dam = number_range(3500,5500);
    }
    sprintf(buf1,"$n calls up the power of #GEARTH#n to attack $N with boulders [#C%d#n]" , dam);
    sprintf(buf2,"You call upon the power of #GEARTH to attack $N with boulders [#C%d#n]" , dam);
    sprintf(buf3,"$n calls upon the power of #GEARTH to attack you with boulders [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(green_magic*10, green_magic*15);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    dam = dam * 8;
    if (!IS_NPC(victim))
    {
      if (IS_SPHERE(ch, SPHERE_WOKANIST) && dam > 10) dam = number_range(2000,4500);
      if (dam > 1000) dam = number_range(2500,4500);
    }
    sprintf(buf1,"$n calls for forth the power of #7AIR#n to strike $N [#C%d#n]" , dam);
    sprintf(buf2,"You call forth the power of #7AIR#n to strike $N [#C%d#n]" , dam);
    sprintf(buf3,"$n calls forth the power of #7AIR#n to strike You with DEVESTATING FORCE! [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(purple_magic*10, purple_magic*15);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    dam = dam * 8;
    if (!IS_NPC(victim))
    {
      if (IS_SPHERE(ch, SPHERE_WOKANIST) && dam > 10) dam = number_range(2000,4500);
      if (dam > 1000) dam = number_range(4500,6500);
    }
    sprintf(buf1,"$n calls for forth the power of #lWATER#n to strike $N [#C%d#n]" , dam);
    sprintf(buf2,"You call forth the power of #lWATER#n to strike $N [#C%d#n]" , dam);
    sprintf(buf3,"$n calls forth the power of #lWATER#n ATONOMIZING You! [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(yellow_magic*10, yellow_magic*15);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    dam = dam * 8;
   return;
  }


     void do_elemshift (CHAR_DATA *ch, char *argument)
	{
  	char arg[MAX_INPUT_LENGTH];
  	char buf[MAX_STRING_LENGTH];
  	one_argument( argument, arg);

  	if (IS_NPC(ch)) return;
  	if (!IS_SPHERE(ch, SPHERE_WOKANIST))
    	{
        send_to_char("Wokanists Only.\n\r",ch);
        return;
    	}
  	if (arg[0] == '\0')
  	{
    	send_to_char("#7Which #RElemental #7form? #7[#RFire#7-#CAir#7-#LWater#7-#oEarth#7-#YMortal#7]#n\n\r", ch);
    	return;
  	}
  	if (!str_cmp(arg,"fire"))
  	{
    	if (IS_SET(ch->affected_by, AFF_POLYMORPH))
    	{
      	send_to_char("#7You're already in a #RELEMENTAL #7form#7!#n\n\r",ch);
      	return;
    	}
    	if (ch->mana < 1000)
  	{
    	send_to_char("#7You don't have enough #Rmana#7.#n\n\r",ch);
    	return;
  	}
    	act( "You become ingulfed in #RF#Yl#Ra#Ym#Re#Ys#n.", ch, NULL, NULL, TO_CHAR);
    	act( "$n's Becomes a #7Human #YT#Ro#Yr#Rc#Yh#n.", ch, NULL, NULL, TO_ROOM );
    	ch->pcdata->powers[ELEMENTAL_FORMS]=FIRE_FORM;
    	SET_BIT(ch->affected_by, AFF_POLYMORPH);
    	ch->hitroll += 400;
    	ch->damroll += 400;
    	ch->armor   -= 200;
    	ch->mana    -= 1000;
    	sprintf(buf, "%s The #RF#Yi#Rr#Ye#n Elemental", ch->name);
    	free_string(ch->morph);
    	ch->morph = str_dup(buf);
    	return;
  	}
  	else if (!str_cmp(arg,"water"))
  	{
    	if (IS_SET(ch->affected_by, AFF_POLYMORPH))
    	{
      	send_to_char("#7You're already in a #RELEMENTAL #7form#7!#n\n\r",ch);
      	return;
    	}
    	if (ch->mana < 1000)
  	{
    	send_to_char("#7You don't have enough #Rmana#7.#n\n\r",ch);
    	return;
  	}
    	act( "Your skin is covered with #7Ice #lCrystals#n.", ch, NULL, NULL, TO_CHAR);
    	act( "$n's skin becomes covered with #7Ice #lcrystals#n.", ch, NULL, NULL, TO_ROOM );
    	ch->pcdata->powers[ELEMENTAL_FORMS]=WATER_FORM;
    	SET_BIT(ch->affected_by, AFF_POLYMORPH);
    	ch->hitroll += 300;
    	ch->damroll += 300;
    	ch->mana    -= 1000;
    	sprintf(buf, "%s The #lWater#n Elemental", ch->name);
    	free_string(ch->morph);
    	ch->morph = str_dup(buf);
    	return;
  	}
  	else if (!str_cmp(arg,"air"))
  	{
    	if (IS_SET(ch->affected_by, AFF_POLYMORPH))
    	{
      	send_to_char("#7You're already in a #RELEMENTAL #7form#7!#n\n\r",ch);
      	return;
    	}
    	if (ch->mana < 1000)
  	{
    	send_to_char("#7You don't have enough #Rmana#7.#n\n\r",ch);
    	return;
  	}
    	act( "You becomes a thin #LM#7i#Ls#7t#n.", ch, NULL, NULL, TO_CHAR);
    	act( "$n's Becomes a thin #LM#7i#Ls#7t#n.", ch, NULL, NULL, TO_ROOM );
    	ch->pcdata->powers[ELEMENTAL_FORMS]=AIR_FORM;
    	SET_BIT(ch->affected_by, AFF_POLYMORPH);
    	ch->hitroll += 200;
    	ch->damroll += 200;
    	ch->armor   -= 200;
    	ch->mana    -= 1000;
    	sprintf(buf, "%s The #7Air#n Elemental", ch->name);
    	free_string(ch->morph);
    	ch->morph = str_dup(buf);
    	return;
  	}
  	else if (!str_cmp(arg,"earth"))
  	{
    	if (IS_SET(ch->affected_by, AFF_POLYMORPH))
    	{
      	send_to_char("#7You're already in a #RELEMENTAL #7form#7!#n\n\r",ch);
      	return;
    	}
    	if (ch->mana < 1000)
  	{
    	send_to_char("#7You don't have enough #Rmana#7.#n\n\r",ch);
    	return;
  	}
    	act( "Your Skin turns to Hard #GE#garth#n.", ch, NULL, NULL, TO_CHAR);
    	act( "$n's skin turns to Hard #GE#garth#n.", ch, NULL, NULL, TO_ROOM );
    	ch->pcdata->powers[ELEMENTAL_FORMS]=EARTH_FORM;
    	SET_BIT(ch->affected_by, AFF_POLYMORPH);
    	ch->hitroll += 500;
   	ch->damroll += 500;
    	ch->armor   -= 500;
    	ch->mana    -= 1000;
    	sprintf(buf, "%s The #GE#garth#n Elemental", ch->name);
    	free_string(ch->morph);
    	ch->morph = str_dup(buf);
    	return;
  	}
  	else if (!str_cmp(arg,"mortal"))
  	{
    	if (!IS_SET(ch->affected_by, AFF_POLYMORPH))
    	{
      	send_to_char("#7Your already in #RMortal #7form.#n\n\r",ch);
      	return;
   	}
    	if (IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], FIRE_FORM))
    	{
      	ch->hitroll -= 400;
      	ch->damroll -= 400;
      	ch->armor   += 200;
    	}
    	else if (IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], AIR_FORM))
    	{
      	ch->hitroll -= 200;
      	ch->damroll -= 200;
      	ch->armor   += 200;
    	}
    	else if (IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], WATER_FORM))
    	{
      	ch->hitroll -= 300;
      	ch->damroll -= 300;
  	}
    	else if (IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], EARTH_FORM))
    	{
      	ch->hitroll -= 500;
      	ch->damroll -= 500;
      	ch->armor   += 500;
  	}
        ch->pcdata->powers[ELEMENTAL_FORMS]=0;
     	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        send_to_char("#7You return to your #RMortal #7form.#n\n\r",ch);
        act("#7$n returns to $s #RMortal #7form.#n\n\r",ch,NULL,NULL,TO_ROOM);
        free_string(ch->morph);
        ch->morph=str_dup("");
        return;
  	}
  	else
  	{
    	send_to_char("#7That is not a valid form.#n\n\r",ch);
    	return;
  	}
	}

