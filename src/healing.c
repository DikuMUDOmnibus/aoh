
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_heal(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_HEALING))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->mana < 750)
        {
                send_to_char("You don't have enough mana.\n\r", ch);
                return;
        }
        ch->mana = ch->mana - 750;
        ch->hit = ch->hit + ch->spl[BLUE_MAGIC] * 10;
        if (ch->hit > ch->max_hit)
                ch->hit = ch->max_hit;
        send_to_char("Xrakisis heals you.\n\r", ch);
        act("$n is slightly healed by Xrakisis.", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, 12);
        return;
}


void do_regrow (CHAR_DATA * ch, char *argument)
{

  if (IS_NPC (ch))
    return;

        if (!IS_SPHERE(ch, SPHERE_HEALING))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

  act ("$n regrows all of their lost limbs.", ch, NULL, NULL, TO_ROOM);
  act ("You regrow all of your lost limbs.", ch, NULL, NULL, TO_CHAR);
  ch->loc_hp[0] = 0;
  ch->loc_hp[1] = 0;
  ch->loc_hp[2] = 0;
  ch->loc_hp[3] = 0;
  ch->loc_hp[4] = 0;
  ch->loc_hp[5] = 0;
  ch->loc_hp[6] = 0;
  WAIT_STATE (ch, 6);

}

void do_regenerate(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_HEALING))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }


        if (ch->position == POS_FIGHTING)
        {
                send_to_char("You cannot regenerate while fighting.\n\r", ch);
                return;
        }

        if (ch->hit >= ch->max_hit && ch->mana >= ch->max_mana
            && ch->move >= ch->max_move)
        {
                send_to_char
                        ("But you are already completely regenerated!\n\r",
                         ch);
                return;
        }


        if (ch->hit >= ch->max_hit && ch->mana >= ch->max_mana
            && ch->move >= ch->max_move)
        {
                send_to_char("Your body has completely regenerated.\n\r", ch);
                act("$n's body completely regenerates itself.", ch, NULL,
                    NULL, TO_ROOM);
        }
        else
                send_to_char("Your body slowly regenerates itself.\n\r", ch);
        if (ch->hit < 1)
        {
                ch->hit = ch->hit + 1;
                update_pos(ch);
                WAIT_STATE(ch, 24);
        }
        else
        {
                ch->hit = UMIN(ch->hit + 1500, ch->max_hit);
                ch->mana = UMIN(ch->mana + 1500, ch->max_mana);
                ch->move = UMIN(ch->move + 1500, ch->max_move);
                WAIT_STATE(ch, 8);
                update_pos(ch);
        }
        return;
}



