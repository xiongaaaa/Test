all: talkA talkB

talkA:talkA.cpp
	gcc $< -o $@

talkB:talkB.cpp
	gcc $< -o $@

.PHONY:clean
clean:
	rm -rf talkA talkB