#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"













void do_enrage(CHAR_DATA *ch, char *argument) 
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT4))
        {
                send_to_char("#0You need combat four for this.#n\n\r", ch);
                return;
        }

  if ((victim = get_char_room(ch, arg)) ==NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim) || victim->level < 3)
  {
    send_to_char("Not on them\n\r",ch);
    return;
  }
  do_say(ch, "Kara, Kara, Xenos!");
  if (number_percent() > 40) do_berserk2(victim,"");
  else send_to_char("#RSomeone is trying to control your actions!!!#n\n\r",victim);
  WAIT_STATE(ch,18);
  return;
}







void do_dtrick(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        OBJ_DATA *obj;
        char      arg1[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        int       randnum;
        int       successchance;

        argument = one_argument(argument, arg1);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT3))
        {
                send_to_char("#0You need combat three for this.#n\n\r", ch);
                return;
        }

        if (ch->move < 2500)
        {
                stc("#0You don't have the required amount of move to preform a Dirty Trick!#n\n\r", ch);
                return;
        }

        if (arg1[0] == '\0')
        {
                stc("#0Preform a Dirty Trick on whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, arg1)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }


        randnum = number_range(1, 4);   //Disarm, Trip, Foot Stomp, Hilt Smash.

        if (randnum == 1)   //<----Disarm
        {
                if (((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
                    || obj->item_type != ITEM_WEAPON)
                {
                        if (((obj = get_eq_char(victim, WEAR_HOLD)) == NULL)
                            || obj->item_type != ITEM_WEAPON)
                                stc("They're not wielding anything to disarm!!#n\n\r", ch);
                        return;
                }

                if (IS_IMMUNE(victim, IMM_DISARM))
                {
                        successchance = number_range(1, 75);
                }

                else
                {
                        successchance = number_range(1, 100);
                }

                if (successchance < 50)
                {
                        stc("#0You try to preform a dirty trick, but fail!#n\n\r", ch);
                        ch->move -= 500;
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        return;
                }

                if (successchance >= 50)
                {
                        sprintf(buf,
                                "#R$n #0slices $s #7dagger #0sneakily accross your wrist, causing you too lose your grip!#n\n\r");
                        act(buf, ch, NULL, victim, TO_VICT);
                        sprintf(buf,
                                "#0You sneakily slice your dagger across #R$N's #0wrist, causing them to lose their grip!#n\n\r");
                        act(buf, ch, NULL, victim, TO_CHAR);

                        obj_from_char(obj);
                        obj_to_room(obj, victim->in_room);
                        ch->move -= 2500;
                        hurt_person(ch, victim, 250);
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        return;
                }
        }

        else if (randnum == 2)  //<----Trip
        {
                if (IS_AFFECTED(victim, AFF_FLYING))
                        return;

                successchance = number_range(1, 100);

                if (successchance < 50)
                {
                        stc("#0You attempt a dirty trick but fail!#n\n\r",
                            ch);
                        ch->move -= 500;
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        return;
                }
                if (successchance >= 50 && victim->wait == 0)
                {
                        sprintf(buf,
                                "#R$n #0kicks your shins, causing your legs to buckle!#n\n\r");
                        act(buf, ch, NULL, victim, TO_VICT);
                        sprintf(buf,
                                "#0You kick #R$N #0in the shins, causing $S legs to buckle!#n\n\r");
                        act(buf, ch, NULL, victim, TO_CHAR);
                        sprintf(buf,
                                "#R$n #0kicks #R$N #0in the shins, causing $S legs to buckle!#n\n\r");
                        act(buf, ch, NULL, victim, TO_NOTVICT);
                        ch->move -= 2500;
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        hurt_person(ch, victim, 500);
                        WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
                        victim->position = POS_RESTING;
                        return;
                }
        }

        else if (randnum == 3)  //<---Foot Stomp
        {
                if (IS_AFFECTED(victim, AFF_FLYING))
                        return;

                successchance = number_range(1, 100);

                if (successchance < 50)
                {
                        stc("#0You attempt to preform a Dirty Trick but fail!#n\n\r", ch);
                        ch->move -= 500;
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        return;
                }

                if (successchance >= 50)
                {
                        sprintf(buf, "#R$n #0stomps on your feet! OW!#n\n\r");
                        act(buf, ch, NULL, victim, TO_VICT);
                        sprintf(buf, "#0You stomp on #R$N's #0feet!#n\n\r");
                        act(buf, ch, NULL, victim, TO_CHAR);
                        sprintf(buf, "#R$n #0stomps on #R$N's #0feet!#n\n\r");
                        act(buf, ch, NULL, victim, TO_NOTVICT);
                        ch->move -= 2500;
                        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                        hurt_person(ch, victim, 500);
                        WAIT_STATE(victim, 3 * PULSE_VIOLENCE);
                        return;
                }
        }

        else if (randnum == 4)  //<---Hilt Bash
        {
                successchance = number_range(1, 100);
                {
                        if (successchance < 50)
                        {
                                stc("#0You attempt a Dirty Trick, but fail!#n\n\r", ch);
                                ch->move -= 500;
                                WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                                return;
                        }
                        if (successchance >= 50)
                        {
                                sprintf(buf,
                                        "#R$n BASHES #0his jeweled dagger hilt into your face! You reel back, stunned!#n\n\r");
                                act(buf, ch, NULL, victim, TO_VICT);
                                sprintf(buf,
                                        "#0You BASH your dagger hilt into #R$N's #0face!#n\n\r");
                                act(buf, ch, NULL, victim, TO_CHAR);
                                sprintf(buf,
                                        "#R$n #RBASHES #0his dagger hilt into #R$N's #0face, causing them to reel back stunned!#n\n\r");
                                act(buf, ch, NULL, victim, TO_NOTVICT);
                                act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
                                act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
                                SET_BIT(victim->loc_hp[LOC_HEAD],
                                        BROKEN_NOSE);
                                ch->move -= 2500;
                                hurt_person(ch, victim, 1000);
                                WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                                WAIT_STATE(victim, 1 * PULSE_VIOLENCE);
                                victim->position = POS_STUNNED;
                                return;
                        }
                        return;
                }
        }
}

void do_fightdance(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT1))
        {
                send_to_char("#0You need combat one for this.#n\n\r", ch);
                return;
        }
	if (IS_SET(ch->newbits, NEW_FIGHTDANCE)) {
	send_to_char("You will no longer dance The Dance of Lloth.\n\r", ch );
	REMOVE_BIT(ch->newbits, NEW_FIGHTDANCE);
	}
	else {
	send_to_char("You will start dancing The Dance of Lloth while fighting.\n\r", ch );
	SET_BIT(ch->newbits, NEW_FIGHTDANCE);
	}

	return;
}




void do_unnerve(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

        if (!IS_SPHERE(ch, SPHERE_COMBAT3))
        {
                send_to_char("#0You need combat three for this.#n\n\r", ch);
                return;
        }

    if (argument[0] == '\0')
    {
	stc("Who do you wish to unnerve?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room(ch, argument) ) == NULL)
    {
	stc("They aren't here.\n\r", ch);
	return;
    }

    WAIT_STATE(ch, 7);
    do_say(ch,"#1Xeus Dominus Mortai!#n");
    do_stance(victim, "");
}


void do_spikes(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg1[MAX_INPUT_LENGTH];
        char      arg2[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        int       cost = 0;
        int       spikes = 0;
        int       urin = 0;
        int       urin_counter = 0;
        int       dam;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT3))
        {
                send_to_char("#0You need combat three for this.#n\n\r", ch);
                return;
        }


        if (arg1[0] == '\0' && arg2[0] == '\0')
        {
                stc("#R+#Y------#G------#g---#G------#Y------#R+#n\n\r", ch);
                sprintf(buf, "    #gY#Gou have #Y%d #Gspikes!#n\n\r",
                        ch->pcdata->stats[DRACONIAN_SPIKES]);
                stc(buf, ch);
                stc("#R+#Y------#G------#g---#G------#Y------#R+#n\n\r", ch);
                return;
        }

        if (!str_cmp(arg1, "release"))
        {
                if (ch->fighting == NULL)
                {
                        stc("Not unless fighting!\n\r", ch);
                        return;
                }

                if (ch->pcdata->stats[DRACONIAN_SPIKES] < 1)
                {
                        stc("You have no spikes to shoot!\n\r", ch);
                        return;
                }
                victim = ch->fighting;
                dam = number_range(ch->pcdata->stats[DRACONIAN_SPIKES] * 20,
                                   ch->pcdata->stats[DRACONIAN_SPIKES] * 40);

                printf(buf,
                        "#GYour spikes shoot from your body, #Rimpaling #y$N!  #0[#R%d#0]#n",
                        dam);
                act(buf, ch, NULL, victim, TO_CHAR);
                printf(buf,
                        "#GA plethora of sharp spikes shoot from #y$n's #Gbody, #Rimpaling #Gyou!  #0[#R%d#0]#n",
                        dam);
                act(buf, ch, NULL, victim, TO_VICT);
                act("#g$n #Gshoots a plethora of sharp spikes from $s body at #y$N, #Rimpaling #Gthem!#n", ch, NULL, victim, TO_NOTVICT);

                hurt_person(ch, victim, dam);
                ch->pcdata->stats[DRACONIAN_SPIKES] = 0;
                return;
        }

        else if (!str_cmp(arg1, "grow"))
        {
                if (arg2[0] == '\0')
                {
                        stc("#gSyntax: #Gspikes grow <amount>#n\n\r", ch);
                        return;
                }

                if (ch->position <= POS_FIGHTING)
                {
                        stc("You must be idle and standing to do this!\n\r",
                            ch);
                        return;
                }

                if (ch->fight_timer > 0)
                {
                        stc("Not with a fight timer!\n\r", ch);
                        return;
                }

                if (arg2[0] == '\0')
                {
                        stc("#GHow many spikes do you wish to grow?#n\n\r",
                            ch);
                        return;
                }

                if (!is_number(arg2))
                {
                        stc("Please enter a numeric value between 1 and 200!\n\r", ch);
                        return;
                }

                if (ch->pcdata->stats[DRACONIAN_SPIKES] >= 200)
                {
                        stc("You cannot grow any more spikes!\n\r", ch);
                        return;
                }

                if (is_number(arg2))
                {

                        spikes = atoi(arg2);

                        cost = spikes * 10000;

                        if (spikes > 200 || spikes < 1)
                        {
                                stc("Please enter a numeric value between 1 and 200!\n\r", ch);
                                return;
                        }

                        for (urin = 0; urin < spikes; urin++)
                        {
                                if (ch->exp >= 10000
                                    && ch->pcdata->stats[DRACONIAN_SPIKES] <
                                    200)
                                {
                                        ch->exp -= 10000;
                                        ch->pcdata->stats[DRACONIAN_SPIKES]++;
                                        urin_counter += 1;
                                }
                        }

                        if (urin_counter == 0)
                        {
                                send_to_char
                                        ("You need more exp to gain any more spikes.\n\r",
                                         ch);
                        }
                        if (urin_counter == 1)
                        {
                                send_to_char("#GYou gain #y1 #Gspike.#n\n\r",
                                             ch);
                        }
                        if (urin_counter > 1)
                        {
                                printf(buf,
                                        "#GYou gain #y%d #Gspikes.#n\n\r",
                                        urin_counter);
                                send_to_char(buf, ch);
                        }
                        return;
                }


        }
        else
        {
                stc("Syntax: spikes grow <amount>\n\r", ch);
                stc("        spikes release\n\r", ch);
                stc("        spikes\n\r", ch);
                return;
        }
}



void do_shatter (CHAR_DATA * ch, char *argument)
{

  char buf[MAX_STRING_LENGTH];
  int damage;
  int damage2;
  int oldl;
  CHAR_DATA *victim;

  if (IS_NPC (ch))
    return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT4))
        {
                send_to_char("#0You need combat four for this.#n\n\r", ch);
                return;
        }

  if (ch->fighting == NULL || IS_NPC ((victim = ch->fighting)))
    {
      stc ("Only against players in combat!\n\r", ch);
      return;
    }
  if (number_range (1,4) == 1)
    {
      stc ("You failed!\n\r", ch);
      WAIT_STATE (ch, 12);
      return;
    }
  damage = number_range (1000, 5000);
  ch->hit -= damage;
  damage2 = damage *2;
  victim->hit -= damage2;
  sprintf (buf,
	   "You burst into a pile of bones as your enemy attacks to avoid them! [#R%d#n]\n\r",
	   damage);
  stc (buf, ch);
  sprintf (buf,
	   "%s bursts into a pile of bones from your attack! [#7%d#n]\n\r",
	   ch->pcdata->switchname, damage2);
  stc (buf, victim);
  hurt_person (ch, ch, damage);
  hurt_person (ch, victim, damage);
  oldl = ch->level;
  ch->level = 12;
  do_peace (ch, "");
  ch->level = oldl;
  WAIT_STATE (ch, 10);
  WAIT_STATE (victim, 20);

}


