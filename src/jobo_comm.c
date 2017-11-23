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

/*
 * Communication storage system. Ye Jobo.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

HISTORY_DATA * history_free;
HISTORY_DATA * stored_tells;

void init_tell_storage()
{
  HISTORY_DATA *history_new;
  HISTORY_DATA *history_prev = NULL;
  int i;

  for (i = 0; i < MAX_STORAGE; i++)
  {
    if (history_free == NULL)
    {
      history_new = alloc_perm(sizeof(*history_new));
    }
    else
    {
      history_new  = history_free;
      history_free = history_free->next;
    }
    history_new->message = str_dup("");
    history_new->next = history_prev;
    history_prev = history_new;
  }
  stored_tells = history_new;
}

void talk_channel( CHAR_DATA *ch, char *argument, int channel, int sub_channel, const char *verb )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int i;

  if (!IS_NPC(ch) && getMight(ch) < 150 &&
     (channel == CHANNEL_CHAT || channel == CHANNEL_MUSIC || channel == CHANNEL_YELL))
  {
    channel = CHANNEL_NEWBIE;
  }

  if (!IS_NPC(ch) && (get_age(ch) - 17) < 2 && (channel == CHANNEL_FLAME || channel == CHANNEL_ROLEPLAY))
  {
    send_to_char("You must be at least 4 hours old to use this channel.\n\r", ch);
    return;
  }
  if (RTIMER(ch->in_room, RTIMER_SILENCE) != 0)
  {
    send_to_char("Something prevents you from speaking in this room.\n\r", ch);
    return;
  }
  if ( argument[0] == '\0' )
  {
    sprintf( buf, "%s what?\n\r", verb );
    buf[0] = UPPER(buf[0]);
    send_to_char(buf,ch);
    return;
  }
  if ( IS_HEAD(ch, LOST_TONGUE) )
  {
    sprintf( buf, "You can't %s without a tongue!\n\r", verb );
    send_to_char( buf, ch );
    return;
  }
  if ( IS_EXTRA(ch, GAGGED) )
  {
    sprintf( buf, "You can't %s with a gag on!\n\r", verb );
    send_to_char( buf, ch );
    return;
  }
  REMOVE_BIT(ch->deaf, channel);
  switch ( channel )
  {
    default:
      if (channel == CHANNEL_FLAME)
      {
        sprintf( buf, "You %s, '#G%s#n'\n\r", verb, argument );
        send_to_char( buf, ch );
        sprintf( buf, "%s %ses, '#G$t#n'",ch->name, verb );
      }
      else if (channel == CHANNEL_CHAT)
      {
        sprintf( buf, "You %s, '#R%s#n'\n\r", verb, argument );
        send_to_char( buf, ch );
        if (ch->trust > 6)
          sprintf( buf, "#y(#G*#y)#C%s#y(#G*#y)#n '#R$t#n'.", ch->name);
        else if (IS_NPC(ch))
          sprintf( buf, "%s chats, '#R$t#n'", ch->short_descr);
        else  
          sprintf( buf, "%s %ss '#R$t#n'", ch->name, verb );
      }
      else
      {
        sprintf( buf, "You %s, '#R%s#n'\n\r", verb, argument );
        send_to_char( buf, ch );
        sprintf( buf, "%s %ss, '#R$t#n'",ch->name,     verb );
      }
      break;
    case CHANNEL_IMMTALK:
      sprintf( buf, "#y.:#P%s#y:.#C $t.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_CLASS:
      switch(sub_channel)
      {
      }
      break;      
    case CHANNEL_NEWBIE:
      if (getMight(ch) < 150)
        sprintf( buf, "%s the newbie chats, #7'#R$t#7'#n",ch->name);
      else
        sprintf( buf, "%s the newbie helper chats, #7'#R$t#7'.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_DKTALK:
      sprintf( buf, "#R*#7*#R* #7%s #R*#7*#R*#7 '$t'.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_ROLEPLAY:
      sprintf( buf, "#o(#R%s#o) #n'#y$t#n'.", ch->name);
      act( buf, ch, argument, NULL, TO_CHAR ); 
      break;
  }

  /* silenced, and they don't know it :) */
  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE)) return;

  /* anti spamming */
  if (!IS_NPC(ch) && (channel == CHANNEL_CHAT || channel == CHANNEL_NEWBIE))
  {
    if (!str_cmp(ch->pcdata->last_global, argument)) return;
    free_string(ch->pcdata->last_global);
    ch->pcdata->last_global = str_dup(argument);
  }

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    CHAR_DATA *gch;

    if (d->connected != CON_PLAYING) continue;
    if ((gch = d->character) == NULL) continue;
    if (gch == ch) continue;
    if (IS_SET(gch->deaf, channel)) continue;

    if (channel == CHANNEL_IMMTALK && !IS_IMMORTAL(gch)) continue;
    if (channel == CHANNEL_CLASS && !IS_IMMORTAL(gch) && (gch->class != ch->class || gch->level < 3))
      continue;
    if (channel == CHANNEL_YELL && gch->in_room && gch->in_room->area != ch->in_room->area)
      continue;

    if (!IS_NPC(gch) && !IS_NPC(ch))
    {
      bool ignore = FALSE;

      for (i = 0; i < MAX_IGNORE; i++)
        if (gch->pcdata->ignore[i] == ch->pcdata->playerid) ignore = TRUE;

      if (ignore) continue;
    }

    if (!IS_NPC(gch) && !IS_NPC(ch)) update_history(gch, ch, buf, argument, FALSE);

    act(buf, ch, argument, gch, TO_VICT);
  }
  return;  
}

