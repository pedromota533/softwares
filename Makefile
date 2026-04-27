PROJECTS = port_scanner tgrep workspace_saver

all: $(PROJECTS) btop

$(PROJECTS):
	$(MAKE) -C $@

btop:
	$(MAKE) -C btop

clean:
	for p in $(PROJECTS); do $(MAKE) -C $$p clean; done

fclean:
	for p in $(PROJECTS); do $(MAKE) -C $$p fclean; done
	$(MAKE) -C btop clean

re: fclean all

.PHONY: all clean fclean re $(PROJECTS) btop
