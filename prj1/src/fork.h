#ifndef __INC_FORK_H
#define __INC_FORK_H

#define PHILOSOPHERS_NUM    5

extern void fork_init(void);
extern void fork_take(int philosopher_id);
extern void fork_put(int philosopher_id);
extern void fork_deinit(void);

#endif
