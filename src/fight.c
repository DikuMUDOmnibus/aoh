/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define MONK_AUTODROP 12
#define FIGHTSTYLE_AUTODROP 0


/*
 * Local functions.
 */
void gain_level (CHAR_DATA * ch, int gain);
void  autodrop            args (( CHAR_DATA *ch ));
void  autodrop2           args (( CHAR_DATA *ch ));
bool  check_dodge         args (( CHAR_DATA *ch, CHAR_DATA *victim, int dt ));
bool  check_parry         args (( CHAR_DATA *ch, CHAR_DATA *victim, int dt ));
bool  check_shieldblock   args (( CHAR_DATA *ch, CHAR_DATA *victim, int dt ));
void  dam_message         args (( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int hit_loc));
void  death_cry           args (( CHAR_DATA *ch ));
void  group_gain          args (( CHAR_DATA *ch, CHAR_DATA *victim ));
int   xp_compute          args (( CHAR_DATA *gch, CHAR_DATA *victim ));
bool  can_counter         args (( CHAR_DATA *ch ));
bool  can_bypass          args (( CHAR_DATA *ch, CHAR_DATA *victim ));
int   number_attacks      args (( CHAR_DATA *ch, CHAR_DATA *victim ));
int   dambonus            args (( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int stance ));
void  decap_message       args (( CHAR_DATA *ch, CHAR_DATA *victim ));
bool  pole_special	  (CHAR_DATA * ch, CHAR_DATA * victim);
int   axe_special	  (CHAR_DATA * ch, CHAR_DATA * victim, int dam);


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update(void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *victim;
  CHAR_DATA *rch;
  CHAR_DATA *rch_next;
  CHAR_DATA *mount;
  int chance;

  chance = number_percent();

  for (ch = char_list; ch != NULL; ch = ch->next)
  {
    ch_next = ch->next;

    if (!IS_NPC(ch))
    {
      if (ch->fight_timer > 0) ch->fight_timer--;
    }

    if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
      continue;
    if (!IS_NPC(ch) && !IS_NPC(victim) && !is_safe(ch, victim) && !is_safe(victim, ch))
    {
      if (ch->fight_timer < 9) ch->fight_timer = 10;
      else if (ch->fight_timer < 25) ch->fight_timer += 3;
    }
    if (IS_AWAKE(ch) && IS_AWAKE(victim) && ch->in_room == victim->in_room)
      multi_hit(ch, victim, TYPE_UNDEFINED);
    else
      stop_fighting(ch, FALSE);
    if ((victim = ch->fighting) == NULL)
      continue;
    /*   
     * Fun for the whole family!
     */
    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
      rch_next = rch->next_in_room;
      if (IS_AWAKE(rch) && rch->fighting == NULL)
      {
        /*
         * Mount's auto-assist their riders and vice versa.
         */
        if ((mount = rch->mount) != NULL)
        {
          if (mount == ch)
            multi_hit(rch, victim, TYPE_UNDEFINED);
          continue;
        }
        /*
         * PC's auto-assist others in their group.
         */
        if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM))
        {
          if ((!IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM)) && is_same_group(ch, rch))
            multi_hit( rch, victim, TYPE_UNDEFINED);
          continue;
        }
        /*
         * NPC's assist NPC's of same type or 12.5% chance regardless.
         */
        if (IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM))
        {
          if (rch->pIndexData == ch->pIndexData || number_bits(3) == 0)
          {
            CHAR_DATA *vch;
            CHAR_DATA *target;
            int number;

            target = NULL;
            number = 0;
            for (vch = ch->in_room->people; vch; vch = vch->next)
            {
              if (can_see(rch, vch) && is_same_group(vch, victim) && number_range(0, number) == 0)
              {
                target = vch;
                number++;
              }
            }
            if (target != NULL)
              multi_hit(rch, target, TYPE_UNDEFINED);
          }
        }
      }
    }
  }
  return;
}

/*
 * Do one group of attacks.
 */
void multi_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
  OBJ_DATA *wield;
  OBJ_DATA *wield1;
  OBJ_DATA *wield2;
  OBJ_DATA *wield3;
  OBJ_DATA *wield4;
        char      buf1[MAX_STRING_LENGTH];
        char      buf2[MAX_STRING_LENGTH];
  int sn, level, chance, unarmed, maxcount, countup, wieldie;
  int wieldorig = 0;
  int wieldtype = 0;
  int dtype;

  if (ch->position < POS_SLEEPING)
    return;


			/*
			 * Swords will very rarely get in an extra round.
			 */

			if ((dtype == WPN_BROADSWORD || dtype == WPN_LONGSWORD || dtype == WPN_BASTARDSWORD)
				&& ch->wait <= 0)
			{
				int chance;
				CHAR_DATA * was_fighting;
				CHAR_DATA * vch;

			if (dtype == WPN_BROADSWORD)   chance = ch->wpn[3] / 100;
			if (dtype == WPN_LONGSWORD)    chance = ch->wpn[2] / 100;
			if (dtype == WPN_BASTARDSWORD) chance = ch->wpn[12] / 100;


				/*
				 * Penalty if the opponent can see.
				 */
				if (can_see (victim, ch))
					chance = chance * 3 / 5;

				for (vch = ch->in_room->people; vch != NULL;
						vch = vch->next_in_room)
				{
					if (vch->fighting == ch
						|| is_same_group (vch->fighting, ch))
					{
						if (number_percent () < chance)
						{
							was_fighting = ch->fighting;
							ch->fighting = vch;

							multi_hit (ch, vch, TYPE_UNDEFINED);

							ch->fighting = was_fighting;
							WAIT_STATE (ch, PULSE_VIOLENCE);
							break;
						}
					}
				}
		}

	
	/*
	 * Extra hit for daggers.
	 */
	if (dtype == WPN_DAGGER || dtype == WPN_KNIFE)
	{
		if (dtype == WPN_DAGGER)  chance = ch->wpn[18] / 65;
		if (dtype == WPN_KNIFE)   chance = ch->wpn[6] / 65;
	}

        if (dt == gsn_darktendrils && IS_SPHERE(ch, SPHERE_ENHANCEMENT))
        {
                one_hit(ch, victim, dt, 1);
                one_hit(ch, victim, dt, 1);
                one_hit(ch, victim, dt, 1);
                one_hit(ch, victim, dt, 1);
                return;
        }
  if (dt == gsn_claws)
  {
    one_hit( ch, victim, dt, 1 );
    one_hit( ch, victim, dt, 1 );
    return;
  }
  if (dt == gsn_tentacle)
  {
    int x;

    act("You shoot forth a shower of tentacles to destroy $N.", ch, NULL, victim, TO_CHAR);
    act("$n shoots forth a shower of tentacles aiming straight at you.", ch, NULL, victim, TO_VICT);
    act("$n shoots forth a shower of tentacles striking $N.", ch, NULL, victim, TO_NOTVICT);

    x = number_range(2, 5);
    while (--x >= 0) one_hit(ch, victim, dt, 1);
    return;
  }
  if ( dt == gsn_fangs )
  {
    one_hit( ch, victim, dt, 1 );
    return;
  } 
  if ( dt== gsn_quills)
  {
    {
      one_hit(ch,victim,dt,1);
      one_hit(ch,victim,dt,1);
      one_hit(ch,victim,dt,1);
    }
    one_hit(ch,victim,dt,1);
    return;
  }


  wield1 = get_eq_char(ch, WEAR_WIELD);
  wield2 = get_eq_char(ch, WEAR_HOLD);
  wield3 = get_eq_char(ch, WEAR_THIRD);
  wield4 = get_eq_char(ch, WEAR_FOURTH);
  if (wield1 != NULL && wield1->item_type == ITEM_WEAPON)
    wieldorig = 1;
  if (wield2 != NULL && wield2->item_type == ITEM_WEAPON)
    wieldorig += 2;
  if (wield3 != NULL && wield3->item_type == ITEM_WEAPON)
    wieldorig += 4;
  if (wield4 != NULL && wield4->item_type == ITEM_WEAPON)
    wieldorig += 8;
  wieldtype = wieldorig;
  wieldie = number_range(1, 4);
  if (wieldorig == 15)
  {
    if (wieldie == 2)
      wieldtype = 8;
    else
      wieldtype = 4;
  }
  if (wieldorig == 14)
  {
    if (wieldie == 2)
      wieldtype = 8;
    else if (wieldie == 1)
      wieldtype = 4;
    else if (wieldie == 3)
      wieldtype = 2;
    else
      wieldtype = 1;
  }
  if (wieldorig == 13)
  {
    if (wieldie == 1)
      wieldtype = 8;
    else if (wieldie == 2)
      wieldtype = 4;
    else
      wieldtype = 1;
  }
  if (wieldorig == 12)
  {
    if (wieldie == 1)
      wieldtype = 8;
    else
      wieldtype = 4;
  }
  if (wieldorig == 11)
  {
    if (wieldie == 1)
      wieldtype = 8;
    if (wieldie == 2)
      wieldtype = 2;
    else
      wieldtype = 1;
  }
  if (wieldorig == 10)
  {
    if (wieldie == 1)
      wieldtype = 8;
    else
      wieldtype = 2;
  }
  if (wieldorig == 9)
  {
    if (wieldie == 1)
      wieldtype = 8;
    else
      wieldtype = 1;
  }
  if (wieldorig == 8)
    wieldtype = 8;
  if (wieldorig == 7)
  {
    if (wieldie == 1)
      wieldtype = 4;
    else if (wieldie == 2)
      wieldtype = 2;
    else
      wieldtype = 1;
  }
  if (wieldorig == 6)
  {
    if (wieldie == 1 || wieldie == 2)
      wieldtype = 2;
    else
      wieldtype = 4;
  }
  if (wieldorig == 5)
  {
    if (wieldie == 1)
      wieldtype = 4;
    else
      wieldtype = 1;
  }
  if (wieldorig == 4)
    wieldtype = 4;
  if (wieldorig == 3)
  {
    if (wieldie == 2 || wieldie == 4)
      wieldtype = 2;
    else
      wieldtype = 1;
  }
  if (wieldorig == 2)
    wieldtype = 2;
  if (wieldorig == 1)
    wieldtype = 1;

  if (wieldtype == 8)
    wield = wield4;
  else if (wieldtype == 4)
    wield = wield3;
  else if (wieldtype == 2)
    wield = wield2;
  else
    wield = wield1;

  if (!IS_NPC(ch) && ch->stance[0] > 0 && number_percent() == 5)
  {
    int stance = ch->stance[0];

    if (ch->stance[stance] >= 200)
    {
      special_move(ch, victim);
      return;
    }
  }

  unarmed = 0;

  one_hit(ch, victim, dt, wieldtype);

  if (victim == NULL || victim->position != POS_FIGHTING)
    return;

  /* Only want one spell per round from spell weapons...otherwise it's 
   * too powerful, and would unbalance player killing (as this is a PK mud).
   */

  if (dt == TYPE_UNDEFINED)
  {
    dt = TYPE_HIT;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
    {
      dt += wield->value[3];
      if (wield->value[0] >= 1)
      {
        /* Look, just don't ask...   KaVir */
        if (wield->value[0] >= 1000)
          sn = wield->value[0] - ((wield->value[0] / 1000) * 1000);
        else
          sn = wield->value[0];
        if (victim->position == POS_FIGHTING && sn != 0)
          (*skill_table[sn].spell_fun) (sn, wield->level, ch, victim);
      }
    }
    if (IS_NPC(ch) && ch->spec_fun)
      (*ch->spec_fun) (ch, "midround");
  }
  if (victim == NULL || victim->position != POS_FIGHTING)
    return;
  if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_garotte || dt == gsn_headbutt || dt == gsn_circle)
    return;
  maxcount = number_attacks(ch, victim);
  if (!IS_NPC(ch))
  {
    chance = 0;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
    {
      int tempnum = wield->value[3];

      chance = (ch->wpn[tempnum]) * 0.5;
    }
    else
      chance = (ch->wpn[0]) * 0.5;
    if (number_percent() <= chance)
      maxcount += 1;
  }
  if (wieldorig == 3)
    maxcount += 1;
  for (countup = 0; countup <= maxcount; countup++)
  {
    wieldtype = wieldorig;
    if (wieldorig == 15)
    {
      if (wieldie == 2)
        wieldtype = 8;
      else
        wieldtype = 4;
    }
    if (wieldorig == 14)
    {
      if (wieldie == 2)
        wieldtype = 8;
      else if (wieldie == 1)
        wieldtype = 4;
      else if (wieldie == 3)
        wieldtype = 2;
      else
        wieldtype = 1;
    }
    if (wieldorig == 13)
    {
      if (wieldie == 1)
        wieldtype = 8;
      else if (wieldie == 2)
        wieldtype = 4;
      else
        wieldtype = 1;
    }
    if (wieldorig == 12)
    {
      if (wieldie == 1)
        wieldtype = 8;
      else
        wieldtype = 4;
    }
    if (wieldorig == 11)
    {
      if (wieldie == 1)
        wieldtype = 8;
      if (wieldie == 2)
        wieldtype = 2;
      else
        wieldtype = 1;
    }
    if (wieldorig == 10)
    {
      if (wieldie == 1)
        wieldtype = 8;
      else
        wieldtype = 2;
    }
    if (wieldorig == 9)
    {
      if (wieldie == 1)
        wieldtype = 8;
      else
        wieldtype = 1;
    }
    if (wieldorig == 7)
    {
      if (wieldie == 1)
        wieldtype = 4;
      else if (wieldie == 2)
        wieldtype = 2;
      else
        wieldtype = 1;
    }
    if (wieldorig == 6)
    {
      if (wieldie == 1 || wieldie == 2)
        wieldtype = 2;
      else
        wieldtype = 4;
    }
    if (wieldorig == 5)
    {
      if (wieldie == 1)
        wieldtype = 4;
      else
        wieldtype = 1;
    }
    if (wieldorig == 3)
    {
      if (wieldie == 2 || wieldie == 4)
        wieldtype = 2;
      else
        wieldtype = 1;
    }
    one_hit(ch, victim, -1, wieldtype);
    if (victim == NULL || victim->position != POS_FIGHTING)
      return;
    if (ch->fighting != victim)
      return;
  }


  if (!IS_NPC(ch) && IS_SET(ch->newbits,NEW_QUILLS))
    multi_hit(ch,victim,gsn_quills);
if (IS_SPHERE(ch, SPHERE_ENHANCEMENT))
	{
	    if (IS_SET(ch->newbits, NEW_DARKTENDRILS))
                multi_hit(ch, victim, gsn_darktendrils);
	}
  if (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FANGS) && number_range(1,2) == 1)
  {
    multi_hit( ch, victim, gsn_fangs );
  }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO || ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;
    

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Atemi #0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Atemi strikes you!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }


   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))  
  {
      sprintf(buf1, "#0$N! is struck by your Aikido Chudan Tsuki (open hand strike)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Chudan Tsuki (open hand strike)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }
   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Aikido Empi (Elbow Strike)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Empi (Elbow Strike)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Aikido Empi (Elbow Strike)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Empi (Elbow Strike)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    dam = (number_range(150,300));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Aikido Geri (Kick)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Geri (Kick)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
 }
   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(150,300));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Aikido Kubishime (Neck Choke)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Kubishime (Neck Choke)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }


   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(150,300));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Aikido Nukite (Fingertip Strike)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Aikido Nukite (Fingertip strike)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }


   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Kendo ashi-kake(trip)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Kendo ashi-kake (Trip)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
      victim->position = POS_STUNNED;
    }
 }


   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(750,900));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Kendo Tsuki(throat Hit)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Kendo Tsuki (Throat Hit)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
}

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;


    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(750,830));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Kendo Uchi(Cut)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Kendo Uchi (Cut)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }
   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;


    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(650,800));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your Kendo Tsuki(throat Hit)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's Kendo Tsuki (Throat Hit)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }
   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,400));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your JiuJitsu Awase Tsuke (U Punch)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's JiuJitsu Awase Tsuke (U Punch)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,400));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your JiuJitsu Awase Tsuke (U Punch)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's JiuJitsu Awase Tsuke (U Punch)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(250,300));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your JiuJitsu Choku tsuke (Straight Punch)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's JiuJitsu Choku tsuke (Straight Punch)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(250,300));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your JiuJitsu Chudan choku tsuke (Straight Punch to the body)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's JiuJitsu Chudan choku tsuke (Straight Punch to the body)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }

 }


   if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 90)
    {
    dam = (number_range(350,500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is struck by your JiuJitsu Mawashi Geri (Round Kick)#0[#R%d#0]#n", dam);
      sprintf(buf2, "#0$n's JiuJitsu Mawashi Geri (Round Kick)!#0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }


if (IS_SPHERE(ch, SPHERE_WOKANIST) && IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], FIRE_FORM))
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int dam;

    if (victim->hit < 1) return;
    if (number_percent() > 60)
    {
    dam = (number_range(1500,2500));
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      sprintf(buf1, "#0$N! is completly engulfed with bright #7white #RF#Yl#Ra#Ym#Re#Ys#n #0[#R%d#0]#n", dam);
      sprintf(buf2, "#0Fire from $n's strikes you!  #0[#R%d#0]#n",dam);
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
    }
      hurt_person(ch, victim, dam);
    }
 }

