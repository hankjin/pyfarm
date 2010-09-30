CXXFLAGS=-g
tests=test nq dp

#chapter6: 3(97)
chapter6=cowxor_orig cowxor_ac rectbarn_ac vans_ac \
	 vans rectbarn cowxor
	
#chapter5: 18 (94)
chapter5=snail_ac fence3_orig wissqu_orig wissqu_ac \
	 fc starry theme \
	 snail fence3 wissqu \
	 milk4_ac bigbrn_orig schlnet_ac latin_orig betsy_orig latin_ac \
	 milk4 window bigbrn schlnet \
	 tour_ac betsy_ac telecow_ac \
	 latin tour charrec betsy telecow \
	 hidden_orig hidden_ac \
	 picture twofive hidden

#chapter4: 15 (76)
chapter4=fence6_orig nuggets_ac cryptcow_orig cryptcow_ac \
         nuggets fence8 fence6 cryptcow \
	 ditch_orig ditch_ac ditch_orig2 stall4_orig stall4_ac \
	 ditch stall4 job cowcycle \
	 buylow_orig buylow_ac shuttle_ac milk6_ac \
	 buylow prime3 race3 lgame \
	 shuttle milk6 frameup 

#chapter3: 21 (61)
chapter3=inflate_orig humble_orig humble_hjz stamps_orig contact_ac \
	 agrinet inflate humble stamps rect1 contact \
	 kimbits_orig ratios_ac butter_orig range_orig \
	 fact4 kimbits spin ratios msquare butter \
	 fence shopping range camelot game1 \
	 fence4 heritage fence9 rockers

#chapter2: 19 (40)
chapter2=castle frac1 sort3 holstein hamming \
	preface subset runround lamps \
	prefix nocows zerosum money concom \
	ttwo maze1 cowtour comehome fracdec

#chapter1: 21
chapter1=ride beads friday gift1 \
	milk2 transform namenum palsquare dualpal \
	milk barn1 calfflac crypt1 \
	packrec clocks ariprog milk3 \
	numtri pprime sprime checker 

orig=milk_orig namenum_orig packrec_orig \
	frac1_orig holstein_orig subset_orig \
	nocows_orig money_orig concom_orig \
	cowtour_orig cowtour_ac

objects=$(test) $(orig) $(chapter1) $(chapter2) $(chapter3) \
	$(chapter4) $(chapter5) $(chapter6) a b c

all: $(objects) $(tests)

clean: $(objects)
	-rm $(objects)