void do_desanc (CHAR_DATA *ch, char *argument)
{
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int sn, random, level;

  argument=one_argument(argument,arg);
  level = ch->spl[RED_MAGIC]/4;

  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT4))
        {
                send_to_char("#0You need combat four for this.#n\n\r", ch);
                return;
        }

  if (ch->mana < 1500)
  {
    send_to_char("You need more mana.\n\r",ch);
    return;
  }
  if (arg[0] == '\0' && ch->fighting != NULL) victim = ch->fighting;
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch );
    return;
  }
  if(is_safe(ch, victim)) return;

  random = number_range(1,3);
  if (random == 1) sn = skill_lookup("desanct");
  else if (random >= 2) 
  {
    send_to_char("Failed.\n\r",ch);
    return;
  }

//    do_force(ch,"all save");

  else sn = 0;
  if (sn  > 0) (*skill_table[sn].spell_fun) (sn,level,ch,victim);
  ch->mana -= 1500;
  WAIT_STATE(ch,8);
  return;
}


void do_shouldercharge( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT2))
        {
                send_to_char("You need combat two for this.\n\r", ch);
                return;
        }

    if (!(arg2[0] == '\0') || ch->fighting != NULL)
    {
      if (arg2[0] == '\0') victim = ch->fighting;
      else if ((victim = get_char_room(ch, arg2)) == NULL)
      {
        send_to_char("They are not here.\n\r", ch );
        return;
      }
}
      if (is_safe(ch, victim)) return;
  act("You take a step back, then launch yourself straight at $N...",ch,NULL,victim,TO_CHAR);
  act("$n takes a step back, then launches $mself straight at you...",ch,NULL,victim,TO_VICT);
  act("$n takes a step back, then launches $mself straight at $N...",ch,NULL,victim,TO_NOTVICT);
  if ( number_range(1,4) == 4 )
  {
    act("$N sidesteps, leaving you wide open!",ch,NULL,victim,TO_CHAR);
    act("You sidestep, leaving $n wide open!",ch,NULL,victim,TO_VICT);
    act("$N sidesteps, leaving $n wide open!",ch,NULL,victim,TO_NOTVICT);
    if ( number_range(1,2) == 1 )
    {
      act("$N takes the opportunity to strike back!",ch,NULL,victim,TO_CHAR);
      act("You take the opportunity to strike back!",ch,NULL,victim,TO_VICT);
      act("$N takes the opportunity to strike back!",ch,NULL,victim,TO_NOTVICT);
      multi_hit(victim,ch,TYPE_UNDEFINED);
    }
    else
    {
      act("You recover yourself before $N can strike back.",ch,NULL,victim,TO_CHAR);
      act("$n recovers $mself before you can strike back.",ch,NULL,victim,TO_VICT);
      act("$n recovers $mself before $N can strike back.",ch,NULL,victim,TO_NOTVICT);
    }
    return;
  }
  if ( number_range(1,4) == 4 )
  {
      if ( IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG) )
      {
	act("Your body hits nothing but air.",ch,NULL,victim,TO_CHAR);
	act("$n hits nothing but air.",ch,NULL,victim,TO_ROOM);
	return;
      }
      act("You collide with $N, tearing away $S legs.",ch,NULL,victim,TO_CHAR);
      act("$n collides with you, tearing away your legs.",ch,NULL,victim,TO_VICT);
      act("$n collides with $N, tearing away $S legs.",ch,NULL,victim,TO_NOTVICT);
      if ( !IS_LEG_L(victim,LOST_LEG) )
      {
	SET_BIT(victim->loc_hp[4], LOST_LEG);
	if ( IS_BLEEDING(victim, BLEEDING_LEG_L) ) REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
	if ( IS_BLEEDING(victim, BLEEDING_FOOT_L) ) REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
      }
      if ( !IS_LEG_R(victim,LOST_LEG) )
      {
	SET_BIT(victim->loc_hp[5], LOST_LEG);
	if ( IS_BLEEDING(victim, BLEEDING_LEG_R) ) REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
	if ( IS_BLEEDING(victim, BLEEDING_FOOT_R) ) REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
      }
      if ( (obj = get_eq_char(victim,WEAR_LEGS)) != NULL ) take_item(victim,obj);
      if ( (obj = get_eq_char(victim,WEAR_FEET)) != NULL ) take_item(victim,obj);
  }

    else
    {
      send_to_char("Bash whom?\n\r",ch);
      return;
    }
      WAIT_STATE(victim, 24);
      WAIT_STATE(ch, 18);
      return;
    }




