

  CFLAGS=-fPIC -g -Wall
  
  SPHINX_LINK=-L/usr/local/lib -lpocketsphinx -lsphinxbase -lsphinxad -llapack -lpthread -lm
  ARIA_LINK=-L/usr/local/Aria/lib -lAria -lpthread -ldl -lrt

  ARIA_INCLUDE=-I/usr/local/Aria/include
  SPHINX_INCLUDE=-I/usr/local/include/sphinxbase
  POCKET_INCLUDE=-I/usr/local/include/pocketsphinx

  # Variable for linking
  LINK = -L $(LIBFT_DIR) -lft $(SPHX_LIBS)

  %: %.cpp
	  $(CXX) $(CFLAGS) $(ARIA_INCLUDE) $(SPHINX_INCLUDE) $(POCKET_INCLUDE) $< -o $@ $(ARIA_LINK) $(SPHINX_LINK)
