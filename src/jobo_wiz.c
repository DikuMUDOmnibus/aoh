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

void do_logstat(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int ratio, mspusers, a, b, c;

  if (IS_NPC(ch))
    return;

  sprintf(buf, "Total Players Created                 : %d players\n\r", top_playerid);
  send_to_char(buf, ch);
  a = mudinfo[MUDINFO_UPDATED] / 120;
  b = a / 24;
  c = a - b * 24;
  sprintf(buf, "Mudinfo : Days running                : %d days and %d hours\n\r", b, c);
  send_to_char(buf, ch);
  sprintf(buf, "Players : Average Online              : %d\n\r", (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS]) / mudinfo[MUDINFO_UPDATED]);
  send_to_char(buf, ch);
  sprintf(buf, "Players : Peak Online                 : %d\n\r", mudinfo[MUDINFO_PEAK_USERS]);
  send_to_char(buf, ch);
  if (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS] == 0) ratio = 0;
  else ratio = (100 * mudinfo[MUDINFO_MCCP_USERS] /
    (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS]));
  sprintf(buf, "Players : Mccp Users                  : %d %%\n\r", ratio);
  send_to_char(buf, ch);
  if (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS] == 0) mspusers = 0;
  else mspusers = (100 * mudinfo[MUDINFO_MSP_USERS] /
    (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS]));
  sprintf(buf, "Players : MSP Users                   : %d %%\n\r", mspusers);
  send_to_char(buf, ch);

  /*
   * a : Amount of kbit data send totally.
   * b : Amount of kbit data send each 10 seconds
   * c : The fractional part
   */
  a = mudinfo[MUDINFO_MBYTE] * 1024 + mudinfo[MUDINFO_BYTE] / 1024;
  b = a / (mudinfo[MUDINFO_UPDATED] * 3);
  c = b / 10;
  c = c * 10;
  c = b - c;
  sprintf(buf, "Datatransfer : Average Rate           : %d.%d kb/sec\n\r", b / 10, c);
  send_to_char(buf, ch);
  b = mudinfo[MUDINFO_DATA_PEAK] / (3 * 1024);
  c = b / 10;
  c = c * 10;
  c = b - c;
  sprintf(buf, "Datatransfer : Peak Rate              : %d.%d kb/sec\n\r", b / 10, c);
  send_to_char(buf, ch);
  sprintf(buf, "Datatransfer : This Week              : %d MB\n\r", mudinfo[MUDINFO_MBYTE]);
  send_to_char(buf, ch);
  sprintf(buf, "Datatransfer : This Boot              : %d MB\n\r", mudinfo[MUDINFO_MBYTE_S]);
  send_to_char(buf, ch);
  return;
}

void do_noset(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("No such player.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  if (IS_SET(victim->pcdata->jflags, JFLAG_NOSET))
  {
    REMOVE_BIT(victim->pcdata->jflags, JFLAG_NOSET);
    send_to_char("Noset removed.\n\r", ch);
    send_to_char("You can set your messages again.\n\r", victim);
    return;
  }

  free_string(victim->pcdata->loginmessage);
  victim->pcdata->loginmessage = str_dup("");
  free_string(victim->pcdata->logoutmessage);
  victim->pcdata->logoutmessage = str_dup("");
  free_string(victim->pcdata->decapmessage);
  victim->pcdata->decapmessage = str_dup("");
  free_string(victim->pcdata->tiemessage);
  victim->pcdata->tiemessage = str_dup("");
  free_string(victim->pcdata->avatarmessage);
  victim->pcdata->avatarmessage = str_dup("");
  REMOVE_BIT(victim->pcdata->jflags, JFLAG_SETTIE);
  REMOVE_BIT(victim->pcdata->jflags, JFLAG_SETAVATAR);
  REMOVE_BIT(victim->pcdata->jflags, JFLAG_SETLOGIN);
  REMOVE_BIT(victim->pcdata->jflags, JFLAG_SETLOGOUT);
  REMOVE_BIT(victim->pcdata->jflags, JFLAG_SETDECAP);
  SET_BIT(victim->pcdata->jflags, JFLAG_NOSET);
  send_to_char("Ok.\n\r", ch);
  send_to_char("All your custom messages have been removed, and you can no longer change your messages.\n\r", victim);
  return;
}

void do_multicheck(CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  DESCRIPTOR_DATA *d2;
  char buf[MAX_STRING_LENGTH];
  bool found = FALSE;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE)
      continue;
    if (d->connected != CON_PLAYING)
      continue;
    for (d2 = d->next; d2 != NULL; d2 = d2->next)
    {
      if (d2->lookup_status != STATUS_DONE)
        continue;
      if (!str_cmp(d->host, d2->host))
      {
        if (d2->connected != CON_PLAYING)
          continue;
        if (d2->character == NULL || d->character == NULL)
          continue;
        found = TRUE;
        sprintf(buf, "%s and %s are multiplaying bitches\n\r", d2->character->name, d->character->name);
        send_to_char(buf, ch);
      }
    }
  }
  if (!found)
    send_to_char("Noone is multiplaying atm.\n\r", ch);
  return;
}