if (IS_SPHERE(ch, SPHERE_WOKANIST) &&  IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], WATER_FORM))
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

   if (number_percent() > 94 && !IS_NPC(victim))
     {
      sprintf(buf1, "#0The Power of your #lwater#0 form stuns $N!#n");
      sprintf(buf2, "#0The Power of $n's #lWater#0 form stuns you!#n");
      act(buf1, ch, NULL, victim, TO_CHAR);
      act(buf2, ch, NULL, victim, TO_VICT);
      victim->position = POS_STUNNED;
     }
 }

        if (wield1 != NULL)
        {
            if (IS_SET(wield1->quest, QUEST_VINEFORGE))
            {
                char      buf1[MAX_STRING_LENGTH];
                char      buf2[MAX_STRING_LENGTH];
                int       dam = number_range(500, 1500);

                sprintf(buf1,"#0Your weapon's #GV#gi#Gn#ge#Gs #0lash out at #R$N!  #0[#R%d#0]#n",  dam);
                sprintf(buf2,"#R$n's #0weapons #GV#gi#Gn#ge#Gs #0lash out at You!  #0[#R%d#0]#n", dam);
                act(buf1, ch, NULL, victim, TO_CHAR);
                act(buf2, ch, NULL, victim, TO_VICT);

                hurt_person(ch, victim, dam);
            }
}

  /*
   * Knockdown and daze effects for mace and flail.
   */
  if (ch->fighting == victim)
  {
  int dam;
    /*
     * Maces knock the opponent down and lag them for a moment.
     */
    if (dtype == WPN_MACE && number_percent() < 5)
    {
      dam += get_curr_dex(ch);
    }
    if (number_percent() <= 5)
    {
      WAIT_STATE (victim, PULSE_VIOLENCE*1);
      victim->position = POS_RESTING;

      act ("Your blow knocks $N to the ground.",
          ch, NULL, victim, TO_CHAR);
      act ("$n knocks you to the ground.",
          ch, NULL, victim, TO_VICT);

    }
    /*
     * Flails daze the opponent and screw up their casting.
     */
    if (dtype == WPN_FLAIL && number_percent() < 5)
    {
      victim->position = POS_STUNNED;

    }
  }



  if (victim == NULL || victim->position != POS_FIGHTING)
    return;

  if (victim->itemaffect < 1)
    return;


  if (IS_NPC(victim) || victim->spl[1] < 4)
    level = victim->level;
  else
    level = (victim->spl[1] * 0.25);




/*    if (number_percent() > 95)
    {
      SET_BIT(ch->affected_by, AFF_FLAMING);
       if (!IS_VAMPAFF(ch, VAM_CLAWS))
      {
        act("Your flaming hands catch $n on fire!", ch, NULL, victim, TO_VICT);
        act("$N's flaming hands catch $n on fire!", ch, NULL, victim, TO_NOTVICT);
        act("$N's flaming hands catch you on fire!.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
      act("Your flaming claws catch $n on fire!", ch, NULL, victim, TO_VICT);
      act("$N's flaming claws catch $n on fire!", ch, NULL, victim, TO_NOTVICT);
      act("$N's flaming claws catch you on fire!.", ch, NULL, victim, TO_CHAR);
      }
    }
*/


  if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) && ch->position == POS_FIGHTING)
    if ((sn = skill_lookup("chaos blast")) > 0)
      (*skill_table[sn].spell_fun) (sn, level, victim, ch);
  return;
}

int number_attacks(CHAR_DATA * ch, CHAR_DATA * victim)
{
  int count = 1;

  if (IS_NPC(ch))
  {
    if (ch->level >= 50)
      count += 1;
    if (ch->level >= 100)
      count += 1;
    if (ch->level >= 500)
      count += 1;
    if (ch->level >= 1000)
      count += 1;
    if (ch->level >= 1500)
      count += 1;
    if (ch->level >= 2000)
      count += 2;
    if (ch->pIndexData->extra_attack > 20)
      count += 20;
    else
      count += ch->pIndexData->extra_attack;
    return count;
  }
  if (IS_NPC(victim))
  {
    if (IS_STANCE(ch, STANCE_VIPER) && number_percent() < ch->stance[STANCE_VIPER] * 0.5)
      count += 1;
    else if (IS_STANCE(ch, STANCE_MANTIS) && number_percent() < ch->stance[STANCE_MANTIS] * 0.5)
      count += 1;
    else if (IS_STANCE(ch, STANCE_TIGER) && number_percent() < ch->stance[STANCE_TIGER] * 0.5)
      count += 1;
    else if (IS_STANCE(ch, STANCE_WOLF) && number_percent() < ch->stance[STANCE_WOLF] * 0.5)
      count += 2;
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_SPEED) && number_percent() < ch->stance[(ch->stance[0])] * 0.5)
      count += 2;
    if (IS_ITEMAFF(ch, ITEMA_SPEED))
      count += 2;
  }
  else
  {
    if (IS_STANCE(ch, STANCE_VIPER) && number_percent() < ch->stance[STANCE_VIPER] * 0.5)
      count += 1;
    else if (IS_STANCE(ch, STANCE_MANTIS) && number_percent() < ch->stance[STANCE_MANTIS] * 0.5)
      count += 1;
    else if (IS_STANCE(ch, STANCE_TIGER) && number_percent() < ch->stance[STANCE_TIGER] * 0.5)
      count += 1;
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_SPEED) && number_percent() < ch->stance[(ch->stance[0])] * 0.5)
      count += 2;

	        if (IS_SET(ch->pcdata->powers[ELEMENTAL_FORMS], AIR_FORM)) count += 2;

    /* some wicked randomness */
    if (number_range(1, 4) == 2)
      count -= 1;
  }
  if (count < 1)
    count = 1;
  return count;
}

/*
 * Hit one guy once.
 */
void one_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt, int handtype)
{
  OBJ_DATA *wield;
  int victim_ac;
  int thac0;
  int thac0_00;
  int thac0_32;
  int dam;
  int diceroll;
  int level;
  int attack_modify;
  int right_hand;
  int dtype;
  const char *attack; // added by xrak

  /*
   * Can't beat a dead char!
   * Guard against weird room-leavings.
   */
  if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
    return;
  if (!IS_NPC(ch))
  {
    if (IS_AFFECTED(ch, AFF_PEACE) && ch->fighting != NULL)
      REMOVE_BIT(ch->affected_by, AFF_PEACE);
  }
  if (!IS_NPC(victim))
  {
    if (IS_AFFECTED(victim, AFF_PEACE) && victim->fighting != NULL)
      REMOVE_BIT(victim->affected_by, AFF_PEACE);
  }
  /*
   * Figure out the type of 'damage message.
   */
  if (handtype == 8)
  {
    wield = get_eq_char(ch, WEAR_FOURTH);
    right_hand = 8;
  }
  else if (handtype == 4)
  {
    wield = get_eq_char(ch, WEAR_THIRD);
    right_hand = 4;
  }
  else if (handtype == 2)
  {
    wield = get_eq_char(ch, WEAR_HOLD);
    right_hand = 2;
  }
  else if (handtype == 0)
  {
    wield = NULL;
    right_hand = 0;
  }
  else
  {
    if (IS_SET(ch->act, PLR_RIGHTHAND))
      wield = get_eq_char(ch, WEAR_WIELD);
    else if (IS_SET(ch->act, PLR_LEFTHAND))
      wield = get_eq_char(ch, WEAR_HOLD);
    else
    {
      if (number_range(1, 10) > 4)
        wield = get_eq_char(ch, WEAR_HOLD);
      else
        wield = get_eq_char(ch, WEAR_WIELD);
    }
    right_hand = 1;             // improve the right hand anyway...
  }
  if (dt == TYPE_UNDEFINED)
  {
    dt = TYPE_HIT;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
      dt += wield->value[3];
    else if (IS_NPC(ch))
      dt += ch->pIndexData->natural_attack;
  }
  if (ch->wpn[dt - 1000] > 5)
    level = (ch->wpn[dt - 1000] / 5);
  else
    level = 1;
  if (level > 40)
    level = 40;



  if (IS_SET(ch->newbits, NEW_TOUGHSKIN))
  dam = dam / 1.4;

  /*
   *  Xrak's Weapon Code
   * (resistances and immmunities for races)
   */

  attack = skill_table[dt].noun_damage;
/*
  if (!str_cmp(attack,"pierce"))
  {
  if (IS_IMMUNE2(victim, IMM2_PIERCE))
  dam = dam / 3;
  if (IS_RES(victim, RES_PIERCE))
  dam = dam / 1.4;
  if (IS_VULN(victim, VULN_PIERCE))
  dam += 300;
  }
  if (!str_cmp(attack,"slash"))
  {
  if (IS_IMMUNE2(victim, IMM2_SLASH))
  dam = dam / 3;
  if (IS_RES(victim, RES_SLASH))
  dam = dam / 1.4;
  if (IS_VULN(victim, VULN_SLASH))
  dam += 300;
  }
*/

  /*
   *  Xrak's Race Modifiers
   *  Similar to anticlasses
   */

  if ((IS_RACE(ch, RACE_HILLDWARF) || IS_RACE(ch, RACE_MTDWARF)
  || IS_RACE(ch, RACE_DEEPDWARF) || IS_RACE(ch, RACE_GREYDWARF)
  || IS_RACE(ch, RACE_JUNGLEDWARF) || IS_RACE(ch, RACE_AQUATICDWARF)
  || IS_RACE(ch, RACE_ARCTICDWARF)) 
  && (IS_RACE(victim, RACE_GOBLIN)
  || IS_RACE(victim, RACE_AIRGOBLIN) || IS_RACE(victim, RACE_AQUATICGOBLIN)
  || IS_RACE(victim, RACE_ARCTICGOBLIN) || IS_RACE(victim, RACE_GOBLINFORESTKITH)
  || IS_RACE(victim, RACE_JUNGLEGOBLIN) || IS_RACE(victim, RACE_DESERTGOBLIN)
  || IS_RACE(victim, RACE_BUGBEAR) || IS_RACE(victim, RACE_HOBGOBLIN)))
  dam *= 1.3;

  if ((IS_RACE(ch, RACE_HILLDWARF) || IS_RACE(ch, RACE_MTDWARF)
  || IS_RACE(ch, RACE_DEEPDWARF) || IS_RACE(ch, RACE_GREYDWARF)
  || IS_RACE(ch, RACE_JUNGLEDWARF) || IS_RACE(ch, RACE_AQUATICDWARF)) 
  && IS_RACE(victim, RACE_ORC))
  dam *= 1.3;

  if ((IS_RACE(ch, RACE_ROCKGNOME) || IS_RACE(ch, RACE_ARCTICGNOME)
  || IS_RACE(ch, RACE_DESERTGNOME))
  && (IS_RACE(victim, RACE_GOBLIN)
  || IS_RACE(victim, RACE_AIRGOBLIN) || IS_RACE(victim, RACE_AQUATICGOBLIN)
  || IS_RACE(victim, RACE_ARCTICGOBLIN) || IS_RACE(victim, RACE_GOBLINFORESTKITH)
  || IS_RACE(victim, RACE_JUNGLEGOBLIN) || IS_RACE(victim, RACE_DESERTGOBLIN)
  || IS_RACE(victim, RACE_BUGBEAR) || IS_RACE(victim, RACE_HOBGOBLIN)
  || IS_RACE(victim, RACE_AQUATICKOBOLD) || IS_RACE(victim, RACE_ARCTICKOBOLD)
  || IS_RACE(victim, RACE_DESERTKOBOLD) || IS_RACE(victim, RACE_EARTHKOBOLD)
  || IS_RACE(victim, RACE_JUNGLEKOBOLD)))
  dam *= 1.3;

  /*
   * Calculate to-hit-armor-class-0 versus armor.
   */
  if (IS_NPC(ch))
  {
    thac0_00 = 20;
    thac0_32 = 0;
  }
  else
  {
    thac0_00 = SKILL_THAC0_00;
    thac0_32 = SKILL_THAC0_32;
  }
  thac0 = interpolate(level, thac0_00, thac0_32) - char_hitroll(ch);
  victim_ac = UMAX(-100, char_ac(victim) / 10);
  if (!can_see(ch, victim))
    victim_ac -= 4;
  if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_3))
    victim_ac += 300;
  if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_2))
    victim_ac += 200;
  if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_1))
    victim_ac += 100;

  /*
   * The moment of excitement!
   */
  while ((diceroll = number_bits(5)) >= 20)
    ;

  if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac))
  {                             /* Miss. */
    damage(ch, victim, 0, dt);
    tail_chain();
    if (!is_safe(ch, victim))
    {
      improve_wpn(ch, dt, right_hand);
      improve_stance(ch);
      improve_str(ch);
      improve_con(ch);
      improve_dex(ch);
      improve_int(ch);
      improve_wis(ch);
    }
    return;
  }

  /*
   * Hit.
   * Calc damage.
   */
  if (IS_NPC(ch))
  {
    dam = number_range(ch->level / 2, ch->level * 3 / 2);
    if (wield != NULL)
      dam += dam * 0.5;
  }
  else
  {
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
      dam = dice(wield->value[1], wield->value[2]);
    else
      dam = dice(4, 10);
  }

            if (IS_ITEMAFF(victim, ITEMA_ILLUSIONS))
            {
                int       ddodge = number_range(1, 50);

                if (ddodge == 10 || ddodge == 20 || ddodge == 30
                    || ddodge == 40 || ddodge == 50)
                {
                        stc("#CYour illusion takes the blow, saving you from harm#n.\n\r", victim);
                        stc("#CYour weapon passes through your opponent without doing any harm#n.\n\r", ch);
                        dam = 0;
                }
            }


  /*
   * Bonuses.
   */
  dam += char_damroll(ch);

/* Axe Special */
  dam = axe_special (ch, victim, dam);

  if (dtype == WPN_SPEAR && number_percent() < 10)
  {
    dam += get_curr_dex(ch);
  }

  if (!IS_AWAKE(victim))
    dam *= 2;
  if (!IS_NPC(ch) && dt >= TYPE_HIT)
    dam = dam + (dam * (UMIN(350, (ch->wpn[dt - 1000] + 1)) / 60));
  dam = up_dam(ch, victim, dam);

  /* superstance modifiers */
  if (IS_NPC(victim))
  {
    if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_3) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 1.5 * (ch->stance[(ch->stance[0])] / 200);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_2) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 1.0 * (ch->stance[(ch->stance[0])] / 200);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_1) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 0.5 * (ch->stance[(ch->stance[0])] / 200);
  }
  else
  {
    if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_3) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 0.5 * (ch->stance[(ch->stance[0])] / 200);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_2) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 0.3 * (ch->stance[(ch->stance[0])] / 200);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_1) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 0.1 * (ch->stance[(ch->stance[0])] / 200);
  }
  if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_3) && victim->stance[(victim->stance[0])] > 100)
    dam *= 0.7;
  else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_2) && victim->stance[(victim->stance[0])] > 100)
    dam *= 0.8;
  else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_1) && victim->stance[(victim->stance[0])] > 100)
    dam *= 0.9;

  if (dt == gsn_garotte)
    dam *= (number_range(3, 5));

  if (dt == gsn_backstab)
    dam *= number_range(2, 4);

         if (IS_SPHERE(victim, SPHERE_ENHANCEMENT))
	  {
            if (IS_SET(victim->newbits, NEW_DARKTENDRILS))
            {
              int       ddodge = number_range(1, 50);

              if (ddodge == 10 || ddodge == 20 || ddodge == 30 || ddodge == 40 || ddodge == 50)
              {
                stc("#0You dark aura absorbs the damage#n.\n\r", victim);
                stc("#0Your opponent's dark aura absorbs your damage#n.\n\r", ch);
                dam = 0;
              }
	    }
          }
  if (!IS_NPC(victim) && IS_SPHERE(victim, SPHERE_ILLUSIONIST) && IS_ITEMAFF(victim, ITEMA_ILLUSIONS))
  {
    int ddodge = number_range(1,50);
    if (ddodge == 10 || ddodge == 20 || ddodge == 30 || ddodge == 40 || ddodge == 50)
    {
      stc("#CYour illusion takes the blow, saving you from harm#n.\n\r",victim);
      stc("#CYour weapon passes through your opponent without doing any harm#n.\n\r",ch);
      dam = 0;
    }
  }

  dam = cap_dam(ch, victim, dam);

  if (IS_NPC(ch) && ch->pIndexData->dam_modifier > 3)
    dam *= 3;
  else if (IS_NPC(ch) && ch->pIndexData->dam_modifier > 0)
    dam *= ch->pIndexData->dam_modifier;

  if (IS_NPC(victim) && !IS_NPC(ch))
  {
    if (victim->pIndexData->toughness > 100)
      victim->pIndexData->toughness = 99;
    dam = dam * (100 - victim->pIndexData->toughness) / 100;
  }



  attack_modify = dice(1, 100);
  randomize_damage(ch, dam, attack_modify);
  damage(ch, victim, dam, dt);

  tail_chain();
  if (!is_safe(ch, victim))
  {
    improve_wpn(ch, dt, right_hand);
    improve_stance(ch);
  }
}

int randomize_damage(CHAR_DATA * ch, int dam, int am)
{
  dam = dam * (am + 50) / 100;
  return dam;
}

int up_dam(CHAR_DATA * ch, CHAR_DATA * victim, int dam)
{
  return dam;
}

int cap_dam(CHAR_DATA * ch, CHAR_DATA * victim, int dam)
{
  if (is_safe(ch, victim))
    return 0;

  if (!IS_NPC(ch) && !IS_NPC(victim))
    dam /= 2;
  if (IS_NPC(ch) && dam > 2000)
    dam = 2000 + (dam - 2000) / 2;

  if (IS_ITEMAFF(victim, ITEMA_RESISTANCE))
  {
    dam *= 75;
    dam /= 100;
  }

if (!IS_NPC(victim) && IS_SPHERE(victim, SPHERE_WOKANIST) && IS_SET(victim->pcdata->powers[ELEMENTAL_FORMS], EARTH_FORM))
  {
	  dam /=1.4;
  }
  if (IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch))
  {
    dam *= 75;
    dam /= 100;
  }
  if (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch))
  {
    dam *= 75;
    dam /= 100;
  }

  if (!IS_NPC(victim))
  {
    dam = dam + (dam / number_range(2, 5) + number_range(10, 50));
    dam *= (number_range(2, 4) * number_range(2, 3) / number_range(4, 6));
  }
  if (dam <= 0)
    dam = 1;
  if (dam > 1000000)
    dam = 1000000;
  if (dam > ch->damcap[DAM_CAP])
    dam = number_range((ch->damcap[DAM_CAP] - 200), (ch->damcap[DAM_CAP] + 100));
  if (IS_AFFECTED(victim, AFF_SANCTUARY))
    dam *= 0.5;
  if (IS_AFFECTED(victim, AFF_HOLYAURA) && dam > 1)
    dam *= 0.5;
  return dam;
}

bool can_counter(CHAR_DATA * ch)
{
  if (!IS_NPC(ch) && IS_STANCE(ch, STANCE_MONKEY))
    return TRUE;
  return FALSE;
}

bool can_bypass(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;
  else if (IS_STANCE(ch, STANCE_VIPER))
    return TRUE;
  else if (IS_STANCE(ch, STANCE_MANTIS))
    return TRUE;
  else if (IS_STANCE(ch, STANCE_TIGER))
    return TRUE;
  else if (IS_STANCE(ch, STANCE_WOLF))
    return TRUE;
  else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_BYPASS))
    return TRUE;
  return FALSE;
}

