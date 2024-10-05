make_r ?= make -f scripts/ci_ubuntu.make

.PHONY: format
format:
	find gbench -name '*pp' -type f | xargs clang-format-17 -i
	find gtest -name '*pp' -type f | xargs clang-format-17 -i
	find include -name '*pp' -type f | xargs clang-format-17 -i
	find samples -name '*pp' -type f | xargs clang-format-17 -i
	find src -name '*pp' -type f | xargs clang-format-17 -i
	find test -name '*pp' -type f | xargs clang-format-17 -i
	$(make_r) add-eol P=.github
	$(make_r) add-eol P=cmake
	$(make_r) add-eol P=gbench
	$(make_r) add-eol P=gtest
	$(make_r) add-eol P=include
	$(make_r) add-eol P=scripts
	$(make_r) add-eol-root

.PHONY: add-eol
add-eol:
	@find $(P) -type f | while read file; do \
        if ! tail -c1 "$$file" | grep -q "^$$"; then \
            echo >> "$$file"; \
        fi \
    done

.PHONY: add-eol-root
add-eol-root:
	@find . -maxdepth 1 -type f | while read file; do \
		if ! tail -c1 "$$file" | grep -q "^$$"; then \
			echo >> "$$file"; \
		fi \
    done

.PHONY: check-git-status
check-git-status:
	@echo "Checking if all files are committed to git..."
	@if [ -n "$$(git status --porcelain)" ]; then \
		echo "The following files are not committed:"; \
		git status --short; \
		echo "Please commit all changes and try again."; \
		git diff --color | cat; \
		exit 1; \
	else \
		echo "All files are committed to git."; \
	fi

.PHONY: install-compiler
install-compiler:
	@if [ "$(compiler)" = "clang" ]; then \
            wget https://apt.llvm.org/llvm.sh; \
            chmod +x llvm.sh; \
            sudo ./llvm.sh $(version); \
            rm llvm.sh;\
    elif [ "$(compiler)" = "gcc" ]; then \
      	sudo apt install -y g++-$(version); \
      	sudo apt install -y gcc-$(version); \
  	else \
      echo "Unknown compiler" >&2; \
  	fi

.PHONY: find-c-compiler
find-c-compiler:
	@if [ "$(compiler)" = "clang" ]; then \
        echo "/usr/bin/clang-$(version)"; \
      elif [ "$(compiler)" = "gcc" ]; then \
        echo "/usr/bin/gcc-$(version)"; \
      else \
        echo "Unknown compiler" >&2;  \
      fi

.PHONY: find-cxx-compiler
find-cxx-compiler:
	@if [ "$(compiler)" = "clang" ]; then \
        echo "/usr/bin/clang++-$(version)"; \
      elif [ "$(compiler)" = "gcc" ]; then \
        echo "/usr/bin/g++-$(version)"; \
      else \
        echo "Unknown compiler" >&2; \
      fi