void do_bomuzite(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg1[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg1);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT2))
        {
                send_to_char("You need combat two for this.\n\r", ch);
                return;
        }
        if (ch->in_room != NULL)
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
                {
                        send_to_char
                                ("You cannot use this power in a safe room.\n\r",
                                 ch);
                        return;
                }
        }


        if (arg1[0] == '\0')
        {
                send_to_char("Bomb who?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, arg1)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (ch == victim)
        {
                send_to_char("Not on yourself!\n\r", ch);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char("Not while fighting!\n\r", ch);
                return;
        }

        if (ch->move < 500)
        {
                send_to_char("You don't have enough movement points.\n\r",
                             ch);
                return;
        }

        if (victim->in_room == ch->in_room)
        {
                act("You toss your bomb onto the floor and put $N to sleep.",
                    ch, NULL, victim, TO_CHAR);
                act("$n tosses a bomb onto the floor.  You feel sleepy.", ch,
                    NULL, victim, TO_VICT);
                victim->position = POS_SLEEPING;
                ch->move -= 500;
                WAIT_STATE(ch, 1);
                return;
        }

        return;
}



void do_kakusu(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT1))
        {
                send_to_char("You need combat one for superior hideing.\n\r", ch);
                return;
        }

        if (ch->move < 500)
        {
                send_to_char
                        ("You don't have 500 move to activate your power.\n\r",
                         ch);
                return;
        }

        if (ch->fight_timer > 0)
        {
                send_to_char("Not until your fight timer expires.\n\r", ch);
                return;
        }

        if (IS_SET(ch->act, AFF_HIDE))
        {
                REMOVE_BIT(ch->act, AFF_HIDE);
                act("$n appears from the shadows.", ch, NULL, NULL, TO_ROOM);
                send_to_char("You appear from the shadows.\n\r", ch);
        }
        else
        {
                act("$n disappears into the shadows.", ch, NULL, NULL,
                    TO_ROOM);
                send_to_char("You disappear into the shadows.\n\r", ch);
                ch->move -= 500;
                SET_BIT(ch->act, AFF_HIDE);
        }
        return;
}