void update_damcap(CHAR_DATA * ch, CHAR_DATA * victim)
{
  int max_dam = 1000;

  if (!IS_NPC(ch))
  {
    if (ch->generation == 1)
      max_dam += 250;
    if (ch->generation == 2)
      max_dam += 200;
    if (ch->generation == 3)
      max_dam += 150;
    if (ch->generation == 4)
      max_dam += 100;
    if (ch->generation == 5)
      max_dam += 50;

    if (ch->pcRaceLevel > 49)
      max_dam += 200;
    if (ch->pcRaceLevel > 99)
      max_dam += 200;
    if (ch->pcRaceLevel > 149)
      max_dam += 200;
    if (ch->pcRaceLevel > 199)
      max_dam += 400;

    if (ch->pcLegacy > 0) max_dam += (ch->pcLegacy * 50);
  }
  if (IS_ITEMAFF(ch, ITEMA_ARTIFACT))
    max_dam += 500;

  if (IS_NPC(victim) || victim->stance[0] != STANCE_MONKEY)
  {
    if (ch->stance[0] == STANCE_BULL)
      max_dam += 200;
    else if (ch->stance[0] == STANCE_DRAGON)
      max_dam += 250;
    else if (ch->stance[0] == STANCE_WOLF)
      max_dam += 250;
    else if (ch->stance[0] == STANCE_TIGER)
      max_dam += 200;
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMCAP_3))
      max_dam += 550;
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMCAP_2))
      max_dam += 400;
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMCAP_1))
      max_dam += 250;
  }

  if (!IS_NPC(victim) && ch->stance[0] != STANCE_MONKEY)
  {
    if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_REV_DAMCAP_3))
      max_dam -= 550;
    else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_REV_DAMCAP_2))
      max_dam -= 400;
    else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_REV_DAMCAP_1))
      max_dam -= 250;
  }

  /* Max Dam Reductions */
  /* Werewolves */

  if (!IS_NPC(victim))
  {
    if (IS_NPC(ch) || ch->stance[0] != STANCE_MONKEY)
    {
      if (victim->stance[0] == STANCE_CRAB)
        max_dam -= 250;
      else if (victim->stance[0] == STANCE_DRAGON)
        max_dam -= 250;
      else if (victim->stance[0] == STANCE_SWALLOW)
        max_dam -= 250;
    }
  }

  if (ch->level >= LEVEL_BUILDER)
    max_dam = 30000;
  ch->damcap[DAM_CAP] = max_dam;
  ch->damcap[DAM_CHANGE] = 0;
  return;
}

/*
 * Inflict damage from a hit.
 */
void damage(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt)
{
  int hit_loc;

  if (victim->position == POS_DEAD)
    return;

  /*
   * Stop up any residual loopholes.
   */
  if (ch->damcap[DAM_CHANGE] == 1)
    update_damcap(ch, victim);
  if (dam > ch->damcap[0])
    dam = ch->damcap[0];

  /*
   * Certain attacks are forbidden.
   * Most other attacks are returned.
   */
  if (victim != ch)
  {
    if (is_safe(ch, victim))
      return;
    if (victim->position > POS_STUNNED)
    {
      if ( victim->fighting == NULL ) set_fighting( victim, ch );
      victim->position = POS_FIGHTING;
    }
    if (victim->position > POS_STUNNED)
    {
      if (ch->fighting == NULL)
        set_fighting(ch, victim );
      /*
       * If victim is charmed, ch might attack victim's master.
       */
      if (IS_NPC(ch) && IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL && victim->master->in_room == ch->in_room && number_bits(3) == 0)
      {
        stop_fighting(ch, FALSE);
        multi_hit(ch, victim->master, TYPE_UNDEFINED);
        return;
      }
    }
    if (victim->master == ch)
    {
      stop_follower(victim, FALSE);
    }

    /*
     * Damage modifiers.
     */
    if (IS_AFFECTED(ch, AFF_HIDE))
    {
      if (!can_see(victim, ch))
      {
        dam *= 1.5;
        send_to_char("You use your concealment to get a surprise attack!\n\r", ch);
      }
      REMOVE_BIT(ch->affected_by, AFF_HIDE);
      act("$n leaps from $s concealment.", ch, NULL, NULL, TO_ROOM);
    }
    if (dam < 0)
      dam = 0;

    if (!IS_NPC(victim))
    {
      if (victim->pcdata->damreduct == 100)
        dam -= 0;
      else if (victim->pcdata->damreductdec > 0)
        dam = (dam * (victim->pcdata->damreduct + 1)) / 100;
      else
        dam = (dam * victim->pcdata->damreduct) / 100;
    }


    /*
     * Check for disarm, trip, parry, and dodge.
     */
    if (dt >= TYPE_HIT)
    {
      if (IS_NPC(ch) && number_percent() < ch->level * 0.5)
        disarm(ch, victim);
      if (IS_NPC(ch) && number_percent() < ch->level * 0.5)
        trip(ch, victim);
      if (check_dodge(ch, victim, dt))
        return;

      if (check_parry(ch, victim, dt))
        return;
      if (check_shieldblock(ch, victim, dt))
	return;
      if (pole_special (ch, victim))
        return;

      if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) && victim->stance[STANCE_CRANE] > 100 && !can_counter(ch) && !can_bypass(ch, victim) && check_parry(ch, victim, dt))
        return;
      else if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) && victim->stance[STANCE_MANTIS] > 100 && !can_counter(ch) && !can_bypass(ch, victim) && check_parry(ch, victim, dt))
        return;

    }

    hit_loc = number_range(1,11);
    dam_message( ch, victim, dam, dt, hit_loc);

    /*
     * Brief 5 System - Jobo
     */
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF5))
    {
      ch->amount_damage_dealt += dam;
      ch->amount_attacks_dealt++;
    }
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_BRIEF5))
    {
      victim->amount_damage_recieved += dam;
      victim->amount_attacks_recieved++;
    }

  }
  if (IS_SET(victim->newbits, NEW_IRONMIND))
  {
    REMOVE_BIT(victim->newbits, NEW_IRONMIND);
    send_to_char("Your focus your full concentration on the attack,\n\r", victim);
    send_to_char("and absorb the full impact into your system, channeling it into a healing force.\n\r", victim);
    victim->hit += dam;
    dam = 0;
  }
  hurt_person(ch, victim, dam);
  dropinvis(ch);
  dropinvis(victim);
  return;
}

void hurt_person(CHAR_DATA * ch, CHAR_DATA * victim, int dam)
{
  char buf[MAX_STRING_LENGTH];

  victim->hit -= dam;
  if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
    victim->hit = 1;

  update_pos(victim);

  switch (victim->position)
  {
    case POS_MORTAL:
      act("$n is mortally wounded, and spraying blood everywhere.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are mortally wounded, and spraying blood everywhere.\n\r", victim);
      break;
    case POS_INCAP:
      act("$n is incapacitated, and bleeding badly.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are incapacitated, and bleeding badly.\n\r", victim);
      break;
    case POS_STUNNED:
      act("$n is stunned, but will soon recover.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are stunned, but will soon recover.\n\r", victim);
      break;
    case POS_DEAD:
      act("$n is DEAD!!", victim, 0, 0, TO_ROOM);
      send_to_char("You have been KILLED!!\n\r\n\r", victim);
      break;
    default:
      if (dam > victim->max_hit / 4)
        send_to_char("That really did HURT!\n\r", victim);
      if (victim->hit < victim->max_hit / 4 && dam > 0)
          send_to_char("You sure are BLEEDING!\n\r", victim);
      break;
  }

  /*
   * Sleep spells and extremely wounded folks.
   */
  if (!IS_AWAKE(victim))
    stop_fighting(victim, FALSE);

  /*
   * Payoff for killing things.
   */
  if (victim->hit <= 0 && IS_NPC(victim))
  {
    group_gain(ch, victim);
    victim->position = POS_DEAD;

    /*
     * quest updating
     */
    if (!IS_NPC(ch))
    {
      QUEST_DATA *quest;
      bool found = FALSE;

      for (quest = ch->pcdata->quests; quest && !found; quest = quest->next)
      {
        if (quest->type == QT_MOB || quest->type == QT_MOB_AND_OBJ)
        {
          if (quest->vnums[0] == victim->pIndexData->vnum)
          {
            quest->vnums[0] = -1;
            found = TRUE;
            send_to_char("#GYou have fulfilled a quest.#n\n\r", ch);
          }
        }
        else if (quest->type == QT_MASS_KILL)
        {
          if (victim->pIndexData->vnum >= quest->vnums[0] && victim->pIndexData->vnum <= quest->vnums[1])
          {
            quest->vnums[3]++;
            if (quest->vnums[3] == quest->vnums[2])
              send_to_char("#GYou have fulfilled a quest.#n\n\r", ch);
            found = TRUE;
          }
        }
      }
    }
  }

  if (victim->position == POS_DEAD)
  {
    if (IS_NPC(victim) && !IS_NPC(ch))
    {
      ch->mkill += 1;

      if (ch->mkill > 4 && ch->level < 3)
      {
        ch->level = 3;
        do_save(ch, "");
      }  
    }
    raw_kill(victim);
    if (!IS_NPC(ch) && !IS_NPC(victim) && victim->pcdata->bounty > 0)
    {
      sprintf(buf, "You recive a %d QP bounty, for killing %s.\n\r", victim->pcdata->bounty, victim->name);
      send_to_char(buf, ch);
      ch->pcdata->quest += victim->pcdata->bounty;
      victim->pcdata->bounty = 0;
    }
    if (IS_SET(ch->act, PLR_AUTOLOOT))
      do_get(ch, "all corpse");
    else
      do_look(ch, "in corpse");
    if (!IS_NPC(ch) && IS_NPC(victim))
    {
      if (IS_SET(ch->act, PLR_AUTOSAC))
        do_sacrifice(ch, "corpse");
    }
    return;
  }
  if (victim == ch)
    return;
  tail_chain();
  return;
}

bool is_safe(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (!ch->in_room)
    return TRUE;                // when someone calls a one_hit or similar on something/someone who COULD be dead/gone.
  if (!victim->in_room)
    return TRUE;
  if (!IS_NPC(victim) && !IS_NPC(ch))
  {
    if (!CAN_PK(ch) || !CAN_PK(victim))
    {
      send_to_char("Both players must be avatars to fight.\n\r", ch);
      return TRUE;
    }
    if (ch->pcdata->safe_counter > 0)
    {
      send_to_char("Your still have a few safe-ticks left.\n\r", ch);
      return TRUE;
    }
    if (victim->pcdata->safe_counter > 0)
    {
      send_to_char("They still have a few safe-ticks left.\n\r", ch);
      return TRUE;
    }
  }
  if (victim->shop_fun || victim->quest_fun)
  {
    send_to_char("That mobile is protected by the gods.\n\r", ch);
    return TRUE;
  }
  if (IS_AFFECTED(ch, AFF_ETHEREAL))
  {
    send_to_char("You cannot while ethereal.\n\r", ch);
    return TRUE;
  }
  if (IS_AFFECTED(victim, AFF_ETHEREAL))
  {
    send_to_char("You cannot fight an ethereal person.\n\r", ch);
    return TRUE;
  }
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
  {
    act("You are too insubstantial!", ch, NULL, victim, TO_CHAR);
    return TRUE;
  }
  if (IS_AFFECTED(victim, AFF_SHADOWPLANE))
  {
    act("$E is too insubstantial!", ch, NULL, victim, TO_CHAR);
    return TRUE;
  }
  if (IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH))
  {
    send_to_char("Objects cannot fight!\n\r", ch);
    return TRUE;
  }
  if (IS_HEAD(victim, LOST_HEAD) || IS_EXTRA(victim, EXTRA_OSWITCH))
  {
    send_to_char("You cannot attack an object.\n\r", ch);
    return TRUE;
  }
  if (IS_SET(victim->extra, EXTRA_AFK))
  {
    send_to_char("They are AFK!\n\r", ch);
    return TRUE;
  }
  if (!IS_NPC(victim) && victim->desc == NULL && victim->timer > 1 && victim->fight_timer == 0 && !in_fortress(ch))
  {
// Timer check to avoid people going ld in the first round.
    send_to_char("Nooo, they are linkdead.\n\r", ch);
    return TRUE;
  }
  if (victim->fight_timer > 0)
    return FALSE;
  if (!IS_NPC(ch))
    if (IS_AFFECTED(ch, AFF_PEACE) && ch->fighting != NULL)
      REMOVE_BIT(ch->affected_by, AFF_PEACE);
  if (!IS_NPC(victim))
    if (IS_AFFECTED(victim, AFF_PEACE) && victim->fighting != NULL)
      REMOVE_BIT(victim->affected_by, AFF_PEACE);
  if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !ragnarok)
  {
    send_to_char("You cannot fight in a safe room.\n\r", ch);
    return TRUE;
  }
/*
  if (IS_NPC(ch) || IS_NPC(victim)) return FALSE;
  if (victim->fighting == ch) return FALSE;
*/
  if (IS_AFFECTED(ch, AFF_PEACE))
  {
    send_to_char("You are unable to attack them.\n\r", ch);
    return TRUE;
  }
  if (IS_AFFECTED(victim, AFF_PEACE))
  {
    send_to_char("You can't seem to attack them.\n\r", ch);
    return TRUE;
  }
  return FALSE;
}

/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
  OBJ_DATA *obj = NULL;
  int chance = 0;
  int dtype;

  if (!IS_AWAKE(victim))
    return FALSE;
  if (IS_NPC(victim))
    obj = NULL;
  else
  {
    if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL || obj->item_type != ITEM_WEAPON)
    {
      if ((obj = get_eq_char(victim, WEAR_HOLD)) == NULL || obj->item_type != ITEM_WEAPON)
        return FALSE;
    }
  }
  if ((dt < 1000 || dt > 1012))
    return FALSE;
  if (dt >= 1000 && dt <= 1012)
  {
    if (!IS_NPC(ch))
      chance -= (ch->wpn[dt - 1000] * 0.1);
    else
      chance -= (ch->level * 0.2);
    if (!IS_NPC(victim))
      chance += (victim->wpn[dt - 1000] * 0.5);
    else
      chance += victim->level;
  }
  else
  {
    if (!IS_NPC(ch))
      chance -= (ch->wpn[0] * 0.1);
    else
      chance -= (ch->level * 0.2);
    if (!IS_NPC(victim))
      chance += (victim->wpn[0] * 0.5);
    else
      chance += victim->level;
  }
  if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) && victim->stance[STANCE_CRANE] > 0 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += (victim->stance[STANCE_CRANE] * 0.25);
  else if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) && victim->stance[STANCE_MANTIS] > 0 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += (victim->stance[STANCE_MANTIS] * 0.25);

/* ------------ This is the part for superstances, Jobo ------------------- */
  if (!IS_NPC(victim) && victim->stance[0] > 12
      && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_PARRY) && victim->stance[(victim->stance[0])] > 100 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += victim->stance[(victim->stance[0])] * 0.25;
/* ------------ This is the end for superstances, Jobo ------------------- */

  chance -= (char_hitroll(ch) * 0.1);
  if (!IS_NPC(ch))
  {

  }
  if (!IS_NPC(victim))
  {

  }
  if (!IS_NPC(ch) && IS_NPC(victim)
  && IS_SET(ch->newbits, NEW_FIGHTDANCE)
  && number_percent() >=50
  && ((( obj = get_eq_char(ch, WEAR_WIELD ))  != NULL && ( obj->value[3] == 4 || obj->value[3] == 11))
      || ((obj = get_eq_char (ch, WEAR_HOLD))!= NULL &&( obj->value[3] == 4 || obj->value[3] == 11))))
  {
    act( "$n's dark weapon dances around your defences, making a strange sound.",  ch, NULL, victim, TO_VICT );
    act( "Your sacrificial weapon hums in the air as it dances $N's defences.", ch, obj, victim, TO_CHAR );
    return FALSE;
  }
  /* cutting corners */
  if      (chance > 80)  chance = 80;
  else if (chance < 20)  chance = 20;

  if (number_percent() < 100 && number_percent() >= chance)
    return FALSE;
  if (!IS_NPC(victim))
  {
    if (IS_SET(victim->pcdata->jflags, JFLAG_BULLY) && number_range(1, 3) != 2)
      return FALSE;
  }
  if (!IS_NPC(victim) && obj != NULL && obj->item_type == ITEM_WEAPON && obj->value[3] >= 0 && obj->value[3] <= 12)
  {
    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF3))
      act("You parry $n's blow with $p.", ch, obj, victim, TO_VICT);
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF3))
      act("$N parries your blow with $p.", ch, obj, victim, TO_CHAR);
    return TRUE;
  }
  if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF3))
    act("You parry $n's attack.", ch, NULL, victim, TO_VICT);
  if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF3))
    act("$N parries your attack.", ch, NULL, victim, TO_CHAR);

  if (dtype == WPN_WHIP)
  {
    chance = ch->wpn[4] / 400;

    if (number_percent () < chance)
    {
      disarm (ch, victim);
    }
  }

  return TRUE;
}

/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
  int chance = 0;

  if (!IS_AWAKE(victim))
    return FALSE;
  if (!IS_NPC(ch))
    chance -= (ch->wpn[dt - 1000] * 0.1);
  else
    chance -= (ch->level * 0.2);
  if (!IS_NPC(victim))
    chance += (victim->wpn[0] * 0.5);
  else
    chance += victim->level;
  if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) && victim->stance[STANCE_MONGOOSE] > 0 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += victim->stance[STANCE_MONGOOSE] * 0.25;
  if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_SWALLOW) && victim->stance[STANCE_SWALLOW] > 0 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += victim->stance[STANCE_SWALLOW] * 0.25;

/* ------------ This is the part for superstances, Jobo ------------------- */
  if (!IS_NPC(victim) && victim->stance[0] > 12
      && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_DODGE) && victim->stance[(victim->stance[0])] > 100 && !can_counter(ch) && !can_bypass(ch, victim))
    chance += victim->stance[(victim->stance[0])] * 0.25;
