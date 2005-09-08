SUBDIRS = src include

EXTRA_DIST = admin/alut.dsp admin/alut.dsw admin/hello_world.dsp autogen.sh examples test_suite

permissions:
	find . -type f \! -perm +222 \! -name "*.sh" -exec chmod +w {} \;

indent:
	@for i in `find . \( -name "*.c" -o -name "*.h" \) \! -name alut.h -print` ; do \
	    cat $$i | ( cd admin ; indent ) > indent.tmp ; \
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