void do_ironclaw(CHAR_DATA *ch, char *argument) {

	if (IS_NPC(ch)) return;

        if (!IS_SPHERE(ch, SPHERE_COMBAT1))
        {
                stc("You need the combat 1 sphere for this.\n\r", ch);
                return;
        }

	if (IS_VAMPAFF(ch, VAM_CLAWS)) 
	{
	send_to_char("You remove the IronClaws from your wrists.\n\r", ch );
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
	return;
	}
	else
	{
	send_to_char("You attach IronClaws to your wrists.\n\r", ch );
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
	return;
	}
}


void do_leech(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim = NULL;
        char      buf[MAX_STRING_LENGTH];

	if (!IS_SPHERE(ch, SPHERE_COMBAT1))
	{
	        stc("You need the combat 1 sphere for this.\n\r", ch);
                return;
	}
        if (argument[0] == '\0')
        {
                stc("Who's life do you wish to leech off of?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, argument)) == NULL)
        {
                stc("They aren't here.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, 10);

        act("$n stares intently at $N.", ch, NULL, victim, TO_NOTVICT);
        act("You stare intently at $N.", ch, NULL, victim, TO_CHAR);
        act("$n stares intently at you.", ch, NULL, victim, TO_VICT);

        if (victim->hit >= 1000)
        {
                sh_int    power;
                sh_int    dam;

                power = 20;
                dam = dice(power, power * 2);
                if (dam > 300)
                        dam = 300 + (dam / 10);
                hurt_person(ch, victim, dam);
                ch->hit += dam;
                if (ch->hit >= ch->max_hit + 500)
                        ch->hit = ch->max_hit + 500;
                printf(buf, "#RYou absorb %d hitpoints.#n\n\r", dam);
                stc(buf, ch);
                printf(buf, "#R%s absorbed %d of your hitpoints!#n\n\r",
                        ch->name, dam);
                stc(buf, victim);
        }
        else
                stc("Nothing seemed to happen.\n\r", ch);
        return;

}

/* you really must take a look at this Jobo */
void do_entomb(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj = NULL;
        OBJ_DATA *objc;

        if (!IS_SPHERE(ch, SPHERE_COMBAT3))
        {
                stc("You need the combat 3 sphere for this.\n\r", ch);
                return;
        }

        if (ch->in_room->sector_type == SECT_AIR)
        {
                send_to_char("This ability cannot be used in the air.\n\r",
                             ch);
                return;
        }

        if (!TIME_UP(ch, TIMER_ENTOMB) && ch->level < 12)
        {
                send_to_char
                        ("This ability can only be used once every 12 hours.\n\r",
                         ch);
                return;
        }

        if ((objc = get_obj_list(ch, "walln", ch->in_room->contents)) != NULL)
                objc = NULL;
        else
        {
                obj = create_object(get_obj_index(30043), 0);
                printf(buf,
                        "A wall of ice is here, blocking your exit north.");
                free_string(obj->description);
                obj->description = str_dup(buf);
                obj_to_room(obj, ch->in_room);
                obj->timer = 5;
                obj->item_type = ITEM_WALL;
        }
        if ((objc = get_obj_list(ch, "walls", ch->in_room->contents)) != NULL)
                objc = NULL;
        else
        {
                obj = create_object(get_obj_index(30044), 0);
                printf(buf,
                        "A wall of ice is here, blocking your exit south.");
                free_string(obj->description);
                obj->description = str_dup(buf);
                obj_to_room(obj, ch->in_room);
                obj->timer = 5;
                obj->item_type = ITEM_WALL;
        }
        if ((objc = get_obj_list(ch, "walle", ch->in_room->contents)) != NULL)
                objc = NULL;
        else
        {
                obj = create_object(get_obj_index(30045), 0);
                printf(buf,
                        "A wall of ice is here, blocking your exit east.");
                free_string(obj->description);
                obj->description = str_dup(buf);
                obj_to_room(obj, ch->in_room);
                obj->timer = 5;
                obj->item_type = ITEM_WALL;
        }
        if ((objc = get_obj_list(ch, "wallw", ch->in_room->contents)) != NULL)
                objc = NULL;
        else
        {
                obj = create_object(get_obj_index(30046), 0);
                printf(buf,
                        "A wall of ice is here, blocking your exit west.");
                free_string(obj->description);
                obj->description = str_dup(buf);
                obj_to_room(obj, ch->in_room);
                obj->timer = 5;
                obj->item_type = ITEM_WALL;
        }

        ch->hit += ch->max_hit * 0.1;
        if (ch->hit > ch->max_hit)
                ch->hit = ch->max_hit;
        SET_TIMER(ch, TIMER_ENTOMB, 12);

        act("$n calls forth sheets of ice, blocking your path in every direction.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You call forth sheets of ice to entrap your prey.\n\r",
                     ch);
        send_to_char("You grin wickedly.\n\r", ch);
        do_say(ch, "Prepare to die! Muahahahaha.");
        WAIT_STATE(ch, 10);
        return;
}



void do_bladejump(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        argument = one_argument(argument, arg);

        if (!IS_SPHERE(ch, SPHERE_COMBAT1))
        {
                stc("You need combat one for this.\n\r",ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("BladeJump to whom?\n\r", ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("You cannot find them.\n\r", ch);
                return;
        }

        if (ch->move < 1000)
        {
                stc("You don't have enough movement points to bladejump to them.\n\r", ch);
                return;
        }

        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("You can't find it's room.\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Your body can't go there.\n\r", ch);
                return;
        }
        if (IS_SET(victim->act, ACT_NOTRAVEL))
        {
                send_to_char("No Can Do.\n\r", ch);
                return;
        }
        if (ch == victim)
        {
                send_to_char("But you're already at yourself!\n\r", ch);
                return;
        }

        if (room_is_private(victim->in_room))
        {
                send_to_char("That room is private right now.\n\r", ch);
                return;
        }
        if (!IS_NPC(victim) && ch->move < victim->max_move)
        {
                if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
                {
                        send_to_char("They are hiding from you.\n\r", ch);
                        return;
                }
                else
                        send_to_char
                                ("You need more move than you're opponent.\n\r",
                                 ch);
        }
        if (victim->in_room == ch->in_room)
        {
                send_to_char("But you're already there!\n\r", ch);
                return;
        }

        act("You jump into the clouds", ch, NULL, victim, TO_CHAR);
        act("$n jumps into the clouds", ch, NULL, victim, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        ch->move -= 1000;
        act("$n jumps out of the clouds and lands infront of $N.", ch, NULL,
            victim, TO_NOTVICT);
        act("$n jumps out of the clouds and lands infront of you.", ch, NULL,
            victim, TO_VICT);
        do_look(ch, "auto");
        return;
}



void do_superberserk(CHAR_DATA *ch, char *argument)
{
  char arg [MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if ( ch->level < skill_table[gsn_berserk].skill_level)
  {
    send_to_char( "You are not wild enough to go berserk.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, 4);
  if ( number_percent( ) > ch->pcdata->learned[gsn_berserk] )
  {
    act("You rant and rave, but nothing much happens.",ch,NULL,NULL,TO_CHAR);
    act("$n gets a wild look in $s eyes, but nothing much happens.",ch,NULL,NULL,TO_ROOM);
    return;
  }
	if (!IS_SPHERE(ch, SPHERE_COMBAT2))
	{
                stc("You need combat one for this.\n\r",ch);
                return;
        }


  act("#0You Roar with Rage and #7B#wE#7R#wS#7E#wR#7K#0 into a Barbarian style Frenzy.#n",ch,NULL,NULL,TO_CHAR);
  act("#0$n Roars with Rage and #7B#wE#7R#wS#7E#wR#7K#wS#0 into a Barbarian style Frenzy!#n",ch,NULL,NULL,TO_ROOM);
        for (vch = char_list; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next;
                if (vch->in_room == NULL)
                        continue;
                if (!IS_NPC(vch)) continue;
                if (ch == vch)
                {
                        act("You throw back your head and howl with rage!",
                            ch, NULL, NULL, TO_CHAR);
                        continue;
                }
                if (!IS_NPC(vch) && vch->pcdata->chobj != NULL)
                        continue;
                if (!IS_NPC(vch))
                {
//                        if (vch->in_room == ch->in_room)
                        if (vch->in_room->area == ch->in_room->area)
                                send_to_char
                                        ("You hear a Warcry close by!\n\r",
                                         vch);
                        else if (!CAN_PK(vch))
                                continue;
                }
                if (vch->in_room == ch->in_room && can_see(ch, vch))
                {
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                        if (vch == NULL || vch->position <= POS_STUNNED)
                                continue;
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                }
        }
return;
}



