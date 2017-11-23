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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

char *const where_name[] = {
  "#R[#CLight#R         ]#n ",
  "#R[#COn Finger     #R]#n ",
  "#R[#COn Finger     #R]#n ",
  "#R[#CAround Neck   #R]#n ",
  "#R[#CAround Neck   #R]#n ",
  "#R[#COn Body       #R]#n ",
  "#R[#COn Head       #R]#n ",
  "#R[#COn Legs       #R]#n ",
  "#R[#COn Feet       #R]#n ",
  "#R[#COn Hands      #R]#n ",
  "#R[#COn Arms       #R]#n ",
  "#R[#CBuckler       #R]#n ",
  "#R[#CAround Body   #R]#n ",
  "#R[#CAround Waist  #R]#n ",
  "#R[#CAround Wrist  #R]#n ",
  "#R[#CAround Wrist  #R]#n ",
  "#R[#CRight Hand    #R]#n ",
  "#R[#CLeft Hand     #R]#n ",
  "#R[#CThird Hand    #R]#n ",
  "#R[#CFourth Hand   #R]#n ",
  "#R[#COn Face       #R]#n ",
  "#R[#CLeft Scabbard #R]#n ",
  "#R[#CRight Scabbard#R]#n ",
  "#R[#CSpecial       #R]#n ",
  "#R[#CFloating      #R]#n ",
  "#R[#CMedal         #R]#n ",
  "#R[#CBodyart       #R]#n ",
};

const char *exitname[6] = {
  "#Cn#0orth#n",
  "#Ce#0ast#n",
  "#Cs#0outh#n",
  "#Cw#0est#n",
  "#Cu#0p#n",
  "#Cd#0own#n"
};

const char *exitname2[6] = {
  "to the north",
  "to the east",
  "to the south",
  "to the west",
  "above you",
  "below you"
};

/*
 * Local functions.
 */
char *format_obj_to_char   args (( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort ));
void  show_char_to_char_0  args (( CHAR_DATA * victim, CHAR_DATA * ch ));
void  show_char_to_char_1  args (( CHAR_DATA * victim, CHAR_DATA * ch ));
void  show_char_to_char    args (( CHAR_DATA * list, CHAR_DATA * ch ));
void  check_left_arm       args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  check_right_arm      args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  check_left_leg       args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  check_right_leg      args (( CHAR_DATA * ch, CHAR_DATA * victim ));
void  obj_score            args (( CHAR_DATA * ch, OBJ_DATA * obj ));

char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
  static char buf[MAX_STRING_LENGTH];



  buf[0] = '\0';
    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC && 
	(obj->pIndexData->vnum != OBJ_VNUM_SLICED_LEG) && (obj->pIndexData->vnum != OBJ_VNUM_SLICED_EAR) 
	&& (obj->pIndexData->vnum != OBJ_VNUM_SLICED_NOSE))
    {
    if (obj->condition == 100)
      strcat(buf, "#r[#G+++++#r]#n ");
    else if (obj->condition >= 80)
      strcat(buf, "#r[#G++++#R+#r]#n ");
    else if (obj->condition >= 60)
      strcat(buf, "#r[#G+++#R++#r]#n ");
    else if (obj->condition >= 40)
      strcat(buf, "#r[#G++#R+++#r]#n ");
    else if (obj->condition >= 20)
      strcat(buf, "#r[#G+#R++++#r]#n ");
    else strcat(buf, "#r[#R+++++#r]#n ");
    }

//   if (obj->material != NULL)
//   {
   int material;
   int component;

   material = obj->material;
   component = obj->c_material;

if (material == M_COPPER || IS_SET(component, C_COPPER)) strcat(buf, "#0[#oCopper#0]#n ");
if (material == M_GOLD || IS_SET(component, C_GOLD))  strcat(buf, "#0[#YGold#0]#n ");
if (material == M_IVORY || IS_SET(component, C_IVORY)) strcat(buf, "#0[#7Ivory#0]#n ");
if (material == M_STEEL || IS_SET(component, C_STEEL))  strcat(buf, "#0[#7Steel#0]#n ");
if (material == M_MITHRIL || IS_SET(component, C_MITHRIL)) strcat(buf, "#0[#cM#Ci#ct#Ch#cr#Ci#cl#0]#n ");
if (material == M_WOOD || IS_SET(component, C_WOOD)) strcat(buf, "#0[#oWood#0]#n ");
if (material == M_ORGANIC  || IS_SET(component, C_ORGANIC)) strcat(buf, "#0[#7O#gr#Gg#ga#7n#gi#Gc#0]#n ");
if (material == M_DIAMOND  || IS_SET(component, C_DIAMOND)) strcat(buf, "#0[#7Diamond#0]#n ");
if (material == M_PLATINUM || IS_SET(component, C_PLATINUM)) strcat(buf, "#0[#7Platinum#0]#n ");
if (material == M_ADAMANTITE || IS_SET(component, C_ADAMANTITE)) strcat(buf, "#0[#CAdamantite#0]#n ");
if (material == M_BRASS || IS_SET(component, C_BRASS)) strcat(buf, "#0[#oBrass#0]#n ");
if (material == M_BRONZE || IS_SET(component, C_BRONZE)) strcat(buf, "#0[#oBronze#0]#n ");
if (material == M_TIN || IS_SET(component, C_TIN)) strcat(buf, "#0[Tin#0]#n ");
if (material == M_EBONY || IS_SET(component, C_EBONY)) strcat(buf, "#0[#nEbony#0]#n ");
if (material == M_RUBY || IS_SET(component, C_RUBY)) strcat(buf, "#0[#RR#ru#Rb#ry#0]#n ");
if (material == M_LIMESTONE || IS_SET(component, C_LIMESTONE)) strcat(buf, "#0[#7L#Gi#7m#Ge#7s#Gt#7o#Gn#7e#0]#n ");
if (material == M_TOPAZ || IS_SET(component, C_TOPAZ))  strcat(buf, "#0[#YTopaz#0]#n ");
if (material == M_AMETHYST || IS_SET(component, C_AMETHYST)) strcat(buf, "#0[#pAmethyst#0]#n ");
if (material == M_JADE || IS_SET(component, C_JADE)) strcat(buf, "#0[#GJade#0]#n ");
if (material == M_ONYX || IS_SET(component, C_ONYX)) strcat(buf, "#0[#nOnyx#0]#n ");
if (material == M_EMERALD || IS_SET(component, C_EMERALD)) strcat(buf, "#0[#GE#gm#Ge#gr#Ga#gl#Gd#0]#n ");
if (material == M_PEARL || IS_SET(component, C_PEARL)) strcat(buf, "#0[#7Pearl#0]#n ");
if (material == M_OPAL  || IS_SET(component, C_OPAL)) strcat(buf, "#0[#7Opal#0]#n ");
if (material == M_SAPPHIRE || IS_SET(component, C_SAPPHIRE)) strcat(buf, "#0[#LSapphire#0]#n ");
if (material == M_LEATHER || IS_SET(component, C_LEATHER)) strcat(buf, "#0[#oLeather#0]#n ");
if (material == M_IRON || IS_SET(component, C_IRON)) strcat(buf, "#0[#0Iron#0]#n ");
if (material == M_QUARTZ || IS_SET(component, C_QUARTZ)) strcat(buf, "#0[#7Quartz#0]#n ");
if (material == M_MARBLE || IS_SET(component, C_MARBLE)) strcat(buf, "#0[#CM#ca#Cr#cb#Cl#ce#0]#n ");
if (material == M_SILVER || IS_SET(component, C_SILVER)) strcat(buf, "#0[#0Silver#0]#n ");
if (material == M_WOOL || IS_SET(component, C_WOOL)) strcat(buf, "#0[#oWool#0]#n ");
if (material == M_CLOTH || IS_SET(component, C_CLOTH)) strcat(buf, "#0[#LCloth#0]#n ");
   
//}

  if (IS_SET(obj->quest, QUEST_ARTIFACT))
    strcat(buf, "#7(Artifact)#n ");
  else if (IS_SET(obj->quest, QUEST_PRIZE))
    strcat(buf, "#r(#CPrize#r)#n ");
  else if (IS_SET(obj->quest, QUEST_RELIC))
    strcat(buf, "#r(#7Relic#r)#n ");
  else if (obj->points < 750 && obj->points != 0)
    strcat(buf, "#r(Legendary)#n ");
  else if (obj->points < 1250 && obj->points != 0)
    strcat(buf, "#7(#RMythical#7)#n ");
  else if (obj->points != 0)
    strcat(buf, "#r(#yPriceless#r)#n ");
  if (IS_OBJ_STAT(obj, ITEM_GLOW))
    strcat(buf, "#y(#rGlow#y)#n ");
  if (IS_OBJ_STAT(obj, ITEM_HUM))
    strcat(buf, "#y(#rHum#y)#n ");
  if (IS_OBJ_STAT(obj, ITEM_INVIS))
    strcat(buf, "#r(Invis)#n ");
  if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
    strcat(buf, "#l(Blue Aura)#n ");
  else if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
    strcat(buf, "#r(Red Aura)#n ");
  else if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
    strcat(buf, "#y(Yellow Aura)#n ");
  if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC))
    strcat(buf, "#c(Magical)#n ");

  if (IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && !IS_AFFECTED(ch, AFF_SHADOWPLANE))
    strcat(buf, "#0(Shadowplane)#n ");
  if (!IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && IS_AFFECTED(ch, AFF_SHADOWPLANE))
    strcat(buf, "#7(Normal plane)#n ");

  if (fShort)
  {
    if (obj->short_descr != NULL)
      strcat(buf, obj->short_descr);
//    if (obj->condition < 100)
//      strcat(buf, " #r(Damaged)#n");


  }
  else
  {
    if (obj->description != NULL)
      strcat(buf, obj->description);
  }

  return buf;
}

int char_damroll(CHAR_DATA * ch)
{
  int value = 0;

  value += ch->damroll;

  if (!IS_NPC(ch))
  {
    value += str_app[get_curr_str(ch)].todam;
    value += (ch->pcLegacy * 50);
  }
  if (ch->level < LEVEL_AVATAR)
    return value;

  if (value > 7000)
    value = 7000;
  return value;
}

int char_hitroll(CHAR_DATA * ch)
{
  int value = 0;

  value += ch->hitroll;

  if (!IS_NPC(ch))
  {
    value += str_app[get_curr_str(ch)].tohit;
    value += (ch->pcLegacy * 50);
  }

  if (ch->level < LEVEL_AVATAR)
    return value;

  if (value > 7000)
    value = 7000;
  return value;
}

int char_ac(CHAR_DATA * ch)
{
  int value = 0;

  value = GET_AC(ch);

  if (IS_AWAKE(ch))
  {
    value += dex_app[get_curr_dex(ch)].defensive;
    value -= (ch->pcLegacy * 150);
  }
  if (value > 7000)
    value = 7000;
  return value;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
  char buf[MAX_STRING_LENGTH];
  char **prgpstrShow;
  int *prgnShow;
  char *pstrShow;
  OBJ_DATA *obj;
  int nShow;
  int iShow;
  int count;
  bool fCombine;

  if (ch->desc == NULL)
    return;

  /*
   * Alloc space for output lines.
   */
  count = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
    count++;
  prgpstrShow = alloc_mem(count * sizeof(char *));
  prgnShow = alloc_mem(count * sizeof(int));
  nShow = 0;

  /*
   * Format the list of objects.
   */
  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
      continue;
    if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
    {
      pstrShow = format_obj_to_char(obj, ch, fShort);
      fCombine = FALSE;

      /*
       * Look for duplicates, case sensitive.
       * Matches tend to be near end so run loop backwords.
       */
      for (iShow = nShow - 1; iShow >= 0; iShow--)
      {
        if (!strcmp(prgpstrShow[iShow], pstrShow))
        {
          prgnShow[iShow]++;
          fCombine = TRUE;
          break;
        }
      }

      /*
       * Couldn't combine, or didn't want to.
       */
      if (!fCombine)
      {
        prgpstrShow[nShow] = str_dup(pstrShow);
        prgnShow[nShow] = 1;
        nShow++;
      }
    }
  }

  /*
   * Output the formatted list.
   */
  for (iShow = 0; iShow < nShow; iShow++)
  {
    if (prgnShow[iShow] != 1)
    {
      sprintf(buf, "(%2d) ", prgnShow[iShow]);
      send_to_char(buf, ch);
    }
    else
    {
      send_to_char("     ", ch);
    }

    send_to_char(prgpstrShow[iShow], ch);
    send_to_char("\n\r", ch);
    free_string(prgpstrShow[iShow]);
  }

  if (fShowNothing && nShow == 0)
  {
    send_to_char("     ", ch);
    send_to_char("Nothing.\n\r", ch);
  }

  /*
   * Clean up.
   */
  free_mem(prgpstrShow, count * sizeof(char *));
  free_mem(prgnShow, count * sizeof(int));

  return;
}

