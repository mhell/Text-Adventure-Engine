
# För implicit rules
CC = clang++
CPPFLAGS = -std=c++0x

mains = application.o globals.o main.o
views = view.o

controllers = controller.o  game_controller.o main_menu_controller.o

utilities = game_builder.o json_tools.o string_replacer.o utils.o

interfaces = game_object.o serializable.o

worlds = world.o

environments = environment.o exterior.o weather.o

actors = actor.o human.o troll.o zombie.o

items = item.o  container.o environmental.o  misc.o

events = event_repository.o event.o

objects = $(mains) $(views) $(controllers) $(utilities) $(interfaces) $(worlds) $(environments) $(actors) $(items) $(events)


lab3 : $(objects)
	clang++ -std=c++0x -lncurses -o make/run.out $(objects)

application.o : application.h controller.h utils.h
globals.o : globals.h
main.o : test_json_prototype_pool.h test_game_builder.h test_game_object.h application.h main_menu_controller.h view.h globals.h

view.o : view.h utils.h

controller.o : controller.h application.h view.h
game_controller.o : game_controller.h controller.h game_delegate.h world.h game_builder.h environment.h exterior.h utils.h globals.h
main_menu_controller.o : main_menu_controller.h controller.h globals.h game_builder.h game_controller.h utils.h
game_builder.o : game_builder.h world.h json_tools.h environment.h actor.h item.h weather.h
json_tools.o : json_tools.h
string_replacer.o : string_replacer.h
utils.o : utils.h

game_object.o : game_object.h game_delegate.h
serializable.o : serializable.h

world.o : World.h game_object_common.h environment.h utils.h

environment.o : environment.h game_object_common.h actor.h item.h string_replacer.h utils.h
exterior.o : exterior.h environment.h weather.h utils.h
weather.o : weather.h game_object_common.h

actor.o : actor.h game_object_common.h item.h environment.h
human.o : human.h actor.h game_object_common.h item.h
troll.o : troll.h actor.h troll.h
zombie.o : zombie.h actor.h

item.o : item.h environment.h game_object_common.h
container.o : container.h item.h environment.h
environmental.o : item.h environmental.h
misc.o : misc.h item.h

event_repository.o : event_repository.h event.h
event.o : event.h serializable.h json_prototype_pool.h



#environment.h : .h filer (se .PHONY) LÖSNING FÖR ENKLARE .H DEPENDENCIES