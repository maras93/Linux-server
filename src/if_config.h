#include "protocol.h"

int send_ifs_all_names(int fd);
int send_ifs_mac(int fd, char *ifs_name);
int send_ifs_info(int fd, struct message *m);