void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  char buf6[MAX_STRING_LENGTH];
  char buf7[MAX_STRING_LENGTH];
  char mount2[MAX_STRING_LENGTH];
  CHAR_DATA *mount;

  buf[0] = '\0';
  buf2[0] = '\0';
  buf3[0] = '\0';
  buf6[0] = '\0';

  if (!IS_NPC(victim) && victim->pcdata->chobj != NULL)
    return;

  if ((mount = victim->mount) != NULL && IS_SET(victim->mounted, IS_MOUNT))
    return;

  if (IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
    strcat(buf, "     ");
  else
  {
    if (!IS_NPC(ch) && IS_NPC(victim) && is_quest_target(ch, victim))
      strcat(buf, "#y(#RQuest#y)#n ");
    if (!IS_NPC(victim) && victim->desc == NULL)
      strcat(buf, "#y(Link-Dead)#n ");
    if (IS_AFFECTED(victim, AFF_INVISIBLE))
      strcat(buf, "#L(Invis)#n ");
    if (IS_AFFECTED(victim, AFF_HIDE))
      strcat(buf, "#0(Hide)#n ");
    if (IS_AFFECTED(victim, AFF_CHARM))
      strcat(buf, "#R(Charmed)#n ");
    if (IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_AFFECTED(victim, AFF_ETHEREAL))
      strcat(buf, "#l(Translucent)#n ");
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
      strcat(buf, "#P(Pink Aura)#n ");
    if (IS_EVIL(victim) && IS_AFFECTED(ch, AFF_DETECT_EVIL))
      strcat(buf, "#R(Red Aura)#n ");
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
      strcat(buf, "#C(White Aura)#n ");
  }

  if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE))
    strcat(buf, "#0(#CNormal plane#0)#n ");
  else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE))
    strcat(buf, "#C(#0Shadowplane#C)#n ");

  if (IS_AFFECTED(victim, AFF_FLAMING))
  {
    if (IS_NPC(victim))
      sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->short_descr);
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
      sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->morph);
    else
      sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->name);
  }

  if (!IS_NPC(victim) && IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
  {
    if (IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED))
      sprintf(buf3, "...%s is gagged and blindfolded, kinky!", victim->morph);
    else if (IS_EXTRA(victim, GAGGED))
      sprintf(buf3, "...%s is gagged!", victim->morph);
    else if (IS_EXTRA(victim, BLINDFOLDED))
      sprintf(buf3, "...%s is blindfolded!", victim->morph);
  }

    if ( IS_EXTRA(victim, PINNEDWALL ) )
    {
        if (IS_NPC(victim))
            sprintf( buf3, "\n\r...%s is pinned to the wall!", victim->short_descr );
        else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
            sprintf( buf3, "\n\r...%s is pinned to the wall!", victim->morph );
        else
            sprintf( buf3, "\n\r...%s is pinned to the wall!", victim->name );
    }


  if (IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
  {
    strcat(buf, victim->morph);
    strcat(buf, " is lying here.");
    strcat(buf, buf2);
    strcat(buf, buf3);
    strcat(buf, buf6);
    strcat(buf, buf7);
    strcat(buf, "\n\r");
    buf[5] = UPPER(buf[5]);
    send_to_char(buf, ch);
    return;
  }

  if (IS_EXTRA(victim, TIED_UP))
  {
    if (IS_NPC(victim))
      sprintf(buf3, "\n\r...%s is tied up", victim->short_descr);
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
      sprintf(buf3, "\n\r...%s is tied up", victim->morph);
    else
      sprintf(buf3, "\n\r...%s is tied up", victim->name);
    if (IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED))
      strcat(buf3, ", gagged and blindfolded!");
    else if (IS_EXTRA(victim, GAGGED))
      strcat(buf3, " and gagged!");
    else if (IS_EXTRA(victim, BLINDFOLDED))
      strcat(buf3, " and blindfolded!");
    else
      strcat(buf3, "!");
  }

  if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
    strcat(buf, victim->morph);
  else if (victim->position == POS_STANDING && victim->long_descr[0] != '\0' && (mount = victim->mount) == NULL)
  {
    strcat(buf, victim->long_descr);
    send_to_char(buf, ch);
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF1))
    {
      if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD))
        act("...$N is surrounded by a swirling shield of #0c#Rh#0a#Ro#0s#n.", ch, NULL, victim, TO_CHAR);
    }
    return;
  }
  else
    strcat(buf, PERS(victim, ch));

  if ((mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
  {
    if (IS_NPC(mount))
      sprintf(mount2, " is here riding %s", mount->short_descr);
    else
      sprintf(mount2, " is here riding %s", mount->name);
    strcat(buf, mount2);
    if (victim->position == POS_FIGHTING)
    {
      strcat(buf, ", fighting ");
      if (victim->fighting == NULL)
        strcat(buf, "thin air??");
      else if (victim->fighting == ch)
        strcat(buf, "YOU!");
      else if (victim->in_room == victim->fighting->in_room)
      {
        strcat(buf, PERS(victim->fighting, ch));
        strcat(buf, ".");
      }
      else
        strcat(buf, "somone who left??");
    }
    else
      strcat(buf, ".");
  }
  else if (victim->position == POS_STANDING && IS_AFFECTED(victim, AFF_FLYING))
    strcat(buf, " is hovering here");
  else
  {
    switch (victim->position)
    {
      case POS_DEAD:
        strcat(buf, " is DEAD!!");
        break;
      case POS_MORTAL:
        strcat(buf, " is #Rmortally wounded#n.");
        break;
      case POS_INCAP:
        strcat(buf, " is #rincapacitated#n.");
        break;
      case POS_STUNNED:
        strcat(buf, " is lying here #Cstunned#n.");
        break;
      case POS_SLEEPING:
        strcat(buf, " is sleeping here.");
        break;
      case POS_RESTING:
        strcat(buf, " is resting here.");
        break;
      case POS_MEDITATING:
        strcat(buf, " is meditating here.");
        break;
      case POS_SITTING:
        strcat(buf, " is sitting here.");
        break;
      case POS_STANDING:
        if (!IS_NPC(victim) && victim->stance[0] == STANCE_NORMAL)
          strcat(buf, " is here, crouched in a fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_VIPER)
          strcat(buf, " is here, crouched in a viper fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRANE)
          strcat(buf, " is here, crouched in a crane fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRAB)
          strcat(buf, " is here, crouched in a crab fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MONGOOSE)
          strcat(buf, " is here, crouched in a mongoose fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_BULL)
          strcat(buf, " is here, crouched in a bull fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MANTIS)
          strcat(buf, " is here, crouched in a mantis fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_DRAGON)
          strcat(buf, " is here, crouched in a dragon fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_TIGER)
          strcat(buf, " is here, crouched in a tiger fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MONKEY)
          strcat(buf, " is here, crouched in a monkey fighting stance.");
        else if (!IS_NPC(victim) && victim->stance[0] == STANCE_SWALLOW)
          strcat(buf, " is here, crouched in a swallow fighting stance.");
        else
          strcat(buf, " is here.");
        break;
      case POS_FIGHTING:
        strcat(buf, " is here, fighting ");
        if (victim->fighting == NULL)
          strcat(buf, "thin air??");
        else if (victim->fighting == ch)
          strcat(buf, "YOU!");
        else if (victim->in_room == victim->fighting->in_room)
        {
          strcat(buf, PERS(victim->fighting, ch));
          strcat(buf, ".");
        }
        else
          strcat(buf, "somone who left??");
        break;
    }
  }

  strcat(buf, buf2);
  strcat(buf, buf3);
  strcat(buf, "\n\r");
  buf[0] = UPPER(buf[0]);
  send_to_char(buf, ch);

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF1))
    return;

  if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD))
    act("...$N is surrounded by a swirling shield of chaos.", ch, NULL, victim, TO_CHAR);
  return;
}

void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  int iWear;
  int percent;
  bool found;

  if (can_see(victim, ch))
  {
    act("$n looks at you.", ch, NULL, victim, TO_VICT);
    act("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
  }

  if (!IS_NPC(ch) && IS_HEAD(victim, LOST_HEAD))
  {
    act("$N is lying here.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (victim->description[0] != '\0')
  {
    send_to_char(victim->description, ch);
  }
  else
  {
    act("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
  }

  if (!IS_NPC(victim) && victim->pcRace != 0)
  {

        char const *eyes;
        char const *build;
        char const *looks;
        char const *hair;


	eyes = pc_eye_table[victim->pcdata->pc_eyes].pc_eyes;
        build = pc_build_table[victim->pcdata->pc_build].pc_build;
        looks = pc_looks_table[victim->pcdata->pc_looks].pc_looks;
        hair = pc_hair_table[victim->pcdata->pc_hair].pc_hair;

   sprintf(buf, "%s has %s eyes, is %s, \n\r", victim->name, eyes, build);
   send_to_char(buf, ch);
   if (victim->sex == SEX_FEMALE) send_to_char("She",ch);
   if (victim->sex == SEX_MALE) send_to_char("He",ch);
   sprintf(buf, " %s and %s. \n\r", looks, hair);
   send_to_char(buf, ch);

  }


  if (victim->max_hit > 0)
    percent = (100 * victim->hit) / victim->max_hit;
  else
    percent = -1;

  strcpy(buf, PERS(victim, ch));

  if (percent >= 100)
    strcat(buf, " is in perfect health.\n\r");
  else if (percent >= 90)
    strcat(buf, " is slightly scratched.\n\r");
  else if (percent >= 80)
    strcat(buf, " has a few bruises.\n\r");
  else if (percent >= 70)
    strcat(buf, " has some cuts.\n\r");
  else if (percent >= 60)
    strcat(buf, " has several wounds.\n\r");
  else if (percent >= 50)
    strcat(buf, " has many nasty wounds.\n\r");
  else if (percent >= 40)
    strcat(buf, " is bleeding freely.\n\r");
  else if (percent >= 30)
    strcat(buf, " is covered in blood.\n\r");
  else if (percent >= 20)
    strcat(buf, " is leaking guts.\n\r");
  else if (percent >= 10)
    strcat(buf, " is almost dead.\n\r");
  else
    strcat(buf, " is DYING.\n\r");

  buf[0] = UPPER(buf[0]);
  send_to_char(buf, ch);

  if (!IS_NPC(victim))
  {
    if (IS_AFFECTED(victim, AFF_INFRARED))
      act("$N's eyes are glowing bright red.", ch, NULL, victim, TO_CHAR);
    if (IS_AFFECTED(victim, AFF_FLYING))
      act("$N is hovering in the air.", ch, NULL, victim, TO_CHAR);
	    if (IS_VAMPAFF(victim, VAM_FANGS)) act("$N has a pair of long, pointed fangs.",ch,NULL,victim,TO_CHAR);
if (IS_AFFECTED(victim, AFF_INFRARED) || IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) 
act("$N's eyes are glowing bright red.",ch,NULL,victim,TO_CHAR);
if (!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CLAWS))
			act("$N has razor sharp talons extending from $S fingers.",ch,NULL,victim,TO_CHAR);
			if (IS_VAMPAFF(victim, VAM_HORNS)) act("$N has a pair of pointed horns extending from $S head.",ch,NULL,victim,TO_CHAR);
			if (IS_VAMPAFF(victim, VAM_WINGS))
					act("$N has a pair of batlike wings spread out from behind $S back.",ch,NULL,victim,TO_CHAR);
		   if (IS_VAMPAFF(victim, VAM_TAIL))
				act("$N has a sharp tail extending from $S spine.",ch,NULL,victim,TO_CHAR); 
		   if (IS_VAMPAFF(victim, VAM_EXOSKELETON))
				act("$N's skin is covered by a hard exoskeleton.",ch,NULL,victim,TO_CHAR);
  }
  found = FALSE;
  for (iWear = 0; iWear < MAX_WEAR; iWear++)
  {
    if ((obj = get_eq_char(victim, iWear)) != NULL && can_see_obj(ch, obj))
    {
      if (!found)
      {
        send_to_char("\n\r", ch);
        act("$N is using:", ch, NULL, victim, TO_CHAR);
        found = TRUE;
      }
      send_to_char(where_name[iWear], ch);
      if (IS_NPC(ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj != obj)
      {
        send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
        send_to_char("\n\r", ch);
      }
      else
        send_to_char("you\n\r", ch);
    }
  }

  if (victim != ch && !IS_NPC(ch) && !IS_HEAD(victim, LOST_HEAD) && number_percent() < ch->pcdata->learned[gsn_peek])
  {
    send_to_char("\n\rYou peek at the inventory:\n\r", ch);
    show_list_to_char(victim->carrying, ch, TRUE, TRUE);
  }

  return;
}

void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
  CHAR_DATA *rch;

  for (rch = list; rch != NULL; rch = rch->next_in_room)
  {
    if (rch == ch)
      continue;

    if (IS_IMMORTAL(ch) || IS_IMMORTAL(rch))
    {
      if (!IS_NPC(rch) && IS_SET(rch->act, PLR_WIZINVIS) && get_trust(ch) < get_trust(rch))
        continue;
    }
    else
    {
      if (!IS_NPC(rch) && (IS_SET(rch->act, PLR_WIZINVIS) || IS_ITEMAFF(rch, ITEMA_VANISH)) && !IS_SET(ch->act, PLR_HOLYLIGHT) && !IS_ITEMAFF(ch, ITEMA_VISION))
        continue;
    }

    if (!IS_NPC(rch) && IS_HEAD(rch, LOST_HEAD))
      continue;

    if (!IS_NPC(rch) && IS_EXTRA(rch, EXTRA_OSWITCH))
      continue;

    if (can_see(ch, rch))
      show_char_to_char_0(rch, ch);
    else if (room_is_dark(ch->in_room) && IS_AFFECTED(rch, AFF_INFRARED))
    {
      send_to_char("You see glowing #Rred#n eyes watching YOU!\n\r", ch);
    }
  }

  return;
}

bool check_blind(CHAR_DATA * ch)
{

  if (IS_EXTRA(ch, BLINDFOLDED))
  {
    send_to_char("You can't see a thing through the blindfold!\n\r", ch);
    return FALSE;
  }

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    return TRUE;

  if (IS_ITEMAFF(ch, ITEMA_VISION))
    return TRUE;

  if (IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R))
  {
    send_to_char("You have no eyes!\n\r", ch);
    return FALSE;
  }

  if (IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT))
  {
    send_to_char("You can't see a thing!\n\r", ch);
    return FALSE;
  }

  return TRUE;
}

void do_truesight(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;

    
  if (IS_SET(ch->act, PLR_HOLYLIGHT))
  {
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char("Your senses return to normal.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char("Your senses increase to incredible proportions.\n\r", ch);
  }

  return;
}  


void do_look(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
//  CHAR_DATA *wizard;
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *pRoomIndex;
  ROOM_INDEX_DATA *location;
  char *pdesc;
  int door;

  if (ch->desc == NULL) // && (wizard = ch->wizard) == NULL)
    return;

  if (ch->in_room == NULL)
    return;

  if (ch->position < POS_SLEEPING)
  {
    send_to_char("You can't see anything but stars!\n\r", ch);
    return;
  }

  if (ch->position == POS_SLEEPING)
  {
    send_to_char("You can't see anything, you're sleeping!\n\r", ch);
    return;
  }

  if (IS_SET(ch->flag2, AFF_TOTALBLIND))
  {
    send_to_char("You can't see anything because you're blinded!\n\r", ch);
    return;
  }

  if (!check_blind(ch))
    return;

  if (!IS_NPC(ch) && IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS) && !IS_ITEMAFF(ch, ITEMA_VISION) && !IS_IMMORTAL(ch))
  {
    send_to_char("It is pitch black ... \n\r", ch);
    return;
  }

  if (!IS_NPC(ch)
      && !IS_SET(ch->act, PLR_HOLYLIGHT)
      && !IS_ITEMAFF(ch, ITEMA_VISION)
      && !IS_AFFECTED(ch, AFF_SHADOWPLANE)
      && !(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL) && room_is_dark(ch->in_room))
  {
    send_to_char("It is pitch black ... \n\r", ch);
    show_char_to_char(ch->in_room->people, ch);
    return;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || !str_cmp(arg1, "auto") || !str_cmp(arg1, "scry"))
  {
    /* 'look' or 'look auto' */
    if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
      act("$p", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
    else if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
      send_to_char("The shadow plane\n\r", ch);
    else if (!IS_SET(ch->act, PLR_MAP))
    {
      if (!IS_NPC(ch) && ch->level > 6)
      {
        sprintf(buf, "%s #0[#7%d#0]#n\n\r", ch->in_room->name, ch->in_room->vnum);
        send_to_char(buf, ch);
      }
      else
      {
        sprintf(buf, "%s\n\r", ch->in_room->name);
        send_to_char(buf, ch);
      }
    }

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) && !IS_SET(ch->act, PLR_MAP))
      do_exits(ch, "auto");

    if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
    {
      act("You are inside $p.", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
      show_list_to_char(ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE);
    }
    else if ((arg1[0] == '\0' || !str_cmp(arg1, "auto")) && IS_AFFECTED(ch, AFF_SHADOWPLANE))
      send_to_char("You are standing in complete darkness.\n\r", ch);
    else if ((!IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF1)) && (arg1[0] == '\0' || !str_cmp(arg1, "auto") || !str_cmp(arg1, "scry")))
    {
      if (IS_SET(ch->act, PLR_MAP))
        draw_map(ch, ch->in_room->description);
      else
        send_to_char(ch->in_room->description, ch);

      if (ch->in_room->blood == 1000)
        sprintf(buf, "You notice that the room is completely drenched in blood.\n\r");
      else if (ch->in_room->blood > 750)
        sprintf(buf, "You notice that there is a very large amount of blood around the room.\n\r");
      else if (ch->in_room->blood > 500)
        sprintf(buf, "You notice that there is a large quantity of blood around the room.\n\r");
      else if (ch->in_room->blood > 250)
        sprintf(buf, "You notice a fair amount of blood on the floor.\n\r");
      else if (ch->in_room->blood > 100)
        sprintf(buf, "You notice several blood stains on the floor.\n\r");
      else if (ch->in_room->blood > 50)
        sprintf(buf, "You notice a few blood stains on the floor.\n\r");
      else if (ch->in_room->blood > 25)
        sprintf(buf, "You notice a couple of blood stains on the floor.\n\r");
      else if (ch->in_room->blood > 0)
        sprintf(buf, "You notice a few drops of blood on the floor.\n\r");
      else
        sprintf(buf, "You notice nothing special in the room.\n\r");
      ADD_COLOUR(ch, buf, L_RED);
      if (ch->in_room->blood > 0)
        send_to_char(buf, ch);
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_FLAMING))
      send_to_char("..This room is engulfed in flames!\n\r", ch);

    show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);

    for (door = 0; door < 6; door++)
    {
      if (ch->in_room == NULL)
        continue;
      if (ch->in_room->exit[door] == NULL)
        continue;

      if (IS_SET(ch->in_room->exit[door]->exit_info, EX_PRISMATIC_WALL))
      {
        sprintf(buf, "     You see a shimmering wall of many colours %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_ICE_WALL))
      {
        sprintf(buf, "     You see a glacier of ice %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_CALTROP_WALL))
      {
        sprintf(buf, "     You see a wall of caltrops %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_FIRE_WALL))
      {
        sprintf(buf, "     You see a blazing wall of fire %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_SWORD_WALL))
      {
        sprintf(buf, "     You see a spinning wall of swords %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_MUSHROOM_WALL))
      {
        sprintf(buf, "     You see a vibrating mound of mushrooms %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_IRON_WALL))
      {
        sprintf(buf, "    You see a solid wall of iron %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
      else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_ASH_WALL))
      {
        sprintf(buf, "    You see a deadly wall of ash %s.\n\r", exitname2[door]);
        send_to_char(buf, ch);
      }
    }
    show_char_to_char(ch->in_room->people, ch);
    if (str_cmp(arg1, "scry"))
      aggr_test(ch);
    return;
  }

  if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in"))
  {
    /* 'look in' */
    if (arg2[0] == '\0')
    {
      send_to_char("Look in what?\n\r", ch);
      return;
    }

    if ((obj = get_obj_here(ch, arg2)) == NULL)
    {
      send_to_char("You do not see that here.\n\r", ch);
      return;
    }

    switch (obj->item_type)
    {
      default:
        send_to_char("That is not a container.\n\r", ch);
        break;

      case ITEM_PORTAL:
        pRoomIndex = get_room_index(obj->value[0]);
        location = ch->in_room;
        if (pRoomIndex == NULL)
        {
          send_to_char("It doesn't seem to lead anywhere.\n\r", ch);
          return;
        }
        if (obj->value[2] == 1)
        {
          send_to_char("It seems to be closed.\n\r", ch);
          return;
        }
        char_from_room(ch);
        char_to_room(ch, pRoomIndex);
        do_look(ch, "scry");
        char_from_room(ch);
        char_to_room(ch, location);
        break;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        if (IS_SET(obj->value[1], CONT_CLOSED))
        {
          send_to_char("It is closed.\n\r", ch);
          break;
        }

        act("$p contains:", ch, obj, NULL, TO_CHAR);
        show_list_to_char(obj->contains, ch, TRUE, TRUE);
        break;
    }
    return;
  }

  if ((victim = get_char_room(ch, arg1)) != NULL)
  {
    show_char_to_char_1(victim, ch);
    return;
  }

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;
    if (vch->in_room == NULL)
      continue;
    if (vch->in_room == ch->in_room)
    {
      if (!IS_NPC(vch) && !str_cmp(arg1, vch->morph))
      {
        show_char_to_char_1(vch, ch);
        return;
      }
      continue;
    }
  }

  if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
  {
    obj = get_obj_in_obj(ch, arg1);
    if (obj != NULL)
    {
      send_to_char(obj->description, ch);
      send_to_char("\n\r", ch);
      return;
    }
  }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
      continue;
    if (can_see_obj(ch, obj))
    {
      pdesc = get_extra_descr(arg1, obj->extra_descr);
      if (pdesc != NULL)
      {
        send_to_char(pdesc, ch);
        return;
      }

      pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
      if (pdesc != NULL)
      {
        send_to_char(pdesc, ch);
        return;
      }
    }

    if (is_name(arg1, obj->name))
    {
      send_to_char(obj->description, ch);
      send_to_char("\n\r", ch);
      return;
    }
  }

  for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
  {
    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
      continue;
    if (can_see_obj(ch, obj))
    {
      pdesc = get_extra_descr(arg1, obj->extra_descr);
      if (pdesc != NULL)
      {
        send_to_char(pdesc, ch);
        return;
      }

      pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
      if (pdesc != NULL)
      {
        send_to_char(pdesc, ch);
        return;
      }
    }

    if (is_name(arg1, obj->name))
    {
      send_to_char(obj->description, ch);
      send_to_char("\n\r", ch);
      return;
    }
  }

  pdesc = get_extra_descr(arg1, ch->in_room->extra_descr);
  if (pdesc != NULL)
  {
    send_to_char(pdesc, ch);
    return;
  }

  if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
    door = 0;
  else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
    door = 1;
  else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
    door = 2;
  else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
    door = 3;
  else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
    door = 4;
  else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
    door = 5;
  else
  {
    send_to_char("You do not see that here.\n\r", ch);
    return;
  }

  /* 'look direction' */
  if ((pexit = ch->in_room->exit[door]) == NULL)
  {
    send_to_char("Nothing special there.\n\r", ch);
    return;
  }
/*
    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );
*/

  if (pexit->keyword != NULL && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ')
  {
    if (IS_SET(pexit->exit_info, EX_CLOSED))
    {
      act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
    }
    else if (IS_SET(pexit->exit_info, EX_ISDOOR))
    {
      act("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
      if ((pexit = ch->in_room->exit[door]) == NULL)
        return;
      if ((pRoomIndex = pexit->to_room) == NULL)
        return;
      location = ch->in_room;
      char_from_room(ch);
      char_to_room(ch, pRoomIndex);
      do_look(ch, "auto");
      char_from_room(ch);
      char_to_room(ch, location);
    }
    else
    {
      if ((pexit = ch->in_room->exit[door]) == NULL)
        return;
      if ((pRoomIndex = pexit->to_room) == NULL)
        return;
      location = ch->in_room;
      char_from_room(ch);
      char_to_room(ch, pRoomIndex);
      do_look(ch, "auto");
      char_from_room(ch);
      char_to_room(ch, location);
    }
  }
  else
  {
    if ((pexit = ch->in_room->exit[door]) == NULL)
      return;
    if ((pRoomIndex = pexit->to_room) == NULL)
      return;
    location = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, pRoomIndex);
    do_look(ch, "auto");
    char_from_room(ch);
    char_to_room(ch, location);
  }

  return;
}

void do_examine(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Examine what?\n\r", ch);
    return;
  }

  do_look(ch, arg);

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    if (obj->condition >= 100)
      sprintf(buf, "You notice that %s is in perfect condition.\n\r", obj->short_descr);
    else if (obj->condition >= 75)
      sprintf(buf, "You notice that %s is in good condition.\n\r", obj->short_descr);
    else if (obj->condition >= 50)
      sprintf(buf, "You notice that %s is in average condition.\n\r", obj->short_descr);
    else if (obj->condition >= 25)
      sprintf(buf, "You notice that %s is in poor condition.\n\r", obj->short_descr);
    else
      sprintf(buf, "You notice that %s is in awful condition.\n\r", obj->short_descr);
    send_to_char(buf, ch);
    switch (obj->item_type)
    {
      default:
        break;

      case ITEM_DRINK_CON:
      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        send_to_char("When you look inside, you see:\n\r", ch);
        sprintf(buf, "in %s", arg);
        do_look(ch, buf);
    }
  }

  return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA * ch, char *argument)
{
  extern char *const dir_name[];
  char buf[MAX_STRING_LENGTH];
  EXIT_DATA *pexit;
  bool found;
  bool fAuto;
  int door;

  buf[0] = '\0';
  fAuto = !str_cmp(argument, "auto");

  if (!check_blind(ch))
    return;

  strcpy(buf, fAuto ? "#0[#GExits#7:#C" : "Obvious exits:\n\r");

  found = FALSE;
  for (door = 0; door <= 5; door++)
  {
    if ((pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL)
    {
      found = TRUE;
      if (fAuto)
      {
        if (IS_SET(pexit->exit_info, EX_CLOSED))
        {
          strcat(buf, " #0(#C");
          strcat(buf, dir_name[door]);
          strcat(buf, "#0)#C");
        }
        else
        {
          strcat(buf, " ");
          strcat(buf, dir_name[door]);
        }
      }
      else
      {
        sprintf(buf + strlen(buf), "%-5s - %s\n\r", capitalize(dir_name[door]), room_is_dark(pexit->to_room) ? "Too dark to tell" : pexit->to_room->name);
      }
    }
  }

  if (!found)
    strcat(buf, fAuto ? " none" : "None.\n\r");

  if (fAuto)
    strcat(buf, "#0]#n\n\r");

  send_to_char(buf, ch);
  return;
}

char *const day_name[] = {
  "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
  "the Great Gods", "the Sun"
};

char *const month_name[] = {
  "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
  "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
  "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
  "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time(CHAR_DATA * ch, char *argument)
{
  extern char str_boot_time[];
  char buf[MAX_STRING_LENGTH];
  char *suf;
  int day;

  day = time_info.day + 1;

  if (day > 4 && day < 20)
    suf = "th";
  else if (day % 10 == 1)
    suf = "st";
  else if (day % 10 == 2)
    suf = "nd";
  else if (day % 10 == 3)
    suf = "rd";
  else
    suf = "th";

  sprintf(buf,
          "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rAge of Heroes started up at %s\rThe system time is %s\r",
          (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
          time_info.hour >= 12 ? "pm" : "am", day_name[day % 7], day, suf, month_name[time_info.month], str_boot_time, (char *) ctime(&current_time));

  send_to_char(buf, ch);
  return;
}

void do_level(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char skill[20];
  char buf[MAX_STRING_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

        {
        stc(" #y_________________________________\n\r",ch);
        stc("#y/  _\\                             \\\n\r",ch);
        stc("#y\\ (_/_____________________________/\n\r",ch);
        sprintf(skill," #y\\   #0*#rR#Red Magic            #0[#R%-3d#0]#y\\\n\r",ch->spl[1]);
        stc(skill,ch);
        sprintf(skill,"  #y\\   #0*#bB#Blue Magic           #0[#R%-3d#0]#y\\\n\r",ch->spl[2]);
        stc(skill,ch);
        sprintf(skill,"   #y\\   #0*#gG#Green Magic          #0[#R%-3d#0]#y\\\n\r",ch->spl[3]);
        stc(skill,ch);
        sprintf(skill,"  #y__\\   #0*#pP#Purple Magic         #0[#R%-3d#0]#y\\\n\r",ch->spl[0]);
        stc(skill,ch);
        sprintf(skill," #y/ )_\\   #0*#yY#Yellow Magic         #0[#R%-3d#0]#y\\\n\r",ch->spl[4]);
        stc(skill,ch);
        stc(" #y\\___/______________________________/#n\n\r",ch);
        }

/*
  sprintf(skill, "   Recall Room #C: %d#n\n\r", ch->home);
  send_to_char(skill, ch);
*/
if (get_room_index (ch->home) != NULL)
      {
         sprintf (skill, "   Recall Room  : %s\n\r",
                  (get_room_index (ch->home))->name);
         send_to_char (skill, ch);
      }
      else
         stc (" Recall Room  : You recall to a room that no longer exists.\n\r", ch);
  if (IS_SET(ch->act, PLR_RIGHTHAND))
    send_to_char("   You favor your #Cright#n arm in combat\n\r", ch);
  else if (IS_SET(ch->act, PLR_LEFTHAND))
    send_to_char("   You favor your #Cleft#n arm in combat\n\r", ch);
  else
    send_to_char("   You fight well with #Cboth#n your arms\n\r", ch);

  sprintf(skill, "   Generation #C: %d#n\n\r", ch->generation);
  send_to_char(skill, ch);

  return;
}


void do_who(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *gch;
  char arg[MAX_INPUT_LENGTH];
  char pkratio[MAX_STRING_LENGTH];
  char kav[MAX_STRING_LENGTH];
  char prof[MAX_STRING_LENGTH];
  char guild[MAX_STRING_LENGTH];
  char faith[MAX_STRING_LENGTH];
  char openb[20];
  char closeb[20];
  char buf[MAX_STRING_LENGTH]; // banners
  char pbuf[MAX_STRING_LENGTH]; // banners
  char buf1[MAX_STRING_LENGTH]; // Admin.
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  char buf4[MAX_STRING_LENGTH];
  char buf5[MAX_STRING_LENGTH];
  char buf6[MAX_STRING_LENGTH];
  char buf7[MAX_STRING_LENGTH];
  char buf8[MAX_STRING_LENGTH]; // avatars : buf2 - buf16
  char buf9[MAX_STRING_LENGTH];
  char buf10[MAX_STRING_LENGTH];
  char buf11[MAX_STRING_LENGTH];
  char buf12[MAX_STRING_LENGTH];
  char buf13[MAX_STRING_LENGTH];
  char buf14[MAX_STRING_LENGTH];
  char buf15[MAX_STRING_LENGTH];
  char buf16[MAX_STRING_LENGTH];
  char buf17[MAX_STRING_LENGTH]; // mortals.
  int nPlayerAll = 0;
  int nPlayerVis = 0;
  int nImmVis = 0;
  int mightRate;
  char racelevel[MAX_STRING_LENGTH];
  char remort[MAX_STRING_LENGTH];
  bool rangeOnly = FALSE;
  bool canDecap;
  bool canKillYou;
  bool avatars = FALSE;
  bool a1 = FALSE;
  bool a2 = FALSE;
  bool a3 = FALSE;
  bool a4 = FALSE;
  bool a5 = FALSE;
  bool a6 = FALSE;
  bool a7 = FALSE;
  bool a8 = FALSE;
  bool a9 = FALSE;
  bool a10 = FALSE;
  bool a11 = FALSE;
  bool a12 = FALSE;
  bool a13 = FALSE;
  bool a14 = FALSE;
  bool a15 = FALSE;
  bool a16 = FALSE;
  bool a17 = FALSE;

  if (IS_NPC(ch)) return;

  one_argument(argument, arg);

  if (!str_cmp(arg, "kill")) rangeOnly = TRUE;

  buf1[0] = '\0';
  buf2[0] = '\0';
  buf3[0] = '\0';
  buf4[0] = '\0';
  buf5[0] = '\0';
  buf6[0] = '\0';
  buf7[0] = '\0';
  buf8[0] = '\0';
  buf9[0] = '\0';
  buf10[0] = '\0';
  buf11[0] = '\0';
  buf12[0] = '\0';   
  buf13[0] = '\0';
  buf14[0] = '\0';
  buf15[0] = '\0';
  buf16[0] = '\0';
  buf17[0] = '\0';

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    char const *title;
    char const *kav;
    
    if (d->connected != CON_PLAYING) continue;
    if ((gch = d->character) == NULL) continue;

    /*
     * Immortals are not players, and should not be counted as such.
     */
    if (gch->level < 7) nPlayerAll++;

    /*
     * Visibility :-)
     */
    if (!can_see(ch, gch)) continue;

    if (gch->level < 7) nPlayerVis++;
    else nImmVis++;

    /*
     * The playerkill range is decided
     */
    if (!str_cmp(ch->pcdata->last_decap[0], gch->name)) canDecap = FALSE;
    else canDecap = fair_fight(ch, gch);
    if (!str_cmp(gch->pcdata->last_decap[0], ch->name)) canKillYou = FALSE;
    else canKillYou = fair_fight(gch, ch);

    if (rangeOnly && !canDecap) continue;

    /*
     * Let's calculate the pkratio and the color of the symbols for players to be guided by.
     */
    if (gch->pkill > 0)
    {
      if (100 * gch->pkill/(gch->pkill + gch->pdeath) < 100 && 100 * gch->pkill/(gch->pkill + gch->pdeath) > 9)
      {
        if (canDecap && canKillYou) sprintf(pkratio,"#R[#G0.%-2d#R]#n", (100 * gch->pkill/(gch->pkill + gch->pdeath)));
        else if (canKillYou) sprintf(pkratio,"#0[#G0.%-2d#0]#n", (100 * gch->pkill/(gch->pkill + gch->pdeath)));
        else if (canDecap) sprintf(pkratio,"#y[#G0.%-2d#y]#n", (100 * gch->pkill/(gch->pkill + gch->pdeath)));
        else sprintf(pkratio,"#L[#G0.%-2d#L]#n", (100 * gch->pkill/(gch->pkill + gch->pdeath)));
      }
      else if (100*gch->pkill/(gch->pkill + gch->pdeath) < 10)
      {
        if (canDecap && canKillYou) sprintf(pkratio,"#R[#G0.0%-1d#R]#n", (100*gch->pkill/(gch->pkill + gch->pdeath)));
        else if (canKillYou) sprintf(pkratio,"#0[#G0.0%-1d#0]#n", (100*gch->pkill/(gch->pkill + gch->pdeath)));
        else if (canDecap) sprintf(pkratio,"#y[#G0.0%-1d#y]#n", (100*gch->pkill/(gch->pkill + gch->pdeath)));
        else sprintf(pkratio,"#L[#G0.0%-1d#L]#n", (100*gch->pkill/(gch->pkill + gch->pdeath)));
      }
      else
      {
        if (canDecap && canKillYou) sprintf(pkratio,"#R[#G1.00#R]#n");
        else if (canKillYou) sprintf(pkratio,"#0[#G1.00#0]#n");
        else if (canDecap) sprintf(pkratio,"#y[#G1.00#y]#n");
        else sprintf(pkratio,"#L[#G1.00#L]#n");
      }
    }
    else
    {
      if (canDecap && canKillYou) sprintf(pkratio,"#R[#G0.00#R]#n");
      else if (canKillYou) sprintf(pkratio,"#0[#G0.00#0]#n");
      else if (canDecap) sprintf(pkratio,"#y[#G0.00#y]#n");
      else sprintf(pkratio,"#L[#G0.00#L]#n");
    }

    /*
     * Now let's parse the title.
     */

    mightRate = getMight(gch);
    
    if ((IS_HEAD(gch,LOST_HEAD) || IS_EXTRA(gch,EXTRA_OSWITCH)) && gch->pcdata->chobj != NULL)
    {
      if (gch->pcdata->chobj->pIndexData->vnum == 12) title = " #CA Head#n     #n ";
      else if (gch->pcdata->chobj->pIndexData->vnum == 30005) title = " #CA Brain#n     ";
      else title = " #GAn Object#n   ";
    }
    else
    {
      switch (gch->level )
      {
        default:
          title = " ";
          break;
        case MAX_LEVEL:
        case MAX_LEVEL - 1:
        case MAX_LEVEL - 2:
        case MAX_LEVEL - 3:
        case MAX_LEVEL - 4:
        case MAX_LEVEL - 5:
          if (IS_EXTRA(gch, EXTRA_AFK)) title = " #C*#0***A#CF#0K***#C*#n ";
          else if (gch->prefix != NULL) title = gch->prefix;
          else if (!str_cmp(gch->pcdata->switchname, "Jobo")) title = " #G(#C=-#RCODER#C-=#G)#n ";
          else if (!str_cmp(gch->pcdata->switchname, "Vladd")) title ="#RLOVE MACHINE#n ";
          else title = " #y>>>>#0GoD#y<<<<#n ";
          break;
        case MAX_LEVEL - 6:
        case MAX_LEVEL - 7:
        case MAX_LEVEL - 8:
        case MAX_LEVEL - 9:
          if (gch->prefix != NULL) title = gch->prefix;
          else if (IS_EXTRA(gch, EXTRA_AFK)) title = " #C*#0***A#CF#0K***#C*#n ";
          else if (IS_SET(gch->pcdata->jflags, JFLAG_BULLY)) title = " #0B#Rully#n       ";
          else if (mightRate > 3500 ) title = "#7Almighty#n";
          else if (mightRate > 3250 ) title = "#CRuler#n";
          else if (mightRate > 3000 ) title = "#oSupreme#n";
          else if (mightRate > 2750 ) title = "#pKing#n";
          else if (mightRate > 2500 ) title = "#RBaron#n";
          else if (mightRate > 2250 ) title = "#yDuke#n";
          else if (mightRate > 2000 ) title = "#gGeneral#n";
          else if (mightRate > 1750 ) title = "#CCaptain#n";
          else if (mightRate > 1500 ) title = "#RMaster#n";
          else if (mightRate > 1250 ) title = "#yLegendary#n";
          else if (mightRate > 1000 ) title = "#GHero#n";
          else if (mightRate >  750 ) title = "#LAdventurer#n";
          else if (mightRate >  500 ) title = "#rVeteran#n";
          else if (mightRate >= 150 ) title = "#oCadet#n";
          else title = "#pWannabe#n";
          break;
        case MAX_LEVEL - 10:
          title = "#RMortal#n";
          break;
        case MAX_LEVEL - 11:
          title = "#CNewbie#n";
          break;
        case MAX_LEVEL - 12:
          title = "#CUndefined#n";
          break;
      }   
    }  
        
//  		  if (gch->pcRace > 0)
		  {strcpy(openb, "" ); strcpy( closeb, "" );}
  		  if (gch->pcRace > 0)
			{
				if (IS_RACE(gch, RACE_HUMAN))
			        sprintf(kav, "%s#0Human#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ELF))
			        sprintf(kav, "%s#0High Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GNOME))
			        sprintf(kav, "%s#0Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GULLYDWARF))
			        sprintf(kav, "%s#Gully Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HILLDWARF))
			        sprintf(kav, "%s#0Hill Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_MTDWARF))
			        sprintf(kav, "%s#0Mountain Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DRAGONKIND))
			        sprintf(kav, "%s#0Dragonkind#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HOBBIT))
			        sprintf(kav, "%s#0Hobbit#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_KENDER))
			        sprintf(kav, "%s#0Kender#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DROW))
			        sprintf(kav, "%s#0Dark Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GIANT))
			        sprintf(kav, "%s#0Giant#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HALFELF))
			        sprintf(kav, "%s#0Half-Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_OGRE))
			        sprintf(kav, "%s#0Ogre#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HALFOGRE))
			        sprintf(kav, "%s#0Half-Ogre#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_MINOTAUR))
			        sprintf(kav, "%s#0Minotaur#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GOBLIN))
			        sprintf(kav, "%s#0Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HOBGOBLIN))
			        sprintf(kav, "%s#0Hobgoblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DUERGARDWARF))
			        sprintf(kav, "%s#0Duergar Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_TROLL))
			        sprintf(kav, "%s#0Troll#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HALFTROLL))
			        sprintf(kav, "%s#0Half Troll#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ORC))
			        sprintf(kav, "%s#0rc#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HALFORC))
			        sprintf(kav, "%s#0Half-Orc#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_CENTAUR))
			        sprintf(kav, "%s#0Centaur#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_WILDELF))
			        sprintf(kav, "%s#0Wild Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AQUATICELF))
			        sprintf(kav, "%s#0Aquatic Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DESERTELF))
			        sprintf(kav, "%s#0Desert Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_FIREELF))
			        sprintf(kav, "%s#0Fire Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GHOSTELF))
			        sprintf(kav, "%s#0Ghost Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GRAYELF))
			        sprintf(kav, "%s#0Gray Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_JUNGLEELF))
			        sprintf(kav, "%s#0Jungle Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_WOODELF))
			        sprintf(kav, "%s#0Wood Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AQUATICDWARF))
			        sprintf(kav, "%s#0Aquatic Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCTICDWARF))
			        sprintf(kav, "%s#0Arctic Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCTICELF))
			        sprintf(kav, "%s#0Arctic Elf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DEEPDWARF))
			        sprintf(kav, "%s#0Deep Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DESERTDWARF))
			        sprintf(kav, "%s#0Desert Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DREAMDWARF))
			        sprintf(kav, "%s#0Dream Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_EARTHDWARF))
			        sprintf(kav, "%s#0Earth Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GOLDDWARF))
			        sprintf(kav, "%s#0Gold Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GREYDWARF))
			        sprintf(kav, "%s#0Grey Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_JUNGLEDWARF))
			        sprintf(kav, "%s#0Jungle Dwarf#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AQUATICKOBOLD))
			        sprintf(kav, "%s#0Aquatic Kobold#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCTICKOBOLD))
			        sprintf(kav, "%s#0Arctic Kobold#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DESERTKOBOLD))
			        sprintf(kav, "%s#0Desert Kobold#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_EARTHKOBOLD))
			        sprintf(kav, "%s#0Earth Kobold#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_JUNGLEKOBOLD))
			        sprintf(kav, "%s#0Jungle Kobold#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GNOLL))
			        sprintf(kav, "%s#0Gnoll#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_HILLGIANT))
			        sprintf(kav, "%s#0Hill Giant#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_FIREGIANT))
			        sprintf(kav, "%s#0Fire Giant#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_FROSTGIANT))
			        sprintf(kav, "%s#0Frost Giant#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_PIXIE))
			        sprintf(kav, "%s#0Pixie#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AIRGNOME))
			        sprintf(kav, "%s#0Air Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AQUATICGNOME))
			        sprintf(kav, "%s#0Aquatic Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_CHAOSGNOME))
			        sprintf(kav, "%s#0Chaos Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DEEPGNOME))
			        sprintf(kav, "%s#0Deep Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DESERTGNOME))
			        sprintf(kav, "%s#0Desert Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_FORESTGNOME))
			        sprintf(kav, "%s#0Forest Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_JUNGLEGNOME))
			        sprintf(kav, "%s#0Jungle Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ROCKGNOME))
			        sprintf(kav, "%s#0Rock Gnome#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_BUGBEAR))
			        sprintf(kav, "%s#0Bugbear#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AIRGOBLIN))
			        sprintf(kav, "%s#0Air Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_AQUATICGOBLIN))
			        sprintf(kav, "%s#0Aquatic Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCTICGOBLIN))
			        sprintf(kav, "%s#0Arctic Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_GOBLINFORESTKITH))
			        sprintf(kav, "%s#0Forest Kith Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_JUNGLEGOBLIN))
			        sprintf(kav, "%s#0Jungle Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_OGREMAGE))
			        sprintf(kav, "%s#0Ogre-Mage#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_DESERTGOBLIN))
			        sprintf(kav, "%s#0Desert Goblin#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCHDEMON))
			        sprintf(kav, "%s#0Arch-Demon#n%s", openb, closeb);
                                else if (IS_RACE(gch, RACE_ARCHANGEL))
			        sprintf(kav, "%s#0Arch-Angel#n%s", openb, closeb);
				}
				else 
			        sprintf(kav, "%s#7None#n%s                ", openb, closeb);

                                


    sprintf(faith, " ");

	sprintf(racelevel, "#L[#y%-3d#L]#n", gch->pcRaceLevel);
	sprintf(remort, "#L[#y%d#L]#n", gch->pcremort);

	if (gch->profession == PROF_MERCENARY) 
		sprintf(prof, "#0Mercenary#n");
	else if (gch->profession == PROF_SOLDIER) 
		sprintf(prof, "#0Soldier#n");
 	else if (gch->profession == PROF_CLERIC) 
		sprintf(prof, "#0Cleric#n");
	else if (gch->profession == PROF_MONK) 
		sprintf(prof, "#0Monk#n");
	else if (gch->profession == PROF_THIEF) 
		sprintf(prof, "#0Thief#n");
	else if (gch->profession == PROF_ASSASSIN) 
		sprintf(prof, "#0Assassin#n");
        else if (gch->profession == PROF_KNIGHT)
                sprintf(prof, "#0Knight#n");
        else if (gch->profession == PROF_BARBARIAN)
                sprintf(prof, "#0Barbarian#n");
	else
	sprintf (prof, "");


	if (gch->guild == GUILD_DARK_HAND) 
		sprintf(guild, "#0Dark Hand#n");
	else if (gch->guild == GUILD_GUARDIAN) 
		sprintf(guild, "#0Guardian#n");
 	else if (gch->guild == GUILD_BEAST) 
		sprintf(guild, "#0Beast#n");
	else if (gch->guild == GUILD_DAMNED) 
		sprintf(guild, "#0Damned#n");
	else if (gch->guild == GUILD_CRUSADERS) 
		sprintf(guild, "#0Crusaders#n");
	else if (gch->guild == GUILD_DRUID) 
		sprintf(guild, "#0Druid#n");
	else
	sprintf (guild, "");

    /* 
     * Let's figure out where to place the player.
     */
        
    if (gch->level > 6 )
    {
      sprintf(buf1 + strlen(buf1), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
      a1 = TRUE;
    }
    else if (gch->level >= 3 && gch->level <= 6)
    {
      if (mightRate > 3500)
      {
      sprintf(buf2 + strlen(buf2), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);

        a2 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 3250)
      {
      sprintf(buf3 + strlen(buf3), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a3 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 3000)
      {
      sprintf(buf4 + strlen(buf4), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a4 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 2750)
      {
      sprintf(buf5 + strlen(buf5), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a5 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 2500)
      {
      sprintf(buf6 + strlen(buf6), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a6 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 2250)
      {
      sprintf(buf7 + strlen(buf7), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a7 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 2000)
      {
      sprintf(buf8 + strlen(buf8), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a8 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 1750)
      {
      sprintf(buf9 + strlen(buf9), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a9 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 1500)
      {
      sprintf(buf10 + strlen(buf10), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a10 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 1250)
      {
      sprintf(buf11 + strlen(buf11), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a11 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 1000)
      {
      sprintf(buf12 + strlen(buf12), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a12 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 750)
      {
      sprintf(buf13 + strlen(buf13), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a13 = TRUE; avatars = TRUE;
      }
      else if (mightRate > 500)  
      {
      sprintf(buf14 + strlen(buf14), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a14 = TRUE; avatars = TRUE;
      }
      else if (mightRate >= 150)   
      {
      sprintf(buf15 + strlen(buf15), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a15 = TRUE; avatars = TRUE;
      }
      else
      {
      sprintf(buf16 + strlen(buf16), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
        a16 = TRUE; avatars = TRUE;
      }
    }
    else if (gch->level < 3)
    {
      sprintf(buf17 + strlen(buf17), " #0The %s %s %s #7%s #0of %s %s %s %s\n\r",
        title, kav, prof, gch->pcdata->switchname, guild, pkratio, racelevel, remort);
      a17 = TRUE;
    }
  }

  /*   
   * Let's send the whole thing to the player.
   */
  sprintf(buf, " %s\n\r", generate_banner( "^1#7AGE OF HEROES#n", 80, "#0" ) );
  send_to_char(buf, ch);

  if (a1)
  {
  sprintf(buf, " %s\n\r", generate_banner( "#yIMMORTALS", 80, "#0" ) );
  send_to_char(buf, ch);
    send_to_char(buf1, ch);
  }         
  if (avatars)
  {
  sprintf(buf, " %s\n\r", generate_banner( "#yAVATARS", 80, "#0" ) );
  send_to_char(buf, ch);
    if (a2) send_to_char(buf2, ch);
    if (a3) send_to_char(buf3, ch);
    if (a4) send_to_char(buf4, ch);
    if (a5) send_to_char(buf5, ch);
    if (a6) send_to_char(buf6, ch);
    if (a7) send_to_char(buf7, ch);
    if (a8) send_to_char(buf8, ch);
    if (a9) send_to_char(buf9, ch);
    if (a10) send_to_char(buf10, ch);
    if (a11) send_to_char(buf11, ch);
    if (a12) send_to_char(buf12, ch);
    if (a13) send_to_char(buf13, ch);
    if (a14) send_to_char(buf14, ch);
    if (a15) send_to_char(buf15, ch);
    if (a16) send_to_char(buf16, ch);
  }
  if (a17)
  {
  sprintf(buf, " %s\n\r", generate_banner( "#yMORTALS", 76, "#0" ) );
    send_to_char(buf, ch);
    send_to_char(buf17, ch);
  }
  sprintf(buf, " %s\n\r", generate_banner( "", 80, "#0" ) );
  send_to_char(buf, ch);

  sprintf(pbuf, "#R%d#0/#R%d #yVisible players and #R%d #yvisible immortals connected to %s#n",
    nPlayerVis, nPlayerAll, nImmVis, MUDNAME);
  sprintf(buf, middle_text(pbuf, 80));
  send_to_char(buf, ch);
  sprintf(buf, "\n\r %s\n\r", generate_banner( "", 80, "#0" ) );
  send_to_char(buf, ch);
/* Still unsure of why this needs to be here - Tijer */
        extern char str_boot_time[];
        char     *suf;
        int       day;

        day = time_info.day + 1;

        if (day > 4 && day < 20)
                suf = "th";
        else if (day % 10 == 1)
                suf = "st";
        else if (day % 10 == 2)
                suf = "nd";
        else if (day % 10 == 3)
                suf = "rd";
        else
                suf = "th";

        sprintf(pbuf, "#0It is %d o'clock %s, Day of %s, %d%s the Month of %s.#n",
                (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
                time_info.hour >= 12 ? "pm" : "am",
                day_name[day % 7],
                day, suf,
                month_name[time_info.month]);
        sprintf(buf, middle_text(pbuf, 80));
        send_to_char(buf, ch);
//        sprintf(buf, "\n\r %s\n\r", generate_banner( "", 80, "#0" ) );
//        send_to_char(buf, ch);
  sprintf(buf, "\n\r %s\n\r", generate_banner( "^1#7http://www.ianshirm.net#n", 80, "#0" ) );
  send_to_char(buf, ch);


  return;
}





void do_inventory(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *portal;
  OBJ_DATA *portal_next;
  ROOM_INDEX_DATA *pRoomIndex;
  ROOM_INDEX_DATA *location;
  bool found;

  if (!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD))
  {
    send_to_char("You are not a container.\n\r", ch);
    return;
  }
  else if (!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH))
  {
    if (!IS_NPC(ch) && (obj = ch->pcdata->chobj) == NULL)
    {
      send_to_char("You are not a container.\n\r", ch);
      return;
    }
    switch (obj->item_type)
    {
      default:
        send_to_char("You are not a container.\n\r", ch);
        break;

      case ITEM_PORTAL:
        pRoomIndex = get_room_index(obj->value[0]);
        location = ch->in_room;
        if (pRoomIndex == NULL)
        {
          send_to_char("You don't seem to lead anywhere.\n\r", ch);
          return;
        }
        char_from_room(ch);
        char_to_room(ch, pRoomIndex);

        found = FALSE;
        for (portal = ch->in_room->contents; portal != NULL; portal = portal_next)
        {
          portal_next = portal->next_content;
          if ((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
          {
            found = TRUE;
            if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
            {
              REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
              do_look(ch, "auto");
              SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
              break;
            }
            else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
            {
              SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
              do_look(ch, "auto");
              REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
              break;
            }
            else
            {
              do_look(ch, "auto");
              break;
            }
          }
        }
        char_from_room(ch);
        char_to_room(ch, location);
        break;

      case ITEM_DRINK_CON:
        if (obj->value[1] <= 0)
        {
          send_to_char("You are empty.\n\r", ch);
          break;
        }
        if (obj->value[1] < obj->value[0] / 5)
          sprintf(buf, "There is a little %s liquid left in you.\n\r", liq_table[obj->value[2]].liq_color);
        else if (obj->value[1] < obj->value[0] / 4)
          sprintf(buf, "You contain a small about of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        else if (obj->value[1] < obj->value[0] / 3)
          sprintf(buf, "You're about a third full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        else if (obj->value[1] < obj->value[0] / 2)
          sprintf(buf, "You're about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        else if (obj->value[1] < obj->value[0])
          sprintf(buf, "You are almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        else if (obj->value[1] == obj->value[0])
          sprintf(buf, "You're completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        else
          sprintf(buf, "Somehow you are MORE than full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
        send_to_char(buf, ch);
        break;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        act("$p contain:", ch, obj, NULL, TO_CHAR);
        show_list_to_char(obj->contains, ch, TRUE, TRUE);
        break;
    }
    return;
  }
  send_to_char("You are carrying:\n\r", ch);
  show_list_to_char(ch->carrying, ch, TRUE, TRUE);
  return;
}

void do_equipment(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  int iWear;
  bool found;

  send_to_char("You are using:\n\r", ch);
  found = FALSE;
  for (iWear = 0; iWear < MAX_WEAR; iWear++)
  {
    if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

    send_to_char(where_name[iWear], ch);
    if (can_see_obj(ch, obj))
    {
      send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
      send_to_char("\n\r", ch);
    }
    else
    {
      send_to_char("something.\n\r", ch);
    }
    found = TRUE;
  }

  if (!found)
    send_to_char("Nothing.\n\r", ch);

  return;
}

void do_credits(CHAR_DATA * ch, char *argument)
{
  do_help(ch, "diku");
  return;
}


void do_where(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  bool found;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Players near you:\n\r", ch);
    found = FALSE;
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if ((d->connected == CON_PLAYING
           || d->connected == CON_EDITING)
          && (victim = d->character) != NULL && !IS_NPC(victim) && victim->in_room != NULL && victim->in_room->area == ch->in_room->area && victim->pcdata->chobj == NULL && can_see(ch, victim))
      {
        found = TRUE;
        sprintf(buf, "%-28s %s\n\r", victim->name, victim->in_room->name);
        send_to_char(buf, ch);
      }
    }
    if (!found)
      send_to_char("None\n\r", ch);
  }
  else
  {
    found = FALSE;
    for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (victim->in_room != NULL
          && victim->in_room->area == ch->in_room->area && !IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(victim, AFF_SNEAK) && can_see(ch, victim) && is_name(arg, victim->name))
      {
        found = TRUE;
        sprintf(buf, "%-28s %s\n\r", PERS(victim, ch), victim->in_room->name);
        send_to_char(buf, ch);
        break;
      }
    }
    if (!found)
      act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
  }

  return;
}

/*
 * Dystopian consider
 */
void do_consider(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int pMight, vMight;

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (arg[0] == '\0')
  {
    send_to_char("Consider killing whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They're not here.\n\r", ch);
    return;
  }
  act("You examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_CHAR);
  act("$n examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_NOTVICT);
  act("$n examines you closely, looking for your weaknesses.", ch, NULL, victim, TO_VICT);

  pMight = getMight(ch);

  if (IS_NPC(victim))
  {
    if (victim->level < 100)
    {
      if (pMight < 150)
        send_to_char("It's one mean bugger, but you think you can take it.\n\r", ch);
      else
        send_to_char("Shouldn't be much of a problem beating that one.\n\r", ch);
    }
    else if (victim->level < 300)
    {
      if (pMight < 150)
        send_to_char("No chance, it'll cream you.\n\r", ch);
      else if (pMight < 400)
        send_to_char("It's one mean bugger, but you think you can take it.\n\r", ch);
      else
        send_to_char("Shouldn't be much of a problem beating that one.\n\r", ch);
    }
    else if (victim->level < 1000)
    {
      if (pMight < 400)
        send_to_char("No chance, it'll cream you.\n\r", ch);
      else if (pMight < 1000)
        send_to_char("It's one mean bugger, but you think you can take it.\n\r", ch);
      else
        send_to_char("Shouldn't be much of a problem beating that one.\n\r", ch);
    }
    else if (victim->level < 2000)
    {
      if (pMight < 1000)
        send_to_char("No chance, it'll cream you.\n\r", ch);
      else if (pMight < 1500)
        send_to_char("It's one mean bugger, but you think you can take it.\n\r", ch);
      else
        send_to_char("Shouldn't be much of a problem beating that one.\n\r", ch);
    }
    else
    {
      if (pMight < 1500)
        send_to_char("No chance, it'll cream you.\n\r", ch);
      else if (pMight < 2000)
        send_to_char("It's one mean bugger, but you think you can take it.\n\r", ch);
      else
        send_to_char("Shouldn't be much of a problem beating that one.\n\r", ch);
    }
    return;
  }

  vMight = getMight(victim);

  if (pMight > 1.5 * vMight)
  {
    act("That would be to easy, better find someone worthy of the challenge.", ch, NULL, victim, TO_CHAR);
  }
  else if (pMight > 1.1 * vMight)
  {
    act("Shouldn't be to hard, but take care.", ch, NULL, victim, TO_CHAR);
  }
  else if (pMight > 0.9 * vMight)
  {
    act("Seems like a fair fight, you should give it a try.", ch, NULL, victim, TO_CHAR);
  }
  else if (pMight > 0.75 * vMight)
  {
    act("With a little luck, you might beat $N.", ch, NULL, victim, TO_CHAR);
  }
  else if (pMight > 0.6 * vMight)
  {
    act("$N will give you a solid beating.", ch, NULL, victim, TO_CHAR);
  }
  else
  {
    act("RUN!!! $N will rip out your heart before you land your first blow", ch, NULL, victim, TO_CHAR);
  }
  return;
}

void set_prefix(CHAR_DATA * ch, char *title)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch))
  {
    bug("Set_title: NPC.", 0);
    return;
  }

  if (isalpha(title[0]) || isdigit(title[0]))
  {
    buf[0] = ' ';
    strcpy(buf + 1, title);
  }
  else
  {
    strcpy(buf, title);
  }

  free_string(ch->prefix);
  ch->prefix = str_dup(buf);
  return;
}

void do_prefix(CHAR_DATA * ch, char *argument)
{

  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    send_to_char("Change your prefix to what?\n\r", ch);
    return;
  }

  if (strlen(argument) > 13)
    argument[13] = '\0';

  smash_tilde(argument);
  sprintf(buf, "#n");
  strcpy(buf, argument);
  set_prefix(ch, argument);
  send_to_char("Prefix Set.\n\r", ch);
}

void set_title(CHAR_DATA * ch, char *title)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch))
  {
    bug("Set_title: NPC.", 0);
    return;
  }

  if (isalpha(title[0]) || isdigit(title[0]))
  {
    buf[0] = ' ';
    strcpy(buf + 1, title);
  }
  else
  {
    strcpy(buf, title);
  }

  free_string(ch->pcdata->title);
  ch->pcdata->title = str_dup(buf);
  return;
}

void do_title(CHAR_DATA * ch, char *argument)
{

  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    send_to_char("Change your title to what?\n\r", ch);
    return;
  }

  if (strlen(argument) > 25)
    argument[25] = '\0';

  smash_tilde(argument);
  sprintf(buf, "#n");
  strcpy(buf, argument);
  set_title(ch, argument);
  send_to_char("Ok.\n\r", ch);
}

void do_afk(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;

  if (ch->fight_timer > 0)
  {
    send_to_char("not with a fighttimer.\n\r", ch);
    return;
  }
//  WAIT_STATE(ch, 36);
  if (IS_SET(ch->extra, EXTRA_AFK))
  {
    send_to_char("You are no longer AFK.\n\r", ch);
    REMOVE_BIT(ch->extra, EXTRA_AFK);
    return;
  }
  if ((ch->in_room == NULL || !IS_SET(ch->in_room->room_flags, ROOM_SAFE)) && ch->level < 6)
  {
    send_to_char("You can only go AFK in a saferoom.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->extra, EXTRA_AFK))
  {
    if (IS_SET(ch->pcdata->tempflag, TEMP_ARTIFACT))
    {
      send_to_char("You cannot go afk with an artifact.\n\r", ch);
      return;
    }
    if (ch->master)
      stop_follower(ch, FALSE);
    send_to_char("You are now AFK.\n\r", ch);
    SET_BIT(ch->extra, EXTRA_AFK);
    return;
  }
  return;
}

void do_description(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (argument[0] != '\0')
  {
    buf[0] = '\0';
    smash_tilde(argument);
    if (argument[0] == '+')
    {
      if (ch->description != NULL)
        strcat(buf, ch->description);
      argument++;
      while (isspace(*argument))
        argument++;
    }

    if (strlen2(argument) > 80)
    {
      send_to_char("80 chars per line, thanks.\n\r", ch);
      return;
    }

    if (strlen2(buf) + strlen2(argument) >= MAX_INPUT_LENGTH - 2)
    {
      send_to_char("Description too long.\n\r", ch);
      return;
    }

    strcat(buf, argument);
    strcat(buf, "\n\r");
    free_string(ch->description);
    ch->description = str_dup(buf);
  }

  send_to_char("Your description is:\n\r", ch);
  send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
  return;
}

void do_report(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  char buf[MAX_STRING_LENGTH];
  char hit_str[MAX_INPUT_LENGTH];
  char mana_str[MAX_INPUT_LENGTH];
  char move_str[MAX_INPUT_LENGTH];
  char mhit_str[MAX_INPUT_LENGTH];
  char mmana_str[MAX_INPUT_LENGTH];
  char mmove_str[MAX_INPUT_LENGTH];
  char exp_str[MAX_INPUT_LENGTH];

  sprintf(hit_str, "%d", ch->hit);
  COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
  sprintf(mana_str, "%d", ch->mana);
  COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
  sprintf(move_str, "%d", ch->move);
  COL_SCALE(move_str, ch, ch->move, ch->max_move);
  sprintf(exp_str, "%2.0f", ch->exp);
//  COL_SCALE(exp_str, ch, ch->exp, 1000);
  sprintf(mhit_str, "%d", ch->max_hit);
  ADD_COLOUR(ch, mhit_str, L_CYAN);
  sprintf(mmana_str, "%d", ch->max_mana);
  ADD_COLOUR(ch, mmana_str, L_CYAN);
  sprintf(mmove_str, "%d", ch->max_move);
  ADD_COLOUR(ch, mmove_str, L_CYAN);
  sprintf(buf, "You report: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r", hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);

  send_to_char(buf, ch);

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;
    if (vch == NULL)
      continue;
    if (vch == ch)
      continue;
    if (vch->in_room == NULL)
      continue;
    if (vch->in_room != ch->in_room)
      continue;
    sprintf(hit_str, "%d", ch->hit);
    COL_SCALE(hit_str, vch, ch->hit, ch->max_hit);
    sprintf(mana_str, "%d", ch->mana);
    COL_SCALE(mana_str, vch, ch->mana, ch->max_mana);
    sprintf(move_str, "%d", ch->move);
    COL_SCALE(move_str, vch, ch->move, ch->max_move);
    sprintf(exp_str, "%2.0f", ch->exp);
//    COL_SCALE(exp_str, vch, ch->exp, 1000);
    sprintf(mhit_str, "%d", ch->max_hit);
    ADD_COLOUR(vch, mhit_str, L_CYAN);
    sprintf(mmana_str, "%d", ch->max_mana);
    ADD_COLOUR(vch, mmana_str, L_CYAN);
    sprintf(mmove_str, "%d", ch->max_move);
    ADD_COLOUR(vch, mmove_str, L_CYAN);
    if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
      sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r", ch->morph, hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);
    else
      sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r", IS_NPC(ch) ? capitalize(ch->short_descr) : ch->name, hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, vch);
  }
  return;
}

void do_practice(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int sn;

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    int col, i;

    col = 0;
    for (i = 0; i < 5; i++)
    {
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
        if (skill_table[sn].name == NULL)
          break;
        if (ch->level < skill_table[sn].skill_level)
          continue;
        if (skill_table[sn].target != i)
          continue;
        switch (i)
        {
          case 0:
            sprintf(buf, "#p%18s #7%3d%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
            break;
          case 1:
            sprintf(buf, "#R%18s #7%3d%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
            break;
          case 2:
            sprintf(buf, "#L%18s #7%3d%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
            break;
          case 3:
            sprintf(buf, "#G%18s #7%3d%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
            break;
          case 4:
            sprintf(buf, "#y%18s #7%3d%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
            break;
          default:
            sprintf(buf, " ");
            break;
        }
        send_to_char(buf, ch);
        if (++col % 3 == 0)
          send_to_char("\n\r", ch);
      }
    }
    send_to_char("#n\n\r", ch);

    if (col % 3 != 0)
      send_to_char("\n\r", ch);

    sprintf(buf, "You have %2.0f exp left.\n\r", ch->exp);
    send_to_char(buf, ch);
  }
  else if (!strcmp(argument, "all"))
  {
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
      if (ch->exp < 5000)
        break;
      if ((skill_table[sn].name != NULL) && (ch->level >= skill_table[sn].skill_level) && ch->pcdata->learned[sn] < 100)
      {
        ch->pcdata->learned[sn] = 100;
        ch->exp -= 5000;
      }
    }
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else
  {
    if (!IS_AWAKE(ch))
    {
      send_to_char("In your dreams, or what?\n\r", ch);
      return;
    }

    if (ch->exp <= 0)
    {
      send_to_char("You have no exp left.\n\r", ch);
      return;
    }

    if ((sn = skill_lookup(argument)) < 0 || (!IS_NPC(ch) && ch->level < skill_table[sn].skill_level))
    {
      send_to_char("You can't practice that.\n\r", ch);
      return;
    }

    if (ch->pcdata->learned[sn] >= SKILL_ADEPT)
    {
      sprintf(buf, "You are already an adept of %s.\n\r", skill_table[sn].name);
      send_to_char(buf, ch);
    }
    else if (ch->pcdata->learned[sn] > 0 && ch->exp < 5000)
    {
      sprintf(buf, "You need 5000 exp to increase %s any more.\n\r", (skill_table[sn].name));
      send_to_char(buf, ch);
    }
    else if (ch->pcdata->learned[sn] == 0 && ch->exp < 5000)
    {
      sprintf(buf, "You need 5000 exp to increase %s.\n\r", skill_table[sn].name);
      send_to_char(buf, ch);
    }
    else
    {
      if (ch->pcdata->learned[sn] == 0)
      {
        ch->exp -= 5000;
        ch->pcdata->learned[sn] += 100;
      }
      else
      {
        ch->exp -= (ch->pcdata->learned[sn] / 2);
        ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
      }
      if (ch->pcdata->learned[sn] < SKILL_ADEPT)
      {
        act("You practice $T.", ch, NULL, skill_table[sn].name, TO_CHAR);
      }
      else
      {
        ch->pcdata->learned[sn] = SKILL_ADEPT;
        act("You are now an adept of $T.", ch, NULL, skill_table[sn].name, TO_CHAR);
      }
    }
  }
  return;
}

void do_password(CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char *pArg;
  char *pwdnew;
  char *p;
  char cEnd;

  if (IS_NPC(ch))
    return;

  /*
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
  pArg = arg1;
  while (isspace(*argument))
    argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';

  pArg = arg2;
  while (isspace(*argument))
    argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Syntax: password <old> <new>.\n\r", ch);
    return;
  }

  if (strcmp(arg1, ch->pcdata->pwd) && strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd))
  {
    WAIT_STATE(ch, 40);
    send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
    return;
  }

  if (strlen(arg2) < 5)
  {
    send_to_char("New password must be at least five characters long.\n\r", ch);
    return;
  }

  /*
   * No tilde allowed because of player file format.
   */
  pwdnew = crypt(arg2, ch->pcdata->switchname);
  for (p = pwdnew; *p != '\0'; p++)
  {
    if (*p == '~')
    {
      send_to_char("New password not acceptable, try again.\n\r", ch);
      return;
    }
  }

  free_string(ch->pcdata->pwd);
  ch->pcdata->pwd = str_dup(pwdnew);
  if (!IS_EXTRA(ch, EXTRA_NEWPASS))
    SET_BIT(ch->extra, EXTRA_NEWPASS);
  save_char_obj(ch);
  if (ch->desc != NULL && ch->desc->connected == CON_PLAYING)
    send_to_char("Ok.\n\r", ch);
  return;
}

void do_socials(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int iSocial;
  int col;

  col = 0;

  for (iSocial = 0; social_table[iSocial].name != NULL && social_table[iSocial].name[0] != '\0'; iSocial++)
  {
    sprintf(buf, "%-12s", social_table[iSocial].name);
    send_to_char(buf, ch);
    if (++col % 6 == 0)
      send_to_char("\n\r", ch);
  }

  if (col % 6 != 0)
    send_to_char("\n\r", ch);

  return;
}

void do_xsocials(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int iSocial;
  int col;

  col = 0;

  for (iSocial = 0; xsocial_table[iSocial].name[0] != '\0'; iSocial++)
  {
    sprintf(buf, "%-12s", xsocial_table[iSocial].name);
    send_to_char(buf, ch);
    if (++col % 6 == 0)
      send_to_char("\n\r", ch);
  }

  if (col % 6 != 0)
    send_to_char("\n\r", ch);

  return;
}

void do_spells(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int sn;
  int col;

  col = 0;
  for (sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++)
  {
    sprintf(buf, "%-12s", skill_table[sn].name);
    send_to_char(buf, ch);
    if (++col % 6 == 0)
      send_to_char("\n\r", ch);
  }

  if (col % 6 != 0)
    send_to_char("\n\r", ch);
  return;
}

/*
 * Contributed by Alander.
 */
void do_commands(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cmd;
  int col;

  col = 0;
  for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
  {
    if (cmd_table[cmd].level == 0 && cmd_table[cmd].level <= get_trust(ch))
    {
      sprintf(buf, "%-12s", cmd_table[cmd].name);
      send_to_char(buf, ch);
      if (++col % 6 == 0)
        send_to_char("\n\r", ch);
    }
  }

  if (col % 6 != 0)
    send_to_char("\n\r", ch);
  return;
}

void do_channels(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;
  if (arg[0] == '\0')
  {
    send_to_char("Channels:", ch);
    send_to_char(!IS_SET(ch->deaf, CHANNEL_CHAT) ? " +CHAT" : " -chat", ch);
    send_to_char(!IS_SET(ch->deaf, CHANNEL_FLAME) ? " +FLAME" : " -flame", ch);
    send_to_char(!IS_SET(ch->deaf, CHANNEL_NEWBIE) ? " +NEWBIE" : " -newbie", ch);
    send_to_char(!IS_SET(ch->deaf, CHANNEL_ROLEPLAY) ? " +ROLEPLAY" : " -roleplay", ch);
    if (IS_IMMORTAL(ch))
    {
      send_to_char(!IS_SET(ch->deaf, CHANNEL_IMMTALK) ? " +IMMTALK" : " -immtalk", ch);
      send_to_char(!IS_SET(ch->deaf, CHANNEL_LOG) ? " +LOG" : " -log", ch);
    }
    send_to_char(!IS_SET(ch->deaf, CHANNEL_MUSIC) ? " +MUSIC" : " -music", ch);
    if (ch->class != 0 || IS_IMMORTAL(ch))
    {
      send_to_char(!IS_SET(ch->deaf, CHANNEL_CLASS) ? " +CLASS" : " -class", ch);
    }
    send_to_char(!IS_SET(ch->deaf, CHANNEL_INFO) ? " +INFO" : " -info", ch);
    send_to_char(!IS_SET(ch->deaf, CHANNEL_TELL) ? " +TELL" : " -tell", ch);
    send_to_char(".\n\r", ch);
  }
  else
  {
    bool fClear;
    int bit;

    if (arg[0] == '+')
      fClear = TRUE;
    else if (arg[0] == '-')
      fClear = FALSE;
    else
    {
      send_to_char("Channels -channel or +channel?\n\r", ch);
      return;
    }

    if (!str_cmp(arg + 1, "chat"))
      bit = CHANNEL_CHAT;
    else if (!str_cmp(arg + 1, "immtalk"))
      bit = CHANNEL_IMMTALK;
    else if (!str_cmp(arg + 1, "flame"))
      bit = CHANNEL_FLAME;
    else if (!str_cmp(arg + 1, "music"))
      bit = CHANNEL_MUSIC;
    else if (!str_cmp(arg + 1, "newbie"))
      bit = CHANNEL_NEWBIE;
    else if (!str_cmp(arg + 1, "dktalk"))
      bit = CHANNEL_DKTALK;
    else if (!str_cmp(arg + 1, "roleplay"))
      bit = CHANNEL_ROLEPLAY;
    else if (!str_cmp(arg + 1, "yell"))
      bit = CHANNEL_YELL;
    else if (!str_cmp(arg + 1, "class"))
      bit = CHANNEL_CLASS;
    else if (IS_IMMORTAL(ch) && !str_cmp(arg + 1, "log"))
      bit = CHANNEL_LOG;
    else if (!str_cmp(arg + 1, "info"))
      bit = CHANNEL_INFO;
    else if (!str_cmp(arg + 1, "tell"))
      bit = CHANNEL_TELL;
    else
    {
      send_to_char("Set or clear which channel?\n\r", ch);
      return;
    }

    if (fClear)
      REMOVE_BIT(ch->deaf, bit);
    else
      SET_BIT(ch->deaf, bit);

    send_to_char("Ok.\n\r", ch);
  }

  return;
}

/*
 * Contributed by Grodyn.
 */
void do_config(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int bit;
  bool fSet;

  if (IS_NPC(ch))
    return;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("[ Keyword  ] Option\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_ANSI)
      ? "[+ANSI     ] You have ansi colour on.\n\r"
      : "[-ansi     ] You have ansi colour off.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_AUTOEXIT)
      ? "[+AUTOEXIT ] You automatically see exits.\n\r"
      : "[-autoexit ] You don't automatically see exits.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_AUTOLOOT)
      ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
      : "[-autoloot ] You don't automatically loot corpses.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_AUTOSAC)
      ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
      : "[-autosac  ] You don't automatically sacrifice corpses.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF1)
      ? "[+BRIEF1   ] You see brief descriptions.\n\r"
      : "[-brief1   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF2)
      ? "[+BRIEF2   ] You see brief descriptions.\n\r"
      : "[-brief2   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF3)
      ? "[+BRIEF3   ] You see brief descriptions.\n\r"
      : "[-brief3   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF4)
      ? "[+BRIEF4   ] You see brief descriptions.\n\r"
      : "[-brief4   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF5)
      ? "[+BRIEF5   ] You see brief descriptions.\n\r"
      : "[-brief5   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_BRIEF6)
      ? "[+BRIEF6   ] You see brief descriptions.\n\r"
      : "[-brief6   ] You see long descriptions.\n\r", ch);
    send_to_char(IS_SET(ch->act, PLR_MAP)
      ? "[+MAP      ] You see the automapper.\n\r"
      : "[-map      ] You don't see the automapper.\n\r", ch);
  }
  else
  {
    if (arg[0] == '+')
      fSet = TRUE;
    else if (arg[0] == '-')
      fSet = FALSE;
    else
    {
      send_to_char("Config -option or +option?\n\r", ch);
      return;
    }

    if (!str_cmp(arg + 1, "ansi"))
      bit = PLR_ANSI;
    else if (!str_cmp(arg + 1, "autoexit"))
      bit = PLR_AUTOEXIT;
    else if (!str_cmp(arg + 1, "autoloot"))
      bit = PLR_AUTOLOOT;
    else if (!str_cmp(arg + 1, "autosac"))
      bit = PLR_AUTOSAC;
    else if (!str_cmp(arg + 1, "map"))
      bit = PLR_MAP;
    else if (!str_cmp(arg + 1, "brief1"))
      bit = PLR_BRIEF1;
    else if (!str_cmp(arg + 1, "brief2"))
      bit = PLR_BRIEF2;
    else if (!str_cmp(arg + 1, "brief3"))
      bit = PLR_BRIEF3;
    else if (!str_cmp(arg + 1, "brief4"))
      bit = PLR_BRIEF4;
    else if (!str_cmp(arg + 1, "brief5"))
      bit = PLR_BRIEF5;
    else if (!str_cmp(arg + 1, "brief6"))
      bit = PLR_BRIEF6;
    else if (!str_cmp(arg + 1, "prompt"))
      bit = PLR_PROMPT;
    else
    {
      send_to_char("Config which option?\n\r", ch);
      return;
    }

    if (fSet)
    {
      SET_BIT(ch->act, bit);
      sprintf(buf, "%s is now ON\n\r", arg + 1);
      send_to_char(buf, ch);
    }
    else
    {
      REMOVE_BIT(ch->act, bit);
      sprintf(buf, "%s is now OFF\n\r", arg + 1);
      send_to_char(buf, ch);
    }
  }

  return;
}

void do_ansi(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_ANSI))
    do_config(ch, "-ansi");
  else
    do_config(ch, "+ansi");
  return;
}

void do_autoexit(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_AUTOEXIT))
    do_config(ch, "-autoexit");
  else
    do_config(ch, "+autoexit");
  return;
}

void do_autoloot(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_AUTOLOOT))
    do_config(ch, "-autoloot");
  else
    do_config(ch, "+autoloot");
  return;
}

void do_autosac(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_AUTOSAC))
    do_config(ch, "-autosac");
  else
    do_config(ch, "+autosac");
  return;
}

void do_map(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_MAP))
    do_config(ch, "-map");
  else
    do_config(ch, "+map");
  return;
}

void do_brief(CHAR_DATA * ch, char *argument)
{
  send_to_char("Please read HELP BRIEF.\n\r", ch);
  return;
}

void do_brief1(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF1))
    do_config(ch, "-brief1");
  else
    do_config(ch, "+brief1");
  return;
}

void do_brief2(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF2))
    do_config(ch, "-brief2");
  else
    do_config(ch, "+brief2");
  return;
}

void do_brief3(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF3))
    do_config(ch, "-brief3");
  else
    do_config(ch, "+brief3");
  return;
}

void do_brief4(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF4))
    do_config(ch, "-brief4");
  else
    do_config(ch, "+brief4");
  return;
}

void do_brief5(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF5))
    do_config(ch, "-brief5");
  else
    do_config(ch, "+brief5");
  return;
}

void do_brief6(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_BRIEF6))
    do_config(ch, "-brief6");
  else
    do_config(ch, "+brief6");
  return;
}

void do_diagnose(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int teeth = 0;
  int ribs = 0;
  CHAR_DATA *victim;

  argument = one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Who do you wish to diagnose?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("Nobody here by that name.\n\r", ch);
    return;
  }
  act("$n examines $N carefully, diagnosing $S injuries.", ch, NULL, victim, TO_NOTVICT);
  act("$n examines you carefully, diagnosing your injuries.", ch, NULL, victim, TO_VICT);
  act("Your diagnoses of $N reveals the following...", ch, NULL, victim, TO_CHAR);
  send_to_char("----------------------------------------------------------------------------\n\r", ch);
  if ((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] + victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] + victim->loc_hp[6]) == 0)
  {
    act("$N has no apparent injuries.", ch, NULL, victim, TO_CHAR);
    send_to_char("----------------------------------------------------------------------------\n\r", ch);
    return;
  }
  /* Check head */
  if (IS_HEAD(victim, LOST_EYE_L) && IS_HEAD(victim, LOST_EYE_R))
    act("$N has lost both of $S eyes.", ch, NULL, victim, TO_CHAR);
  else if (IS_HEAD(victim, LOST_EYE_L))
    act("$N has lost $S left eye.", ch, NULL, victim, TO_CHAR);
  else if (IS_HEAD(victim, LOST_EYE_R))
    act("$N has lost $S right eye.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, LOST_EAR_L) && IS_HEAD(victim, LOST_EAR_R))
    act("$N has lost both of $S ears.", ch, NULL, victim, TO_CHAR);
  else if (IS_HEAD(victim, LOST_EAR_L))
    act("$N has lost $S left ear.", ch, NULL, victim, TO_CHAR);
  else if (IS_HEAD(victim, LOST_EAR_R))
    act("$N has lost $S right ear.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, LOST_NOSE))
    act("$N has lost $S nose.", ch, NULL, victim, TO_CHAR);
  else if (IS_HEAD(victim, BROKEN_NOSE))
    act("$N has got a broken nose.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, BROKEN_JAW))
    act("$N has got a broken jaw.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, LOST_HEAD))
  {
    act("$N has had $S head cut off.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_HEAD))
      act("...Blood is spurting from the stump of $S neck.", ch, NULL, victim, TO_CHAR);
  }
  else
  {
    if (IS_BODY(victim, BROKEN_NECK))
      act("$N has got a broken neck.", ch, NULL, victim, TO_CHAR);
    if (IS_BODY(victim, CUT_THROAT))
    {
      act("$N has had $S throat cut open.", ch, NULL, victim, TO_CHAR);
      if (IS_BLEEDING(victim, BLEEDING_THROAT))
        act("...Blood is pouring from the wound.", ch, NULL, victim, TO_CHAR);
    }
  }
  if (IS_HEAD(victim, BROKEN_SKULL))
    act("$N has got a broken skull.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, LOST_TOOTH_1))
    teeth += 1;
  if (IS_HEAD(victim, LOST_TOOTH_2))
    teeth += 2;
  if (IS_HEAD(victim, LOST_TOOTH_4))
    teeth += 4;
  if (IS_HEAD(victim, LOST_TOOTH_8))
    teeth += 8;
  if (IS_HEAD(victim, LOST_TOOTH_16))
    teeth += 16;
  if (teeth > 0)
  {
    sprintf(buf, "$N has had %d teeth knocked out.", teeth);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  if (IS_HEAD(victim, LOST_TONGUE))
    act("$N has had $S tongue ripped out.", ch, NULL, victim, TO_CHAR);
  if (IS_HEAD(victim, LOST_HEAD))
  {
    send_to_char("----------------------------------------------------------------------------\n\r", ch);
    return;
  }
  /* Check body */
  if (IS_BODY(victim, BROKEN_RIBS_1))
    ribs += 1;
  if (IS_BODY(victim, BROKEN_RIBS_2))
    ribs += 2;
  if (IS_BODY(victim, BROKEN_RIBS_4))
    ribs += 4;
  if (IS_BODY(victim, BROKEN_RIBS_8))
    ribs += 8;
  if (IS_BODY(victim, BROKEN_RIBS_16))
    ribs += 16;
  if (ribs > 0)
  {
    sprintf(buf, "$N has got %d broken ribs.", ribs);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  if (IS_BODY(victim, BROKEN_SPINE))
    act("$N has got a broken spine.", ch, NULL, victim, TO_CHAR);
  /* Check arms */
  check_left_arm(ch, victim);
  check_right_arm(ch, victim);
  check_left_leg(ch, victim);
  check_right_leg(ch, victim);
  send_to_char("----------------------------------------------------------------------------\n\r", ch);
  return;
}

void check_left_arm(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char finger[10];
  int fingers = 0;

  if (IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM))
  {
    act("$N has lost both of $S arms.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_ARM_L) && IS_BLEEDING(victim, BLEEDING_ARM_R))
      act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_ARM_L))
      act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_ARM_R))
      act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_ARM_L(victim, LOST_ARM))
  {
    act("$N has lost $S left arm.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_ARM_L))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM))
    act("$N arms are both broken.", ch, NULL, victim, TO_CHAR);
  else if (IS_ARM_L(victim, BROKEN_ARM))
    act("$N's left arm is broken.", ch, NULL, victim, TO_CHAR);
  if (IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_ARM))
  {
    act("$N has lost both of $S hands.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_HAND_L) && IS_BLEEDING(victim, BLEEDING_HAND_R))
      act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_HAND_L))
      act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_HAND_R))
      act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_ARM_L(victim, LOST_HAND))
  {
    act("$N has lost $S left hand.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_HAND_L))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_ARM_L(victim, LOST_FINGER_I))
    fingers += 1;
  if (IS_ARM_L(victim, LOST_FINGER_M))
    fingers += 1;
  if (IS_ARM_L(victim, LOST_FINGER_R))
    fingers += 1;
  if (IS_ARM_L(victim, LOST_FINGER_L))
    fingers += 1;
  if (fingers == 1)
    sprintf(finger, "finger");
  else
    sprintf(finger, "fingers");
  if (fingers > 0 && IS_ARM_L(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has lost %d %s and $S thumb from $S left hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (fingers > 0)
  {
    sprintf(buf, "$N has lost %d %s from $S left hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_ARM_L(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has lost the thumb from $S left hand.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  fingers = 0;
  if (IS_ARM_L(victim, BROKEN_FINGER_I) && !IS_ARM_L(victim, LOST_FINGER_I))
    fingers += 1;
  if (IS_ARM_L(victim, BROKEN_FINGER_M) && !IS_ARM_L(victim, LOST_FINGER_M))
    fingers += 1;
  if (IS_ARM_L(victim, BROKEN_FINGER_R) && !IS_ARM_L(victim, LOST_FINGER_R))
    fingers += 1;
  if (IS_ARM_L(victim, BROKEN_FINGER_L) && !IS_ARM_L(victim, LOST_FINGER_L))
    fingers += 1;
  if (fingers == 1)
    sprintf(finger, "finger");
  else
    sprintf(finger, "fingers");
  if (fingers > 0 && IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has broken %d %s and $S thumb on $S left hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (fingers > 0)
  {
    sprintf(buf, "$N has broken %d %s on $S left hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has broken the thumb on $S left hand.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  return;
}

void check_right_arm(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char finger[10];
  int fingers = 0;

  if (IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM))
    return;
  if (IS_ARM_R(victim, LOST_ARM))
  {
    act("$N has lost $S right arm.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_ARM_R))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (!IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM))
    act("$N's right arm is broken.", ch, NULL, victim, TO_CHAR);
  else if (IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, BROKEN_ARM))
    act("$N's right arm is broken.", ch, NULL, victim, TO_CHAR);
  if (IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND))
    return;
  if (IS_ARM_R(victim, LOST_HAND))
  {
    act("$N has lost $S right hand.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_HAND_R))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_ARM_R(victim, LOST_FINGER_I))
    fingers += 1;
  if (IS_ARM_R(victim, LOST_FINGER_M))
    fingers += 1;
  if (IS_ARM_R(victim, LOST_FINGER_R))
    fingers += 1;
  if (IS_ARM_R(victim, LOST_FINGER_L))
    fingers += 1;
  if (fingers == 1)
    sprintf(finger, "finger");
  else
    sprintf(finger, "fingers");
  if (fingers > 0 && IS_ARM_R(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has lost %d %s and $S thumb from $S right hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (fingers > 0)
  {
    sprintf(buf, "$N has lost %d %s from $S right hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_ARM_R(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has lost the thumb from $S right hand.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  fingers = 0;
  if (IS_ARM_R(victim, BROKEN_FINGER_I) && !IS_ARM_R(victim, LOST_FINGER_I))
    fingers += 1;
  if (IS_ARM_R(victim, BROKEN_FINGER_M) && !IS_ARM_R(victim, LOST_FINGER_M))
    fingers += 1;
  if (IS_ARM_R(victim, BROKEN_FINGER_R) && !IS_ARM_R(victim, LOST_FINGER_R))
    fingers += 1;
  if (IS_ARM_R(victim, BROKEN_FINGER_L) && !IS_ARM_R(victim, LOST_FINGER_L))
    fingers += 1;
  if (fingers == 1)
    sprintf(finger, "finger");
  else
    sprintf(finger, "fingers");
  if (fingers > 0 && IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has broken %d %s and $S thumb on $S right hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (fingers > 0)
  {
    sprintf(buf, "$N has broken %d %s on $S right hand.", fingers, finger);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB))
  {
    sprintf(buf, "$N has broken the thumb on $S right hand.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  return;
}

void check_left_leg(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char toe[10];
  int toes = 0;

  if (IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG))
  {
    act("$N has lost both of $S legs.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_LEG_L) && IS_BLEEDING(victim, BLEEDING_LEG_R))
      act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_LEG_L))
      act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_LEG_R))
      act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_LEG_L(victim, LOST_LEG))
  {
    act("$N has lost $S left leg.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_LEG_L))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG))
    act("$N legs are both broken.", ch, NULL, victim, TO_CHAR);
  else if (IS_LEG_L(victim, BROKEN_LEG))
    act("$N's left leg is broken.", ch, NULL, victim, TO_CHAR);
  if (IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT))
  {
    act("$N has lost both of $S feet.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_FOOT_L) && IS_BLEEDING(victim, BLEEDING_FOOT_R))
      act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_FOOT_L))
      act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
    else if (IS_BLEEDING(victim, BLEEDING_FOOT_R))
      act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_LEG_L(victim, LOST_FOOT))
  {
    act("$N has lost $S left foot.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_FOOT_L))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_LEG_L(victim, LOST_TOE_A))
    toes += 1;
  if (IS_LEG_L(victim, LOST_TOE_B))
    toes += 1;
  if (IS_LEG_L(victim, LOST_TOE_C))
    toes += 1;
  if (IS_LEG_L(victim, LOST_TOE_D))
    toes += 1;
  if (toes == 1)
    sprintf(toe, "toe");
  else
    sprintf(toe, "toes");
  if (toes > 0 && IS_LEG_L(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has lost %d %s and $S big toe from $S left foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (toes > 0)
  {
    sprintf(buf, "$N has lost %d %s from $S left foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_LEG_L(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has lost the big toe from $S left foot.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  toes = 0;
  if (IS_LEG_L(victim, BROKEN_TOE_A) && !IS_LEG_L(victim, LOST_TOE_A))
    toes += 1;
  if (IS_LEG_L(victim, BROKEN_TOE_B) && !IS_LEG_L(victim, LOST_TOE_B))
    toes += 1;
  if (IS_LEG_L(victim, BROKEN_TOE_C) && !IS_LEG_L(victim, LOST_TOE_C))
    toes += 1;
  if (IS_LEG_L(victim, BROKEN_TOE_D) && !IS_LEG_L(victim, LOST_TOE_D))
    toes += 1;
  if (toes == 1)
    sprintf(toe, "toe");
  else
    sprintf(toe, "toes");
  if (toes > 0 && IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has broken %d %s and $S big toe from $S left foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (toes > 0)
  {
    sprintf(buf, "$N has broken %d %s on $S left foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has broken the big toe on $S left foot.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  return;
}

void check_right_leg(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char toe[10];
  int toes = 0;

  if (IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG))
    return;
  if (IS_LEG_R(victim, LOST_LEG))
  {
    act("$N has lost $S right leg.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_LEG_R))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (!IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG))
    act("$N's right leg is broken.", ch, NULL, victim, TO_CHAR);
  if (IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT))
    return;
  if (IS_LEG_R(victim, LOST_FOOT))
  {
    act("$N has lost $S right foot.", ch, NULL, victim, TO_CHAR);
    if (IS_BLEEDING(victim, BLEEDING_FOOT_R))
      act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_LEG_R(victim, LOST_TOE_A))
    toes += 1;
  if (IS_LEG_R(victim, LOST_TOE_B))
    toes += 1;
  if (IS_LEG_R(victim, LOST_TOE_C))
    toes += 1;
  if (IS_LEG_R(victim, LOST_TOE_D))
    toes += 1;
  if (toes == 1)
    sprintf(toe, "toe");
  else
    sprintf(toe, "toes");
  if (toes > 0 && IS_LEG_R(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has lost %d %s and $S big toe from $S right foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (toes > 0)
  {
    sprintf(buf, "$N has lost %d %s from $S right foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_LEG_R(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has lost the big toe from $S right foot.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  toes = 0;
  if (IS_LEG_R(victim, BROKEN_TOE_A) && !IS_LEG_R(victim, LOST_TOE_A))
    toes += 1;
  if (IS_LEG_R(victim, BROKEN_TOE_B) && !IS_LEG_R(victim, LOST_TOE_B))
    toes += 1;
  if (IS_LEG_R(victim, BROKEN_TOE_C) && !IS_LEG_R(victim, LOST_TOE_C))
    toes += 1;
  if (IS_LEG_R(victim, BROKEN_TOE_D) && !IS_LEG_R(victim, LOST_TOE_D))
    toes += 1;
  if (toes == 1)
    sprintf(toe, "toe");
  else
    sprintf(toe, "toes");
  if (toes > 0 && IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has broken %d %s and $S big toe on $S right foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (toes > 0)
  {
    sprintf(buf, "$N has broken %d %s on $S right foot.", toes, toe);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else if (IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG))
  {
    sprintf(buf, "$N has broken the big toe on $S right foot.");
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  return;
}

void obj_score(CHAR_DATA * ch, OBJ_DATA * obj)
{
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;
  int itemtype;

  /*
   * very anoying bug with peoples heads not fading.
   */
  if (!obj)
    return;
  if (!obj->short_descr)
    return;

  sprintf(buf, "You are %s.\n\r", obj->short_descr);
  send_to_char(buf, ch);

  sprintf(buf, "Type %s, Extra flags %s.\n\r", item_type_name(obj), extra_bit_name(obj->extra_flags));
  send_to_char(buf, ch);

  sprintf(buf, "You weigh %d pounds and are worth %d gold coins.\n\r", obj->weight, obj->cost);
  send_to_char(buf, ch);

  if (obj->questmaker != NULL && strlen(obj->questmaker) > 1 && obj->questowner != NULL && strlen(obj->questowner) > 1)
  {
    sprintf(buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker, obj->questowner);
    send_to_char(buf, ch);
  }
  else if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
  {
    sprintf(buf, "You were created by %s.\n\r", obj->questmaker);
    send_to_char(buf, ch);
  }
  else if (obj->questowner != NULL && strlen(obj->questowner) > 1)
  {
    sprintf(buf, "You are owned by %s.\n\r", obj->questowner);
    send_to_char(buf, ch);
  }

  switch (obj->item_type)
  {
    case ITEM_SCROLL:
    case ITEM_POTION:
      sprintf(buf, "You contain level %d spells of:", obj->value[0]);
      send_to_char(buf, ch);

      if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
      {
        send_to_char(" '", ch);
        send_to_char(skill_table[obj->value[1]].name, ch);
        send_to_char("'", ch);
      }

      if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
      {
        send_to_char(" '", ch);
        send_to_char(skill_table[obj->value[2]].name, ch);
        send_to_char("'", ch);
      }

      if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
      {
        send_to_char(" '", ch);
        send_to_char(skill_table[obj->value[3]].name, ch);
        send_to_char("'", ch);
      }

      send_to_char(".\n\r", ch);
      break;

    case ITEM_QUEST:
      sprintf(buf, "Your quest point value is %d.\n\r", obj->value[0]);
      send_to_char(buf, ch);
      break;

    case ITEM_WAND:
    case ITEM_STAFF:
      sprintf(buf, "You have %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0]);
      send_to_char(buf, ch);

      if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
      {
        send_to_char(" '", ch);
        send_to_char(skill_table[obj->value[3]].name, ch);
        send_to_char("'", ch);
      }

      send_to_char(".\n\r", ch);
      break;

    case ITEM_WEAPON:
      sprintf(buf, "You inflict %d to %d damage in combat (average %d).\n\r", obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2);
      send_to_char(buf, ch);

      if (obj->value[0] >= 1000)
        itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
      else
        itemtype = obj->value[0];

      if (itemtype > 0)
      {
        if (obj->level < 10)
          sprintf(buf, "You are a minor spell weapon.\n\r");
        else if (obj->level < 20)
          sprintf(buf, "You are a lesser spell weapon.\n\r");
        else if (obj->level < 30)
          sprintf(buf, "You are an average spell weapon.\n\r");
        else if (obj->level < 40)
          sprintf(buf, "You are a greater spell weapon.\n\r");
        else if (obj->level < 50)
          sprintf(buf, "You are a major spell weapon.\n\r");
        else
          sprintf(buf, "You are a supreme spell weapon.\n\r");
        send_to_char(buf, ch);
      }

      if (itemtype == 1)
        sprintf(buf, "You are dripping with corrosive acid.\n\r");
      else if (itemtype == 4)
        sprintf(buf, "You radiate an aura of darkness.\n\r");
      else if (itemtype == 30)
        sprintf(buf, "You are the bane of all evil.\n\r");
      else if (itemtype == 34)
        sprintf(buf, "You drink the souls of your victims.\n\r");
      else if (itemtype == 37)
        sprintf(buf, "You have been tempered in hellfire.\n\r");
      else if (itemtype == 48)
        sprintf(buf, "You crackle with sparks of lightning.\n\r");
      else if (itemtype == 53)
        sprintf(buf, "You are dripping with a dark poison.\n\r");
      else if (itemtype > 0)
        sprintf(buf, "You have been imbued with the power of %s.\n\r", skill_table[itemtype].name);
      if (itemtype > 0)
        send_to_char(buf, ch);

      if (obj->value[0] >= 1000)
        itemtype = obj->value[0] / 1000;
      else
        break;

      if (itemtype == 4 || itemtype == 1)
        sprintf(buf, "You radiate an aura of darkness.\n\r");
      else if (itemtype == 27 || itemtype == 2)
        sprintf(buf, "You allow your wielder to see invisible things.\n\r");
      else if (itemtype == 39 || itemtype == 3)
        sprintf(buf, "You grant your wielder the power of flight.\n\r");
      else if (itemtype == 45 || itemtype == 4)
        sprintf(buf, "You allow your wielder to see in the dark.\n\r");
      else if (itemtype == 46 || itemtype == 5)
        sprintf(buf, "You render your wielder invisible to the human eye.\n\r");
      else if (itemtype == 52 || itemtype == 6)
        sprintf(buf, "You allow your wielder to walk through solid doors.\n\r");
      else if (itemtype == 54 || itemtype == 7)
        sprintf(buf, "You protect your wielder from evil.\n\r");
      else if (itemtype == 57 || itemtype == 8)
        sprintf(buf, "You protect your wielder in combat.\n\r");
      else if (itemtype == 9)
        sprintf(buf, "You allow your wielder to walk in complete silence.\n\r");
      else if (itemtype == 10)
        sprintf(buf, "You surround your wielder with a shield of lightning.\n\r");
      else if (itemtype == 11)
        sprintf(buf, "You surround your wielder with a shield of fire.\n\r");
      else if (itemtype == 12)
        sprintf(buf, "You surround your wielder with a shield of ice.\n\r");
      else if (itemtype == 13)
        sprintf(buf, "You surround your wielder with a shield of acid.\n\r");
      else if (itemtype == 14)
        sprintf(buf, "You channel the power of god into your wielder.\n\r");
      else if (itemtype == 15)
        sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
      else if (itemtype == 16)
        sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
      else if (itemtype == 17)
        sprintf(buf, "You enable your wielder to move at supernatural speed.\n\r");
      else if (itemtype == 18)
        sprintf(buf, "You can slice through armour without difficulty.\n\r");
      else if (itemtype == 19)
        sprintf(buf, "You protect your wielder from player attacks.\n\r");
      else if (itemtype == 20)
        sprintf(buf, "You surround your wielder with a shield of darkness.\n\r");
      else if (itemtype == 21)
        sprintf(buf, "You grant your wielder superior protection.\n\r");
      else if (itemtype == 22)
        sprintf(buf, "You grant your wielder supernatural vision.\n\r");
      else if (itemtype == 23)
        sprintf(buf, "You make your wielder fleet-footed.\n\r");
      else if (itemtype == 24)
        sprintf(buf, "You conceal your wielder from sight.\n\r");
      else if (itemtype == 25)
        sprintf(buf, "You invoke the power of your wielders beast.\n\r");
      else
        sprintf(buf, "You are bugged...please report it.\n\r");
      if (itemtype > 0)
        send_to_char(buf, ch);
      break;

    case ITEM_ARMOR:
      sprintf(buf, "Your armor class is %d.\n\r", obj->value[0]);
      send_to_char(buf, ch);
      if (obj->value[3] < 1)
        break;
      if (obj->value[3] == 4 || obj->value[3] == 1)
        sprintf(buf, "You radiate an aura of darkness.\n\r");
      else if (obj->value[3] == 27 || obj->value[3] == 2)
        sprintf(buf, "You allow your wearer to see invisible things.\n\r");
      else if (obj->value[3] == 39 || obj->value[3] == 3)
        sprintf(buf, "You grant your wearer the power of flight.\n\r");
      else if (obj->value[3] == 45 || obj->value[3] == 4)
        sprintf(buf, "You allow your wearer to see in the dark.\n\r");
      else if (obj->value[3] == 46 || obj->value[3] == 5)
        sprintf(buf, "You render your wearer invisible to the human eye.\n\r");
      else if (obj->value[3] == 52 || obj->value[3] == 6)
        sprintf(buf, "You allow your wearer to walk through solid doors.\n\r");
      else if (obj->value[3] == 54 || obj->value[3] == 7)
        sprintf(buf, "You protect your wearer from evil.\n\r");
      else if (obj->value[3] == 57 || obj->value[3] == 8)
        sprintf(buf, "You protect your wearer in combat.\n\r");
      else if (obj->value[3] == 9)
        sprintf(buf, "You allow your wearer to walk in complete silence.\n\r");
      else if (obj->value[3] == 10)
        sprintf(buf, "You surround your wearer with a shield of lightning.\n\r");
      else if (obj->value[3] == 11)
        sprintf(buf, "You surround your wearer with a shield of fire.\n\r");
      else if (obj->value[3] == 12)
        sprintf(buf, "You surround your wearer with a shield of ice.\n\r");
      else if (obj->value[3] == 13)
        sprintf(buf, "You surround your wearer with a shield of acid.\n\r");
      else if (obj->value[3] == 14)
        sprintf(buf, "You protect your wearer from attacks from DarkBlade clan guardians.\n\r");
      else if (obj->value[3] == 15)
        sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
      else if (obj->value[3] == 16)
        sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
      else if (obj->value[3] == 17)
        sprintf(buf, "You enable your wearer to move at supernatural speed.\n\r");
      else if (obj->value[3] == 18)
        sprintf(buf, "You can slice through armour without difficulty.\n\r");
      else if (obj->value[3] == 19)
        sprintf(buf, "You protect your wearer from player attacks.\n\r");
      else if (obj->value[3] == 20)
        sprintf(buf, "You surround your wearer with a shield of darkness.\n\r");
      else if (obj->value[3] == 21)
        sprintf(buf, "You grant your wearer superior protection.\n\r");
      else if (obj->value[3] == 22)
        sprintf(buf, "You grant your wearer supernatural vision.\n\r");
      else if (obj->value[3] == 23)
        sprintf(buf, "You make your wearer fleet-footed.\n\r");
      else if (obj->value[3] == 24)
        sprintf(buf, "You conceal your wearer from sight.\n\r");
      else if (obj->value[3] == 25)
        sprintf(buf, "You invoke the power of your wearers beast.\n\r");
      else
        sprintf(buf, "You are bugged...please report it.\n\r");
      if (obj->value[3] > 0)
        send_to_char(buf, ch);
      break;
  }

  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location != APPLY_NONE && paf->modifier != 0)
    {
      sprintf(buf, "You affect %s by %d.\n\r", affect_loc_name(paf->location), paf->modifier);
      send_to_char(buf, ch);
    }
  }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location != APPLY_NONE && paf->modifier != 0)
    {
      sprintf(buf, "You affect %s by %d.\n\r", affect_loc_name(paf->location), paf->modifier);
      send_to_char(buf, ch);
    }
  }
  return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';
  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    do_help(ch, "prompt");
    return;
  }

  if (!strcmp(argument, "on"))
  {
    if (IS_EXTRA(ch, EXTRA_PROMPT))
      send_to_char("But you already have customised prompt on!\n\r", ch);
    else
    {
      send_to_char("Ok.\n\r", ch);
      SET_BIT(ch->extra, EXTRA_PROMPT);
    }
    return;
  }
  else if (!strcmp(argument, "off"))
  {
    if (!IS_EXTRA(ch, EXTRA_PROMPT))
      send_to_char("But you already have customised prompt off!\n\r", ch);
    else
    {
      send_to_char("Ok.\n\r", ch);
      REMOVE_BIT(ch->extra, EXTRA_PROMPT);
    }
    return;
  }
  else if (!strcmp(argument, "clear"))
  {
    free_string(ch->prompt);
    ch->prompt = str_dup("");
    return;
  }
  else
  {
    if (strlen(argument) > 80)
      argument[80] = '\0';
    smash_tilde(argument);
    strcat(buf, argument);
  }

  free_string(ch->prompt);
  ch->prompt = str_dup(buf);
  send_to_char("Ok.\n\r", ch);
  return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_cprompt(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';
  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    do_help(ch, "cprompt");
    return;
  }

  if (!strcmp(argument, "clear"))
  {
    free_string(ch->cprompt);
    ch->cprompt = str_dup("");
    return;
  }
  else
  {
    if (strlen(argument) > 80)
      argument[80] = '\0';
    smash_tilde(argument);
    strcat(buf, argument);
  }

  free_string(ch->cprompt);
  ch->cprompt = str_dup(buf);
  send_to_char("Ok.\n\r", ch);
  return;
}

void do_finger(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_STRING_LENGTH];
  char strsave[MAX_INPUT_LENGTH];
  char *buf;
  char buf2[MAX_INPUT_LENGTH];
  FILE *fp;
  int num;
  int num2;
  int extra;

  if (IS_NPC(ch))
    return;
  one_argument(argument, arg);

  if (!check_parse_name(argument))
  {
    send_to_char("Thats an illegal name.\n\r", ch);
    return;
  }
  if (!char_exists(TRUE, argument))
  {
    send_to_char("That player doesn't exist.\n\r", ch);
    return;
  }
  sprintf(strsave, "%sbackup/%s", PLAYER_DIR, capitalize(argument));
  if ((fp = fopen(strsave, "r")) != NULL)
  {
    sprintf(buf2, "%s\n\r", generate_banner( "#yAGE OF HEROES", 68, "#0" ) );
    send_to_char(buf2, ch);

    buf = fread_string(fp);
    send_to_char(buf, ch);
    send_to_char(" ", ch);
    free_string(buf);

    buf = fread_string(fp);
    send_to_char(buf, ch);
    free_string(buf);

    buf = fread_string(fp);
    if (ch->level > 6)
    {
      send_to_char(" @", ch);
      send_to_char(buf, ch);
    }
    free_string(buf);

    sprintf(buf2, "\n\r%s\n\r", generate_banner( "", 68, "#0" ) );
    send_to_char(buf2, ch);
    send_to_char("Last connected ", ch);
    send_to_char("at ", ch);

    buf = fread_string(fp);
    send_to_char(buf, ch);
    free_string(buf);

    sprintf(buf2, ".\n\r%s\n\r", generate_banner( "", 68, "#0" ) ); 
    send_to_char(buf2, ch);
    extra = fread_number(fp);
    num = fread_number(fp);
    send_to_char("Sex: ", ch);

    buf = fread_string(fp);
    if (num == SEX_MALE)
      send_to_char("Male. ", ch);
    else if (num == SEX_FEMALE)
    {
      send_to_char("Female. ", ch);
      if (IS_SET(extra, EXTRA_PREGNANT))
        other_age(ch, extra, TRUE, buf);
    }
    else
      send_to_char("None. ", ch);
    free_string(buf);

    buf = fread_string(fp);
    other_age(ch, extra, FALSE, buf);
    free_string(buf);

    num = fread_number(fp);
    switch (num)
    {
      default:
        sprintf(buf2, "They are mortal, ");
        break;
      case LEVEL_AVATAR:
        sprintf(buf2, "They are an Avatar, ");
        break;
      case LEVEL_BUILDER:
      case LEVEL_QUESTMAKER:
      case LEVEL_ENFORCER:
      case LEVEL_JUDGE:
      case LEVEL_HIGHJUDGE:
      case LEVEL_IMPLEMENTOR:
        sprintf(buf2, "They are a God, ");
        break;
    }
    send_to_char(buf2, ch);

    num = fread_number(fp);
    if (num > 0)
      num2 = (num / 3600);
    else
      num2 = 0;
    sprintf(buf2, "and have been playing for %d hours.\n\r", num2);
    send_to_char(buf2, ch);

    buf = fread_string(fp);
    if (strlen(buf) > 2)
    {
      if (IS_SET(extra, EXTRA_MARRIED))
        sprintf(buf2, "They are married to %s.\n\r", buf);
      else
        sprintf(buf2, "They are engaged to %s.\n\r", buf);
      send_to_char(buf2, ch);
    }
    free_string(buf);

    num = fread_number(fp);
    num2 = fread_number(fp);
    sprintf(buf2, "Player kills: %d, Player Deaths: %d.\n\r", num, num2);
    send_to_char(buf2, ch);
    num = fread_number(fp);
    num2 = fread_number(fp);
    sprintf(buf2, "Arena kills: %d, Arena Deaths: %d.\n\r", num, num2);
    send_to_char(buf2, ch);
    num = fread_number(fp);
    num2 = fread_number(fp);
    sprintf(buf2, "Mob Kills: %d, Mob Deaths: %d.\n\r", num, num2);
    send_to_char(buf2, ch);

    sprintf(buf2, "%s\n\r", generate_banner( "", 68, "#0" ) ); 
    send_to_char(buf2, ch);
  }
  else
  {
    bug("Do_finger: fopen", 0);
    return;
  }
  fclose(fp);
  return;
}

