PROJECTS = port_scanner tgrep

all: $(PROJECTS)

$(PROJECTS):
	$(MAKE) -C $@

clean:
	for p in $(PROJECTS); do $(MAKE) -C $$p clean; done

fclean:
	for p in $(PROJECTS); do $(MAKE) -C $$p fclean; done

re: fclean all

.PHONY: all clean fclean re $(PROJECTS)
