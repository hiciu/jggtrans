#include "ggtrans.h"
#include "jabber.h"
#include "iq.h"
#include "stats.h"
#include "debug.h"
#include "conf.h"
#include "jid.h"
#include "sessions.h"
#include <time.h>

static int stat_uptime(void){
	return time(NULL)-start_time;
}

static int stat_packets_in(void){
	return packets_in;
}

static int stat_packets_out(void){
	return packets_out;
}

static int stat_gg_messages_in(void){
	return gg_messages_in;
}

static int stat_gg_messages_out(void){
	return gg_messages_out;
}

static int stat_users_registered(void){

	return -1;
}
	
static void count_session(gpointer key,gpointer value,gpointer data){
int *counter=(int *)data;

	counter++;
}
	
static int stat_users_online(void){
int counter=0;

	g_hash_table_foreach(sessions_jid,count_session,&counter);
	return counter;
}

static void count_connected_session(gpointer key,gpointer value,gpointer data){
const Session *sess=(Session *)value;
int *counter=(int *)data;

	if (sess->connected) counter++;
}

static int stat_users_connected(void){
int counter=0;

	g_hash_table_foreach(sessions_jid,count_connected_session,&counter);
	return counter;
}

static struct stats_s {
	const char *name;
	const char *units;
	int (*reader)(void);
	}stats[]={
		{ "time/uptime","seconds",stat_uptime },
		{ "bandwidth/packets-in","packets",stat_packets_in },
		{ "bandwidth/packets-out","packets",stat_packets_out },
		{ "bandwidth/gg-messages-in","messages",stat_gg_messages_in },
		{ "bandwidth/gg-messages-out","messages",stat_gg_messages_out },
		{ "users/registered","users",stat_users_registered },
		{ "users/online","users",stat_users_online },
		{ "users/connected","users",stat_users_connected },
		{ NULL,NULL,NULL }
	};

void jabber_iq_get_server_stats(Stream *s,const char *from,const char * to,const char *id,xmlnode q){
xmlnode result;
xmlnode n,stat,err;
char *str;
int i;

	n=xmlnode_get_firstchild(q);
	result=xmlnode_new_tag("query");
	xmlnode_put_attrib(result,"xmlns","http://jabber.org/protocol/stats");
	if (n==NULL){
		for(i=0;stats[i].name;i++){
			stat=xmlnode_insert_tag(result,"stat");
			xmlnode_put_attrib(stat,"name",stats[i].name);
		}
	}
	for(;n;n=xmlnode_get_nextsibling(q)){
		str=xmlnode_get_name(n);
		if (!strcmp(str,"stat")) continue;
		str=xmlnode_get_attrib(n,"name");
		if (!str) continue;
		stat=xmlnode_insert_tag(result,"stat");
		xmlnode_put_attrib(stat,"name",str);
		for(i=0;stats[i].name;i++)
			if (!strcmp(stats[i].name,str)) break;
		if (!stats[i].name){
			err=xmlnode_insert_tag(stat,"error");
			xmlnode_put_attrib(err,"code","404");
			xmlnode_insert_cdata(err,_("Statistic not found"),-1);
			continue;
		}
		xmlnode_put_attrib(stat,"units",stats[i].units);
		str=g_strdup_printf("%i",stats[i].reader());
		xmlnode_put_attrib(stat,"value",str);
		g_free(str);
	}
	jabber_iq_send_result(s,from,to,id,result);
}