void do_openthearena(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (arena_open)
    return;
  arena_open = TRUE;
  return;
}

void do_showsilence(CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *gch;
  char buf[MAX_STRING_LENGTH];
  bool found = FALSE;

  if (IS_NPC(ch))
    return;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected != CON_PLAYING)
      continue;
    if (d->character != NULL)
      gch = d->character;
    else
      continue;
    if (gch->level > 6)
      continue;
    if (IS_SET(gch->act, PLR_SILENCE))
    {
      found = TRUE;
      sprintf(buf, "%-15s is silenced\n\r", gch->name);
      send_to_char(buf, ch);
    }
  }
  if (!found)
    send_to_char("Noone is silenced.\n\r", ch);
  return;
}

void do_asperson(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *tmp;
  char arg[MAX_INPUT_LENGTH];
  bool afk = FALSE;

  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("That would be a bad idea.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  tmp = victim->desc;
  victim->desc = ch->desc;
  if (IS_SET(victim->extra, EXTRA_AFK))
  {
    afk = TRUE;
    REMOVE_BIT(victim->extra, EXTRA_AFK);
  }
  interpret(victim, argument);
  victim->desc = tmp;
  if (afk)
    SET_BIT(victim->extra, EXTRA_AFK);
  return;
}

void do_bully(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;
  if (ch->level < MAX_LEVEL)
    return;
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("That would be a bad idea.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  if (IS_SET(victim->pcdata->jflags, JFLAG_BULLY))
  {
    REMOVE_BIT(victim->pcdata->jflags, JFLAG_BULLY);
    send_to_char("You are to nice.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->pcdata->jflags, JFLAG_BULLY);
    send_to_char("Take that bully.\n\r", ch);
  }
  return;
}

void do_offline(CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *dummy;
  DESCRIPTOR_DATA *temp;
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;
  if (ch->level < MAX_LEVEL)
  {
    send_to_char("This action is restricted.\n\r", ch);
    return;
  }
  if (strlen(arg) < 3)
  {
    send_to_char("What player do you wish to load?\n\r", ch);
    return;
  }
  if (argument[0] == '\0')
  {
    send_to_char("What do you want the player to do?\n\r", ch);
    return;
  }
  arg[0] = UPPER(arg[0]);

  if (descriptor_free == NULL)
  {
    dummy = alloc_perm(sizeof(*dummy));
  }
  else
  {
    dummy = descriptor_free;
    descriptor_free = descriptor_free->next;
  }

  if (load_char_obj(dummy, arg))
  {
    victim = dummy->character;
    temp = victim->desc;
    victim->desc = ch->desc;
    interpret(victim, argument);
    victim->desc = temp;
  }
  else
  {
    send_to_char("Player doesn't exist.\n\r", ch);
    dummy->next = descriptor_free;
    descriptor_free = dummy;
    return;
  }

  /*
   * Clean up.
   */
  save_char_obj(victim);
  free_char(victim);

  dummy->next = descriptor_free;
  descriptor_free = dummy;

  return;
}

void do_newban(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  BAN_DATA *pban;

  if (IS_NPC(ch))
    return;

  argument = one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    strcpy(buf, "Newbie Banned sites:\n\r");
    for (pban = newbieban_list; pban != NULL; pban = pban->next)
    {
      strcat(buf, pban->name);
      strcat(buf, "    (");
      strcat(buf, pban->reason);
      strcat(buf, ")\n\r");
    }
    send_to_char(buf, ch);
    return;
  }
  for (pban = newbieban_list; pban != NULL; pban = pban->next)
  {
    if (!str_cmp(arg, pban->name))
    {
      send_to_char("That site is already banned!\n\r", ch);
      return;
    }
  }
  if (newbieban_free == NULL)
  {
    pban = alloc_perm(sizeof(*pban));
  }
  else
  {
    pban = newbieban_free;
    newbieban_free = newbieban_free->next;
  }
  pban->name = str_dup(arg);
  if (argument[0] == '\0')
    pban->reason = str_dup("no reason given");
  else
    pban->reason = str_dup(argument);
  pban->next = newbieban_list;
  newbieban_list = pban;
  send_to_char("Ok.\n\r", ch);
  save_newbiebans();
  return;
}

void do_newallow(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  BAN_DATA *prev;
  BAN_DATA *curr;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Remove which site from the ban list?\n\r", ch);
    return;
  }
  prev = NULL;
  for (curr = newbieban_list; curr != NULL; prev = curr, curr = curr->next)
  {
    if (!str_cmp(arg, curr->name))
    {
      if (prev == NULL)
        newbieban_list = newbieban_list->next;
      else
        prev->next = curr->next;
      free_string(curr->name);
      free_string(curr->reason);
      curr->next = newbieban_free;
      newbieban_free = curr;
      send_to_char("Ok.\n\r", ch);
      save_newbiebans();
      return;
    }
  }
  send_to_char("Site is not banned.\n\r", ch);
  return;
}

void do_logstatclear(CHAR_DATA * ch, char *argument)
{
  int i;

  if (IS_NPC(ch))
    return;

  for (i = 0; i < MUDINFO_MAX; i++)
  {
    mudinfo[i] = 0;
  }

  update_mudinfo();
  send_to_char("Cleared.\n\r", ch);
  return;
}

void do_leaderclear(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (ch->level < 7)
    return;
  free_string(leader_board.pkscore_name);
  leader_board.pkscore_name = str_dup("Noone");
  leader_board.pkscore_number = 0;
  free_string(leader_board.pk_name);
  leader_board.pk_name = str_dup("Noone");
  leader_board.pk_number = 0;
  free_string(leader_board.mobkills_name);
  leader_board.mobkills_name = str_dup("Noone");
  leader_board.mobkills_number = 0;
  free_string(leader_board.quest_name);
  leader_board.quest_name = str_dup("Noone");
  leader_board.quest_number = 0;
  free_string(leader_board.time_name);
  leader_board.time_name = str_dup("Noone");
  leader_board.time_number = 0;
  free_string(leader_board.arena_name);
  leader_board.arena_name = str_dup("Noone");
  leader_board.arena_number = 0;
  save_leaderboard();
  send_to_char("Leader board cleared.\n\r", ch);
  return;
}

void do_ccenter(CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    sprintf(buf, "  #0[#G***#0]   #yDystopian Control Center   #0[#G***#0]#n\n\r\n\r");
    send_to_char(buf, ch);
    sprintf(buf, "  #R(#0lexp#R) #yMinimum Experience        #G%7d#n\n\r", ccenter[CCENTER_MIN_EXP]);
    send_to_char(buf, ch);
    sprintf(buf, "  #R(#0uexp#R) #yMaximum Experience        #G%7d#n\n\r", ccenter[CCENTER_MAX_EXP]);
    send_to_char(buf, ch);
    sprintf(buf, "  #R(#0elvl#R) #yExperience Level              #G%3d#n\n\r", ccenter[CCENTER_EXP_LEVEL]);
    send_to_char(buf, ch);
    sprintf(buf, "  #R(#0qlvl#R) #yQuestpoint Level              #G%3d#n\n\r", ccenter[CCENTER_QPS_LEVEL]);
    send_to_char(buf, ch);
    send_to_char("\n\r\n\r'ccenter reset' to restore default values.\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "lexp"))
  {
    ccenter[CCENTER_MIN_EXP] = atoi(arg2);
    send_to_char("Ok. Value Changed.\n\r", ch);
    save_ccenter();
    do_ccenter(ch, "");
    return;
  }
  else if (!str_cmp(arg1, "uexp"))
  {
    ccenter[CCENTER_MAX_EXP] = atoi(arg2);
    send_to_char("Ok. Value Changed.\n\r", ch);
    save_ccenter();
    do_ccenter(ch, "");
    return;
  }
  else if (!str_cmp(arg1, "elvl"))
  {
    ccenter[CCENTER_EXP_LEVEL] = atoi(arg2);
    send_to_char("Ok. Value Changed.\n\r", ch);
    save_ccenter();
    do_ccenter(ch, "");
    return;
  }
  else if (!str_cmp(arg1, "qlvl"))
  {
    ccenter[CCENTER_QPS_LEVEL] = atoi(arg2);   
    send_to_char("Ok. Value Changed.\n\r", ch);
    save_ccenter();
    do_ccenter(ch, "");
    return;
  }
  else if (!str_cmp(arg1, "reset"))
  {
    ccenter[CCENTER_MIN_EXP] = 4000;
    ccenter[CCENTER_MAX_EXP] = 3000000;
    ccenter[CCENTER_EXP_LEVEL] = 100;
    ccenter[CCENTER_QPS_LEVEL] = 100;
    send_to_char("Ok. Values Reset.\n\r", ch);
    save_ccenter();
    do_ccenter(ch, "");
    return;
  }
  else
  {
    send_to_char("Nope.\n\r", ch);
    return;
  }
}

void do_displayvotes(CHAR_DATA * ch, char *argument)
{
  POLL_DATA *poll;
  VOTE_DATA *vote;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char tbuf[MAX_STRING_LENGTH];
  bool found = FALSE;
  int i = 0;

  if (IS_NPC(ch))
    return;
  one_argument(argument, arg);

  for (poll = poll_list; poll; poll = poll->next)
  {
    if (str_cmp(poll->name, arg))
      continue;
    found = TRUE;
    sprintf(buf, "     #G[#R***#G] #CVotes for the poll on #y%s #G[#R***#G]#n\n\r\n\r", poll->name);
    send_to_char(buf, ch);
    for (vote = poll->votes; vote; vote = vote->next)
    {
      if (i++ > 50)
        break;
      sprintf(buf, "%-12s  %-40s %2d\n\r", vote->pname, vote->phost, vote->choice);
      send_to_char(buf, ch);
    }
    if (i == 0)
      break;
    sprintf(buf, "\n\r#GScore:");
    for (i = 0; i < MAX_VOTE_OPTIONS; i++)
    {
      if (!str_cmp(poll->options[i], "<null>"))
        break;
      sprintf(tbuf, "   #R[#0%d#R] #G%d votes", i + 1, poll->vcount[i]);
      strcat(buf, tbuf);
    }
    strcat(buf, "#n\n\r");
    send_to_char(buf, ch);
  }
  if (!found)
    send_to_char("No such poll.\n\r", ch);
  return;
}

void do_addpoll(CHAR_DATA * ch, char *argument)
{
  POLL_DATA *poll;
  char arg[MAX_INPUT_LENGTH];
  char darg[MAX_INPUT_LENGTH];
  int i, days;

  if (IS_NPC(ch))
    return;
  argument = one_argument(argument, arg);
  one_argument(argument, darg);

  if (arg[0] == '\0')
  {
    send_to_char("Which poll do you wish to start?\n\r", ch);
    return;
  }
  days = atoi(darg);
  if (days < 1 || days > 10)
  {
    send_to_char("Between 1 and 10 days please.\n\r", ch);
    return;
  }
  poll = alloc_perm(sizeof(*poll));
  poll->name = str_dup(arg);
  poll->expire = current_time + (days * 24L * 3600L);
  for (i = 0; i < MAX_VOTE_OPTIONS; i++)
  {
    poll->options[i] = str_dup("<null>");
    poll->vcount[i] = 0;
  }
  poll->next = poll_list;
  poll_list = poll;
  send_to_char("Poll added, you'll need to edit it with polledit.\n\r", ch);
  save_polls();
  save_subvotes(poll);
  return;
}

void do_polledit(CHAR_DATA * ch, char *argument)
{
  POLL_DATA *poll;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool found = FALSE;
  int i;

  if (IS_NPC(ch))
    return;
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    send_to_char("Which poll do you wish to change?\n\r", ch);
    return;
  }
  for (poll = poll_list; poll; poll = poll->next)
  {
    if (str_cmp(poll->name, arg1))
      continue;
    found = TRUE;
    break;
  }
  if (!found)
  {
    send_to_char("No such poll.\n\r", ch);
    return;
  }
  if ((i = atoi(arg2)) < 1 || i > MAX_VOTE_OPTIONS)
  {
    send_to_char("Please pick a valid field.\n\r", ch);
    return;
  }
  free_string(poll->options[i - 1]);
  poll->options[i - 1] = str_dup(argument);
  send_to_char("Options set.\n\r", ch);
  save_polls();
  return;
}

void do_stoppoll(CHAR_DATA * ch, char *argument)
{
  POLL_DATA *poll;
  char arg[MAX_INPUT_LENGTH];
  bool found = FALSE;

  if (IS_NPC(ch))
    return;
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Which poll do you wish to stop?\n\r", ch);
    return;
  }
  for (poll = poll_list; poll; poll = poll->next)
  {
    if (!str_cmp(poll->name, arg))
    {
      found = TRUE;
      break;
    }
  }
  if (!found)
  {
    send_to_char("No such poll.\n\r", ch);
    return;
  }
  complete_poll(poll);
  send_to_char("Ok.\n\r", ch);
  return;
}

