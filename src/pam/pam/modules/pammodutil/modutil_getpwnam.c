/*
 * $Id: modutil_getpwnam.c,v 1.2 2002/03/27 02:36:46 bbraun Exp $
 *
 * This function provides a thread safer version of getpwnam() for use
 * with PAM modules that care about this sort of thing.
 *
 * XXX - or at least it should provide a thread-safe alternative.
 */

#include "pammodutil.h"

#include <pwd.h>
#include <stdlib.h>

struct passwd *_pammodutil_getpwnam(pam_handle_t *pamh, const char *user)
{
#ifdef HAVE_GETPWNAM_R

    void *buffer=NULL;
    size_t length = PWD_INITIAL_LENGTH;

    do {
	int status;
	void *new_buffer;
	struct passwd *result = NULL;

	new_buffer = realloc(buffer, sizeof(struct passwd) + length);
	if (new_buffer == NULL) {

	    D(("out of memory"));

	    /* no memory for the user - so delete the memory */
	    if (buffer) {
		free(buffer);
	    }
	    return NULL;
	}
	buffer = new_buffer;

	/* make the re-entrant call to get the pwd structure */
	status = getpwnam_r(user, buffer,
			    sizeof(struct passwd) + (char *) buffer,
			    length, &result);
	if (!status && result) {
	    status = pam_set_data(pamh, "_pammodutil_getpwnam", result,
				  _pammodutil_cleanup);
	    if (status == PAM_SUCCESS) {
		D(("success"));
		return result;
	    }

	    D(("was unable to register the data item [%s]",
	       pam_strerror(pamh, status)));

	    free(buffer);
	    return NULL;

	}
	
	length <<= 1;

    } while (length < PWD_ABSURD_PWD_LENGTH);

    D(("pwd structure took %u bytes or so of memory",
       length+sizeof(struct passwd)));

    free(buffer);
    return NULL;

#else /* ie. ifndef HAVE_GETPWNAM_R */

    /*
     * Sorry, there does not appear to be a reentrant version of
     * getpwnam(). So, we use the standard libc function.
     */
    
    return getpwnam(user);

#endif /* def HAVE_GETPWNAM_R */
}
