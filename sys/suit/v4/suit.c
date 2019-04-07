#include "suit/v4/suit.h"
#include "suit/v4/policy.h"

#include "log.h"

static volatile int _subs_thread_pid;

int suit_get_status_subs(void)
{
    return _subs_thread_pid;
}

void suit_set_status_subs(int pid)
{
    _subs_thread_pid = pid;
}

int suit_v4_policy_check(suit_v4_manifest_t *manifest)
{
    if (SUIT_DEFAULT_POLICY & ~(manifest->validated)) {
        LOG_INFO("SUIT policy check failed!\n");
        return -1;
    }
    else {
        LOG_INFO("SUIT policy check OK.\n");
        return 0;
    }
}
