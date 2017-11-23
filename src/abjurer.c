
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_shield( CHAR_DATA *ch, char *argument )
{
  char arg [MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;


  if (!IS_SPHERE(ch, SPHERE_ABJURER))
  {
    send_to_char("#0Shield is an Abjurer ability.#n\n\r",ch);
    return;
  }

  if (!IS_IMMUNE(ch,IMM_SHIELDED) )
  {
    send_to_char("You shield your aura from those around you.\n\r",ch);
    SET_BIT(ch->immune, IMM_SHIELDED);
    return;
  }
  send_to_char("You stop shielding your aura.\n\r",ch);
  REMOVE_BIT(ch->immune, IMM_SHIELDED);
  return;
}

void do_holyaura(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       sn, level;
        int       mcost;

        argument = one_argument(argument, arg);
        mcost = number_range(2000, 3000);

  if (!IS_SPHERE(ch, SPHERE_ABJURER))
  {
    send_to_char("#0Holy Aura is an Abjurer ability.#n\n\r",ch);
    return;
  }

        if (ch->mana < mcost)
        {
                send_to_char("You don't have enough mana for that!\n\r", ch);
                return;
        }

        if ((sn = skill_lookup("holy aura")) < 0)
                return;

        if (arg[0] == '\0')
        {
                send_to_char("Cast Holy Aura on whom?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch,  arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }


        if (is_affected(victim, skill_lookup("holy aura")))
        {
                stc("They're already affected by holy aura!\n\r", ch);
                return;
        }

        stc("You cast Holy Aura on them!\n\r", ch);
        act("$n starts glowing bright #wwhite#n.", ch, NULL, NULL, TO_ROOM);
        level = 2550;
        (*skill_table[sn].spell_fun) (sn, level, victim, victim);
        WAIT_STATE(ch, 12);
        ch->mana -= mcost;
        return;
}


void do_dimedodge( CHAR_DATA *ch, char *argument )
{
   
  if (IS_NPC(ch)) return;


  if (!IS_SPHERE(ch, SPHERE_ABJURER))
  {
    send_to_char("#0Dimension Dodge is an Abjurer ability.#n\n\r",ch);
    return;
  }
 
  if (IS_SET(ch->pcdata->powers[WRAITH_POWERS], WRAITH_DIMEDODGE))
  {
    REMOVE_BIT(ch->pcdata->powers[WRAITH_POWERS], WRAITH_DIMEDODGE);
    act( "$n loses his superior dodging",ch,NULL,NULL,TO_ROOM);
    send_to_char("Your dodging goes back to normal.\n\r",ch);
    return;
  }
    
   SET_BIT(ch->pcdata->powers[WRAITH_POWERS], WRAITH_DIMEDODGE);
    act( "$n gains superior dodging..",ch,NULL,NULL,TO_ROOM);
    send_to_char("You now can jump dimensions to dodge attacks.\n\r",ch);
    return;
}

