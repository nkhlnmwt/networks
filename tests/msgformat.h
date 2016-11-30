#define MSG_LEN 63

struct msg_prot
{
  int msg_type;
  char msg[MSG_LEN];
};

void auxzero (struct msg_prot *var)	/* Make everything to zero */
{
  var->msg_type = 0;
  bzero(var->msg, MSG_LEN);	
}
  
