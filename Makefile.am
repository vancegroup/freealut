SUBDIRS = src include

EXTRA_DIST = autogen.sh examples test_suite win_build

permissions:
	find . -type f \! -perm +222 \! -name "*.sh" -exec chmod +w {} \;

indent:
	@for i in `find . \( -name "*.c" -o -name "*.h" \) \! -name alut.h -print` ; do \
	    cat $$i | indent > indent.tmp ; \
	    if diff $$i indent.tmp > /dev/null 2>&1 ; then \
		: ; \
	    else \
		echo Reformatted $$i ; \
		$(RM) $$i ; \
		mv indent.tmp $$i ; \
	    fi ; \
	done
	@$(RM) indent.tmp

.PHONY: permissions indent