HISTORY_DATA *generate_history()
{
  HISTORY_DATA *history_new;
  HISTORY_DATA *history_prev = NULL;
  int i;

  for (i = 0; i < MAX_HISTORY; i++)
  {
    if (history_free == NULL)
    {
      history_new = alloc_perm(sizeof(*history_new));
    }
    else
    {
      history_new  = history_free;
      history_free = history_free->next;
    }
    history_new->message = str_dup("");
    history_new->next = history_prev;
    history_prev = history_new;
  }
  return history_new;
}

void strip_history(CHAR_DATA *ch)
{
  HISTORY_DATA *pHistory;
  HISTORY_DATA *pHistory_next;

  for (pHistory = ch->pcdata->history; pHistory; pHistory = pHistory_next)
  {
    pHistory_next = pHistory->next;

    free_string(pHistory->message);
    pHistory->next = history_free;
    history_free = pHistory;    
  }
}

void do_history(CHAR_DATA *ch, char *argument)
{
  HISTORY_DATA *history;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  for (history = ch->pcdata->history; history; history = history->next)
  {
    if (strlen(history->message) > 3)
    {
      sprintf(buf, " %s\n\r", history->message);
      send_to_char(buf, ch);
    }
  }
}

void update_history(CHAR_DATA *ch, CHAR_DATA *talker, char *buf, char *argument, bool tell)
{
  char message[MAX_STRING_LENGTH];
  char tbuf[MAX_STRING_LENGTH];
  char xbuf[MAX_STRING_LENGTH];
  int i = 0;

  if (IS_NPC(ch)) return;

  sprintf(xbuf, "%s", line_indent(strip_ansi(argument), 22, 80));

  if (!tell)
  {
    sprintf(tbuf, strip_ansi(buf));
    while (tbuf[i] != '\0' && tbuf[i] != ' ') i++;
    tbuf[i] = '\0';
    sprintf(message, "#C%-18s #0: #R%-s#n", tbuf, xbuf);
  }
  else
  {
    sprintf(message, "#0[#GT#0] #C%-14s #0: #R%-s#n", talker->name, xbuf);
  }

  attach_history(ch, message);
  return;
}

void attach_history(CHAR_DATA *ch, char *message)
{
  HISTORY_DATA *history;
  HISTORY_DATA *history_prev1;
  HISTORY_DATA *history_prev2;

  if (history_free == NULL)
  {
    history = alloc_perm(sizeof(*history));
  }
  else
  {
    history      = history_free;
    history_free = history_free->next;
  }
  history->message = str_dup(message);
  history->next = ch->pcdata->history;
  ch->pcdata->history = history;

  for (history = ch->pcdata->history; history; history = history->next)
  {
    if (history_prev1) history_prev2 = history_prev1;
    history_prev1 = history;
  }
  history_prev2->next = NULL;
  free_string(history_prev1->message);
  history_prev1->next = history_free;
  history_free = history_prev1;
}

/*
 * Will remove the last line in the current note,
 * if there is a note, and it has a line to remove.
 */
void delete_last_line_in_note(CHAR_DATA *ch)
{
  char buf[4 * MAX_STRING_LENGTH];
  char *ptr;
  bool found = FALSE;
  int nCount = 0;

  buf[0] = '\0';

  if (IS_NPC(ch)) return;
  if (ch->pcdata->in_progress->text == NULL)
  {
    send_to_char("No note to delete lines in.\n\r", ch);
    return;
  }
  if (strlen(ch->pcdata->in_progress->text) < 1)
  {
    send_to_char("Empty note, nothing to delete.\n\r", ch);
    return;
  }
  strcpy(buf, ch->pcdata->in_progress->text);
  ptr = buf;
  while (*ptr != '\0')
  {
    if (*ptr == '\n') nCount++;
    ptr++;
  }
  if (nCount == 1)
  {
    free_string(ch->pcdata->in_progress->text);
    ch->pcdata->in_progress->text = NULL;
    send_to_char("Entire note deleted.\n\r", ch);
    return;
  }
  else
  {
    while (*ptr != '\n' || !found)
    {
      if (*ptr == '\n') found = TRUE;
      ptr--;
    }
  }
  ptr++;
  *ptr = '\0';
  free_string(ch->pcdata->in_progress->text);
  ch->pcdata->in_progress->text = str_dup(buf);
  send_to_char("Line deleted.\n\r", ch);
}