/* ------------ This is the end for superstances, Jobo ------------------- */

  /* cutting corners */
  if      (chance > 80)  chance = 80;
  else if (chance < 20)  chance = 20;


  /* cutting again */
  if (chance > 90) chance = 90;

  if (number_percent() >= chance)
    return FALSE;
  if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF3))
    act("You dodge $n's attack.", ch, NULL, victim, TO_VICT);
  if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF3))
    act("$N dodges your attack.", ch, NULL, victim, TO_CHAR);
  return TRUE;
}


/*
 * Check for shield block.
 */
bool check_shieldblock(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
  OBJ_DATA *obj = NULL;
  int chance = 0;

//  if (IS_NPC(victim))
//    obj = NULL;
//  else
//  {
     if ((obj = get_eq_char(victim, WEAR_SHIELD)) == NULL) // || obj->item_type != ITEM_WEAR_SHIELD)
        return FALSE;
//  }
  if (!IS_AWAKE(victim))
    return FALSE;
  if (!IS_NPC(ch))
    chance -= (ch->wpn[dt - 1000] * 0.1);
  else
    chance -= (ch->level * 0.2);
  if (!IS_NPC(victim))
    chance += (victim->wpn[0] * 0.5);
  else
    chance += victim->level;

  /* cutting corners */
  if      (chance > 80)  chance = 80;
  else if (chance < 20)  chance = 20;


  /* cutting again */
  if (chance > 90) chance = 90;

  if (number_percent() >= chance)
    return FALSE;
  if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF3))
    act("You Shield Block $n's attack.", ch, NULL, victim, TO_VICT);
  if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF3))
    act("$N Shield Blocks your attack.", ch, NULL, victim, TO_CHAR);
  return TRUE;
}




/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA * victim)
{
  CHAR_DATA *mount;

  if (victim->hit > 0)
  {
    if (victim->position <= POS_STUNNED)
    {
      bool gm_stance = FALSE;

      victim->position = POS_STANDING;
      if (!IS_NPC(victim) && victim->stance[0] > 0)
      {
        int stance = victim->stance[0];

        if (victim->stance[stance] >= 200)
          gm_stance = TRUE;
      }
      if (IS_NPC(victim) || victim->max_hit * 0.25 > victim->hit)
      {
        act("$n clambers back to $s feet.", victim, NULL, NULL, TO_ROOM);
        act("You clamber back to your feet.", victim, NULL, NULL, TO_CHAR);
      }
      else
      {
        act("$n flips back up to $s feet.", victim, NULL, NULL, TO_ROOM);
        act("You flip back up to your feet.", victim, NULL, NULL, TO_CHAR);
      }
    }
    return;
  }
  else if ((mount = victim->mount) != NULL)
  {
    if (victim->mounted == IS_MOUNT)
    {
      act("$n rolls off $N.", mount, NULL, victim, TO_ROOM);
      act("You roll off $N.", mount, NULL, victim, TO_CHAR);
    }
    else if (victim->mounted == IS_RIDING)
    {
      act("$n falls off $N.", victim, NULL, mount, TO_ROOM);
      act("You fall off $N.", victim, NULL, mount, TO_CHAR);
    }
    mount->mount = NULL;
    victim->mount = NULL;
    mount->mounted = IS_ON_FOOT;
    victim->mounted = IS_ON_FOOT;
  }
  if (!IS_NPC(victim) && victim->hit <= -11 && IS_HERO(victim))
  {
    victim->hit = -10;
    if (victim->position == POS_FIGHTING)
      stop_fighting(victim, TRUE);
    return;
  }
  if (!IS_NPC(victim) && victim->hit < -9 && victim->pcRace == 0)
  {
    victim->hit = 1;
    if (killperson(victim))
      return;;
  }
  if (IS_NPC(victim) && victim->hit < -6)
  {
    victim->position = POS_DEAD;
    return;
  }
  if (victim->hit <= -6)
    victim->position = POS_MORTAL;
  else if (victim->hit <= -3)
    victim->position = POS_INCAP;
  else
    victim->position = POS_STUNNED;
  return;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch->fighting != NULL)
    return;
  ch->fighting = victim;
  ch->position = POS_FIGHTING;
  ch->damcap[DAM_CHANGE] = 1;
  autodrop2(ch);
  autodrop2(victim);
  dropinvis(ch);
  dropinvis(victim);
  return;
}

void stop_fighting(CHAR_DATA * ch, bool fBoth)
{
  CHAR_DATA *fch;

  for (fch = char_list; fch != NULL; fch = fch->next)
  {
    if (fch == ch || (fBoth && fch->fighting == ch))
    {
      fch->fighting = NULL;
      fch->position = POS_STANDING;
      update_pos(fch);
    }
  }
  return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  char *name;

  if (!ch)
    return;
  if (ch->in_room == NULL)
  {
    return;
  }
  if (IS_NPC(ch))
  {
    name = ch->short_descr;
    corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
    corpse->timer = number_range(4, 8);
    corpse->value[2] = ch->pIndexData->vnum;
  }
  else
  {
    name = ch->name;
    corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
    corpse->timer = number_range(25, 40);
  }
  sprintf(buf, corpse->short_descr, name);
  free_string(corpse->short_descr);
  corpse->short_descr = str_dup(buf);
  sprintf(buf, corpse->description, name);
  free_string(corpse->description);
  corpse->description = str_dup(buf);
  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    obj_from_char(obj);
    if (IS_SET(obj->extra_flags, ITEM_VANISH))
      extract_obj(obj);
    else
    {
      if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE)))
        SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
      obj_to_obj(obj, corpse);
    }
  }
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
    SET_BIT(corpse->extra_flags, ITEM_SHADOWPLANE);
  obj_to_room(corpse, ch->in_room);
  return;
}

void make_part(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int vnum;

  argument = one_argument(argument, arg);
  vnum = 0;

  if (arg[0] == '\0')
    return;
  if (ch->in_room == NULL)
    return;

  if (!str_cmp(arg, "head"))
    vnum = OBJ_VNUM_SEVERED_HEAD;
  else if (!str_cmp(arg, "arm"))
    vnum = OBJ_VNUM_SLICED_ARM;
  else if (!str_cmp(arg, "leg"))
    vnum = OBJ_VNUM_SLICED_LEG;
  else if (!str_cmp(arg, "heart"))
    vnum = OBJ_VNUM_TORN_HEART;
  else if (!str_cmp(arg, "turd"))
    vnum = OBJ_VNUM_TORN_HEART;
  else if (!str_cmp(arg, "entrails"))
    vnum = OBJ_VNUM_SPILLED_ENTRAILS;
  else if (!str_cmp(arg, "brain"))
    vnum = OBJ_VNUM_QUIVERING_BRAIN;
  else if (!str_cmp(arg, "eyeball"))
    vnum = OBJ_VNUM_SQUIDGY_EYEBALL;
  else if (!str_cmp(arg, "blood"))
    vnum = OBJ_VNUM_SPILT_BLOOD;
  else if (!str_cmp(arg, "face"))
    vnum = OBJ_VNUM_RIPPED_FACE;
  else if (!str_cmp(arg, "windpipe"))
    vnum = OBJ_VNUM_TORN_WINDPIPE;
  else if (!str_cmp(arg, "cracked_head"))
    vnum = OBJ_VNUM_CRACKED_HEAD;
  else if (!str_cmp(arg, "ear"))
    vnum = OBJ_VNUM_SLICED_EAR;
  else if (!str_cmp(arg, "nose"))
    vnum = OBJ_VNUM_SLICED_NOSE;
  else if (!str_cmp(arg, "tooth"))
    vnum = OBJ_VNUM_KNOCKED_TOOTH;
  else if (!str_cmp(arg, "tongue"))
    vnum = OBJ_VNUM_TORN_TONGUE;
  else if (!str_cmp(arg, "hand"))
    vnum = OBJ_VNUM_SEVERED_HAND;
  else if (!str_cmp(arg, "foot"))
    vnum = OBJ_VNUM_SEVERED_FOOT;
  else if (!str_cmp(arg, "thumb"))
    vnum = OBJ_VNUM_SEVERED_THUMB;
  else if (!str_cmp(arg, "index"))
    vnum = OBJ_VNUM_SEVERED_INDEX;
  else if (!str_cmp(arg, "middle"))
    vnum = OBJ_VNUM_SEVERED_MIDDLE;
  else if (!str_cmp(arg, "ring"))
    vnum = OBJ_VNUM_SEVERED_RING;
  else if (!str_cmp(arg, "little"))
    vnum = OBJ_VNUM_SEVERED_LITTLE;
  else if (!str_cmp(arg, "toe"))
    vnum = OBJ_VNUM_SEVERED_TOE;

  if (vnum != 0)
  {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    char *name;

    name = IS_NPC(ch) ? ch->short_descr : ch->name;
    obj = create_object(get_obj_index(vnum), 0);
    obj->timer = number_range(4, 7);
    if (!str_cmp(arg, "head") && IS_NPC(ch))
      obj->value[1] = ch->pIndexData->vnum;
    else if (!str_cmp(arg, "head") && !IS_NPC(ch))
    {
      ch->pcdata->chobj = obj;
      obj->chobj = ch;
      obj->timer = number_range(1, 2);
      obj->item_type = ITEM_HEAD;
    }
    else if (!str_cmp(arg, "arm"))
      SET_BIT(obj->extra_flags2, ITEM_ARM);
    else if (!str_cmp(arg, "brain") && !IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH) && IS_HEAD(ch, LOST_HEAD))
    {
      if (ch->pcdata->chobj != NULL)
        ch->pcdata->chobj->chobj = NULL;
      ch->pcdata->chobj = obj;
      obj->chobj = ch;
      obj->timer = number_range(1, 2);
      obj->item_type = ITEM_HEAD;
    }
    if (vnum == OBJ_VNUM_SPILT_BLOOD)
      obj->timer = 2;
    if (!IS_NPC(ch))
    {
      sprintf(buf, obj->name, name);
      free_string(obj->name);
      obj->name = str_dup(buf);
    }
    else
    {
      sprintf(buf, obj->name, "mob");
      free_string(obj->name);
      obj->name = str_dup(buf);
    }
    sprintf(buf, obj->short_descr, name);
    free_string(obj->short_descr);
    obj->short_descr = str_dup(buf);
    sprintf(buf, obj->description, name);
    free_string(obj->description);
    obj->description = str_dup(buf);
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
      SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
    obj_to_room(obj, ch->in_room);
  }
  return;
}

void raw_kill(CHAR_DATA * victim)
{
  CHAR_DATA *mount;

  stop_fighting(victim, TRUE);
  make_corpse(victim);
  if ((mount = victim->mount) != NULL)
  {
    if (victim->mounted == IS_MOUNT)
    {
      act("$n rolls off the corpse of $N.", mount, NULL, victim, TO_ROOM);
      act("You roll off the corpse of $N.", mount, NULL, victim, TO_CHAR);
    }
    else if (victim->mounted == IS_RIDING)
    {
      act("$n falls off $N.", victim, NULL, mount, TO_ROOM);
      act("You fall off $N.", victim, NULL, mount, TO_CHAR);
    }
    mount->mount = NULL;
    victim->mount = NULL;
    mount->mounted = IS_ON_FOOT;
    victim->mounted = IS_ON_FOOT;
  }
  if (IS_NPC(victim))
  {
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL - 1)].killed++;
    extract_char(victim, TRUE);
    return;
  }
  extract_char(victim, FALSE);
  while (victim->affected)
    affect_remove(victim, victim->affected);
  if (IS_AFFECTED(victim, AFF_POLYMORPH) && IS_AFFECTED(victim, AFF_ETHEREAL))
  {
    victim->affected_by = AFF_POLYMORPH + AFF_ETHEREAL;
  }
  else if (IS_AFFECTED(victim, AFF_POLYMORPH))
    victim->affected_by = AFF_POLYMORPH;
  else if (IS_AFFECTED(victim, AFF_ETHEREAL))
    victim->affected_by = AFF_ETHEREAL;
  else
    victim->affected_by = 0;
  REMOVE_BIT(victim->extra, TIED_UP);
  REMOVE_BIT(victim->extra, GAGGED);
  REMOVE_BIT(victim->extra, BLINDFOLDED);
  victim->itemaffect = 0;
  victim->loc_hp[0] = 0;
  victim->loc_hp[1] = 0;
  victim->loc_hp[2] = 0;
  victim->loc_hp[3] = 0;
  victim->loc_hp[4] = 0;
  victim->loc_hp[5] = 0;
  victim->loc_hp[6] = 0;
  victim->armor = 100;
  victim->position = POS_RESTING;
  victim->hit = UMAX(1, victim->hit);
  victim->mana = UMAX(1, victim->mana);
  victim->move = UMAX(1, victim->move);
  victim->hitroll = 0;
  victim->damroll = 0;
  victim->saving_throw = 0;
  victim->carry_weight = 0;
  victim->carry_number = 0;
  do_call(victim, "all");
  save_char_obj(victim);
  return;
}

void behead(CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *location;

  if (IS_NPC(victim))
    return;
  location = victim->in_room;
  stop_fighting(victim, TRUE);
  make_corpse(victim);
  extract_char(victim, FALSE);
  char_from_room(victim);
  char_to_room(victim, location);
  if (victim == NULL)
  {
    bug("Behead: Victim no longer exists.", 0);
    return;
  }
  make_part(victim, "head");
  while (victim->affected)
    affect_remove(victim, victim->affected);
  victim->affected_by = 0;
  REMOVE_BIT(victim->extra, TIED_UP);
  REMOVE_BIT(victim->extra, GAGGED);
  REMOVE_BIT(victim->extra, BLINDFOLDED);
  REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
  REMOVE_BIT(victim->extra, EXTRA_LABOUR);
  victim->itemaffect = 0;
  victim->loc_hp[0] = 0;
  victim->loc_hp[1] = 0;
  victim->loc_hp[2] = 0;
  victim->loc_hp[3] = 0;
  victim->loc_hp[4] = 0;
  victim->loc_hp[5] = 0;
  victim->loc_hp[6] = 0;
  victim->affected_by = 0;
  victim->armor = 100;
  victim->position = POS_STANDING;
  victim->hit = 1;
  victim->mana = UMAX(1, victim->mana);
  victim->move = UMAX(1, victim->move);
  victim->hitroll = 0;
  victim->damroll = 0;
  victim->saving_throw = 0;
  victim->carry_weight = 0;
  victim->carry_number = 0;
  victim->pcdata->sit_safe = 0;
  victim->fight_timer = 0;
  SET_BIT(victim->loc_hp[0], LOST_HEAD);
  SET_BIT(victim->affected_by, AFF_POLYMORPH);
  sprintf(buf, "the severed head of %s", victim->name);
  free_string(victim->morph);
  victim->morph = str_dup(buf);
  do_call(victim, "all");
  save_char_obj(victim);
  return;
}

void group_gain(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *gch;
  CHAR_DATA *lch;
  CHAR_DATA *mount;
  int xp;
  int temp;
  int members;
  int gold;
  int silver;
  int copper;

  /*
   * Monsters don't get kill xp's or alignment changes.
   * P-killing doesn't help either.
   * Dying of mortal wounds or poison doesn't give xp to anyone!
   */
  if ((IS_NPC(ch) && (mount = ch->mount) == NULL) || victim == ch)
    return;
  members = 0;
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (IS_NPC(gch))
      continue;
    if (is_same_group(gch, ch))
      members++;
  }
  if (members == 0)
  {
    bug("Group_gain: members.", members);
    members = 1;
  }
  lch = (ch->leader != NULL) ? ch->leader : ch;
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    int xp_modifier = 100;

    if (IS_NPC(gch))
      continue;
    if (!is_same_group(gch, ch))
      continue;
    xp =xp_compute(gch, victim);

    xp *= ccenter[CCENTER_EXP_LEVEL];
    xp /= 100;

    sprintf(buf2, "#RExp modifiers  #G:");
    if ((IS_EVIL(gch) && IS_GOOD(victim)) || (IS_GOOD(gch) && IS_EVIL(victim)))
    {
      xp_modifier += 25;
      strcat(buf2, " #Calignment#n");
    }
    if ((IS_EVIL(gch) && IS_EVIL(victim)) || (IS_GOOD(gch) && IS_GOOD(victim)))
    {
      xp_modifier -= 25;
      strcat(buf2, " #palignment#n");
    }
    if (gch != NULL && gch->desc != NULL)
    {
      if (gch->desc->out_compress)
      {
        xp_modifier += 25;
        strcat(buf2, " #Cmccp#n");
      }
      else if ((get_age(gch) - 17) >= 2)
      {
        xp_modifier -= 5;
        strcat(buf2, " #pmccp#n");
      }
    }
    if ((get_age(gch) - 17) < 2)  /* 4 hours worth of newbie exp. */
    {
      xp_modifier += 200;
      strcat(buf2, " #Cnewbie#n");
    }
    if ((temp = getMightMod(gch)) > 0)
    {
      xp_modifier += temp;
      strcat(buf2, " #Csize#n");
    }
    if (!IS_NPC(gch) && gch->pcdata->time_tick > 49)
    {
      xp_modifier += gch->pcdata->time_tick / 5;
      strcat(buf2, " #Ctime#n");
    }
    if (!IS_NPC(gch) && IS_SET(gch->pcdata->tempflag, TEMP_EDGE))
    {
      xp_modifier += 15;
      strcat(buf2," #Cedge#n");
    }
    if (members > 1)
    {
      strcat(buf2, " #pgrouped#n");
      xp_modifier +=40;
    }
    if (double_exp)
    {
        strcat(buf2, " #CDouble Exp#n");
	xp += xp *2;
    }
    strcat(buf2, "\n\r");
    xp = xp * xp_modifier / 100;
    if (!IS_SET(gch->act, PLR_BRIEF4))
      send_to_char(buf2, gch);
    sprintf(buf2, "#RTotal modifier #G:#n %d percent bonus\n\r", xp_modifier - 100);
    if (!IS_SET(gch->act, PLR_BRIEF4))
      send_to_char(buf2, gch);
    if ((get_age(gch) - 17) > 2 && ch->level < 3)
      xp /= 10;                 /* anti mortal exp bot thingie */
//    if (gch->exp > 2000000000)
//      xp = 0;                   /* avoid overflow */
    if (xp > ccenter[CCENTER_MAX_EXP])
      xp = number_range(ccenter[CCENTER_MAX_EXP] * 0.9, ccenter[CCENTER_MAX_EXP] * 1.1);

    if ((get_age(gch) - 17) >= 2 && !IS_SET(gch->pcdata->jflags, JFLAG_POLICY))
    {
      xp = 0;
      send_to_char("#LYou must accept the policy before you can gain more experience [#7HELP POLICY#L]#n\n\r", gch);
    }

    sprintf(buf, "You receive %d experience points.\n\r", xp);
    send_to_char(buf, gch);    

