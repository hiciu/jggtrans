#ifndef requests_h
#define requests_h

#include "users.h"

struct request_s;

typedef enum request_type_e{
	RT_NONE=0,
	RT_SEARCH,
	RT_VCARD,
	RT_CHANGE
}RequestType;

typedef struct request_s{
	char *from; /* jid of requesting user */
	char *id;  /* ID if user request (<iq/>) */
	xmlnode query; /* The query */
	RequestType type;
	
	struct gg_http* gghttp; 
	GIOChannel *ioch;
	guint io_watch;

	struct stream_s *stream;
}Request;

Request * add_request(RequestType type,const char *from,const char *id,xmlnode query,struct gg_http *gghttp,struct stream_s *stream);
int remove_request(Request *r);

#endif
