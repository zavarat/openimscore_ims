/*
 * $Id: t_reply.h 991 2010-09-21 08:29:51Z aon $
 *
 * Copyright (C) 2001-2003 FhG Fokus
 *
 * This file is part of ser, a free SIP server.
 *
 * ser is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * ser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef _T_REPLY_H
#define _T_REPLY_H

#include "defs.h"
#include "../../rpc.h"
#include "../../tags.h"

#include "h_table.h"


extern int restart_fr_on_each_reply;
extern int tm_aggregate_auth; /* aggregate authorization header value */

/* reply processing status */
enum rps {
	/* something bad happened */
	RPS_ERROR=0,	
	/* transaction completed but we still accept the reply */
	RPS_PUSHED_AFTER_COMPLETION,
	/* reply discarded */
	RPS_DISCARDED,
	/* reply stored for later processing */
	RPS_STORE,
	/* transaction completed */
	RPS_COMPLETED,
	/* provisional reply not affecting transaction state */
	RPS_PROVISIONAL
};

extern char tm_tags[TOTAG_VALUE_LEN];
extern char *tm_tag_suffix;

enum route_mode { MODE_REQUEST=1, MODE_ONREPLY, MODE_ONFAILURE };
extern enum route_mode rmode;

/* has this to-tag been never seen in previous 200/INVs? */
int unmatched_totag(struct cell *t, struct sip_msg *ack);

/* branch bitmap type */
typedef unsigned int branch_bm_t;

/* reply export types */
typedef int (*treply_f)(struct sip_msg * , unsigned int , char * );
typedef int (*treply_wb_f)( struct cell* trans,
	unsigned int code, char * text, char * body, 
	char * new_header, char * to_tag);
typedef int (*tenter_ctx_f)(unsigned int new_hash_index, unsigned int new_label,
		enum route_mode * crt_rmode, enum route_mode new_rmode, 
		struct cell** crt_trans, struct cell ** new_trans);
typedef int (*texit_ctx_f)(struct cell * new_trans, enum route_mode new_rmode);

int t_enter_ctx(unsigned int new_hash_index, unsigned int new_label,
		enum route_mode * crt_rmode, enum route_mode new_rmode, 
		struct cell** crt_trans, struct cell ** new_trans);
int t_exit_ctx(struct cell * new_trans, enum route_mode new_rmode);

/* wrapper function needed after changes in w_t_reply */
int w_t_reply_wrp(struct sip_msg *m, unsigned int code, char *txt);

#define LOCK_REPLIES(_t) lock(&(_t)->reply_mutex )
#define UNLOCK_REPLIES(_t) unlock(&(_t)->reply_mutex )

/* This function is called whenever a reply for our module is received;
 * we need to register this function on module initialization;
 * Returns :   0 - core router stops
 *             1 - core router relay statelessly
 */
int reply_received( struct sip_msg  *p_msg ) ;

/* return 1 if a failure_route processes */
int run_failure_handlers(struct cell *t, struct sip_msg *rpl,
					int code, int extra_flags);
typedef int (*run_failure_handlers_f)(struct cell*, struct sip_msg*, int, int);


/* Retransmits the last sent inbound reply.
 * Returns  -1 - error
 *           1 - OK
 */
int t_retransmit_reply( /* struct sip_msg * */  );


/* send a UAS reply
 * Warning: 'buf' and 'len' should already have been build.
 * returns 1 if everything was OK or -1 for error
 */


int t_reply_with_body( struct cell *trans, unsigned int code, 
		       char * text, char * body, char * new_header, char * to_tag );


/* send a UAS reply
 * returns 1 if everything was OK or -1 for error
 */
int t_reply( struct cell *t, struct sip_msg * , unsigned int , char * );
/* the same as t_reply, except it does not claim
   REPLY_LOCK -- useful to be called within reply
   processing
*/
int t_reply_unsafe( struct cell *t, struct sip_msg * , unsigned int , char * );


enum rps relay_reply( struct cell *t, struct sip_msg *p_msg, int branch, 
	unsigned int msg_status, branch_bm_t *cancel_bitmap );

enum rps local_reply( struct cell *t, struct sip_msg *p_msg, int branch,
    unsigned int msg_status, branch_bm_t *cancel_bitmap );

void set_final_timer( /* struct s_table *h_table,*/ struct cell *t );

void cleanup_uac_timers( struct cell *t );

void on_negative_reply( struct cell* t, struct sip_msg* msg,
	int code, void *param  );

/* set which 'reply' structure to take if only negative
   replies arrive 
*/
void t_on_negative( unsigned int go_to );
unsigned int get_on_negative();
void t_on_reply( unsigned int go_to );
unsigned int get_on_reply();

int t_retransmit_reply( struct cell *t );

void tm_init_tags();

/* selects the branch for fwd-ing the reply */
int t_pick_branch(int inc_branch, int inc_code, struct cell *t, int *res_code);

extern const char* rpc_reply_doc[2];
void rpc_reply(rpc_t* rpc, void* c);

#endif