if (double_coins)
{
gold = number_range(3,6); // victim->level / 4;
silver = number_range(10,46); // im->level;
copper = number_range(200,600); //victim->level *= 10;
}
if (!double_coins)
{
gold = number_range(1,3); // victim->level / 4;
silver = number_range(5,23); // im->level;
copper = number_range(100,300); //victim->level *= 10;
}


  if (IS_NPC(victim))
  {
   sprintf(buf, "You receive %d Gold, %d Silver and %d Copper.\n\r", gold, silver, copper);
   send_to_char(buf, gch);
   gch->pcgold += gold;
   gch->pcsilver += silver;
   gch->pccopper += copper;
  }


/*
if (IS_NPC(victim))
{
level = (victim->pIndexData->level);
}

gold = (level /3 );
silver = level;
copper = (level *=3);


    sprintf(buf, "You receive %d Gold, %d Silver and %d Copper.\n\r", gold, silver, copper);
    send_to_char(buf, gch);

    ch->pcgold += gold;
    ch->pcsilver += silver;
    ch->pccopper += copper;


*/

    if ((mount = gch->mount) != NULL)
      send_to_char(buf, mount);
//    gain_exp(gch, xp);
    gain_level(gch, xp);
  }
  return;
}

int xp_compute(CHAR_DATA * gch, CHAR_DATA * victim)
{
  double xp;


  if (victim->level < 100)
    xp = 300 - URANGE(-5, 3 - (victim->level * 5 / 3), 6) * 50;
  else
    xp = 300 - URANGE(-10, 3 - (victim->level * 5 / 3), 6) * 50;

  /*
   * Adjust for popularity of target:
   *   -1/8 for each target over  'par' (down to -100%)
   *   +1/8 for each target under 'par' (  up to + 25%)
   */
  xp -= xp * number_range(-2, 2) / 8;
  xp = number_range(xp * 3 / 4, xp * 5 / 4);
  xp = UMAX(0, xp);
  xp = (xp * (victim->level) * 0.60);
  xp = xp / 2;                  /* Put in cause players compaling to much exp :P */
  if (!IS_NPC(gch))
  {
    gch->pcdata->score[SCORE_TOTAL_LEVEL] += victim->level;
    if (victim->level > gch->pcdata->score[SCORE_HIGH_LEVEL])
      gch->pcdata->score[SCORE_HIGH_LEVEL] += victim->level;
    gch->pcdata->score[SCORE_TOTAL_XP] += xp;
    if (xp > gch->pcdata->score[SCORE_HIGH_XP])
      gch->pcdata->score[SCORE_HIGH_XP] += xp;
  }
/*

gold = number_range(1,3); // victim->level / 4;
silver = number_range(5,23); // im->level;
copper = number_range(100,300); //victim->level *= 10;


  if (IS_NPC(victim))
  {
   sprintf(buf, "You receive %d Gold, %d Silver and %d Copper.\n\r", gold, silver, copper);
   send_to_char(buf, gch);
   gch->pcgold += gold;
   gch->pcsilver += silver;
   gch->pccopper += copper;
  }
*/
  if (xp < 0)
    xp = 0;
  if (gch->exp < 0) gch->exp = 0;
  if (xp > 0 && xp < ccenter[CCENTER_MIN_EXP])
    xp = number_range(ccenter[CCENTER_MIN_EXP] * 0.9, ccenter[CCENTER_MIN_EXP] * 1.1);
  if (IS_NPC(victim) && (IS_SET(victim->act, ACT_NOEXP)))
    return 0;
  return (int) xp;
}

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int hit_loc )
{
  static char *const attack_table[] = {
    "stab", "cut", "stabs", "pierce", "thwack", "slash",
    "slice", "slash", "slash", "hack", "crush", "lash", "pound",
    "smash", "smash", "stab", "thwack", "slash", "hit"
  };
  static char *const attack_table2[] = {
    "stabs", "cuts", "stabs", "pierces", "thwacks", "slashes",
    "slices", "slashes", "slashes", "hacks", "crushes", "lashes", "pounds"
    "smashes", "smashes", "stabs", "thwacks", "slashes", "hits"
  };
  char buf1[512], buf2[512], buf3[512];
  const char *vs;
  const char *vp;
  const char *attack;
  const char *attack2;
  const char *loc;
  int damp;
  int bodyloc;
  bool critical = FALSE;
  char punct;
  OBJ_DATA *obj;

  if (dam == 0)
  {
    vs = " miss";
    vp = " misses";
  }
  else if (dam <= 50)
  {
    vs = " #Clightly#n";
    vp = " #Clightly#n";
  }
  else if (dam <= 250)
  {
    vs = " #yhard#n";
    vp = " #yhard#n";
  }
  else if (dam <= 500)
  {
    vs = " #Lvery hard#n";
    vp = " #Lvery hard#n";
  }
  else if (dam <= 750)
  {
    vs = " #Gextremely hard!#n";
    vp = " #Gextremely hard!#n";
  }
  else if (dam <= 1250)
  {
    vs = " #7incredibly hard!#n";
    vp = "#7 incredibly hard!#n";
  }
  else if (dam <= 2000)
  {
    vs = " #rshredding flesh!#n";
    vp = " #rshredding flesh!#n";
  }
  else if (dam <= 3000)
  {
    vs = " #osplintering bone!#n";
    vp = " #osplintering bone!#n";
  }
  else if (dam <= 4500)
  {
    vs = " #Rspraying blood like a fine red mist!#n";
    vp = " #Rspraying blood like a fine red mist!#n";
  }
  else if (dam <= 6000)
  {
    vs = " #yso hard it hurts just to see!#n";
    vp = " #yso hard it hurts just to see!#n";
  }
  else if (dam <= 8000)
  {
    vs = " #G<#y*#L{#R*#L}#y*#G> #sextracting organs #G<#y*#L{#R*#L}#y*#G>#n";
    vp = " #G<#y*#L{#R*#L}#y*#G> #sextracting organs #G<#y*#L{#R*#L}#y*#G>#n";
  }
  else
  {
    vs = " #R()#G()#R() #CHumiliatingly Hard #R()#G()#R()#n";
    vp = " #R()#G()#R() #CHumiliatingly Hard #R()#G()#R()#n";
  }

 switch(hit_loc)
  {
    case 1:
      if ( number_range(1,2) == 1 ) obj = get_eq_char(victim,WEAR_FINGER_L);
      else obj = get_eq_char(victim,WEAR_FINGER_R);
      loc = number_range(1,2) == 1 ? "left hand fingers" : "right hand fingers";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	break;

    case 2:
      if ( number_range(1,2) == 1 ) obj = get_eq_char(victim,WEAR_NECK_1);
      else obj = get_eq_char(victim,WEAR_NECK_2);
      loc = number_range(1,2) == 1 ? "neck" : "throat";
      if (obj != NULL) dam *= (obj->toughness / 100 );
      break;

    case 3:
      obj = get_eq_char(victim,WEAR_BODY);
      switch(number_range(1,5)){
	case 1:
	  loc = "right shoulder";
          if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "left shoulder";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "back";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 4:
	  loc = "chest";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 5:
	  loc = "stomach";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 4:
      obj = get_eq_char(victim,WEAR_HEAD);
      switch(number_range(1,3)){
	case 1:
	  loc = "head";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "left ear";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "right ear";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 5:
      obj = get_eq_char(victim,WEAR_LEGS);
      switch(number_range(1,4)){
	case 1:
	  loc = "left calf";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "left thigh";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "right calf";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 4:
	  loc = "right thigh";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 6:
      obj = get_eq_char(victim,WEAR_FEET);
      switch(number_range(1,4)){
	case 1:
	  loc = "left foot";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "left ankle";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "right foot";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 4:
	  loc = "right ankle";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 7:
      obj = get_eq_char(victim,WEAR_HANDS);
      switch(number_range(1,2)){
	case 1:
	  loc = "left hand";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "right hand";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 8:
      obj = get_eq_char(victim,WEAR_ARMS);
      switch(number_range(1,4)){
	case 1:
	  loc = "left bicep";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "left elbow";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "right bicep";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 4:
	  loc = "right elbow";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;

    case 9:
      obj = get_eq_char(victim,WEAR_WAIST);
      loc = "waist";
      if (obj != NULL) dam *= (obj->toughness / 100 );
      break;

    case 10:
      if ( number_range(1,2) == 1 ){
	obj = get_eq_char(victim,WEAR_WRIST_L);
	loc = number_range(1,2) == 1 ? "left wrist" : "left forearm";
      if (obj != NULL) dam *= (obj->toughness / 100 );
      }
      else {
	obj = get_eq_char(victim,WEAR_WRIST_R);
	loc = number_range(1,2) == 1 ? "right wrist" : "right forearm";
      if (obj != NULL) dam *= (obj->toughness / 100 );
      }
      break;

    case 11:
      obj = get_eq_char(victim,WEAR_FACE);
      switch(number_range(1,4)){
	case 1:
	  loc = "face";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 2:
	  loc = "eyes";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 3:
	  loc = "nose";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
	case 4:
	  loc = "jaw";
      if (obj != NULL) dam *= (obj->toughness / 100 );
	  break;
      }
      break;
  }

  if (!loc) loc = "(no loc)";

  /* If victim's hp are less/equal to 'damp', attacker gets a death blow */
  if (IS_NPC(victim))
    damp = 0;
  else
    damp = -10;
  if ((victim->hit - dam > damp) || (dt >= 0 && dt < MAX_SKILL) || (IS_NPC(victim) && IS_SET(victim->act, ACT_NOPARTS)))
  {
    punct = (dam <= 250) ? ' ' : ' ';
    if ( dt == TYPE_HIT && !IS_NPC(ch) && !IS_VAMPAFF(ch,VAM_CLAWS) )
    {
      if (dam == 0)
      {
        sprintf( buf1, "$n %s $N's %s #g>#G>#7%d#G<#g<#n", vp, loc, dam );
        sprintf( buf2, "You %s $N's %s #g>#G>#7%d#G<#g<#n", vs, loc, dam );
        sprintf( buf3, "$n %s your %s", vp, loc );

      }
      else
      {
        sprintf( buf1, "$n %s $N's %s #g>#G>#7%d#G<#g<#n", vp, loc, dam );
        sprintf( buf2, "You %s $N's %s #g>#G>#7%d#G<#g<#n", vs, loc, dam );
        sprintf( buf3, "$n %s your %s #g>#G>#7%d#G<#g<#n", vp, loc, dam );
        critical = TRUE;
      }
    }
    {
      if (dt >= 0 && dt < MAX_SKILL)
      {
        attack = skill_table[dt].noun_damage;
        attack2 = skill_table[dt].noun_damage;
      }
      else if (dt >= TYPE_HIT && dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0]))
      {
        attack = attack_table[dt - TYPE_HIT];
        attack2 = attack_table2[dt - TYPE_HIT];
      }
      else
      {
        dt = TYPE_HIT;
        attack = attack_table[0];
        attack2 = attack_table2[0];
      }
      if (dam == 0)
      {
        sprintf( buf1, "$n's %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
        sprintf( buf2, "Your %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
        sprintf( buf3, "$n's %s hits your %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
       }
      else
      {
        if (dt >= 0 && dt < MAX_SKILL)
        {
        sprintf( buf1, "$n's %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack2, loc, vp, dam );
        sprintf( buf2, "Your %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
        sprintf( buf3, "$n's %s hits your %s,%s #g>#G>#7%d#G<#g<#n",  attack2, loc, vp, dam );

	}        
        else
        {
        sprintf( buf1, "$n's %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
        sprintf( buf2, "Your %s hits $N's %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );
        sprintf( buf3, "$n's %s hits your %s,%s #g>#G>#7%d#G<#g<#n",  attack, loc, vp, dam );

          critical = TRUE;
        
}
      }
    }

    /*
     * Message to room.
     */
    act_brief(buf1, ch, NULL, victim, TO_NOTVICT, PLR_BRIEF6);

    /*
     * Message to attacker
     */
    if (!IS_SET(ch->act, PLR_BRIEF5))
    {
      if (!(IS_SET(ch->act, PLR_BRIEF2) && (dam == 0 || dt == skill_lookup("lightning bolt") ||
                                            dt == skill_lookup("acid blast") || dt == skill_lookup("chill touch") || dt == skill_lookup("fireball"))))
      {
        if (!(dt == skill_lookup("chaos blast") && !IS_NPC(victim)))
          act(buf2, ch, NULL, victim, TO_CHAR);
      }
    }

    /*
     * Message to victim
     */
    if (!IS_SET(victim->act, PLR_BRIEF5))
    {
      if (!(IS_SET(victim->act, PLR_BRIEF2) && (dam == 0 || dt == skill_lookup("lightning bolt") ||
                                                dt == skill_lookup("acid blast") || dt == skill_lookup("chill touch") || dt == skill_lookup("fireball"))))
        act(buf3, ch, NULL, victim, TO_VICT);
    }

    if (critical)
      critical_hit(ch, victim, dt, dam);
    return;
  }
  if (dt == TYPE_HIT && !IS_NPC(ch))
  {
    damp = number_range(1, 5);
    if (damp == 1)
    {
      act("You ram your fingers into $N's eye sockets and rip $S face off.", ch, NULL, victim, TO_CHAR);
      act("$n rams $s fingers into $N's eye sockets and rips $S face off.", ch, NULL, victim, TO_NOTVICT);
      act("$n rams $s fingers into your eye sockets and rips your face off.", ch, NULL, victim, TO_VICT);
      make_part(victim, "face");
    }
    else if (damp == 2)
    {
      act("You grab $N by the throat and tear $S windpipe out.", ch, NULL, victim, TO_CHAR);
      act("$n grabs $N by the throat and tears $S windpipe out.", ch, NULL, victim, TO_NOTVICT);
      act("$n grabs you by the throat and tears your windpipe out.", ch, NULL, victim, TO_VICT);
      make_part(victim, "windpipe");
    }
    else if (damp == 3)
    {
      act("You punch your fist through $N's stomach and rip out $S entrails.", ch, NULL, victim, TO_CHAR);
      act("$n punches $s fist through $N's stomach and rips out $S entrails.", ch, NULL, victim, TO_NOTVICT);
      act("$n punches $s fist through your stomach and rips out your entrails.", ch, NULL, victim, TO_VICT);
      make_part(victim, "entrails");
    }
    else if (damp == 4)
    {
      if (!IS_BODY(victim, BROKEN_SPINE))
        SET_BIT(victim->loc_hp[1], BROKEN_SPINE);
      act("You hoist $N above your head and slam $M down upon your knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_CHAR);
      act("$n hoists $N above $s head and slams $M down upon $s knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_NOTVICT);
      act("$n hoists you above $s head and slams you down upon $s knee.\n\rThere is a loud cracking sound as your spine snaps.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 5)
    {
      act("You lock your arm around $N's head, and give it a vicious twist.", ch, NULL, victim, TO_CHAR);
      act("$n locks $s arm around $N's head, and gives it a vicious twist.", ch, NULL, victim, TO_NOTVICT);
      act("$n locks $s arm around your head, and gives it a vicious twist.", ch, NULL, victim, TO_VICT);
      if (!IS_BODY(victim, BROKEN_NECK))
      {
        act("There is a loud snapping noise as your neck breaks.", victim, NULL, NULL, TO_CHAR);
        act("There is a loud snapping noise as $n's neck breaks.", victim, NULL, NULL, TO_ROOM);
        SET_BIT(victim->loc_hp[1], BROKEN_NECK);
      }
    }
    return;
  }
  if (dt >= 0 && dt < MAX_SKILL)
    attack = skill_table[dt].noun_damage;
  else if (dt >= TYPE_HIT && dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0]))
    attack = attack_table[dt - TYPE_HIT];
  else
  {
    dt = TYPE_HIT;
    attack = attack_table[18];
  }
  if (!str_cmp(attack,"slash") || !str_cmp(attack,"slice"))
  {
    damp = number_range(1, 8);
    if (damp == 1)
    {
      act("You swing your blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_CHAR);
      act("$n swings $s blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_NOTVICT);
      act("$n swings $s blade in a low arc, rupturing your abdominal cavity.\n\rYour entrails spray out over a wide area.", ch, NULL, victim, TO_VICT);
      make_part(victim, "entrails");
    }
    else if (damp == 2)
    {
      act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
      act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
      act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 3)
    {
      if (!IS_BODY(victim, CUT_THROAT))
        SET_BIT(victim->loc_hp[1], CUT_THROAT);
      if (!IS_BLEEDING(victim, BLEEDING_THROAT))
        SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
      act("Your blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_CHAR);
      act("$n's blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_NOTVICT);
      act("$n's blow slices open your carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_VICT);
      make_part(victim, "blood");
    }
    else if (damp == 4)
    {
      if (!IS_BODY(victim, CUT_THROAT))
        SET_BIT(victim->loc_hp[1], CUT_THROAT);
      if (!IS_BLEEDING(victim, BLEEDING_THROAT))
        SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
      act("You swing your blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
      act("$n swings $s blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
      act("$n swings $s blade across your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
      make_part(victim, "blood");
    }
    else if (damp == 5)
    {
      if (!IS_HEAD(victim, BROKEN_SKULL))
      {
        act("You swing your blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s blade down upon your head, splitting it open.\n\rYour brains pour out of your forehead.", ch, NULL, victim, TO_VICT);
        make_part(victim, "brain");
        SET_BIT(victim->loc_hp[0], BROKEN_SKULL);
      }
      else
      {
        act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
        act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
        act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
      }
    }
    else if (damp == 6)
    {
      act("You swing your blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_CHAR);
      act("$n swings $s blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_NOTVICT);
      act("$n swings $s blade between your legs, nearly splitting you in half.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 7)
    {
      if (!IS_ARM_L(victim, LOST_ARM) && !IS_SET(ch->newbits, NEW_TOUGHSKIN))
      {
        act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
        make_part(victim, "arm");
        SET_BIT(victim->loc_hp[2], LOST_ARM);
        if (!IS_BLEEDING(victim, BLEEDING_ARM_L))
          SET_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
        if (IS_BLEEDING(victim, BLEEDING_HAND_L))
          REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
      }
      else if (!IS_ARM_R(victim, LOST_ARM) && !IS_SET(ch->newbits, NEW_TOUGHSKIN))
      {
        act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
        make_part(victim, "arm");
        SET_BIT(victim->loc_hp[3], LOST_ARM);
        if (!IS_BLEEDING(victim, BLEEDING_ARM_R))
          SET_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
        if (IS_BLEEDING(victim, BLEEDING_HAND_R))
          REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
      }
      else
      {
        act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
        act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
        act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
      }
    }
    else if (damp == 8)
    {
      if (!IS_LEG_L(victim, LOST_LEG))
      {
        act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
        make_part(victim, "leg");
        SET_BIT(victim->loc_hp[4], LOST_LEG);
        if (!IS_BLEEDING(victim, BLEEDING_LEG_L))
          SET_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
        if (IS_BLEEDING(victim, BLEEDING_FOOT_L))
          REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
      }
      else if (!IS_LEG_R(victim, LOST_LEG))
      {
        act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
        make_part(victim, "leg");
        SET_BIT(victim->loc_hp[5], LOST_LEG);
        if (!IS_BLEEDING(victim, BLEEDING_LEG_R))
          SET_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
        if (IS_BLEEDING(victim, BLEEDING_FOOT_R))
          REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
      }
      else
      {
        act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
        act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
        act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
      }
    }
  }
  else if (!str_cmp(attack,"stab") || !str_cmp(attack,"pierce"))
  {
    damp = number_range(1, 5);
    if (damp == 1)
    {
      act("You defty invert your weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_CHAR);
      act("$n defty inverts $s weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_NOTVICT);
      act("$n defty inverts $s weapon and plunge it point first into your chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_VICT);
      make_part(victim, "blood");
    }
    else if (damp == 2)
    {
      act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
      act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
      act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 3)
    {
      act("You thrust your weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_CHAR);
      act("$n thrusts $s weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_NOTVICT);
      act("$n thrusts $s weapon up under your jaw and through your head.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 4)
    {
      act("You ram your weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_CHAR);
      act("$n rams $s weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_NOTVICT);
      act("$n rams $s weapon through your body, pinning you to the ground.", ch, NULL, victim, TO_VICT);
    }
    else if (damp == 5)
    {
      act("You stab your weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_CHAR);
      act("$n stabs $s weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_NOTVICT);
      act("$n stabs $s weapon into your eye and out the back of your head.", ch, NULL, victim, TO_VICT);
      if (!IS_HEAD(victim, LOST_EYE_L) && number_percent() < 50)
        SET_BIT(victim->loc_hp[0], LOST_EYE_L);
      else if (!IS_HEAD(victim, LOST_EYE_R))
        SET_BIT(victim->loc_hp[0], LOST_EYE_R);
      else if (!IS_HEAD(victim, LOST_EYE_L))
        SET_BIT(victim->loc_hp[0], LOST_EYE_L);
    }
  }
  else if (!str_cmp(attack,"blast") || !str_cmp(attack,"pound") || !str_cmp(attack,"crush"))
  {
    damp = number_range(1, 3);
    bodyloc = 0;
    if (damp == 1)
    {
      act("Your blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_CHAR);
      act("$n's blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_NOTVICT);
      act("$n's blow smashes through your chest, caving in half your ribcage.", ch, NULL, victim, TO_VICT);
      if (IS_BODY(victim, BROKEN_RIBS_1))
      {
        bodyloc += 1;
        REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
      }
      if (IS_BODY(victim, BROKEN_RIBS_2))
      {
        bodyloc += 2;
        REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
      }
      if (IS_BODY(victim, BROKEN_RIBS_4))
      {
        bodyloc += 4;
        REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
      }
      if (IS_BODY(victim, BROKEN_RIBS_8))
      {
        bodyloc += 8;
        REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
      }
      if (IS_BODY(victim, BROKEN_RIBS_16))
      {
        bodyloc += 16;
        REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
      }
      bodyloc += number_range(1, 3);
      if (bodyloc > 24)
        bodyloc = 24;
      if (bodyloc >= 16)
      {
        bodyloc -= 16;
        SET_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
      }
      if (bodyloc >= 8)
      {
        bodyloc -= 8;
        SET_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
      }
      if (bodyloc >= 4)
      {
        bodyloc -= 4;
        SET_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
      }
      if (bodyloc >= 2)
      {
        bodyloc -= 2;
        SET_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
      }
      if (bodyloc >= 1)
      {
        bodyloc -= 1;
        SET_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
      }
    }
    else if (damp == 2)
    {
      act("Your blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_CHAR);
      act("$n's blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_NOTVICT);
      act("$n's blow smashes your spine, shattering it in several places.", ch, NULL, victim, TO_VICT);
      if (!IS_BODY(victim, BROKEN_SPINE))
        SET_BIT(victim->loc_hp[1], BROKEN_SPINE);
    }
    else if (damp == 3)
    {
      if (!IS_HEAD(victim, BROKEN_SKULL))
      {
        act("You swing your weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, leaking out brains.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, covering you with brains.", ch, NULL, victim, TO_NOTVICT);
        act("$n swings $s weapon down upon your head.\n\rYour head cracks open like an overripe melon, spilling your brains everywhere.", ch, NULL, victim, TO_VICT);
        make_part(victim, "brain");
        SET_BIT(victim->loc_hp[0], BROKEN_SKULL);
      }
      else
      {
        act("You hammer your weapon into $N's side, crushing bone.", ch, NULL, victim, TO_CHAR);
        act("$n hammers $s weapon into $N's side, crushing bone.", ch, NULL, victim, TO_NOTVICT);
        act("$n hammers $s weapon into your side, crushing bone.", ch, NULL, victim, TO_VICT);
      }
    }
  }
  else if (!IS_NPC(ch) && !str_cmp(attack,"bite"))
  {
    act("You sink your teeth into $N's throat and tear out $S jugular vein.\n\rYou wipe the blood from your chin with one hand.", ch, NULL, victim, TO_CHAR);
    act("$n sink $s teeth into $N's throat and tears out $S jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_NOTVICT);
    act("$n sink $s teeth into your throat and tears out your jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_VICT);
    make_part(victim, "blood");
    if (!IS_BODY(victim, CUT_THROAT))
      SET_BIT(victim->loc_hp[1], CUT_THROAT);
    if (!IS_BLEEDING(victim, BLEEDING_THROAT))
      SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
  }
  else if (!IS_NPC(ch) && !str_cmp(attack,"claw"))
  {
    damp = number_range(1, 2);
    if (damp == 1)
    {
      act("You tear out $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
      act("$n tears out $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
      act("$n tears out your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
      make_part(victim, "blood");
      if (!IS_BODY(victim, CUT_THROAT))
        SET_BIT(victim->loc_hp[1], CUT_THROAT);
      if (!IS_BLEEDING(victim, BLEEDING_THROAT))
        SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
    }
    if (damp == 2)
    {
      if (!IS_HEAD(victim, LOST_EYE_L) && number_percent() < 50)
      {
        act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
        act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
        act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
        make_part(victim, "eyeball");
        SET_BIT(victim->loc_hp[0], LOST_EYE_L);
      }
      else if (!IS_HEAD(victim, LOST_EYE_R))
      {
        act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
        act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
        act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
        make_part(victim, "eyeball");
        SET_BIT(victim->loc_hp[0], LOST_EYE_R);
      }
      else if (!IS_HEAD(victim, LOST_EYE_L))
      {
        act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
        act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
        act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
        make_part(victim, "eyeball");
        SET_BIT(victim->loc_hp[0], LOST_EYE_L);
      }
      else
      {
        act("You claw open $N's chest.", ch, NULL, victim, TO_CHAR);
        act("$n claws open $N's chest.", ch, NULL, victim, TO_NOTVICT);
        act("$n claws open $N's chest.", ch, NULL, victim, TO_VICT);
      }
    }
  }
  else if (!str_cmp(attack,"whip"))
  {
    act("You entangle $N around the neck, and squeeze out $S life.", ch, NULL, victim, TO_CHAR);
    act("$n entangles $N around the neck, and squeezes out $S life.", ch, NULL, victim, TO_NOTVICT);
    act("$n entangles you around the neck, and squeezes the life out of you.", ch, NULL, victim, TO_VICT);
    if (!IS_BODY(victim, BROKEN_NECK))
      SET_BIT(victim->loc_hp[1], BROKEN_NECK);
  }
  else if (!str_cmp(attack,"suck") || !str_cmp(attack,"grep"))
  {
    act("You place your weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_CHAR);
    act("$n places $s weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_NOTVICT);
    act("$n places $s weapon on your head and suck out your brains.", ch, NULL, victim, TO_VICT);
  }
  else
  {
    /*bug( "Dam_message: bad dt %d.", dt ); */
  }
  return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm(CHAR_DATA * ch, CHAR_DATA * victim)
{
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch) && victim->level > 2 && number_percent() > 10)
    return;
  if (!IS_IMMUNE(ch, IMM_DISARM))
    return;
  if (((obj = get_eq_char(victim, WEAR_WIELD)) == NULL) || obj->item_type != ITEM_WEAPON)
  {
    if (((obj = get_eq_char(victim, WEAR_HOLD)) == NULL) || obj->item_type != ITEM_WEAPON)
      return;
  }
  sprintf(buf, "$n disarms you!");
  ADD_COLOUR(ch, buf, WHITE);
  act(buf, ch, NULL, victim, TO_VICT);
  sprintf(buf, "You disarm $N!");
  ADD_COLOUR(ch, buf, WHITE);
  act(buf, ch, NULL, victim, TO_CHAR);
  sprintf(buf, "$n disarms $N!");
  ADD_COLOUR(ch, buf, WHITE);
  act(buf, ch, NULL, victim, TO_NOTVICT);
  obj_from_char(obj);
  if (IS_SET(obj->extra_flags, ITEM_LOYAL) && (!IS_NPC(victim)))
  {
    act("$p leaps back into your hand!", victim, obj, NULL, TO_CHAR);
    act("$p leaps back into $n's hand!", victim, obj, NULL, TO_ROOM);
    obj_to_char(obj, victim);
    do_wear(victim, obj->name);
  }
  else if (IS_NPC(victim))
    obj_to_char(obj, victim);
  else
    obj_to_room(obj, victim->in_room);
  return;
}

/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_AFFECTED(victim, AFF_FLYING))
    return;
  if (IS_NPC(ch) && victim->level > 2 && number_percent() > 5)
    return;
  if (victim->wait == 0)
  {
    sprintf(buf, "$n trips you and you go down!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf, ch, NULL, victim, TO_VICT);
    sprintf(buf, "You trip $N and $E goes down!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n trips $N and $E goes down!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
    victim->position = POS_RESTING;
  }
  return;
}

void do_kill(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);
  if (IS_NPC(ch) && ch->desc == NULL)
    return;
  if (arg[0] == '\0')
  {
    send_to_char("Kill whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("You cannot kill yourself!\n\r", ch);
    return;
  }
  if (is_safe(ch, victim))
    return;
  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (ch->position == POS_FIGHTING)
  {
    send_to_char("You do the best you can!\n\r", ch);
    return;
  }
//  WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
  WAIT_STATE(ch, 2);
  if (!IS_NPC(ch) && !IS_NPC(victim))
  {
    ch->fight_timer += 3;
    victim->fight_timer += 3;
  }
  multi_hit(ch, victim, TYPE_UNDEFINED);
  return;
}

void do_backstab(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Backstab whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("How can you sneak up on yourself?\n\r", ch);
    return;
  }
  if (is_safe(ch, victim))
    return;
  if (((obj = get_eq_char(ch, WEAR_WIELD)) == NULL || obj->value[3] != 11) && ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || obj->value[3] != 11))
  {
    send_to_char("You need to wield a piercing weapon.\n\r", ch);
    return;
  }
  if (victim->fighting != NULL)
  {
    send_to_char("You can't backstab a fighting person.\n\r", ch);
    return;
  }
  if (victim->hit < victim->max_hit)
  {
    act("$N is hurt and suspicious ... you can't sneak up.", ch, NULL, victim, TO_CHAR);
    return;
  }
  WAIT_STATE(ch, skill_table[gsn_backstab].beats);
  if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_BACKSTAB))
    damage(ch, victim, 0, gsn_backstab);
  else if (!IS_AWAKE(victim) || IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_backstab])
    multi_hit(ch, victim, gsn_backstab);
  else
    damage(ch, victim, 0, gsn_backstab);
  return;
}

void do_flee(CHAR_DATA * ch, char *argument)
{
  ROOM_INDEX_DATA *was_in;
  ROOM_INDEX_DATA *now_in;
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];    
  int attempt;

  WAIT_STATE(ch, 4);            /* to prevent spam fleeing */

  if (IS_EXTRA(ch, TIED_UP))
  {
    send_to_char("Not while tied up.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    if (ch->position == POS_FIGHTING)
      ch->position = POS_STANDING;
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }
  if (ch->move <= 0)
  {
    send_to_char("You can't find the energy to leave.\n\r", ch);
    return;
  }
  if (IS_SET(ch->flag2, AFF_TOTALBLIND))
  {
    send_to_char("You are unable to see anything, let alone flee!\n\r", victim);
    return;
  }
  if (IS_RACE(victim, RACE_GNOLL) && IS_SET(victim->newbits,NEW_JAWLOCK) && number_percent( ) > 30)
  {
    sprintf(buf,"Not with %s's jaws clamped on your neck!",victim->name);
    send_to_char(buf,ch);
    act("$n cant escape $N's clamped jaws!",ch,NULL,victim,TO_ROOM);
    return;
  }
  was_in = ch->in_room;
  for (attempt = 0; attempt < 6; attempt++)
  {
    EXIT_DATA *pexit;
    int door;

    door = number_door();
    if ((pexit = was_in->exit[door]) == 0 || pexit->to_room == NULL || IS_SET(pexit->exit_info, EX_CLOSED))
      continue;
    move_char(ch, door);
    if ((now_in = ch->in_room) == was_in)
      continue;
    ch->in_room = was_in;
    act("$n has fled!", ch, NULL, NULL, TO_ROOM);
    ch->in_room = now_in;
    if (!IS_NPC(ch))
      send_to_char("You flee from combat!  Coward!\n\r", ch);
    stop_fighting(ch, TRUE);
    return;
  }
  send_to_char("You were unable to escape!\n\r", ch);
  return;
}

void do_rescue(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *fch;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Rescue whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("What about fleeing instead?\n\r", ch);
    return;
  }
  if (!IS_NPC(ch) && IS_NPC(victim))
  {
    send_to_char("Doesn't need your help!\n\r", ch);
    return;
  }
  if (ch->fighting == victim)
  {
    send_to_char("Too late.\n\r", ch);
    return;
  }
  if ((fch = victim->fighting) == NULL)
  {
    send_to_char("That person is not fighting right now.\n\r", ch);
    return;
  }
  if (is_safe(ch, fch) || is_safe(ch, victim))
    return;
  WAIT_STATE(ch, skill_table[gsn_rescue].beats);
  if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_rescue])
  {
    send_to_char("You fail the rescue.\n\r", ch);
    return;
  }
  act("You rescue $N!", ch, NULL, victim, TO_CHAR);
  act("$n rescues you!", ch, NULL, victim, TO_VICT);
  act("$n rescues $N!", ch, NULL, victim, TO_NOTVICT);
  stop_fighting( fch, FALSE );
  stop_fighting( victim, FALSE );
  set_fighting( ch, fch );
  set_fighting(fch, ch );
  return;
}

void do_kick(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  int dam;
  int stance;

  if (!IS_NPC(ch) && ch->level < skill_table[gsn_kick].skill_level)
  {
    send_to_char("First you should learn to kick.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, skill_table[gsn_kick].beats);
  if ((IS_NPC(ch) && ch->level < 1800))
  {
    dam = 500;
    damage(ch, victim, dam, gsn_kick);
    return;
  }
  if ((IS_NPC(ch)) || number_percent() < ch->pcdata->learned[gsn_kick])
    dam = number_range(1, 4);
  else
  {
    dam = 0;
    damage(ch, victim, dam, gsn_kick);
    return;
  }
  dam += char_damroll(ch);
  if (dam == 0)
    dam = 1;
  if (!IS_AWAKE(victim))
    dam *= 2;
  if (IS_NPC(ch))
    dam *= 100;
  if (!IS_NPC(ch))
    dam = dam + (dam * ((ch->wpn[0] + 1) / 100));
  if (!IS_NPC(ch))
  {
    stance = ch->stance[0];
    if (IS_STANCE(ch, STANCE_NORMAL))
      dam *= 1.25;
    else
      dam = dambonus(ch, victim, dam, stance);
  }
  if (!IS_NPC(ch) && !IS_NPC(victim) && dam > 750)
    dam = 750;
  if (dam <= 0)
    dam = 2;
  dam = cap_dam(ch, victim, dam);
  if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_KICK))
    damage(ch, victim, 0, gsn_kick);
  else
    damage(ch, victim, dam, gsn_kick);
  return;
}

int dambonus(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int stance)
{
  if (dam < 1)
    return 0;
  if (stance < 1)
    return dam;
  if (!IS_NPC(ch) && !can_counter(victim))
  {
    if (IS_STANCE(ch, STANCE_MONKEY))
    {
      int mindam = dam * 0.25;

      dam *= (ch->stance[STANCE_MONKEY] + 1) / 200;
      if (dam < mindam)
        dam = mindam;
    }
    else if (IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100)
      dam += dam * (ch->stance[STANCE_BULL] / 100);
    else if (IS_STANCE(ch, STANCE_DRAGON) && ch->stance[STANCE_DRAGON] > 100)
      dam += dam * (ch->stance[STANCE_DRAGON] / 100);
    else if (IS_STANCE(ch, STANCE_WOLF) && ch->stance[STANCE_WOLF] > 100)
      dam += dam * (ch->stance[STANCE_WOLF] / 100);
    else if (IS_STANCE(ch, STANCE_TIGER) && ch->stance[STANCE_TIGER] > 100)
      dam += dam * (ch->stance[STANCE_TIGER] / 100);
/* ------------ This is the part for superstances, Jobo ------------------- */
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_3) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 3 * (ch->stance[(ch->stance[0])] / 100);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_2) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * 2 * (ch->stance[(ch->stance[0])] / 100);
    else if (ch->stance[0] > 12 && IS_SET(ch->stance[(ch->stance[0] + 6)], STANCEPOWER_DAMAGE_1) && ch->stance[(ch->stance[0])] > 100)
      dam += dam * (ch->stance[(ch->stance[0])] / 100);
/* ------------ This is the end for superstances, Jobo ------------------- */

    else if (ch->stance[0] > 0 && ch->stance[stance] < 100)
      dam *= 0.5;
  }
  if (!IS_NPC(victim) && !can_counter(ch))
  {
    if (IS_STANCE(victim, STANCE_CRAB) && victim->stance[STANCE_CRAB] > 100)
      dam /= victim->stance[STANCE_CRAB] / 100;
    else if (IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100)
      dam /= victim->stance[STANCE_DRAGON] / 100;
    else if (IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100)
      dam /= victim->stance[STANCE_DRAGON] / 100;
    else if (IS_STANCE(victim, STANCE_SWALLOW) && victim->stance[STANCE_SWALLOW] > 100)
      dam /= victim->stance[STANCE_SWALLOW] / 100;
/* ------------ This is the part for superstances, Jobo ------------------- */
    else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_3) && victim->stance[(victim->stance[0])] > 100)
      dam /= 3 * (victim->stance[(victim->stance[0])] / 100);
    else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_2) && victim->stance[(victim->stance[0])] > 100)
      dam /= 2 * (victim->stance[(victim->stance[0])] / 100);
    else if (victim->stance[0] > 12 && IS_SET(victim->stance[(victim->stance[0] + 6)], STANCEPOWER_RESIST_1) && victim->stance[(victim->stance[0])] > 100)
      dam /= (victim->stance[(victim->stance[0])] / 100);
/* ------------ This is the end for superstances, Jobo ------------------- */
  }
  return dam;
}

void do_punch(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int dam;

  one_argument(argument, arg);
  if (IS_NPC(ch))
    return;
  if (ch->level < skill_table[gsn_punch].skill_level)
  {
    send_to_char("First you should learn to punch.\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (ch == victim)
  {
    send_to_char("You cannot punch yourself!\n\r", ch);
    return;
  }
  if (is_safe(ch, victim))
    return;
  if (victim->hit < victim->max_hit)
  {
    send_to_char("They are hurt and suspicious.\n\r", ch);
    return;
  }
  if (victim->position < POS_FIGHTING)
  {
    send_to_char("You can only punch someone who is standing.\n\r", ch);
    return;
  }
  act("You draw your fist back and aim a punch at $N.", ch, NULL, victim, TO_CHAR);
  act("$n draws $s fist back and aims a punch at you.", ch, NULL, victim, TO_VICT);
  act("$n draws $s fist back and aims a punch at $N.", ch, NULL, victim, TO_NOTVICT);
  WAIT_STATE(ch, skill_table[gsn_punch].beats);
  if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_punch])
    dam = number_range(1, 4);
  else
  {
    dam = 0;
    damage(ch, victim, dam, gsn_punch);
    return;
  }
  dam += char_damroll(ch);
  if (dam == 0)
    dam = 1;
  if (!IS_AWAKE(victim))
    dam *= 2;
  if (!IS_NPC(ch))
    dam = dam + (dam * (ch->wpn[0] / 100));
  if (dam <= 0)
    dam = 1;
  if (dam > 1000)
    dam = 1000;

  damage(ch, victim, dam, gsn_punch);
  if (victim == NULL || victim->position == POS_DEAD || dam < 1)
    return;
  if (victim->position == POS_FIGHTING)
    stop_fighting(victim, TRUE);
  if (number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE))
  {
    act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
    act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
    SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
  }
  else if (number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW))
  {
    act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
    act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
    SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
  }
  act("You fall to the ground stunned!", victim, NULL, NULL, TO_CHAR);
  act("$n falls to the ground stunned!", victim, NULL, NULL, TO_ROOM);
  victim->position = POS_STUNNED;
  if (dam > 1000)
    dam = 1000;
  return;
}

void do_berserk(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *mount;
  int number_hit = 0;

  argument = one_argument(argument, arg);
  if (IS_NPC(ch))
    return;
  if (ch->level < skill_table[gsn_berserk].skill_level)
  {
    send_to_char("You are not wild enough to go berserk.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, 5);
  if (number_percent() > ch->pcdata->learned[gsn_berserk])
  {
    act("You rant and rave, but nothing much happens.", ch, NULL, NULL, TO_CHAR);
    act("$n gets a wild look in $s eyes, but nothing much happens.", ch, NULL, NULL, TO_ROOM);
    return;
  }
  act("You go BERSERK!", ch, NULL, NULL, TO_CHAR);
  act("$n goes BERSERK!", ch, NULL, NULL, TO_ROOM);
  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;
    if (number_hit > 4)
      continue;
    if (vch->in_room == NULL)
      continue;
    if (!IS_NPC(vch))
      continue;
    if (ch == vch)
      continue;
    if (vch->in_room == ch->in_room)
    {
      if ((mount = ch->mount) != NULL)
        if (mount == vch)
          continue;
      if (can_see(ch, vch))
      {
        multi_hit(ch, vch, TYPE_UNDEFINED);
        number_hit++;
      }
    }
  }
  return;
}

void do_berserk2(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *mount;
  int number_hit = 0;

  argument = one_argument(argument, arg);
  if (IS_NPC(ch))
    return;
  if (ch->level < skill_table[gsn_berserk].skill_level)
  {
    send_to_char("You are not wild enough to go berserk.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, skill_table[gsn_berserk].beats);
  if (number_percent() > ch->pcdata->learned[gsn_berserk])
  {
    act("You rant and rave, but nothing much happens.", ch, NULL, NULL, TO_CHAR);
    act("$n gets a wild look in $s eyes, but nothing much happens.", ch, NULL, NULL, TO_ROOM);
    return;
  }
  act("You go BERSERK!", ch, NULL, NULL, TO_CHAR);
  act("$n goes BERSERK!", ch, NULL, NULL, TO_ROOM);
  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;
    if (number_hit > 4)
      continue;
    if (vch->in_room == NULL)
      continue;
    if (!IS_NPC(vch) && vch->pcdata->chobj != NULL)
      continue;
    if (ch == vch)
      continue;
    if (vch->in_room == ch->in_room)
    {
      if ((mount = ch->mount) != NULL)
        if (mount == vch)
          continue;
      if (can_see(ch, vch))
      {
        multi_hit(ch, vch, TYPE_UNDEFINED);
        number_hit++;
      }
    }
  }
  return;
}

/* Hurl skill by KaVir */
void do_hurl(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  CHAR_DATA *mount;
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit;
  EXIT_DATA *pexit_rev;
  char buf[MAX_INPUT_LENGTH];
  char direction[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int door;
  int rev_dir;
  int dam;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  if (!IS_NPC(ch) && ch->pcdata->learned[gsn_hurl] < 1)
  {
    send_to_char("Maybe you should learn the skill first?\n\r", ch);
    return;
  }
  if (arg1[0] == '\0')
  {
    send_to_char("Who do you wish to hurl?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg1)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("How can you hurl yourself?\n\r", ch);
    return;
  }
  if (!IS_NPC(victim) && is_safe(ch, victim))
    return;
  if ((mount = victim->mount) != NULL && victim->mounted == IS_MOUNT)
  {
    send_to_char("But they have someone on their back!\n\r", ch);
    return;
  }
  else if ((mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
  {
    send_to_char("But they are riding!\n\r", ch);
    return;
  }
  if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_HURL))
  {
    send_to_char("You are unable to get their feet of the ground.\n\r", ch);
    return;
  }
  if (IS_NPC(victim) && victim->level > 900)
  {
    send_to_char("You are unable to get their feet of the ground.\n\r", ch);
    return;
  }
  if ((victim->hit < victim->max_hit) || (victim->position == POS_FIGHTING && victim->fighting != ch))
  {
    act("$N is hurt and suspicious, and you are unable to approach $M.", ch, NULL, victim, TO_CHAR);
    return;
  }
  WAIT_STATE(ch, skill_table[gsn_hurl].beats);
  if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_hurl])
  {
    send_to_char("You are unable to get their feet of the ground.\n\r", ch);
    multi_hit(victim, ch, TYPE_UNDEFINED);
    return;
  }
  rev_dir = 0;
  if (arg2[0] == '\0')
    door = number_range(0, 3);
  else
  {
    if (!str_cmp(arg2, "n") || !str_cmp(arg2, "north"))
      door = 0;
    else if (!str_cmp(arg2, "e") || !str_cmp(arg2, "east"))
      door = 1;
    else if (!str_cmp(arg2, "s") || !str_cmp(arg2, "south"))
      door = 2;
    else if (!str_cmp(arg2, "w") || !str_cmp(arg2, "west"))
      door = 3;
    else
    {
      send_to_char("You can only hurl people north, south, east or west.\n\r", ch);
      return;
    }
  }
  if (door == 0)
  {
    sprintf(direction, "north");
    rev_dir = 2;
  }
  if (door == 1)
  {
    sprintf(direction, "east");
    rev_dir = 3;
  }
  if (door == 2)
  {
    sprintf(direction, "south");
    rev_dir = 0;
  }
  if (door == 3)
  {
    sprintf(direction, "west");
    rev_dir = 1;
  }
  if ((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
  {
    sprintf(buf, "$n hurls $N into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    sprintf(buf, "You hurl $N into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n hurls you into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_VICT);
    dam = number_range(ch->level, (ch->level * 4));
    victim->hit = victim->hit - dam;
    update_pos(victim);
    if (IS_NPC(victim) && !IS_NPC(ch))
      ch->mkill = ch->mkill + 1;
    if (!IS_NPC(victim) && IS_NPC(ch))
      victim->mdeath = victim->mdeath + 1;
    if (victim->position == POS_DEAD)
    {
      raw_kill(victim);
      return;
    }
    return;
  }
  pexit = victim->in_room->exit[door];
  if (IS_SET(pexit->exit_info, EX_PRISMATIC_WALL))
  {
    sprintf(buf, "$n hurls $N into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    sprintf(buf, "You hurl $N into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n hurls you into the %s wall.", direction);
    act(buf, ch, NULL, victim, TO_VICT);
    dam = number_range(ch->level, (ch->level * 4));
    victim->hit = victim->hit - dam;
    update_pos(victim);
    if (IS_NPC(victim) && !IS_NPC(ch))
      ch->mkill = ch->mkill + 1;
    if (!IS_NPC(victim) && IS_NPC(ch))
      victim->mdeath = victim->mdeath + 1;
    if (victim->position == POS_DEAD)
    {
      raw_kill(victim);
      return;
    }
    return;
  }
  if (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(victim, AFF_PASS_DOOR) && !IS_AFFECTED(victim, AFF_ETHEREAL))
  {
    if (IS_SET(pexit->exit_info, EX_LOCKED))
      REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    if (IS_SET(pexit->exit_info, EX_CLOSED))
      REMOVE_BIT(pexit->exit_info, EX_CLOSED);
    sprintf(buf, "$n hoists $N in the air and hurls $M %s.", direction);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    sprintf(buf, "You hoist $N in the air and hurl $M %s.", direction);
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n hurls you %s, smashing you through the %s.", direction, pexit->keyword);
    act(buf, ch, NULL, victim, TO_VICT);
    sprintf(buf, "There is a loud crash as $n smashes through the $d.");
    act(buf, victim, NULL, pexit->keyword, TO_ROOM);
    if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir]) != NULL && pexit_rev->to_room == ch->in_room && pexit_rev->keyword != NULL)
    {
      if (IS_SET(pexit_rev->exit_info, EX_LOCKED))
        REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
      if (IS_SET(pexit_rev->exit_info, EX_CLOSED))
        REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
      if (door == 0)
        sprintf(direction, "south");
      if (door == 1)
        sprintf(direction, "west");
      if (door == 2)
        sprintf(direction, "north");
      if (door == 3)
        sprintf(direction, "east");
      char_from_room(victim);
      char_to_room(victim, to_room);
      sprintf(buf, "$n comes smashing in through the %s $d.", direction);
      act(buf, victim, NULL, pexit->keyword, TO_ROOM);
      dam = number_range(ch->level, (ch->level * 6));
      victim->hit = victim->hit - dam;
      update_pos(victim);
      if (IS_NPC(victim) && !IS_NPC(ch))
        ch->mkill = ch->mkill + 1;
      if (!IS_NPC(victim) && IS_NPC(ch))
        victim->mdeath = victim->mdeath + 1;
      if (victim->position == POS_DEAD)
      {
        raw_kill(victim);
        return;
      }
    }
  }
  else
  {
    sprintf(buf, "$n hurls $N %s.", direction);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    sprintf(buf, "You hurl $N %s.", direction);
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n hurls you %s.", direction);
    act(buf, ch, NULL, victim, TO_VICT);
    if (door == 0)
      sprintf(direction, "south");
    if (door == 1)
      sprintf(direction, "west");
    if (door == 2)
      sprintf(direction, "north");
    if (door == 3)
      sprintf(direction, "east");
    char_from_room(victim);
    char_to_room(victim, to_room);
    sprintf(buf, "$n comes flying in from the %s.", direction);
    act(buf, victim, NULL, NULL, TO_ROOM);
    dam = number_range(ch->level, (ch->level * 2));
    victim->hit = victim->hit - dam;
    update_pos(victim);
    if (IS_NPC(victim) && !IS_NPC(ch))
      ch->mkill = ch->mkill + 1;
    if (!IS_NPC(victim) && IS_NPC(ch))
      victim->mdeath = victim->mdeath + 1;
    if (victim->position == POS_DEAD)
    {
      raw_kill(victim);
      return;
    }
  }
  return;
}

void do_disarm(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int percent;

  if (!IS_NPC(ch) && ch->level < skill_table[gsn_disarm].skill_level)
  {
    send_to_char("You don't know how to disarm opponents.\n\r", ch);
    return;
  }

  if ((get_eq_char(ch, WEAR_WIELD) == NULL) && (get_eq_char(ch, WEAR_HOLD) == NULL))
  {
    send_to_char("You must wield a weapon to disarm.\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (((obj = get_eq_char(victim, WEAR_WIELD)) == NULL) && ((obj = get_eq_char(victim, WEAR_HOLD)) == NULL))
  {
    send_to_char("Your opponent is not wielding a weapon.\n\r", ch);
    return;
  }

  WAIT_STATE(ch, skill_table[gsn_disarm].beats);
  percent = number_percent() + victim->level - ch->level;
  if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM))
    send_to_char("You failed.\n\r", ch);
  else if (IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3)
    disarm(ch, victim);
  else
    send_to_char("You failed.\n\r", ch);
  return;
}

void do_sla(CHAR_DATA * ch, char *argument)
{
  send_to_char("If you want to SLAY, spell it out.\n\r", ch);
  return;
}

/* Had problems with people not dying when POS_DEAD...KaVir */
bool killperson(CHAR_DATA * victim)
{
//  DESCRIPTOR_DATA *d;
//  char buf[MAX_STRING_LENGTH];

  send_to_char("You have been KILLED!!\n\r\n\r", victim);
/*
  if (victim->level < 2)
  {
    sprintf(buf, "%s dies a horrible death and leaves Dystopia", victim->name);
    death_info(buf);
    if ((d = victim->desc) != NULL)
      close_socket2(d, FALSE);
    extract_char(victim, TRUE);
    return TRUE;
  }
*/
  victim->mdeath++;
  raw_kill(victim);
  return FALSE;
}

/* For decapitating players - KaVir */
void do_decapitate(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location;
  bool can_decap = TRUE;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  bool deathmatch = FALSE;

  if (IS_NPC(ch))
    return;
  one_argument(argument, arg);

  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your in the arena.\n\r", ch);
      return;
    }
  }
  if (arg[0] == '\0')
  {
    send_to_char("Decapitate whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (ch == victim)
  {
    send_to_char("That might be a bit tricky...\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You can only decapitate other players.\n\r", ch);
    return;
  }
  if (victim->position > 1)
  {
    send_to_char("You can only do this to mortally wounded players.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim))
    return;
  if (in_fortress(ch) && !arena2death)
  {
    fortresskill(ch, victim);
    return;
  }
  else if (in_fortress(ch) && arena2death)
  {
    deathmatch = TRUE;
    arena2 = TRUE;
    if ((location = get_room_index(ROOM_VNUM_VICTORY)) == NULL)
      return;
    char_from_room(victim);
    char_to_room(victim, location);
    char_from_room(ch);
    char_to_room(ch, location);
    ch->fight_timer = 0;
    update_bets(ch, victim, TRUE);
  }
  else if (ragnarok)
  {
    ragnarokdecap(ch, victim);
    return;
  }

  /* limit */
  if (reachedDecapLimit(ch))
  {
    send_to_char("You have reached the decap limit for your current size, you need to gain more hps.\n\r", ch);
    return;
  }

  /* blah */
  if (getMight(ch) < 150)
  {
    send_to_char("If you wish to pk, then you should move into the pk-range.\n\r", ch);
    return;
  }

  /* spamcap */
  if (!str_cmp(ch->pcdata->last_decap[0], victim->name) && str_cmp(ch->pcdata->retaliation, victim->name))
  {
    send_to_char("Don't spamcap, it's not nice, find someone else to pick on.\n\r", ch);
    return;
  }

  if (!fair_fight(ch, victim))
    can_decap = FALSE;
  if (ch->level > 6 && can_decap == FALSE)
    can_decap = TRUE;
  if (!can_decap)
  {
    behead(victim);
    dropinvis(ch);
    players_decap++;
    ch->pcdata->mean_paradox_counter++;
    ch->pcdata->bounty += number_range(60, 120);
    sprintf(buf, "#P%s #owas torn to pieces by #R%s #c(#0Paradox Counter#c)#n", victim->name, ch->name);
    death_info(buf);
    if (ch->pcdata->mean_paradox_counter > 2)
    {
      ch->pcdata->mean_paradox_counter = 0;
      do_paradox(ch, "self");
    }
    return;
  }

  if (victim->pcdata->bounty > 0)
  {
    sprintf(buf, "You recive a %d QP bounty, for killing %s.\n\r", victim->pcdata->bounty, victim->name);
    send_to_char(buf, ch);
    ch->pcdata->quest += victim->pcdata->bounty;
    victim->pcdata->bounty = 0;
  }

  /* update quest status */
  {
    QUEST_DATA *quest;
    bool found = FALSE;

    for (quest = ch->pcdata->quests; quest && !found; quest = quest->next)
    {
      if (quest->type == QT_PK)
      {
        if (quest->vnums[0] == victim->pcdata->playerid)
        {
          quest->vnums[0] = -1;
          found = TRUE;
          send_to_char("#GYou have fulfilled a quest.#n\n\r", ch);
        }
      }
    }
  }

  update_edge(ch);

  ch->exp += victim->exp / 2;
  victim->exp -= victim->exp / 2;

  send_losthead(victim);
  send_decap(ch);

  /*
   * hitpoints reward for killing
   */
  ch->hit += victim->max_hit * 0.1;
  if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;

  /*
   * Update the last decaps to prevent spamcapping.
   */
  free_string(ch->pcdata->last_decap[1]);
  ch->pcdata->last_decap[1] = str_dup(ch->pcdata->last_decap[0]);
  free_string(ch->pcdata->last_decap[0]);
  ch->pcdata->last_decap[0] = str_dup(victim->name);

  /*
   * Retaliation update
   */
  free_string(victim->pcdata->retaliation);
  victim->pcdata->retaliation = str_dup(ch->name);
  if (!str_cmp(ch->pcdata->retaliation, victim->name))
  {
    free_string(ch->pcdata->retaliation);
    ch->pcdata->retaliation = str_dup("Noone");
  }

  act("A misty white vapour pours from $N's corpse into your body.", ch, NULL, victim, TO_CHAR);
  act("A misty white vapour pours from $N's corpse into $n's body.", ch, NULL, victim, TO_NOTVICT);
  act("You double over in agony as raw energy pulses through your veins.", ch, NULL, NULL, TO_CHAR);
  act("$n doubles over in agony as sparks of energy crackle around $m.", ch, NULL, NULL, TO_NOTVICT);
  if (!deathmatch)
    ch->fight_timer += 10;
  behead(victim);
  dropinvis(ch);

  if (ch->pcdata->mean_paradox_counter > 0)
    ch->pcdata->mean_paradox_counter--;
  ch->pkill = ch->pkill + 1;
  victim->pdeath = victim->pdeath + 1;
  victim->pcdata->safe_counter = 60;
  //victim->level = 2;
  decap_message(ch, victim);
  sprintf(log_buf, "%s decapitated by %s at %d.", victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum);
  log_string(log_buf);
  players_decap++;
  ch->pcdata->bounty += number_range(30, 80);
  return;
}

void decap_message(CHAR_DATA * ch, CHAR_DATA * victim)
{
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  bool unarmed = FALSE;

  if (IS_NPC(ch) || IS_NPC(victim))
    return;
  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL)
    {
      unarmed = TRUE;
    }
  }
  if ((obj != NULL && obj->item_type != ITEM_WEAPON))
    unarmed = TRUE;

  /* The players own decap message */
  if (IS_SET(ch->pcdata->jflags, JFLAG_SETDECAP))
  {
    special_decap_message(ch, victim);
    return;
  }

  if (unarmed)
  {
    if (victim->sex == SEX_MALE)
      sprintf(buf, "#P%s #ygot his head torn off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else if (victim->sex == SEX_FEMALE)
      sprintf(buf, "#P%s #ygot her head torn off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else
      sprintf(buf, "#P%s #ygot its head torn off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  }
  else if (obj->value[3] == 1)
  {
    if (victim->sex == SEX_MALE)
      sprintf(buf, "#P%s #ygot his head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else if (victim->sex == SEX_FEMALE)
      sprintf(buf, "#P%s #ygot her head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else
      sprintf(buf, "#P%s #ygot its head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  }
  else if (obj->value[3] == 2)
  {
    if (victim->sex == SEX_MALE)
      sprintf(buf, "#P%s #ygot his heart stabbed through by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else if (victim->sex == SEX_FEMALE)
      sprintf(buf, "#P%s #ygot her heart stabbed through by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else
      sprintf(buf, "#P%s #ygot its heart stabbed through by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  }
  else if (obj->value[3] == 3)
  {
    if (victim->sex == SEX_MALE)
      sprintf(buf, "#P%s #ygot his head slashed off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else if (victim->sex == SEX_FEMALE)
      sprintf(buf, "#P%s #ygot her head slashed off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else
      sprintf(buf, "#P%s #ygot its head slashed off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  }
  else if (obj->value[3] == 4)
    sprintf(buf, "#P%s #ygot strangled by #R%s", victim->pcdata->switchname, ch->pcdata->switchname);
  else if (obj->value[3] == 5)
    sprintf(buf, "#R%s #yruns a clawed hand through #P%s #yand pulls out the heart#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else if (obj->value[3] == 6)
    sprintf(buf, "#R%s #yshoots #P%s #yseveral times and spits on the corpse#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else if (obj->value[3] == 7)
    sprintf(buf, "#R%s #ypounds #P%s #yon the head and the skull caves in#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else if (obj->value[3] == 8)
    sprintf(buf, "#R%s #ycrushes #P%s #yto a bloody pulp#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else if (obj->value[3] == 9)
    sprintf(buf, "#P%s #yhas been grepped by #R%s#y, that's just mean!#n", victim->pcdata->switchname, ch->pcdata->switchname);
  else if (obj->value[3] == 10)
    sprintf(buf, "#P%s #ywas bitten to death by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  else if (obj->value[3] == 11)
    sprintf(buf, "#R%s #yhas punctured the lungs of #P%s#y, what a meanie!#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else if (obj->value[3] == 12)
    sprintf(buf, "#R%s #ygrabs #P%s #yby the head and sucks the brain out#n", ch->pcdata->switchname, victim->pcdata->switchname);
  else
  {
    if (victim->sex == SEX_MALE)
      sprintf(buf, "#P%s #ygot his head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else if (victim->sex == SEX_FEMALE)
      sprintf(buf, "#P%s #ygot her head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
    else
      sprintf(buf, "#P%s #ygot its head sliced off by #R%s#n", victim->pcdata->switchname, ch->pcdata->switchname);
  }
  death_info(buf);
  return;
}

void do_crack(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  OBJ_DATA *right;
  OBJ_DATA *left;

  right = get_eq_char(ch, WEAR_WIELD);
  left = get_eq_char(ch, WEAR_HOLD);
  if (right != NULL && right->pIndexData->vnum == 12)
    obj = right;
  else if (left != NULL && left->pIndexData->vnum == 12)
    obj = left;
  else
  {
    send_to_char("You are not holding any heads.\n\r", ch);
    return;
  }
  act("You hurl $p at the floor.", ch, obj, NULL, TO_CHAR);
  act("$n hurls $p at the floor.", ch, obj, NULL, TO_ROOM);
  act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_CHAR);
  if (obj->chobj != NULL)
  {
    act("$p cracks open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_NOTVICT);
    act("$p crack open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_VICT);
  }
  else
  {
    act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_ROOM);
  }
  crack_head(ch, obj, obj->name);
  obj_from_char(obj);
  extract_obj(obj);
}

void crack_head(CHAR_DATA * ch, OBJ_DATA * obj, char *argument)
{
  CHAR_DATA *victim;
  MOB_INDEX_DATA *pMobIndex;

  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  if (str_cmp(arg2, "mob") && obj->chobj != NULL && !IS_NPC(obj->chobj) && IS_AFFECTED(obj->chobj, AFF_POLYMORPH))
  {
    victim = obj->chobj;
    make_part(victim, "cracked_head");
    make_part(victim, "brain");
    sprintf(buf, "the quivering brain of %s", victim->name);
    free_string(victim->morph);
    victim->morph = str_dup(buf);
    return;
  }
  else if (!str_cmp(arg2, "mob"))
  {
    if ((pMobIndex = get_mob_index(obj->value[1])) == NULL)
      return;
    victim = create_mobile(pMobIndex);
    char_to_room(victim, ch->in_room);
    make_part(victim, "cracked_head");
    make_part(victim, "brain");
    extract_char(victim, TRUE);
    return;
  }
  return;
}


bool has_timer(CHAR_DATA * ch)
{
  if (ch->fight_timer > 0 && !IS_NPC(ch))
  {
    send_to_char("Not until your fight timer runs out!\n\r", ch);
    return TRUE;
  }
  return FALSE;
}

void do_fightstyle (CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if (arg[0] == '\0')
  {
    sprintf(buf, "#CFight Styles\n\r");
    send_to_char(buf,ch);
    sprintf(buf, "#R[#0Aikido#R]#0 circular flowing movements, attempts to avoid full-body grappling whenever possible#n\n\r");
    send_to_char(buf,ch);
    sprintf(buf, "#R[#0JiuJitsu#R]#0 Art of gentleness, It applies grappling, throwing, and holds, as well as a few striking techniques #n\n\r");
    send_to_char(buf,ch);
    sprintf(buf, "#R[#0Kendo#R]#0 Art of sword combat#n\n\r");
    send_to_char(buf,ch);
  }
  else if (!str_cmp(arg, "none"))
  {
    send_to_char("You no longer assume a fightstyle.\n\r", ch);
    ch->pcstyle[FIGHTSTYLE_AUTODROP] = FIGHTSTYLE_NONE;
  }
  else if (!str_cmp(arg, "aikido"))
  {
    if ( ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
    send_to_char("Your already in Aikido Fightstyle.\n\r",ch);
    else
    send_to_char("You now use the Aikido fight style.\n\r",ch );
    ch->pcstyle[FIGHTSTYLE_AUTODROP] = FIGHTSTYLE_AIKIDO;
  }
  else if (!str_cmp(arg, "jiujitsu"))
  {
    if ( ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
    send_to_char("Your already in the Jiujitsu Fightstyle.\n\r",ch);
    else
    send_to_char("You now use the JiuJitsu fight style.\n\r",ch );
    ch->pcstyle[FIGHTSTYLE_AUTODROP] = FIGHTSTYLE_JIUJITSU;
  }
  else if (!str_cmp(arg, "kendo"))
  {
    if ( ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
    send_to_char("Your already in the Kendo Fightstyle.\n\r",ch);
    else
    send_to_char("You now use the Kendo fight style.\n\r",ch );
    ch->pcstyle[FIGHTSTYLE_AUTODROP] = FIGHTSTYLE_KENDO;
  }

  else send_to_char("You can't set your fight style to that!\n\r",ch);
return;
}


void do_autostance(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
  if (IS_NPC(ch))
    return;
  if (!str_cmp(arg, "none"))
  {
    send_to_char("You no longer autostance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_NONE;
  }
  else if (!str_cmp(arg, "crane"))
  {
    send_to_char("You now autostance into the crane stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_CRANE;
  }
  else if (!str_cmp(arg, "crab"))
  {
    send_to_char("You now autostance into the crab stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_CRAB;
  }
  else if (!str_cmp(arg, "bull"))
  {
    send_to_char("You now autostance into the bull stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_BULL;
  }
  else if (!str_cmp(arg, "viper"))
  {
    send_to_char("You now autostance into the viper stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_VIPER;
  }
  else if (!str_cmp(arg, "mongoose"))
  {
    send_to_char("You now autostance into the mongoose stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_MONGOOSE;
  }
  else if (!str_cmp(arg, "mantis") && ch->stance[STANCE_CRANE] >= 200 && ch->stance[STANCE_VIPER] >= 200)
  {
    send_to_char("You now autostance into the mantis stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_MANTIS;
  }
  else if (!str_cmp(arg, "monkey") && ch->stance[STANCE_CRANE] >= 200 && ch->stance[STANCE_MONGOOSE] >= 200)
  {
    send_to_char("You now autostance into the monkey stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_MONKEY;
  }
  else if (!str_cmp(arg, "swallow") && ch->stance[STANCE_CRAB] >= 200 && ch->stance[STANCE_MONGOOSE] >= 200)
  {
    send_to_char("You now autostance into the swallow stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SWALLOW;
  }
  else if (!str_cmp(arg, "tiger") && ch->stance[STANCE_BULL] >= 200 && ch->stance[STANCE_VIPER] >= 200)
  {
    send_to_char("You now autostance into the tiger stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_TIGER;
  }
  else if (!str_cmp(arg, "dragon") && ch->stance[STANCE_CRAB] >= 200 && ch->stance[STANCE_BULL] >= 200)
  {
    send_to_char("You now autostance into the dragon stance.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_DRAGON;
  }
  else if (!str_cmp(arg, "ss1") && ch->stance[19] != -1)
  {
    send_to_char("You now autostance into superstance one.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SS1;
  }
  else if (!str_cmp(arg, "ss2") && ch->stance[20] != -1)
  {
    send_to_char("You now autostance into superstance two.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SS2;
  }
  else if (!str_cmp(arg, "ss3") && ch->stance[21] != -1)
  {
    send_to_char("You now autostance into superstance three.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SS3;
  }
  else if (!str_cmp(arg, "ss4") && ch->stance[22] != -1)
  {
    send_to_char("You now autostance into superstance four.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SS4;
  }
  else if (!str_cmp(arg, "ss5") && ch->stance[23] != -1)
  {
    send_to_char("You now autostance into superstance five.\n\r", ch);
    ch->stance[MONK_AUTODROP] = STANCE_SS5;
  }
  else
    send_to_char("You can't set your autostance to that!\n\r", ch);
}

void autodrop2(CHAR_DATA * ch)
{
        char      buf[MAX_INPUT_LENGTH];
        char      buf2[MAX_INPUT_LENGTH];
        char      stylename[8];

        if (IS_NPC(ch))
                return;
        if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_NONE)
//                sprintf(stylename, "none");
               return;
        if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_AIKIDO)
                sprintf(stylename, "Aikido");
        else if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_JIUJITSU)
                sprintf(stylename, "JiuJitsu");
        else if (ch->pcstyle[FIGHTSTYLE_AUTODROP] == FIGHTSTYLE_KENDO)
                sprintf(stylename, "Kendo");
         else
                return;

		if (ch->pcstyle[0] < 1)
		{
                ch->pcstyle[0] = ch->pcstyle[FIGHTSTYLE_AUTODROP];;
                sprintf(buf, "#wYou assume the #Y%s#w fight style.", stylename);
                act(buf, ch, NULL, NULL, TO_CHAR);
                sprintf(buf2, "#w$n assumes the #Y%s#w fight style.",  stylename);
                act(buf2, ch, NULL, NULL, TO_ROOM);
		}

}

void autodrop(CHAR_DATA * ch)
{
  char buf[MAX_INPUT_LENGTH];
  char buf2[MAX_INPUT_LENGTH];
  char stancename[10];

  if (IS_NPC(ch))
    return;
  if (ch->stance[MONK_AUTODROP] == STANCE_NONE)
    return;
  if (ch->stance[MONK_AUTODROP] == STANCE_VIPER)
    sprintf(stancename, "viper");
  else if (ch->stance[MONK_AUTODROP] == STANCE_CRANE)
    sprintf(stancename, "crane");
  else if (ch->stance[MONK_AUTODROP] == STANCE_CRAB)
    sprintf(stancename, "crab");
  else if (ch->stance[MONK_AUTODROP] == STANCE_MONGOOSE)
    sprintf(stancename, "mongoose");
  else if (ch->stance[MONK_AUTODROP] == STANCE_BULL)
    sprintf(stancename, "bull");
  else if (ch->stance[MONK_AUTODROP] == STANCE_MANTIS)
    sprintf(stancename, "mantis");
  else if (ch->stance[MONK_AUTODROP] == STANCE_DRAGON)
    sprintf(stancename, "dragon");
  else if (ch->stance[MONK_AUTODROP] == STANCE_TIGER)
    sprintf(stancename, "tiger");
  else if (ch->stance[MONK_AUTODROP] == STANCE_MONKEY)
    sprintf(stancename, "monkey");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SWALLOW)
    sprintf(stancename, "swallow");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SS1)
    sprintf(stancename, "ss1");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SS2)
    sprintf(stancename, "ss2");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SS3)
    sprintf(stancename, "ss3");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SS4)
    sprintf(stancename, "ss4");
  else if (ch->stance[MONK_AUTODROP] == STANCE_SS5)
    sprintf(stancename, "ss5");
  else if (ch->stance[MONK_AUTODROP])
    sprintf(stancename, "wolf");
  else
    return;
  if (ch->stance[0] < 1)
  {
    ch->stance[0] = ch->stance[MONK_AUTODROP];;
    sprintf(buf, "#7You autodrop into the #y%s#7 stance.", stancename);
    act(buf, ch, NULL, NULL, TO_CHAR);
    sprintf(buf2, "#7$n autodrops into the #y%s#7 stance.", stancename);
    act(buf2, ch, NULL, NULL, TO_ROOM);
  }
}

void dropinvis(CHAR_DATA * ch)
{
  if (ch->level < 7 && IS_SET(ch->act, AFF_HIDE))
    REMOVE_BIT(ch->act, AFF_HIDE);
  if (ch->level < 7 && IS_SET(ch->act, PLR_WIZINVIS))
    REMOVE_BIT(ch->act, PLR_WIZINVIS);
  return;
}


bool pole_special (CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;
    int dtype;

    if (!IS_AWAKE (victim))
        return FALSE;

  if (dtype != WPN_POLEARM)
    return FALSE;

  chance = ch->wpn[15] / 400;

    if (!can_see (victim, ch))
        chance /= 2;
  /*
   * It's harder to keep another polearm at bay.
   */

    if (number_percent () >= chance)
        return FALSE;

    act ("You manage to keep $n out of range.",
      ch, NULL, victim, TO_VICT);
    act ("$N's weapon keeps you out of range.",
      ch, NULL, victim, TO_CHAR);

    return TRUE;
}

/*
 * Critical hit damage modifier for axes.
 */
int axe_special (CHAR_DATA * ch, CHAR_DATA * victim, int dam)
{
    int chance;
    int dtype;

  /*
   * Make sure we have an axe.
   */
  if (dtype != WPN_POLEARM)
    return dam;

  /*
   * Base is 1/5th our axe skill.  Bonus if victim can't see us, penalty
   * if we can't see them.
   */

  chance = ch->wpn[9] / 400;


    if (!can_see (victim, ch))
        chance *= 1.5;

    if (!can_see (ch, victim))
        chance /= 3;

    if (number_percent () >= chance)
        return dam;

  /*
   * We got a critical hit!
   */
    return dam;
}

