// SPDX-License-Identifier: GPL-2.0-or-later
/* Common capabilities, needed by capability.o.
Added code
 */

#include <linux/capability.h>
#include <linux/audit.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/lsm_hooks.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/ptrace.h>
#include <linux/xattr.h>
#include <linux/hugetlb.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/prctl.h>
#include <linux/securebits.h>
#include <linux/user_namespace.h>
#include <linux/binfmts.h>
#include <linux/personality.h>
#include <linux/mnt_idmapping.h>
#include <uapi/linux/lsm.h>

/*
 * If a non-root user executes a setuid-root binary in
 * !secure(SECURE_NOROOT) mode, then we raise capabilities.
 * However if fE is also set, then the intent is for only
 * the file capabilities to be applied, and the setuid-root
 * bit is left on either to change the uid (plausible) or
 * to get full privilege on a kernel without file capabilities
 * support.  So in that case we do not raise capabilities.
 *
 * Warn if that happens, once per boot.
 */
static void warn_setuid_and_fcaps_mixed(const char *fname)
{
	static int warned;
	if (!warned) {
		printk(KERN_INFO "warning: `%s' has both setuid-root and"
			" effective capabilities. Therefore not raising all"
			" capabilities.\n", fname);
		warned = 1;
	}
}

/**
 * cap_capable - Determine whether a task has a particular effective capability
 * @cred: The credentials to use
 * @targ_ns:  The user namespace in which we need the capability
 * @cap: The capability to check for
 * @opts: Bitmask of options defined in include/linux/security.h
 *
 * Determine whether the nominated task has the specified capability amongst
 * its effective set, returning 0 if it does, -ve if it does not.
 *
 * NOTE WELL: cap_has_capability() cannot be used like the kernel's capable()
 * and has_capability() functions.  That is, it has the reverse semantics:
 * cap_has_capability() returns 0 when a task has a capability, but the
 * kernel's capable() and has_capability() returns 1 for this case.
 */
int cap_capable(const struct cred *cred, struct user_namespace *targ_ns,
		int cap, unsigned int opts)
{
	struct user_namespace *ns = targ_ns;

	/* See if cred has the capability in the target user namespace
	 * by examining the target user namespace and all of the target
	 * user namespace's parents.
	 */
	for (;;) {
		/* Do we have the necessary capabilities? */
		if (ns == cred->user_ns)
			return cap_raised(cred->cap_effective, cap) ? 0 : -EPERM;

		/*
		 * If we're already at a lower level than we're looking for,
		 * we're done searching.
		 */
		if (ns->level <= cred->user_ns->level)
			return -EPERM;

		/* 
		 * The owner of the user namespace in the parent of the
		 * user namespace has all caps.
		 */
		if ((ns->parent == cred->user_ns) && uid_eq(ns->owner, cred->euid))
			return 0;

		/*
		 * If you have a capability in a parent user ns, then you have
		 * it over all children user namespaces as well.
		 */
		ns = ns->parent;
	}

	/* We never get here */
}

/**
 * cap_settime - Determine whether the current process may set the system clock
 * @ts: The time to set
 * @tz: The timezone to set
 *
 * Determine whether the current process may set the system clock and timezone
 * information, returning 0 if permission granted, -ve if denied.
 */
int cap_settime(const struct timespec64 *ts, const struct timezone *tz)
{
	if (!capable(CAP_SYS_TIME))
		return -EPERM;
	return 0;
}
