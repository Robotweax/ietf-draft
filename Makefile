XML2RFC ?= xml2rfc
DRAFTS := $(wildcard drafts/*.xml)

.PHONY: check render
check:
	python3 tools/check.py

render: check
	mkdir -p build
	@set -e; for source in $(DRAFTS); do \
	  name=$$(basename "$$source" .xml); \
	  $(XML2RFC) "$$source" --text --out "build/$$name.txt"; \
	  $(XML2RFC) "$$source" --html --out "build/$$name.html"; \
	done
