/* $Id: jid.c,v 1.12 2003/05/09 10:32:59 jajcus Exp $ */

/*
 *  (C) Copyright 2002 Jacek Konieczny <jajcus@pld.org.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ggtrans.h"
#include <stdio.h>
#include "jid.h"
#include "jabber.h"
#include "ctype.h"
#include "debug.h"

int jid_is_my(const char *jid){
int i,at,slash;
int just_digits;

	if (!jid) return 0;

	just_digits=1;
	at=-1;
	slash=-1;
	/* split into parts, check for numeric username */
	for(i=0;jid[i];i++){
		if (jid[i]=='/' && slash<0) slash=i;
		else if (jid[i]=='@')
			if (!just_digits){
				debug(L_("Non-digits before '@' in jid: %s"),jid);
				return 0;
			}
			else at=i;
		else if (!isdigit(jid[i]) && just_digits) just_digits=0;
	}

	if (slash>=0 && slash<at){
		g_warning(N_("slash<at (%i<%i) in %s"),slash,at,jid);
		return 0;
	}

	/* check hostname */
	if (slash<0){
		if ( g_strcasecmp(jid+at+1,my_name) ){
			debug(L_("Bad hostname (%s) in JID: %s"),jid+at+1,jid);
			return 0;
		}
	} else{
		if ( slash-at-1!=strlen(my_name) ){
			debug(L_("Bad hostname len (%i) instead of %i in JID: %s"),slash-at-1,strlen(my_name),jid);
			return 0;
		}

		if ( g_strncasecmp(jid+at+1,my_name,slash-at-1) ){
			debug(L_("Bad hostname in JID: %s[%i:%i]"),jid,at+1,slash-at-2);
			return 0;
		}
	}

	return 1;
}

int jid_is_me(const char *jid){
int i,slash;

	if (!jid) return 0;

	slash=-1;

	for(i=0;jid[i];i++){
		if (jid[i]=='/' && slash<0) slash=i;
		else if (jid[i]=='@') return 0;
	}

	if (slash<0){
		if ( g_strcasecmp(jid,my_name) ) return 0;
	} else if ( slash!=strlen(my_name) || g_strncasecmp(jid,my_name,slash) ) return 0;

	return 1;
}


int jid_has_uin(const char *jid){
char *p;

	p=strchr(jid,'@');
	if (p) return 1;
	return 0;
}

int jid_get_uin(const char *jid){

	return atoi(jid);
}

const char *jid_get_resource(const char *jid){
const char *p;

	p=strchr(jid,'/');
	if (p) p++;
	return p;
}

char * jid_my_registered(){

	return g_strdup_printf("%s/registered",my_name);
}

char * jid_build(long unsigned int uin){
	return g_strdup_printf("%lu@%s",uin,my_name);
}

char * jid_build_full(long unsigned int uin){
	return g_strdup_printf("%lu@%s/%lu",uin,my_name,uin);
}

char * jid_normalized(const char *jid){
int i,slash;
char *r;

	g_assert(jid!=NULL);
	if (!jid) return 0;

	slash=-1;
	/* split into parts */
	for(i=0;jid[i];i++){
		if (jid[i]=='/' && slash<0) slash=i;
	}

	g_assert(slash!=0);

	if (slash<0) r=g_strdup(jid);
	else r=g_strndup(jid,slash);

	g_strdown(r);

	return r;
}