void do_addchange2(CHAR_DATA * ch, char *argument)
{
  CHANGE_DATA *change;
  CHANGE_DATA *cchange;
  char *strtime;
  char buf[50];
  char tbuf[MAX_INPUT_LENGTH];
  int i;

  if (IS_NPC(ch))
    return;

  /* we need something to add to the list */
  if (argument[0] == '\0' || strlen(argument) < 5)
  {
    send_to_char("What did you change?\n\r", ch);
    return;
  }

  /* Set the current time */
  strtime = ctime(&current_time);
  for (i = 0; i < 6; i++)
  {
    buf[i] = strtime[i + 4];
  }
  buf[6] = '\0';

  sprintf(tbuf, "%s", line_indent(argument, 25, 80));

  /* If we have a free change, we reuse it */
  if (change_free)
  {
    change = change_free;
    change_free = change_free->next;
    change->next = NULL;
    if (change_free)
      change_free->prev = NULL;
  }
  else
    change = alloc_perm(sizeof(*change));

  /* set the strings for the change */
  change->imm = str_dup(ch->name);
  change->text = str_dup(tbuf);
  change->date = str_dup(buf);

  /* If theres already a change, just add to the list */
  if (change_last)
  {
    change_last->next = change;
    change->prev = change_last;
    change->next = NULL;
    change_last = change;
  }
  else                          // there are no changes.
  {
    change->next = change_list;
    change_list = change;
    change_last = change;
  }

  /* Removing the oldest change if the list has gone beyond the max */
  cchange = change_list;
  i = 0;
  while ((cchange = cchange->next) != NULL)
    i++;
  if (i >= MAX_CHANGE)
    remove_change(1);

  send_to_char("Change added.\n\r", ch);
  save_changes();
  return;
}

void do_delchange2(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  bool found = FALSE;
  int i;

  if (IS_NPC(ch))
    return;

  one_argument(argument, arg);

  if ((i = atoi(arg)) < 1)
  {
    send_to_char("Which number change did you want to remove ?\n\r", ch);
    return;
  }
  found = remove_change(i);
  if (!found)
    send_to_char("No such change.\n\r", ch);
  else
    send_to_char("Change removed.\n\r", ch);
  save_changes();
  return;
}

void do_pathfind(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *path;

  one_argument(argument, arg);
  if ((victim = get_char_world(ch, arg)) == NULL) return;

  if ((path = pathfind(ch, victim)) != NULL)
    sprintf(buf, "Path: %s\n\r", path);
  else
    sprintf(buf, "Path: Unknown.\n\r");
  send_to_char(buf, ch);

  return;
}
